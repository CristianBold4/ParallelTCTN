#include "GraphStream.h"

GraphStream::GraphStream(const std::string &filename, const char &delimiter, int skip) {

    graph_stream_.open(filename.c_str(), std::ios_base::in);
    if (!graph_stream_.is_open()) {
        std::cerr << "Error! Graph Stream file not opened.\n";
        return;
    }
    delimiter_ = delimiter;
    skip_ = skip;
    read_chunk();

}

GraphStream::~GraphStream() {

    this->graph_stream_.close();

}

void GraphStream::read_chunk() {

    std::string line;
    EdgeStream edge;
    int u, v, time;
    long nline = 0;

    // -- read chunk
    while(nline++ < CHUNK_SIZE && std::getline(graph_stream_, line)) {

        std::stringstream ss(line);

        // -- read tokens
        ss >> u >> v >> time;

        // -- skip lines
        if (nline < skip_) continue;
        // -- check for self loops
        if (u == v) continue;

        edge.u = u;
        edge.v = v;
        edge.time = time;
        chunk_queue_.push(edge);

    }

}

EdgeStream GraphStream::next() {

    if (chunk_queue_.size() <= 1) {
        read_chunk();
    }

    EdgeStream edge =  chunk_queue_.front();
    chunk_queue_.pop();

    return edge;

}