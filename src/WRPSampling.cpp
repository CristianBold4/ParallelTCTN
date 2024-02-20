#include "WRPSampling.h"
#include "FixedSizePQ.h"
#include <algorithm>
#include <random>
#include <iomanip>
#include <iostream>
#include <cassert>
#include <sstream>

WRPSampling::WRPSampling(long k, int random_seed, double alpha, double beta,
                         const ankerl::unordered_dense::map<long, int> &oracle) : k_(k), alpha_(alpha),
                                                                                             beta_(beta),
                                                                                             WR_size_((long) (k *
                                                                                                              alpha)),
                                                                                             H_size_((long) (
                                                                                                     (k - WR_size_) *
                                                                                                     beta)),
                                                                                             SL_size_(k - WR_size_ -
                                                                                                      H_size_),
                                                                                             oracle_(oracle) {

    std::cout << "Starting TONIC Algo with alpha =  " << alpha << ", beta = " << beta << ", Memory Budget = " << k
              << "...\n";

    subgraph_ = Subgraph(k);
    waiting_room_ = new Edge[WR_size_];
    heavy_edges_pq_ = FixedSizePQ<EdgeHeaviness, LightnessComparator>(H_size_);
    light_edges_sample_ = new Edge[SL_size_];
    set_light_edges_ids_.reserve(SL_size_);
    set_edges_ids_.reserve(k);
    e2 = std::mt19937(random_seed);

}

double WRPSampling::get_global_triangles() const {
    return global_triangles_cnt_;
}

double WRPSampling::get_local_triangles(int node) const {
    return local_triangles_cnt_.at(node);
}

void WRPSampling::get_local_triangles_map(ankerl::unordered_dense::map<int, double> &local_triangles) const {
    local_triangles.clear();
    for (auto node_count : local_triangles_cnt_){
        local_triangles.emplace(node_count);
    }
}

// -- perform hashing of unordered edge in a graph
unsigned long long WRPSampling::edge_to_id(const int u, const int v) {

    int nu = (u < v ? u : v);
    int nv = (u < v ? v : u);
    unsigned long long id = static_cast<unsigned long long>(MAX_N_NODES)
                            * static_cast<unsigned long long>(nu) + static_cast<unsigned long long>(nv);
    return id;

}


bool WRPSampling::add_edge_subgraph(const int u, const int v, char label) {

    set_edges_ids_.insert(edge_to_id(u, v));
    if (label == 'L') {
        set_light_edges_ids_.insert(edge_to_id(u, v));
    }
    return subgraph_.add_edge(u, v, label);
}

bool WRPSampling::remove_edge_subgraph(const int u, const int v) {

    set_edges_ids_.erase(edge_to_id(u, v));
    return subgraph_.remove_edge(u, v);
}


void WRPSampling::count_triangles(const int source, const int dest) {

    // -- retrieve the node with min degree in which iterate the common neighs
    int du = subgraph_.get_degree_node(source);
    int dv = subgraph_.get_degree_node(dest);
    // -- u is the minimum degree node
    int u = (du <= dv ? source : dest);
    int v = (du <= dv ? dest : source);

    std::vector<std::pair<int, bool>> min_neighs;
    subgraph_.return_neighbors(u, min_neighs);

    double cumulative_count = 0.0;

    for (const auto &u_neighs: min_neighs) {
        int w = u_neighs.first;

        double increment_T = 1.0;
        bool vw_in_sample;

        if (w != v) {

            // -- common neighbors
            if (set_edges_ids_.find(edge_to_id(v, w)) != set_edges_ids_.end()) {
                // -- triangle {u, v, w} discovered
                vw_in_sample = (set_light_edges_ids_.find(edge_to_id(v, w)) != set_light_edges_ids_.end());

                if (!u_neighs.second and vw_in_sample) {
                    increment_T = std::max(1.0, ((double) (SL_cur_) / SL_size_) *
                                                ((double) ((SL_cur_ - 1.0))) / (SL_size_ - 1.0));
                } else if (!u_neighs.second or vw_in_sample) {
                    increment_T = std::max(1.0, ((double) (SL_cur_) / SL_size_));
                }

                cumulative_count += increment_T;
                // -- update local counter for node w
                //if (local_triangles_cnt_.contains(w)) {
                //    local_triangles_cnt_[w] += increment_T;
                //} else {
                //    local_triangles_cnt_[w] = increment_T;
                //}
            }

        }

    }

    // -- increment counters
    if (cumulative_count > 0) {
        global_triangles_cnt_ += cumulative_count;
        // -- update local counter for node u
        //if (local_triangles_cnt_.contains(u)) {
        //    local_triangles_cnt_[u] += cumulative_count;
        //} else {
        //    local_triangles_cnt_[u] = cumulative_count;
        //}
        // -- update local counter for node v
        //if (local_triangles_cnt_.contains(v)) {
        //    local_triangles_cnt_[v] += cumulative_count;
        //} else {
        //    local_triangles_cnt_[v] = cumulative_count;
        //}

    }

}


