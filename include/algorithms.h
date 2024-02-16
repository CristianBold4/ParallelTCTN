#ifndef TCTN_ALGORITHMS_H
#define TCTN_ALGORITHMS_H

#include <random>
#include <array>
#include "Subgraph.h"
#include "NodesMap.h"
#include "utils.h"

#define wedge std::pair<Edge, Edge>

std::tuple<counts, chrono_t, double, double, EdgeMap> exact_algorithm(const t_edges& temporal_edges, int delta);
std::tuple<counts, chrono_t, double, double> sampling_algorithm(const t_edges& temporal_edges, int delta, float p, std::default_random_engine rand_eng);
std::tuple<counts, chrono_t, double, double> oracle_algorithm(const t_edges& temporal_edges, int delta, float p, EdgeSet& oracle, std::default_random_engine rand_eng);
int check_triangles(const wedge &w, const Edge& e, int delta);
void count_triangles(const Subgraph &sg, NodesMap &nm, const Edge &e, int delta, counts &l);
void count_triangles_oracle(const Subgraph &sg, NodesMap &nm, const Edge &e, int delta, counts &l1, counts &l2, counts &l3);
std::vector<wedge> collect_wedges(const Subgraph &sg, const Edge &e, int delta);
#endif //TCTN_ALGORITHMS_H
