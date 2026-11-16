"""What NetworkX sees of nx-cpp, through the entry points in pyproject.toml."""

from nx_cpp import _core


class CppGraph:
    """A C++ graph, plus the NetworkX node labels (C++ nodes are just 0..n-1)."""

    __networkx_backend__ = "cpp"

    def __init__(self, graph, nodes):
        self.graph = graph
        self.nodes = nodes


def degree_centrality(G):
    return dict(zip(G.nodes, _core.degree_centrality(G.graph)))


def closeness_centrality(G, u=None, distance=None, wf_improved=True, *, sp=None, n_jobs=0):
    return dict(zip(G.nodes, _core.closeness_centrality(G.graph, n_jobs)))


class BackendInterface:
    """The object the `networkx.backends` entry point points to."""

    degree_centrality = staticmethod(degree_centrality)
    closeness_centrality = staticmethod(closeness_centrality)

    @staticmethod
    def convert_from_nx(G, *args, **kwargs):
        nodes = list(G)
        index = {node: i for i, node in enumerate(nodes)}
        graph = _core.Graph(len(nodes))
        for u, v in G.edges():
            graph.add_edge(index[u], index[v])
        return CppGraph(graph, nodes)

    @staticmethod
    def convert_to_nx(result, *, name=None):
        return result  # the algorithms already return plain dicts

    @staticmethod
    def can_run(name, args, kwargs):
        """Only simple undirected graphs and the default arguments are supported."""
        G = args[0] if args else kwargs["G"]
        if not isinstance(G, CppGraph) and (G.is_directed() or G.is_multigraph()):
            return False
        if name == "closeness_centrality":
            defaults = {"u": None, "distance": None, "wf_improved": True, "sp": None}
            passed = {**dict(zip(defaults, args[1:])), **kwargs}
            return all(passed.get(key, value) is value for key, value in defaults.items())
        return True


def get_info():
    """Backend metadata, for the `networkx.backend_info` entry point."""
    return {
        "backend_name": "cpp",
        "project": "nx-cpp",
        "package": "nx_cpp",
        "short_summary": "A small NetworkX backend written in C++.",
        "functions": {
            "degree_centrality": {},
            "closeness_centrality": {
                "additional_parameters": {
                    "n_jobs : int, default=0": (
                        "Number of threads to use. 0 means all the cores and "
                        "1 means no threading."
                    ),
                },
            },
        },
    }
