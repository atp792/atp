#include "chunk_merger.h"

#include <queue>

chunk_merger_t::chunk_merger_t(std::string const& input_filename, std::string const& output_filename)
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

void chunk_merger_t::merge_chunks(std::vector<external_chunk_t> & chunks)
{
   auto cmp = [](external_chunk_ptr_t const& lhs, external_chunk_ptr_t const& rhs) {
      return rhs->top() < lhs->top();
   };
   //wiki says that tournament tree is better, but again lets keep it simple
   std::priority_queue<external_chunk_ptr_t, std::vector<external_chunk_ptr_t>, decltype(cmp)> q(cmp);
   for (auto & chunk : chunks)
   {
      chunk.set_file(fin_);
      q.push(&chunk);
   }

   while (!q.empty())
   {
      auto & chunk = *q.top();
      q.pop();

      auto const& record = chunk.top();
      fout_ << record.number << ". " << record.str << "\r\n";

      chunk.pop();
      if (!chunk.empty())
      {
         q.push(&chunk);
      }
   }

   fout_.close();
   if (!fout_)
      throw std::runtime_error("Failed to write merged chunks");
}