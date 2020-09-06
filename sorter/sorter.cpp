#include <iostream>
#include <vector>

#include "timer.h"
#include "chunk_sorter.h"
#include "chunk_merger.h"

int main(int argc, char *argv[])
{
   if (argc != 3 && argc != 4) {
      std::cerr << "Usage: " << argv[0] << " input_filename output_filename [intermediate_filename]" << std::endl;
      return 1;
   }

   std::string const input_filename = argv[1];
   std::string const output_filename = argv[2];
   std::string const intermediate_filename = argc == 4 ? argv[3] : "intermediate_output.txt";

   timer_t all_timer("all");

   std::vector<external_chunk_t> external_chunks;

   try
   {
      timer_t sort_timer("sort_chunks");
      std::cout << "Starting to sort chunks" << std::endl;
      chunk_sorter_t sorter(input_filename, intermediate_filename);
      external_chunks = sorter.sort_chunks();
   }
   catch (std::runtime_error const& e)
   {
      std::cerr << "Failed to sort chunks: " << e.what() << std::endl;
      return 1;
   }

   try
   {
      timer_t sort_timer("merge_chunks");
      std::cout << "Starting to merge sorted chunks" << std::endl;
      chunk_merger_t merger(intermediate_filename, output_filename);
      merger.merge_chunks(external_chunks);
   }
   catch (std::runtime_error const& e)
   {
      std::cerr << "Failed to sort chunks: " << e.what() << std::endl;
      return 1;
   }


   return 0;
}
