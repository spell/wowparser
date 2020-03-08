//
// Created by Jesse on 07/03/2020.
//

#include <iostream>
#include "reader.hpp"

reader::reader(const std::string &path, const size_t buffer_size)
		: file(path, std::ifstream::binary), buffer(buffer_size), buffer_size(buffer_size) {
	buffer_position = buffer.end();
}

std::string reader::read_line() {
	std::string line;

	while (true) {
		if (buffer_position == buffer.end() && !next_chunk()) {
			return line;
		}

		auto it = std::find(buffer_position, buffer.end(), '\r');
		line += std::string(buffer_position, it);
		buffer_position = it;

		if (buffer_position == buffer.end()) {
			continue;
		}

		if (buffer_position + 1 == buffer.end()) {
			file.seekg(1, std::ifstream::cur);
			next_chunk();
		} else {
			buffer_position = it + 2; // to skip \r\n
		}

		line_index++;
		return line;
	}
}

bool reader::next_chunk() {
	if (file.eof()) {
		return false;
	}

	buffer = std::vector<char>(buffer_size);
	file.read(buffer.data(), buffer_size);
	buffer_position = buffer.begin();
	return true;
}
