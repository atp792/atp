#include "chunk_reader.h"

namespace
{
   static const size_t g_max_record_length = 1050;
}

std::vector<char> read_chunk(std::ifstream & f, size_t chunk_size)
{
   std::vector<char> v;
   // Create a vector to store the data + try to guess how much elements we need to read before line end
   v.reserve(chunk_size + g_max_record_length); //1050 - max string length + several more bytes for max int length
   v.resize(chunk_size);
   // Load the data
   f.read((char*)&v[0], chunk_size);

   // Read till next line end
   while (v.back() != '\n')
   {
      v.push_back(' ');
      f.read(&v.back(), 1);

      if (f.eof())
      {
         throw std::runtime_error("Failed to read chunk");
      }
   }
   return v;
}