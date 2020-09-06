#pragma once

#include "external_chunk.h"

#include <string>
#include <vector>
#include <atomic>
#include <fstream>
#include <mutex>

struct chunk_sorter_t
{
   chunk_sorter_t(std::string const& input_filename, std::string const& output_filename);
   std::vector<external_chunk_t> sort_chunks();

private:
   void parse_sort_and_write_chunk(std::vector<char> chunk);

private:
   std::vector<external_chunk_t> external_chunks_;

   std::atomic<int> active_tasks_count_{ 0 };
   std::mutex write_mutex_;

   std::ifstream fin_;
   std::ofstream fout_;
   std::vector<char> fin_buf_;
   std::vector<char> fout_buf_;
};