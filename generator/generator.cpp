#include <iostream>
#include <fstream>
#include <cassert>
#include <chrono>
#include <string>
#include <vector>

#include "fast_random_generator.h"

namespace
{
   //generator setting. Should be moved to program options or config file, but kept like this to keep test assignment small
   static const int g_max_number = INT_MAX;
   static const unsigned long g_min_string_size = 128;
   static const unsigned long g_max_string_size = 1024;
   static const float g_duplicate_ratio = 0.01f;
   static const float g_change_duplicate_ratio = 0.1f;

   static fast_random_generator_t g_random_gen;

   // Generates and writes data to output file.
   // File size will approximatly be equal to approximate_size_in_bytes.
   // Output data consists of lines with format "number. string\r\n" where:
   // number is in range 0, `g_max_number`
   // string size is in range `g_min_string_length`, `g_max_string_length`
   // Also around `g_duplicate_ratio` part of strings will be duplicates
   // Note: file should be passed opened + write errors are not checked it this function
   void generate_data(std::ofstream & fout, long long const approximate_size_in_bytes)
   {
      std::string duplicated_string;
      int record_count = 0;
      // tellp works a bit slow, so lets call it once in one hundred operations
      // This can be improved and this magic number should be removed, , but kept like this to keep test assignment small
      while (record_count % 100 != 0 || fout.tellp() < approximate_size_in_bytes)
      {
         record_count++;
         fout << g_random_gen.generate_number(0, INT_MAX) << ". ";

         bool const create_duplicate = !duplicated_string.empty() && g_random_gen.generate_float_number(0, 1) < g_duplicate_ratio;

         if (create_duplicate)
         {
            // Also this can be improved - now duplicates go rather near each other
            fout << duplicated_string << "\r\n";
         }
         else
         {
            auto const next_string = g_random_gen.generate_string(g_min_string_size, g_max_string_size);
            fout << next_string << "\r\n";

            if (g_random_gen.generate_float_number(0, 1) < g_change_duplicate_ratio)
            {
               duplicated_string = next_string;
            }
         }
      };
   }
}

int main(int argc, char *argv[])
{
   // It is better to use boost program options here or something similar
   if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " output_filename approximate_size_in_bytes" << std::endl;
      return 1;
   }

   auto const approximate_size_in_bytes = std::strtoll(argv[2], nullptr, 10);
   if (approximate_size_in_bytes == 0 || approximate_size_in_bytes == LLONG_MAX)
   {
      std::cerr << "size_in_bytes should be a positive number smaller than " << ULONG_MAX << std::endl;;
      return 1;
   }

   auto start_time = std::chrono::high_resolution_clock::now();

   // Also it is good to ask if user wants to rewrite file
   std::ofstream f(argv[1], std::ios::out | std::ios::binary);
   if (!f) {
      std::cerr << "Failed to open " << argv[1] << " for writing" << std::endl;
      return 1;
   }

   // Set big buffer for output file, this speedups output
   const size_t bufsize = 10 * 1024 * 1024;
   std::vector<char> buf(bufsize);
   f.rdbuf()->pubsetbuf(&buf[0], bufsize);

   generate_data(f, approximate_size_in_bytes);

   // close output file and check if any error has happend
   f.close();
   if (!f)
   {
      std::cerr << "Failed to write to output file " << argv[1] << std::endl;
      return 1;
   }

   auto finish_time = std::chrono::high_resolution_clock::now();
   std::cout << std::chrono::duration<double>(finish_time - start_time).count() << std::endl;

   return 0;
}
