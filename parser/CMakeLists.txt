cmake_minimum_required(VERSION 3.16)

find_package(Boost 1.72 REQUIRED COMPONENTS iostreams)

add_executable(parser main.cpp parameters.hpp event.hpp)

set_target_properties(parser PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES)

target_link_libraries(parser
        Boost::iostreams)
