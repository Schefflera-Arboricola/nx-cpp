// Python bindings: exposes Graph and the algorithms as the `nx_cpp._core` module.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "nxcpp/executor.hpp"

namespace py = pybind11;
using nxcpp::Graph;

PYBIND11_MODULE(_core, m) {
    py::class_<Graph>(m, "Graph")
        .def(py::init<std::size_t>())
        .def("add_edge", &Graph::add_edge)
        .def("num_nodes", &Graph::num_nodes);

    m.def("degree_centrality", &nxcpp::degree_centrality);

    // The GIL is released while C++ is running, since no Python objects are touched.
    m.def(
        "closeness_centrality",
        [](const Graph& g, unsigned n_jobs) {
            return nxcpp::make_executor(n_jobs)->closeness_centrality(g);
        },
        py::call_guard<py::gil_scoped_release>());
}
