#pragma once

#include <string>

struct fast_random_generator_t
{
   unsigned long generate_number();
   unsigned long generate_number(unsigned long lo, unsigned long hi);
   float generate_float_number(float lo, float hi);

   std::string generate_string(unsigned long lo_size, unsigned long hi_size);

private:
   unsigned long x_{ 123456789 };
   unsigned long y_{ 362436069 };
   unsigned long z_{ 521288629 };
};

