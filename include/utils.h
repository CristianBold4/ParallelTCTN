#ifndef TCTN_UTILS_H
#define TCTN_UTILS_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <vector>
#include <tuple>
#include <array>
#include <unordered_map>

#include <cmath>
#include <chrono>
#include <algorithm>

#include "ankerl/unordered_dense.h"
#include "Edge.h"
#include "EdgeSet.h"
#include "EdgeMap.h"
#include "DegreeMap.h"

const int n_motifs = 8;

#define counts std::array<double, n_motifs>
#define t_edges std::vector<Edge>
#define chrono_t std::chrono::duration<double>

t_edges get_temporal_edges(const std::string& dataset_path, std::string delimiter);

DegreeMap compute_temporal_degrees(const t_edges& edges, int delta);
DegreeMap compute_static_degrees(const t_edges& edges, int delta);

EdgeSet perfect_oracle(EdgeMap& edges_dictionary, float percentage_retain, bool corruption);
EdgeSet degree_oracle(const t_edges& edges,
                      DegreeMap& temporal_degrees,
                      DegreeMap& static_degrees,
                      float percentage_retain);

void print_progress_bar(int percentage);

void save_results(counts count_res, chrono_t time, double avg_memory, double max_memory, const std::string& file);
void save_multiple_results(const std::vector<counts>& count_res,
                           const std::vector<chrono_t>& times,
                           const std::vector<double>& avg_memory,
                           const std::vector<double>& max_memory,
                           const std::string& file);

void save_subgraph_size(const std::vector<int>& subgraph_sizes, const std::string& file);
void save_multiple_subgraph_size(const std::vector<std::vector<int>>& subgraph_sizes, const std::string& file);

void save_edge_map(const EdgeMap& edge_map, const std::string& file);
void save_oracle(EdgeSet& oracle, const std::string& file);
EdgeMap load_edge_map(const std::string& edge_map_file);
EdgeSet load_oracle(const std::string& oracle_file);

void save_preprocessed_edges(const std::vector<Edge>& edges, const std::string& file);
std::vector<Edge> load_preprocessed_edges(const std::string& preprocessed_dataset, std::string delimiter);

#endif //TCTN_UTILS_H
