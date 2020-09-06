#pragma once

struct record_t
{
   int number{ 0 };
   char * str{ nullptr };

   int read_from_blob(char * blob, size_t blob_size);
   bool operator < (record_t const& rhs) const;
};