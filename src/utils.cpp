#include "../include/utils.h"

t_edges get_temporal_edges(const std::string& dataset_path, std::string delimiter) {

    std::ifstream file(dataset_path);
    std::string line;
    std::vector<Edge> temporal_edges;
    EdgeSet edge_set;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter[0]))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        int time = std::stoi(string_edge.at(2));

        if (src == dst) //avoid self-loops
            continue;

        Edge e(src, dst, time);

        if(edge_set.contains(e))
            continue;

        temporal_edges.push_back(e);
        edge_set.add_edge(e);
    }

    std::sort(temporal_edges.begin(), temporal_edges.end());
    return temporal_edges;
}

DegreeMap compute_temporal_degrees(const t_edges& edges, int delta) {

    DegreeMap dm;
    ankerl::unordered_dense::map<int, std::vector<int>> map;

    for(auto edge : edges) {
        int u = edge.get_src();
        int v = edge.get_dst();
        int t = edge.get_time();
        map[u].emplace_back(t);
        map[v].emplace_back(t);
        dm.add_pair(u, t);
        dm.add_pair(v, t);
    }

    for(const auto& p : map) {
        int node = p.first;
        std::vector<int> timestamps = p.second;
        for(auto t : timestamps) {
            for(auto t_i : timestamps) {
                if(t_i >= t - delta && t_i <= t + delta)
                    dm.increment_value(node, t_i);
            }
        }
    }

    return dm;
}

DegreeMap compute_static_degrees(const t_edges& edges, int delta) {

    DegreeMap dm;
    ankerl::unordered_dense::map<int, std::vector<std::pair<int, int>>> map;

    for(auto edge : edges) {
        int u = edge.get_src();
        int v = edge.get_dst();
        int t = edge.get_time();
        map[u].emplace_back(v, t);
        map[v].emplace_back(u, t);
        dm.add_pair(u, t);
        dm.add_pair(v, t);
    }

    for(const auto& p : map) {
        auto list = p.second;
        for(const auto &i : list) {
            for(const auto &j : list) {
                if(j.second >= i.second - delta && j.second <= i.second + delta && j.first != i.first)
                    dm.increment_value(p.first, i.second);
            }
        }
    }

    return dm;
}

EdgeSet perfect_oracle(EdgeMap& edge_map, int threshold, bool corruption) {

    EdgeSet oracle;
    std::vector<Edge> heavy_edges = edge_map.get_heavy_edges(threshold);

    std::size_t n_heavy = heavy_edges.size();

    if(!corruption) {
        for(const Edge& e: heavy_edges)
            oracle.add_edge(e);
        return oracle;
    }

    for(const Edge& e: heavy_edges) {

        double ratio = 1.0 / (1.0 + std::exp(-(edge_map.get_heaviness(e) - threshold)));
        double rnd = ((double)rand()) / RAND_MAX;

        if(ratio > rnd)
            oracle.add_edge(e);
    }

    return oracle;
}

EdgeSet perfect_oracle(EdgeMap& edge_map, float percentage_retain, bool corruption) {

    EdgeSet oracle;
    auto [sorted_heavy_edges, min_heaviness] = edge_map.get_heavy_edges(percentage_retain);

    if(!corruption) {
        for(const Edge& e: sorted_heavy_edges)
            oracle.add_edge(e);
        return oracle;
    }

    for(const Edge& e: sorted_heavy_edges) {

        double ratio = 1.0 / (1.0 + std::exp(-((float)edge_map.get_heaviness(e) - min_heaviness)));
        double rnd = ((double)rand()) / RAND_MAX;

        if(ratio > rnd)
            oracle.add_edge(e);
    }

    return oracle;
}

EdgeSet degree_oracle(const t_edges& edges,
                      DegreeMap& temporal_degrees,
                      DegreeMap& static_degrees,
                      float percentage_retain) {

    EdgeSet oracle;
    EdgeMap edge_map;

    for(const auto& e: edges) {

        int temp_deg_u = temporal_degrees.get_value(e.get_src(), e.get_time());
        int temp_deg_v = temporal_degrees.get_value(e.get_dst(), e.get_time());
        int stat_deg_u = static_degrees.get_value(e.get_src(), e.get_time());
        int stat_deg_v = static_degrees.get_value(e.get_dst(), e.get_time());

        int temporal_tr_estimate = std::min(temp_deg_u, temp_deg_v);
        int static_tr_estimate = std::min(stat_deg_u, stat_deg_v);

        float alpha = 1.0;

        int tr_estimate = (int)(alpha * (float)temporal_tr_estimate) + (int)((1.0 - alpha) * (float)static_tr_estimate);

        edge_map.add_edge(e);
        edge_map.increment_value(e, tr_estimate);
    }

    auto [sorted_heavy_edges, min_heaviness] = edge_map.get_heavy_edges(percentage_retain);

    for(const Edge& e: sorted_heavy_edges)
        oracle.add_edge(e);

    return oracle;
}

