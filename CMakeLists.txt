cmake_minimum_required(VERSION 3.16)

find_package(unordered_dense CONFIG REQUIRED)

project(ParallelTCTN)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -march=native")
# set(CMAKE_CXX_FLAGS_DEBUG "-g")


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

add_executable(BuildExact src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
        src/Utils.cpp
        include/Utils.h
        src/ParallelTCTN.cpp
        include/ParallelTCTN.h
)

target_include_directories(ParallelTCTN PRIVATE include)
target_include_directories(BuildExact PRIVATE include)

target_link_libraries(ParallelTCTN unordered_dense::unordered_dense)
target_link_libraries(BuildExact unordered_dense::unordered_dense)

