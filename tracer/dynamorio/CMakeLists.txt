cmake_minimum_required(VERSION 3.7)
project(ChampSim_tracer_DR)

# Point this to your DynamoRIO cmake directory if not in your system PATH
find_package(DynamoRIO)

if (NOT DynamoRIO_FOUND)
  message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)

add_library(champsim_tracer SHARED champsim_tracer_dr.c)
configure_DynamoRIO_client(champsim_tracer)
use_DynamoRIO_extension(champsim_tracer drmgr)
use_DynamoRIO_extension(champsimtracer droption)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
target_compile_options(client PRIVATE -std=gnu99)

