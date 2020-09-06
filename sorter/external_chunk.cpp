#include "external_chunk.h"
#include "chunk_reader.h"

#include <algorithm>
#include <fstream>

namespace {
   static std::mutex g_read_mutex;
   static const size_t g_external_chunk_data_part_size = 1024 * 1024 * 10;
}

void external_chunk_t::set_file(std::ifstream & fin)
{
   fin_ = &fin;
   read_next_data_part();
}

void external_chunk_t::read_next_data_part()
{
   read_future = std::async([this]() {
      auto next_idx = (active_idx + 1) % 2;
      std::lock_guard<std::mutex> guard(g_read_mutex);
      fin_->seekg(begin_);
      size_t next_chunk_size = std::min(g_external_chunk_data_part_size, end_ - begin_);
      data[next_idx] = read_chunk(*fin_, next_chunk_size);
      begin_ += data[next_idx].size();

      size_t cur_idx = 0;
      record_t record;
      while (cur_idx < data[next_idx].size())
      {
         cur_idx += record.read_from_blob((char*)&data[next_idx][cur_idx], end_ - cur_idx);
         records[next_idx].push_back(record);
      }
   });
}

record_t const& external_chunk_t::top() const
{
   if (records[active_idx].empty())
   {
      read_future.get();
      read_future = std::future<void>();
      active_idx = (active_idx + 1) % 2;
   }
   return records[active_idx].front();
}

void external_chunk_t::pop()
{
   if (begin_ < end_ && !read_future.valid())
   {
      read_next_data_part();
   }
   records[active_idx].pop_front();
}

bool external_chunk_t::empty() const
{
   return records[active_idx].empty() && !read_future.valid();
}
