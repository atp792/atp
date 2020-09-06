#pragma once

#include "external_chunk.h"

#include <string>
#include <vector>
#include <fstream>
#include <mutex>

struct chunk_merger_t
{
   chunk_merger_t(std::string const& input_filename, std::string const& output_filename);
   void merge_chunks(std::vector<external_chunk_t> & chunks);

private:
   std::vector<external_chunk_t> external_chunks_;

   std::mutex read_mutex_;

   std::ifstream fin_;
   std::ofstream fout_;
   std::vector<char> fin_buf_;
   std::vector<char> fout_buf_;
};