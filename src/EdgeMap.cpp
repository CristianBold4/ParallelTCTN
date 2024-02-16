#include "../include/EdgeMap.h"

std::vector<Edge> EdgeMap::get_heavy_edges(int threshold) {

    std::vector<Edge> heavy_edges;

    for(const auto& elem : edge_map)
        if(elem.second > threshold)
            heavy_edges.emplace_back(elem.first);

    return heavy_edges;
}

bool compare_heavy(std::pair<Edge, int> p1, std::pair<Edge, int> p2) { return p1.second > p2.second; } //descending order

std::tuple<std::vector<Edge>, float> EdgeMap::get_heavy_edges(float percentage_retain) {

    std::vector<std::pair<Edge, int>> tmp;
    tmp.reserve(edge_map.size());
    for(auto item : edge_map)
        tmp.emplace_back(item);

    std::sort(tmp.begin(), tmp.end(), compare_heavy);

    std::vector<Edge> sorted_heavy_edges;
    int min_heaviness = INT32_MAX;

    for(auto item : tmp)
        if(sorted_heavy_edges.size() < (int)(percentage_retain * (float)tmp.size())) {
            sorted_heavy_edges.emplace_back(item.first);
            if(min_heaviness > item.second)
                min_heaviness = item.second;
        }

    return {sorted_heavy_edges, min_heaviness};
}

EdgeMap EdgeMap::get_heavy_map(float percentage_retain) {

    std::vector<std::pair<Edge, int>> tmp;
    tmp.reserve(edge_map.size());
    for(auto item : edge_map)
        tmp.emplace_back(item);

    std::sort(tmp.begin(), tmp.end(), compare_heavy);

    EdgeMap sorted_heavy_map;

    for(auto item : tmp)
        if(sorted_heavy_map.size() < (int)(percentage_retain * (float)tmp.size())) {
            sorted_heavy_map.add_edge(item.first);
            sorted_heavy_map.increment_value(item.first, item.second);
        }

    return sorted_heavy_map;
}
