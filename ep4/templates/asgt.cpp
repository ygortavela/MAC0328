#include <utility>
#include <tuple>
#include <vector>
#include <limits>
#include <math.h>
#include <queue>
#include <cstdlib>
#include <iostream>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>


struct BundledVertex
{
  int parent;
  BundledVertex() : parent(-1) {}
};

struct BundledArc
{
  int capacity;
  int flow;
  int orientation;
  BundledArc() : capacity(0), flow(0), orientation(1) {}
};

struct BundledGraph
{
  int source;
  int target;
  int vertices_reachable_by_source;
  BundledGraph() : vertices_reachable_by_source(0) {}
};

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS,
                              BundledVertex,
                              BundledArc,
                              BundledGraph> Digraph;

typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;
typedef boost::graph_traits<Digraph>::vertices_size_type VerticesSize;

Digraph read_flow_digraph(std::istream& is);
Digraph build_residual_digraph(const Digraph& flow_digraph);
void run_bfs(Digraph& digraph);

Digraph read_flow_digraph(std::istream& is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n; is >> n;

  Digraph flow_digraph(n);

  size_t m; is >> m;

  is >> flow_digraph[boost::graph_bundle].source >> flow_digraph[boost::graph_bundle].target;

  while (m--) {
    int u, v; is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, flow_digraph);
    is >> flow_digraph[a].capacity;
  }

  return flow_digraph;
}

Digraph build_residual_digraph(const Digraph& flow_digraph) {
  VerticesSize vertices_amount = boost::num_vertices(flow_digraph);
  Digraph residual_digraph(vertices_amount);
  Vertex u, v;
  Arc positive_arc, negative_arc;
  int positive_arc_rc, negative_arc_rc;

  for (const auto& arc : boost::make_iterator_range(boost::edges(flow_digraph))) {
    u = boost::source(arc, flow_digraph);
    v = boost::target(arc, flow_digraph);
    positive_arc_rc = flow_digraph[arc].capacity - flow_digraph[arc].flow;
    negative_arc_rc = flow_digraph[arc].flow;

    if (positive_arc_rc != 0) {
      std::tie(positive_arc, std::ignore) = boost::add_edge(u, v, residual_digraph);
      residual_digraph[positive_arc].capacity = positive_arc_rc;
    }

    if (negative_arc_rc != 0) {
      std::tie(negative_arc, std::ignore) = boost::add_edge(v, u, residual_digraph);
      residual_digraph[negative_arc].orientation = -1;
      residual_digraph[negative_arc].capacity = negative_arc_rc;
    }

    std::cout << positive_arc_rc << " " << negative_arc_rc << std::endl;
  }

  residual_digraph[boost::graph_bundle].source = flow_digraph[boost::graph_bundle].source;
  residual_digraph[boost::graph_bundle].target = flow_digraph[boost::graph_bundle].target;

  return residual_digraph;
}

void run_bfs(Digraph& digraph) {
  std::queue<Vertex> vertex_to_process;
  Vertex source = digraph[boost::graph_bundle].source;

  vertex_to_process.push(source);
  digraph[source].parent = source;
  digraph[boost::graph_bundle].vertices_reachable_by_source++;

  while (!vertex_to_process.empty()) {
    Vertex current_vertex = vertex_to_process.front();

    vertex_to_process.pop();

    for (auto ep = boost::out_edges(current_vertex, digraph); ep.first != ep.second; ++ep.first) {
      Vertex target_vertex = boost::target(*ep.first, digraph);

      if (digraph[target_vertex].parent != -1) {
        vertex_to_process.push(target_vertex);
        digraph[target_vertex].parent = current_vertex;
        digraph[boost::graph_bundle].vertices_reachable_by_source++;
      }
    }
  }
}

int main(int argc, char** argv)
{
  Digraph flow_digraph{read_flow_digraph(std::cin)};

  return EXIT_SUCCESS;
}
