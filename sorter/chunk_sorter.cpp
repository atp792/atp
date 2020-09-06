#include "chunk_sorter.h"

#include "record.h"
#include "external_chunk.h"
#include "chunk_reader.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <future>
#include <thread>
#include <cassert>
#include <iostream>

namespace
{
   // It is better to add dynamic calculation based on available memory, threads count and maybe disk r/w speed.
   static const size_t g_chunk_count = 32; 
   // One chunk no more then 100gb / g_chunk_count = 3 gb. So around 3 * 3 gb = 9gb at most would be used for chunks
   static const size_t g_max_active_chunks = 2; 
   static const size_t g_average_str_len = 512;
}

chunk_sorter_t::chunk_sorter_t(std::string const& input_filename, std::string const& output_filename)
   : fin_(input_filename, std::ios::binary | std::ios::in)
   , fout_(output_filename, std::ios::binary | std::ios::out)
{
   if (!fin_)
      throw std::runtime_error("Failed to open input file for reading");

   if (!fout_)
      throw std::runtime_error("Failed to open output file for writing");

   // set buffers to speedup input and output
   fin_buf_.resize(1024 * 1024);
   fin_.rdbuf()->pubsetbuf(&fin_buf_[0], fin_buf_.size());
   fout_buf_.resize(1024 * 1024);
   fout_.rdbuf()->pubsetbuf(&fout_buf_[0], fout_buf_.size());
}

std::vector<external_chunk_t> chunk_sorter_t::sort_chunks()
{
   active_tasks_count_ = 0;
   external_chunks_.clear();

   fin_.seekg(0, std::ios_base::end);
   std::size_t size = fin_.tellg();
   fin_.seekg(0, std::ios_base::beg);

   size_t cur_pos = 0;
   size_t chunk_size = size / g_chunk_count + 1;

   std::vector<std::future<void>> tasks;

   while (cur_pos < size)
   {
      while (active_tasks_count_ > g_max_active_chunks) {}

      size_t cur_chunk_size = std::min(chunk_size, size - cur_pos);
      auto chunk = read_chunk(fin_, cur_chunk_size);
      cur_pos += chunk.size();
      auto task = std::async(&chunk_sorter_t::parse_sort_and_write_chunk, this, std::move(chunk));
      tasks.push_back(std::move(task));
      active_tasks_count_ += 1;

      std::cout << "Sorting chunks progress " << 1. * cur_pos / size << " out of 1"<< std::endl;
   }

   for (auto & task : tasks)
      task.get();

   fout_.close();
   if (!fout_)
   {
      throw std::runtime_error("Failed to write sorted chunks");
   }

   return std::move(external_chunks_);
}

void chunk_sorter_t::parse_sort_and_write_chunk(std::vector<char> chunk)
{
   std::vector<record_t> records;
   records.reserve(chunk.size() / g_average_str_len); //trying to guess how much to reserve

   record_t record;

   size_t cur_idx = 0;
   while (cur_idx < chunk.size())
   {
      cur_idx += record.read_from_blob((char*)&chunk[cur_idx], chunk.size() - cur_idx);
      records.push_back(record);
   }

   std::sort(records.begin(), records.end());

   {
      std::lock_guard<std::mutex> guard(write_mutex_);

      size_t chunk_begin = fout_.tellp();
      for (auto const& record : records)
      {
         fout_ << record.number << ". " << record.str << "\r\n";
      }
      size_t chunk_end = fout_.tellp();
      external_chunks_.emplace_back(chunk_begin, chunk_end);
   }

   chunk.clear();

   active_tasks_count_ -= 1;
}