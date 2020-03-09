#include <iostream>
#include <vector>
#include <chrono>
#include <map>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include "reader.hpp"
#include "parameters.hpp"
#include "event.hpp"

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

void process_line(std::string_view line) {
	auto params = split_parameters(get_event(line));

	combat_event e;
	e.event_type = params[0];
	e.prefix_type = event_type_to_prefix(e.event_type);
	e.suffix_type = event_type_to_suffix(e.event_type);

	if (e.prefix_type != subevent_prefix_type::unknown && e.suffix_type != subevent_suffix_type::unknown) {
		parse_parameters(params, e.base, 1);

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
				e.prefix = environmental_parameters();
				parse_parameters(params, std::get<environmental_parameters>(*e.prefix), offset);
				offset += std::tuple_size_v<environmental_parameters>;
				break;
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
}

int main() {
	try {
		boost::iostreams::mapped_file_source file_source(
				R"(C:\Program Files (x86)\World of Warcraft\_retail_\Logs\WoWCombatLog.txt)");
		boost::iostreams::stream<boost::iostreams::mapped_file_source> stream(file_source, std::ios::binary);

		if (!stream.is_open()) {
			return 1;
		}

		std::cout << "Successfully opened memory mapped file\n"
		          << "\tSize: " << stream->size() / 1024 / 1024 << "MB\n"
		          << "\tAlignment: " << stream->alignment() / 1024 << " KB\n";

		auto start = std::chrono::system_clock::now();
		std::string line;
		uint64_t line_index = 0;

		while (std::getline(stream, line, '\r')) {
			line_index++;
			process_line(line);
			stream.seekg(1, std::ios::cur); // skip \n
		}

		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "parsing finished in " << duration.count() << "ms"
		          << " (" << line_index << " lines)\n";
	} catch (std::exception &e) {
		std::cerr << "[ERR] " << e.what() << std::endl;
	}

	return 0;
}
