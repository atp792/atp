#pragma once

#include <string>
#include <atomic>

struct external_sorter_t
{
   void sort_chunks(std::string const& input_filename, std::string const& output_filename);

private:
   std::atomic<int> active_tasks_count_ = 0;
};