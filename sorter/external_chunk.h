#pragma once

#include "record.h"

#include <vector>
#include <deque>
#include <future>
#include <fstream>

struct external_chunk_t
{
   external_chunk_t(size_t begin, size_t end)
      : begin_(begin)
      , end_(end)
   {}

   void set_file(std::ifstream & fin);

   record_t const& top() const;

   void pop();
   bool empty() const;

private:
   void read_next_data_part();

private:
   std::ifstream * fin_{ nullptr };
   size_t begin_{ 0 };
   size_t end_{ 0 };

   std::deque<record_t> records[2];
   std::vector<char> data[2];

   mutable size_t active_idx{ 0 };
   mutable std::future<void> read_future;
};

typedef external_chunk_t * external_chunk_ptr_t;