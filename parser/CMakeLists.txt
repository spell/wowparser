cmake_minimum_required(VERSION 3.15)

find_package(Boost REQUIRED COMPONENTS iostreams program_options)

add_executable(parser main.cpp parameters.hpp event.hpp)

set_target_properties(parser PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES)

target_link_libraries(parser
        Boost::iostreams
        Boost::program_options)
