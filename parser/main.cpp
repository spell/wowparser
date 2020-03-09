#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/program_options.hpp>

#include "parameters.hpp"
#include "event.hpp"

constexpr auto win_log_path = R"(C:\Program Files (x86)\World of Warcraft\_retail_\Logs\WoWCombatLog.txt)";

std::vector<std::string_view> split_parameters(std::string_view s) {
	int last_index = 0;
	bool in_quotes = false;
	std::vector<std::string_view> params;
	params.reserve(64);

	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ',' && !in_quotes) {
			params.push_back(s.substr(last_index, i - last_index));
			last_index = i + 1; // skip comma
		}

		if (s[i] == '\\') {
			i++;
			continue;
		}

		if (s[i] == '"') {
			in_quotes = !in_quotes;
		}
	}

	// get the last param and return
	params.push_back(s.substr(last_index, s.length() - last_index));
	return params;
}

std::string_view get_event(std::string_view s) {
	auto timestamp_end = s.find("  ");
	return s.substr(timestamp_end + 2);
}

combat_event process_line(std::ostream &out, uint64_t index, std::string_view line) {
	auto params = split_parameters(get_event(line));

	combat_event e;
	e.event_type = params[0];
	e.prefix_type = event_type_to_prefix(e.event_type);
	e.suffix_type = event_type_to_suffix(e.event_type);

	if (e.prefix_type != subevent_prefix_type::unknown && e.suffix_type != subevent_suffix_type::unknown) {
		parse_parameters(params, e.base, 1);
		write_parameters<200>(out, e.base);

		auto offset = 1 + std::tuple_size_v<base_parameters>;

		switch (e.prefix_type) {
			case subevent_prefix_type::swing: {
				e.prefix = swing_parameters();
				parse_parameters(params, std::get<swing_parameters>(*e.prefix), offset);
				offset += std::tuple_size_v<swing_parameters>;
				break;
			}

			case subevent_prefix_type::range:
			case subevent_prefix_type::spell:
			case subevent_prefix_type::spell_periodic:
			case subevent_prefix_type::spell_building: {
				e.prefix = spell_parameters();
				parse_parameters(params, std::get<spell_parameters>(*e.prefix), offset);
				offset += std::tuple_size_v<spell_parameters>;
				break;
			}

			case subevent_prefix_type::environmental: {
				return e;
			}

			default:
				throw std::runtime_error("process_line: fatal exception; unreachable code");
		}

		switch (e.suffix_type) {
			case subevent_suffix_type::damage: {
				e.suffix = damage_parameters();
				parse_parameters(params, e.advanced, offset);
				parse_parameters(params, std::get<damage_parameters>(*e.suffix),
				                 offset + std::tuple_size_v<advanced_parameters>);
				break;
			}

			case subevent_suffix_type::missed: {
				e.suffix = missed_parameters();
				parse_parameters(params, std::get<missed_parameters>(*e.suffix), offset);
				break;
			}

			case subevent_suffix_type::heal: {
				e.suffix = heal_parameters();
				parse_parameters(params, e.advanced, offset);
				parse_parameters(params, std::get<heal_parameters>(*e.suffix),
				                 offset + std::tuple_size_v<advanced_parameters>);
				break;
			}

			default:
				throw std::runtime_error("process_line: fatal exception; unreachable code");
		}
	}

	return e;
}

int main(int argc, char **argv) {
	std::string path;
	int report_progress = 0;

	try {
		boost::program_options::options_description general("general");
		general.add_options()
				("help,h", "prints this help message")
				("version,v", "prints version information")
				("verbose,x", "write logging information")
				("progress,p", boost::program_options::value<int>(&report_progress),
				 "reports progress every arg percent")
				("combat-log,L", boost::program_options::value<std::string>(&path)->default_value(win_log_path),
				 "path to the log file");

		// parse program options
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, general), vm);
		boost::program_options::notify(vm);

		if (vm.count("help")) {
			std::cout << general << std::endl;
			return 0;
		}

		if (vm.count("version")) {
			std::cout << "wowparser v0.1.0-alpha" << std::endl;
			return 0;
		}
	} catch (std::exception &e) {
		std::cerr << "[ERR] could not parse arguments: " << e.what() << std::endl;
		return 1;
	}

	try {
		boost::iostreams::mapped_file_source file_source(path);
		boost::iostreams::stream<boost::iostreams::mapped_file_source> stream(file_source, std::ios::binary);
		std::ofstream out_file("output.wcl", std::ios::binary | std::ios::out);

		if (!stream.is_open()) {
			return 1;
		}

		std::string line;
		uint64_t line_index = 0;
		uint64_t bytes_read = 0;
		uint64_t size = stream->size();
		uint64_t progress = 0;
		uint64_t progress_tick = size / 100 * report_progress;
		auto start = std::chrono::system_clock::now();

		while (std::getline(stream, line, '\r')) {
			process_line(out_file, ++line_index, line);
			stream.seekg(1, std::ios::cur);
			bytes_read += line.size() + 2;

			if (report_progress && bytes_read / progress_tick > progress) {
				std::cout << ++progress * report_progress << "% complete"
				          << " (" << bytes_read << " / " << size << " bytes read)"
				          << std::endl;
			}
		}

		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "parsing finished in " << float(duration.count()) / 1000 << " sec\n";
	} catch (std::exception &e) {
		std::cerr << "[ERR] " << e.what() << std::endl;
	}

	return 0;
}
