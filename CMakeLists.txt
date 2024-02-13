cmake_minimum_required(VERSION 3.16)

find_package(unordered_dense CONFIG REQUIRED)

project(ParallelTCTN)
project(BuildOracle)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_STANDARD 17)

add_executable(ParallelTCTN src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
        src/Utils.cpp
        include/Utils.h
        src/ParallelTCTN.cpp
        include/ParallelTCTN.h
)

add_executable(BuildOracle src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
        src/Utils.cpp
        include/Utils.h
)

target_include_directories(ParallelTCTN PRIVATE include)
target_include_directories(BuildOracle PRIVATE include)

target_link_libraries(ParallelTCTN unordered_dense::unordered_dense)
target_link_libraries(BuildOracle unordered_dense::unordered_dense)

