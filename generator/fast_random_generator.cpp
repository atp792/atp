#include "fast_random_generator.h"

#include <algorithm>

namespace
{
   static const char g_charset[] =
      " "
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
   static const size_t g_charset_size = (sizeof(g_charset) - 1);
}

unsigned long fast_random_generator_t::generate_number()
{
   unsigned long t;
   x_ ^= x_ << 16;
   x_ ^= x_ >> 5;
   x_ ^= x_ << 1;

   t = x_;
   x_ = y_;
   y_ = z_;
   z_ = t ^ x_ ^ y_;

   return z_;
}

unsigned long fast_random_generator_t::generate_number(unsigned long lo, unsigned long hi)
{
   return lo + generate_number() % (hi - lo);
}

float fast_random_generator_t::generate_float_number(float lo, float hi)
{
   return lo + (hi - lo) * generate_number() / ULONG_MAX;
}

std::string fast_random_generator_t::generate_string(unsigned long lo_size, unsigned long hi_size)
{
   unsigned long size = generate_number(lo_size, hi_size);

   auto randchar = [this]() -> char
   {
      return g_charset[generate_number() % g_charset_size];
   };
   std::string str(size, 0);
   std::generate_n(str.begin(), size, randchar);
   return str;
}