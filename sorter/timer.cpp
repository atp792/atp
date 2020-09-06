#include "timer.h"

#include <iostream>

timer_t::timer_t(std::string const& name)
   : name_(name)
   , start_(std::chrono::high_resolution_clock::now())
{

}

timer_t::~timer_t()
{
   auto finish = std::chrono::high_resolution_clock::now();
   std::chrono::duration<double> elapsed = finish - start_;
   std::cout << name_ << " elapsed time: " << elapsed.count() << " s\n";
}