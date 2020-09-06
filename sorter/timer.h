#pragma once

#include <string>
#include <chrono>

struct timer_t
{
   timer_t(std::string const& name);
   ~timer_t();

private:
   std::string name_;
   std::chrono::time_point<std::chrono::steady_clock> start_;
};