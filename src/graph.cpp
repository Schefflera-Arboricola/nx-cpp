#include "nxcpp/graph.hpp"

#include <queue>

namespace nxcpp {

void Graph::add_edge(Node u, Node v) {
    adj_.at(u).push_back(v);
    adj_.at(v).push_back(u);  // a self-loop counts twice in the degree, like in NetworkX
}

std::vector<std::optional<std::size_t>> Graph::bfs_distances(Node source) const {
    std::vector<std::optional<std::size_t>> dist(num_nodes());
    dist.at(source) = 0;

    std::queue<Node> to_visit;
    to_visit.push(source);
    while (!to_visit.empty()) {
        const Node u = to_visit.front();
        to_visit.pop();
        for (const Node v : adj_[u]) {
            if (!dist[v]) {
                dist[v] = *dist[u] + 1;
                to_visit.push(v);
            }
        }
    }
    return dist;
}

std::vector<double> degree_centrality(const Graph& g) {
    const std::size_t n = g.num_nodes();
    if (n < 2) return std::vector<double>(n, 1.0);  // same as NetworkX

    std::vector<double> result(n);
    for (Node u = 0; u < n; ++u) {
        result[u] = static_cast<double>(g.neighbors(u).size()) / static_cast<double>(n - 1);
    }
    return result;
}

}  // namespace nxcpp
