#pragma once

#include <vector>
#include <fstream>

std::vector<char> read_chunk(std::ifstream & f, size_t cur_chunk_size);