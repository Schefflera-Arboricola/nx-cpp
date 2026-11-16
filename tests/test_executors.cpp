#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include "nxcpp/executor.hpp"

using Catch::Approx;
using nxcpp::Graph;

TEST_CASE("bfs_distances marks unreachable nodes as nullopt") {
    Graph g(3);
    g.add_edge(0, 1);
    const auto dist = g.bfs_distances(0);
    CHECK(dist[0] == 0);
    CHECK(dist[1] == 1);
    CHECK_FALSE(dist[2].has_value());
}

TEST_CASE("add_edge rejects nodes that are not in the graph") {
    Graph g(2);
    CHECK_THROWS_AS(g.add_edge(0, 5), std::out_of_range);
}

TEST_CASE("centralities on a path graph") {
    Graph g(4);  // 0 - 1 - 2 - 3
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);

    const auto degree = nxcpp::degree_centrality(g);
    CHECK(degree[0] == Approx(1.0 / 3));
    CHECK(degree[1] == Approx(2.0 / 3));

    const auto closeness = nxcpp::SerialExecutor{}.closeness_centrality(g);
    CHECK(closeness[0] == Approx(0.5));
    CHECK(closeness[1] == Approx(0.75));
}

TEST_CASE("parallel executor gives the same result as the serial one") {
    Graph g(50);  // a ring with a few chords
    for (nxcpp::Node u = 0; u < 50; ++u) g.add_edge(u, (u + 1) % 50);
    for (nxcpp::Node u = 0; u < 50; u += 7) g.add_edge(u, (u + 20) % 50);

    const auto expected = nxcpp::SerialExecutor{}.closeness_centrality(g);
    // more jobs than nodes is also fine
    for (const unsigned n_jobs : {1u, 3u, 8u, 64u}) {
        CHECK(nxcpp::ParallelExecutor{n_jobs}.closeness_centrality(g) == expected);
    }
}

TEST_CASE("make_executor") {
    CHECK(nxcpp::make_executor(1)->name() == "serial");
    CHECK(nxcpp::make_executor(4)->name() == "parallel");
}
