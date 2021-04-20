cmake_minimum_required(VERSION 3.10)
project(map_0417)

set(CMAKE_CXX_STANDARD 14)

add_executable(map_0417 main.cpp
        exceptions.hpp
        class-matrix.hpp
        class-bint.hpp
        class-integer.hpp
        utility.hpp
        map.hpp
        maptest.hpp
        )