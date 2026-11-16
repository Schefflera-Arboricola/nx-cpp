#include "nxcpp/executor.hpp"

#include <algorithm>
#include <thread>

namespace nxcpp {

double Executor::closeness_of(const Graph& g, Node u) {
    std::size_t reachable = 0;  // nodes reachable from u, other than u
    std::size_t total_distance = 0;
    for (const auto& d : g.bfs_distances(u)) {
        if (d && *d > 0) {
            ++reachable;
            total_distance += *d;
        }
    }
    if (total_distance == 0) return 0.0;

    const auto r = static_cast<double>(reachable);
    const auto n = static_cast<double>(g.num_nodes());
    // (r / total_distance) is the closeness within u's component,
    // and (r / (n - 1)) scales it down by the size of that component.
    return (r / static_cast<double>(total_distance)) * (r / (n - 1));
}

std::vector<double> SerialExecutor::closeness_centrality(const Graph& g) const {
    std::vector<double> result(g.num_nodes());
    for (Node u = 0; u < g.num_nodes(); ++u) result[u] = closeness_of(g, u);
    return result;
}

ParallelExecutor::ParallelExecutor(unsigned n_jobs)
    : n_jobs_(n_jobs != 0 ? n_jobs : std::max(1u, std::thread::hardware_concurrency())) {}

std::vector<double> ParallelExecutor::closeness_centrality(const Graph& g) const {
    const std::size_t n = g.num_nodes();
    std::vector<double> result(n);
    const std::size_t chunk_size = (n + n_jobs_ - 1) / n_jobs_;  // ceil(n / n_jobs)

    // Each thread only writes to its own slice of `result` and only reads
    // from `g`, so no locks are needed.
    std::vector<std::thread> workers;
    for (std::size_t start = 0; start < n; start += chunk_size) {
        const std::size_t stop = std::min(start + chunk_size, n);
        workers.emplace_back([&g, &result, start, stop] {
            for (Node u = start; u < stop; ++u) result[u] = closeness_of(g, u);
        });
    }
    for (auto& worker : workers) worker.join();
    return result;
}

std::unique_ptr<Executor> make_executor(unsigned n_jobs) {
    if (n_jobs == 1) return std::make_unique<SerialExecutor>();
    return std::make_unique<ParallelExecutor>(n_jobs);
}

}  // namespace nxcpp
