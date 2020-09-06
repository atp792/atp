cmake_minimum_required (VERSION 3.6)

project (altium_test_project)	

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory (generator)
add_subdirectory (sorter)
