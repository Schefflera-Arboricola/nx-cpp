#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "nxcpp/graph.hpp"

namespace nxcpp {

/// Runs the algorithms that are "one independent computation per node".
/// Subclasses decide *how* the per-node work is run.
class Executor {
public:
    virtual ~Executor() = default;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    /// Closeness centrality with the Wasserman-Faust scaling, so that it
    /// also works on disconnected graphs (NetworkX's default).
    [[nodiscard]] virtual std::vector<double> closeness_centrality(const Graph& g) const = 0;

protected:
    /// Closeness of a single node. Shared by all executors.
    [[nodiscard]] static double closeness_of(const Graph& g, Node u);
};

/// A plain loop over all the nodes.
class SerialExecutor final : public Executor {
public:
    [[nodiscard]] std::string_view name() const noexcept override { return "serial"; }
    [[nodiscard]] std::vector<double> closeness_centrality(const Graph& g) const override;
};

/// Splits the nodes into chunks and runs one std::thread per chunk
/// (the same idea as node chunking in nx-parallel).
class ParallelExecutor final : public Executor {
public:
    /// n_jobs = 0 means "use all the available cores".
    explicit ParallelExecutor(unsigned n_jobs = 0);

    [[nodiscard]] std::string_view name() const noexcept override { return "parallel"; }
    [[nodiscard]] std::vector<double> closeness_centrality(const Graph& g) const override;

private:
    unsigned n_jobs_;
};

/// SerialExecutor if n_jobs == 1, otherwise a ParallelExecutor.
[[nodiscard]] std::unique_ptr<Executor> make_executor(unsigned n_jobs);

}  // namespace nxcpp
