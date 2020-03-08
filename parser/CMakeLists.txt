cmake_minimum_required(VERSION 3.16)

add_executable(parser main.cpp reader.cpp reader.hpp parameters.hpp event.hpp)

set_target_properties(parser PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES)

target_compile_options(parser
        PRIVATE /std:c++latest)
