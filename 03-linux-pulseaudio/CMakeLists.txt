cmake_minimum_required(VERSION 3.10)
project(PulsePlayer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(PulsePlayer main.cpp)

target_link_libraries(PulsePlayer "-lpulse-simple -lpulse")

