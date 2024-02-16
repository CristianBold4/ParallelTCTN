cmake_minimum_required(VERSION 3.16)

find_package(unordered_dense CONFIG REQUIRED)

project(TCTN)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3")

add_library(utils include/utils.h src/utils.cpp)
add_library(algorithms include/algorithms.h src/algorithms.cpp)
add_library(subgraph include/Subgraph.h src/Subgraph.cpp)
add_library(edge_map include/EdgeMap.h src/EdgeMap.cpp)
add_library(edge include/Edge.h src/Edge.cpp)
add_library(nodes_map include/NodesMap.h src/NodesMap.cpp)

add_executable(TCTN src/main.cpp)
add_executable(build_oracle src/build_oracle.cpp)

target_link_libraries(edge_map unordered_dense::unordered_dense)
target_link_libraries(subgraph unordered_dense::unordered_dense)
target_link_libraries(nodes_map unordered_dense::unordered_dense)
target_link_libraries(utils edge edge_map)
target_link_libraries(algorithms utils subgraph nodes_map)
target_link_libraries(TCTN algorithms)
target_link_libraries(build_oracle algorithms)