char WRPSampling::sample_edge(int u, int v) {

    char curr_label;
    Edge uv;
    // -- order the edge
    if (u < v)
        uv = std::make_pair(u, v);
    else
        uv = std::make_pair(v, u);

    Edge lightest_edge_H;
    int current_heaviness, lightest_heaviness_H;
    Edge uv_sample, xy, uv_replace;
    EdgeHeaviness lightest_pair_H;

    if (H_cur_ < H_size_) {
        // -- insert edge into H
        int heaviness = 0;
        auto oracle_it = oracle_.find(edge_to_id(uv.first, uv.second));
        if (oracle_it != oracle_.end()) {
            // -- edge predicted to be heavy
            heaviness = oracle_it->second;
        }

        curr_label = 'H';
        H_cur_++;
        heavy_edges_pq_.push(std::make_pair(uv, heaviness));
        return curr_label;

    } else {

        // -- H is full -> retrieve the lightest heavy edge between the current (uv) and the lightest in H
        if (SL_cur_ < SL_size_) {

            uv_sample = uv;
            curr_label = 'L';
            auto oracle_it = oracle_.find(edge_to_id(uv.first, uv.second));
            if (oracle_it != oracle_.end()) {
                lightest_pair_H = heavy_edges_pq_.top();
                lightest_edge_H = lightest_pair_H.first;
                lightest_heaviness_H = lightest_pair_H.second;
                current_heaviness = oracle_it->second;

                if (current_heaviness > lightest_heaviness_H or
                    (current_heaviness == lightest_heaviness_H and next_double() < 0.5)) {
                    // -- remove the smallest element
                    heavy_edges_pq_.pop();
                    // -- insert current into H
                    curr_label = 'H';
                    heavy_edges_pq_.push(std::make_pair(uv, current_heaviness));
                    // -- change the label in the subgraph and edge to sample
                    subgraph_.change_edge_label(lightest_edge_H.first, lightest_edge_H.second, 'L');
                    set_light_edges_ids_.insert(edge_to_id(lightest_edge_H.first, lightest_edge_H.second));
                    uv_sample = lightest_edge_H;

                }
            }

            // -- insert edge into SL
            light_edges_sample_[SL_cur_++] = uv_sample;
            return curr_label;

        } else if (WR_cur_ < WR_size_) {
            // -- insert edge into WR
            waiting_room_[WR_cur_++] = uv;
            curr_label = 'W';
            return curr_label;

        } else {

            curr_label = 'W';
            // -- all sets are full -> need to resort to sampling
            SL_cur_++;
            // -- pop the oldest edge from WR
            long index_oldest_edge = WR_cur_ % WR_size_;
            WR_cur_++;
            xy = waiting_room_[index_oldest_edge];
            // -- insert the current edge into WR
            waiting_room_[index_oldest_edge] = uv;
            // -- uv_sample stores the lightest edge between the popped and the lightest in H
            int u_sample = xy.first;
            int v_sample = xy.second;
            auto oracle_it = oracle_.find(edge_to_id(xy.first, xy.second));
            if (oracle_it != oracle_.end()) {
                // -- popped edge is predicted to be heavy
                // -- get the smallest elem from H
                lightest_pair_H = heavy_edges_pq_.top();
                lightest_edge_H = lightest_pair_H.first;
                lightest_heaviness_H = lightest_pair_H.second;
                current_heaviness = oracle_it->second;

                if (current_heaviness > lightest_heaviness_H or
                    (current_heaviness == lightest_heaviness_H and next_double() < 0.5)) {
                    // -- remove the smallest elem
                    heavy_edges_pq_.pop();
                    u_sample = lightest_edge_H.first;
                    v_sample = lightest_edge_H.second;
                    // -- insert the heavier edge into H
                    heavy_edges_pq_.push(std::make_pair(xy, current_heaviness));
                    // -- change the edge label in the subgraph
                    subgraph_.change_edge_label(xy.first, xy.second, 'H');

                }
            }

            // -- perform sampling in SL
            double p = SL_size_ / (double) SL_cur_;
            uv_sample = std::make_pair(u_sample, v_sample);
            double r = next_double();

            if (r < p) {
                // -- edge is sampled
                subgraph_.change_edge_label(u_sample, v_sample, 'L');
                set_light_edges_ids_.insert(edge_to_id(u_sample, v_sample));
                // -- select edge to be evicted
                int replace_idx = (int) (rand() % SL_size_);
                uv_replace = light_edges_sample_[replace_idx];

                // -- remove replaced edge from subgraph
                if (remove_edge_subgraph(uv_replace.first, uv_replace.second)) {
                    // -- insert the sampled edge
                    set_light_edges_ids_.erase(edge_to_id(uv_replace.first, uv_replace.second));
                    light_edges_sample_[replace_idx] = uv_sample;
                } else {
                    std::cerr << "Error in removing the uv_replace edge at time " << t_ << "\n";
                }

            } else {
                // -- edge is not resampled, just remove it from the subgraph
                if (!remove_edge_subgraph(u_sample, v_sample)) {
                    std::cerr << "Error in removing the uv_sample edge at  time " << t_ << "\n";
                }

            }

        }
    }

    return curr_label;

}


void WRPSampling::process_edge(int u, int v) {

    count_triangles(u, v);
    char label = sample_edge(u, v);

    // -- update subgraph
    if (!add_edge_subgraph(u, v, label)) {
        printf("Error in adding edge (%d, %d) with label %c at time %lld\n", u, v, label, t_);
    }

    //    assert(size_subgraph() <= k_);
    //    assert(H_cur_ <= H_size_);

    this->t_++;

}

// -- MT rg (as the one used in numpy/python)
double WRPSampling::next_double() {
    int a = e2() >> 5;
    int b = e2() >> 6;
    return (a * 67108864.0 + b) / 9007199254740992.0;
}

WRPSampling::~WRPSampling() {
    delete[] light_edges_sample_;
    delete[] waiting_room_;
    subgraph_.clear();
}
