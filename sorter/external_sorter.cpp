#include "external_sorter.h"
#include "timer.h"

#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <thread>

namespace
{
   static const size_t g_average_str_len = 512;
   static const size_t g_chunk_count = 32; //TODO: add dynamic calculation?
   static const size_t g_max_active_chunks = 5; //one chunk no more then 100gb / 32 = 3 gb. 

   std::vector<char> read_chunk(std::ifstream & f, size_t cur_chunk_size)
   {
      std::vector<char> v;
      // Create a vector to store the data + try to guess how much elements we need to read before line end
      v.reserve(cur_chunk_size + 1050); //1050 - max string length + several more bytes for max int length
      v.resize(cur_chunk_size);
      // Load the data
      f.read((char*)&v[0], cur_chunk_size);

      // Read till next line end
      while (v.back() != '\n')
      {
         v.push_back(' ');
         f.read(&v.back(), 1);

         if (f.eof())
         {
            //TODO: tell that error happend
            assert(false);
            return v;
         }
      }
      return v;
   }
}

void external_sorter_t::sort_chunks(std::string const& input_filename, std::string const& output_filename)
{
   timer_t read_timer("sort_chunks");

   std::ifstream fin(input_filename, std::ios::binary | std::ios::in);
   if (!fin)
      throw std::runtime_error("Failed to open input file for reading");

   std::ofstream fout("C:\\work\\altium_test\\intermediate_output.txt", std::ios::binary | std::ios::out);
   if (!fout)
      throw std::runtime_error("Failed to open output file for writing");

   std::vector<char> fin_buf(1024 * 1024);
   fin.rdbuf()->pubsetbuf(&fin_buf[0], fin_buf.size());

   std::vector<char> fout_buf(1024 * 1024);
   fout.rdbuf()->pubsetbuf(&fout_buf[0], fout_buf.size());

   fin.seekg(0, std::ios_base::end);
   std::size_t size = fin.tellg();
   fin.seekg(0, std::ios_base::beg);

   size_t cur_pos = 0;
   size_t chunk_size = size / g_chunk_count + 1;

   while (cur_pos < size)
   {
      while (active_tasks_count_ > g_max_active_chunks) {}

      size_t cur_chunk_size = std::min(chunk_size, size - cur_pos);
      auto v = read_chunk(fin, cur_chunk_size);
      cur_pos += v.size();
      std::async(parse_sort_and_write_chunk, std::move(v), std::ref(fout));
      active_tasks_count_ += 1;
   }

   while (active_tasks_count_ != 0) {} //TODO: add sleep?
   fout.close();
}

void external_sorter_t::parse_sort_and_write_chunk(std::vector<char> && chunk, std::ofstream & fout)
{
   std::vector<record_t> records;
   records.reserve(chunk.size() / g_average_str_len); //try to guess how much to reserve
   size_t cur_idx = 0;

   record_t record;

   {
      timer_t timer("parse");
      while (cur_idx < chunk.size())
      {
         cur_idx += record.read_from_blob((char*)&chunk[cur_idx]);
         records.push_back(record);
      }
   }

   {
      timer_t sort_timer("sort");
      std::sort(records.begin(), records.end());
   }

   {
      std::lock_guard<std::mutex> guard(g_write_mutex);
      timer_t sort_timer("write");
      external_chunk_t external_chunk;
      external_chunk.begin = fout.tellp();
      for (auto const& record : records)
      {
         fout << record.number << ". " << record.str << "\r\n";
      }
      external_chunk.end = fout.tellp();
      g_external_chunks.emplace_back(std::move(external_chunk));
   }

   chunk.clear();

   active_tasks_count_ -= 1;
}