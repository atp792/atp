#include "record.h"

#include <cstdlib>
#include <string>

int record_t::read_from_blob(char * blob, size_t blob_size)
{
   number = atoi(blob);

   int cur_blob_idx = 2; //at least one digit and one dot
   bool space_symbol_reached = false;
   while (blob[cur_blob_idx] != '\n')
   {
      if (!space_symbol_reached && blob[cur_blob_idx] == ' ')
      {
         space_symbol_reached = true;
         str = &blob[cur_blob_idx + 1];
      }
      cur_blob_idx += 1;

      if (cur_blob_idx == blob_size)
      {
         throw std::runtime_error("Failed to parse blob - lines should end with \\n");
      }
   }
   if (cur_blob_idx <= 4)
   {
      throw std::runtime_error("Failed to parse blob - not enough data");
   }
   blob[cur_blob_idx - 1] = '\0'; // change /cr to null

   return cur_blob_idx + 1;
}

bool record_t::operator < (record_t const& rhs) const
{
   auto const res = strcmp(str, rhs.str);
   return res == 0 ? number < rhs.number : res == -1;
}