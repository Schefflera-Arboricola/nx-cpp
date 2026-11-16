#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace nxcpp {

using Node = std::size_t;

/// An undirected, unweighted graph stored as an adjacency list.
/// Nodes are the integers 0..n-1.
class Graph {
public:
    explicit Graph(std::size_t num_nodes) : adj_(num_nodes) {}

    /// Adds the edge u-v. Throws std::out_of_range if a node doesn't exist.
    void add_edge(Node u, Node v);

    [[nodiscard]] std::size_t num_nodes() const noexcept { return adj_.size(); }
    [[nodiscard]] std::span<const Node> neighbors(Node u) const { return adj_.at(u); }

    /// BFS distances from `source`; std::nullopt for unreachable nodes.
    [[nodiscard]] std::vector<std::optional<std::size_t>> bfs_distances(Node source) const;

private:
    std::vector<std::vector<Node>> adj_;
};

/// degree(u) / (n - 1) for every node u.
[[nodiscard]] std::vector<double> degree_centrality(const Graph& g);

}  // namespace nxcpp
