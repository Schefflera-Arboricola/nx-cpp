"""Checks that the "cpp" backend gives the same results as NetworkX."""

import networkx as nx
import pytest

from nx_cpp import BackendInterface

GRAPHS = {
    "karate_club": nx.karate_club_graph(),
    "disconnected": nx.disjoint_union(nx.path_graph(5), nx.cycle_graph(4)),
    "string_nodes": nx.relabel_nodes(nx.path_graph(4), str),
    "self_loop": nx.Graph([(0, 0), (0, 1), (1, 2)]),
    "single_node": nx.empty_graph(1),
    "empty": nx.Graph(),
}


def test_backend_is_registered():
    assert "cpp" in nx.config.backends


@pytest.mark.parametrize("G", GRAPHS.values(), ids=GRAPHS.keys())
def test_degree_centrality(G):
    assert nx.degree_centrality(G, backend="cpp") == pytest.approx(nx.degree_centrality(G))


@pytest.mark.parametrize("n_jobs", [0, 1, 3])
@pytest.mark.parametrize("G", GRAPHS.values(), ids=GRAPHS.keys())
def test_closeness_centrality(G, n_jobs):
    result = nx.closeness_centrality(G, backend="cpp", n_jobs=n_jobs)
    assert result == pytest.approx(nx.closeness_centrality(G))


def test_can_run():
    can_run = BackendInterface.can_run
    G = nx.path_graph(3)
    assert can_run("closeness_centrality", (G,), {})
    assert not can_run("closeness_centrality", (G,), {"u": 0})
    assert not can_run("closeness_centrality", (G,), {"wf_improved": False})
    assert not can_run("degree_centrality", (nx.DiGraph(G),), {})