void save_subgraph_size(const std::vector<int>& subgraph_sizes, const std::string& file) {

    std::ofstream of(file, std::ios_base::app);

    for(int elem : subgraph_sizes)
        of << elem << "\n";

    of << "\n";

    of.close();
}

void save_multiple_subgraph_size(const std::vector<std::vector<int>>& subgraph_sizes, const std::string& file) {

    std::size_t trials = subgraph_sizes.size();

    for(int i = 0; i < trials; i++)
        save_subgraph_size(subgraph_sizes[i], file);

    std::ofstream of(file, std::ios_base::app);

    of << "\n";
    of.close();
}

void save_edge_map(const EdgeMap& edge_map, const std::string& file) {

    std::ofstream of(file);
    std::string delimiter = " ";

    for(auto elem : edge_map.get_map())
        of << elem.first << delimiter << elem.second << "\n";

    of.close();
}

void save_oracle(EdgeSet& oracle, const std::string& file) {

    std::ofstream of(file);
    std::string delimiter = " ";

    for(auto elem : oracle.get_set())
        of << elem << "\n";

    of.close();
}

EdgeSet load_oracle(const std::string& oracle_file) {

    std::ifstream file(oracle_file);
    std::string line;
    std::string delimiter = " ";
    EdgeSet oracle;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter[0]))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        int time = std::stoi(string_edge.at(2));

        Edge e(src, dst, time);
        oracle.add_edge(e);
    }

    file.close();

    return oracle;
}

EdgeMap load_edge_map(const std::string& edge_map_file) {

    std::ifstream file(edge_map_file);
    std::string line;
    std::string delimiter = " ";
    EdgeMap edge_map;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter[0]))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        int time = std::stoi(string_edge.at(2));
        int heaviness = std::stoi(string_edge.at(3));

        Edge e(src, dst, time);
        edge_map.add_edge(e);
        edge_map.increment_value(e, heaviness);
    }

    file.close();

    return edge_map;
}

void save_results(counts count_res, chrono_t time, double avg_memory, double max_memory, const std::string& file) {

    std::ofstream of(file, std::ios_base::app);
    std::string delimiter = " ";
    for(int i = 0; i < n_motifs; i++)
        of << count_res[i] << delimiter;
    of << time.count() << delimiter;
    of << avg_memory << delimiter;
    of << max_memory << "\n";
    of.close();
}

void save_multiple_results(const std::vector<counts>& count_res,
                           const std::vector<chrono_t>& times,
                           const std::vector<double>& avg_memory,
                           const std::vector<double>& max_memory,
                           const std::string& file) {

    std::size_t trials = count_res.size();
    for(int i = 0; i < trials; i++)
        save_results(count_res[i], times[i], avg_memory[i], max_memory[i], file);
    std::ofstream of(file, std::ios_base::app);
    of << "\n";
    of.close();
}

void save_preprocessed_edges(const std::vector<Edge>& edges, const std::string& file) {

    std::ofstream of(file);
    std::string delimiter = " ";

    for(Edge e : edges)
        of << e.get_src() << delimiter << e.get_dst() << delimiter << e.get_time() << "\n";

    of.close();
}

std::vector<Edge> load_preprocessed_edges(const std::string& preprocessed_dataset, std::string delimiter) {

    std::vector<Edge> temporal_edges;
    std::ifstream file(preprocessed_dataset);
    std::string line;

    while(std::getline(file, line)) {

        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> string_edge;

        while (std::getline(ss, token, delimiter[0]))
            string_edge.push_back(token);

        int src = std::stoi(string_edge.at(0));
        int dst = std::stoi(string_edge.at(1));
        int time = std::stoi(string_edge.at(2));

        Edge e(src, dst, time);
        temporal_edges.push_back(e);

    }

    return temporal_edges;
}

void print_progress_bar(int percentage) {

    const int width = 50;
    int fill = percentage * width / 100;

    std::cout << "[";

    for (int i = 0; i < fill; i++)
        std::cout << "=";

    for (int i = fill; i < width; i++)
        std::cout << " ";

    std::cout << "] " << std::setw(3) << percentage + 1 << "%\n";
    std::cout.flush();
}