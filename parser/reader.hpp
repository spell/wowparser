//
// Created by Jesse on 07/03/2020.
//

#pragma once

#include <fstream>
#include <vector>

/*!
 * reader implements a buffered file reader
 */
class reader {
	bool next_chunk();

protected:
	std::ifstream file;
	std::vector<char> buffer;
	size_t buffer_size;
	std::vector<char>::iterator buffer_position;
	uint64_t line_index{0};

public:
	/*!
	 * creates a new buffered file reader
	 * @param path        the path to the file to read
	 * @param buffer_size the size of the internal buffer
	 */
	explicit reader(const std::string &path, size_t buffer_size = 1024 * 1024);

	/*!
	 * reads a line from the file
	 * @return the line that was read
	 */
	std::string read_line();

	/*!
	 * gets the current line index in the log
	 * @return
	 */
	[[nodiscard]] uint64_t get_line_index() const {
		return line_index;
	}

	[[nodiscard]] bool eof() const {
		return file.eof();
	}
};
