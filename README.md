# nx-cpp

A small [NetworkX backend](https://networkx.org/documentation/stable/reference/backends.html) written in C++20.

```py
>>> import networkx as nx
>>> G = nx.karate_club_graph()
>>> nx.closeness_centrality(G, backend="cpp")
{0: 0.5689655172413793, 1: 0.4852941176470588, ...}
>>> nx.closeness_centrality(G, backend="cpp", n_jobs=4)  # 4 threads
```

On a random graph with 3000 nodes and ~22k edges, `closeness_centrality` took ~14 s with NetworkX, ~1.3 s with `n_jobs=1` and ~0.2 s with all the cores on my laptop.

## How it works

1. `pyproject.toml` registers the `networkx.backends` and `networkx.backend_info` entry points. This is how NetworkX finds the backend.
2. `python/nx_cpp/interface.py` has the `BackendInterface` that NetworkX calls. It converts the NetworkX graph into a C++ graph, and tells NetworkX what the backend cannot run (directed graphs, multigraphs, non-default arguments).
3. `src/bindings.cpp` exposes the C++ code to Python with [pybind11](https://pybind11.readthedocs.io), and releases the GIL while C++ is running.
4. The C++ library (`include/nxcpp`, `src`):
   - `Graph` – an undirected, unweighted adjacency list, with BFS.
   - `Executor` – an abstract class for "run this once per node" algorithms. `SerialExecutor` is a plain loop, and `ParallelExecutor` splits the nodes into `n_jobs` chunks and runs one `std::thread` per chunk (the same chunking idea as in nx-parallel).

## Supported algorithms

| Algorithm | Extra parameters |
| --- | --- |
| `degree_centrality` | |
| `closeness_centrality` | `n_jobs` – number of threads (`0` = all the cores, `1` = no threading) |

## Development

You need a C++20 compiler and CMake (>= 3.23).

**Python package and tests** (compares the results with NetworkX):

```sh
pip install ".[test]"
pytest tests
```

**C++ tests** (uses [Conan 2](https://docs.conan.io/2/installation.html) to install [Catch2](https://github.com/catchorg/Catch2)):

```sh
conan profile detect                                   # only needed once
conan install . --build=missing -s compiler.cppstd=20  # installs Catch2, generates the CMake toolchain and presets
cmake --preset conan-release
cmake --build --preset conan-release
ctest --preset conan-release
```

## Ideas for later

- Betweenness centrality, and weighted and directed graphs.
- Avoid copying the graph edge by edge in `convert_from_nx`.
- Run NetworkX's own test suite against the backend (`NETWORKX_TEST_BACKEND=cpp`).
