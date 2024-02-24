cmake_minimum_required(VERSION 3.16)


set(UNORDERED_DENSE_LIB "${CMAKE_CURRENT_SOURCE_DIR}/unordered_dense/build/unordered_dense_install")
list(APPEND CMAKE_PREFIX_PATH "${UNORDERED_DENSE_LIB}")

project(Tonic)
project(DataPreprocessing)
project(BuildOracle)

find_package(OpenMP REQUIRED)
find_package(unordered_dense CONFIG REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_STANDARD 17)

add_executable(Tonic src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
	src/WRPSampling.cpp
	include/WRPSampling.h
        src/Utils.cpp
        include/Utils.h
)

add_executable(DataPreprocessing src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
        src/WRPSampling.cpp
        include/WRPSampling.h
        src/Utils.cpp
        include/Utils.h
)

add_executable(BuildOracle src/main.cpp
        src/GraphStream.cpp
        include/GraphStream.h
        src/Subgraph.cpp
        include/Subgraph.h
        src/WRPSampling.cpp
        include/WRPSampling.h
        src/Utils.cpp
        include/Utils.h
)

target_include_directories(Tonic PRIVATE include)
target_include_directories(DataPreprocessing PRIVATE include)
target_include_directories(BuildOracle PRIVATE include)

target_link_libraries(Tonic unordered_dense::unordered_dense OpenMP::OpenMP_CXX)
target_link_libraries(DataPreprocessing unordered_dense::unordered_dense OpenMP::OpenMP_CXX)
target_link_libraries(BuildOracle unordered_dense::unordered_dense OpenMP::OpenMP_CXX)
