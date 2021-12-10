#include <utility>
#include <tuple>
#include <vector>
#include <limits>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>

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
  int index;
  BundledArc() : capacity(0), flow(0), orientation(1), index(0) {}
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

std::vector<Arc> arc_list;

Digraph read_flow_digraph(std::istream& is);
void edmonds_karp_max_integral_flow(Digraph& flow_digraph);
Digraph build_residual_digraph(const Digraph& flow_digraph);
void run_bfs(Digraph& digraph);
void augment_flow_digraph(const Digraph& residual_digraph, Digraph& flow_digraph);

Digraph read_flow_digraph(std::istream& is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n; is >> n;

  Digraph flow_digraph(n);

  size_t m; is >> m;

  arc_list.reserve(m);

  int s, t; is >> s >> t;

  flow_digraph[boost::graph_bundle].source = --s;
  flow_digraph[boost::graph_bundle].target = --t;

  for (int i = 1; m--; i++) {
    int u, v; is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, flow_digraph);
    flow_digraph[a].index = i;
    arc_list.push_back(a);
    is >> flow_digraph[a].capacity;
  }

  return flow_digraph;
}

void edmonds_karp_max_integral_flow(Digraph& flow_digraph) {
  Vertex target_vertex = flow_digraph[boost::graph_bundle].target;

  while (1) {
    Digraph residual_digraph = build_residual_digraph(flow_digraph);
    run_bfs(residual_digraph);

    if (residual_digraph[target_vertex].parent != -1) {
      augment_flow_digraph(residual_digraph, flow_digraph);
    } else {
      std::cout << 1 << " " << residual_digraph[boost::graph_bundle].vertices_reachable_by_source << std::endl;
      break;
    }
  }

}

Digraph build_residual_digraph(const Digraph& flow_digraph) {
  VerticesSize vertices_amount = boost::num_vertices(flow_digraph);
  Digraph residual_digraph(vertices_amount);
  Vertex u, v;
  Arc positive_arc, negative_arc;
  int positive_arc_rc, negative_arc_rc;

  for (const auto& arc : arc_list) {
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

      if (digraph[target_vertex].parent == -1) {
        vertex_to_process.push(target_vertex);
        digraph[target_vertex].parent = current_vertex;
        digraph[boost::graph_bundle].vertices_reachable_by_source++;
      }
    }
  }
}

void augment_flow_digraph(const Digraph& residual_digraph, Digraph& flow_digraph) {
  std::stack<Arc> min_st_path;
  Vertex source_vertex = residual_digraph[boost::graph_bundle].source;
  Vertex target_vertex = residual_digraph[boost::graph_bundle].target;
  Vertex current_vertex = residual_digraph[target_vertex].parent;
  Arc arc = boost::edge(current_vertex, target_vertex, residual_digraph).first;
  int min_residual_capacity = residual_digraph[arc].capacity;


  min_st_path.push(arc);

  while (current_vertex != source_vertex) {
    arc = boost::edge(residual_digraph[current_vertex].parent, current_vertex, residual_digraph).first;
    min_st_path.push(arc);
    min_residual_capacity = std::min(min_residual_capacity, residual_digraph[arc].capacity);
    current_vertex = residual_digraph[current_vertex].parent;
  }

  std::cout << 0 << " " << min_residual_capacity << " " << min_st_path.size() << std::endl;

  while (!min_st_path.empty()) {
    Arc top_arc = min_st_path.top(), flow_arc;
    int arc_orientation = residual_digraph[top_arc].orientation;

    if (arc_orientation == -1) {
      flow_arc = boost::edge(boost::target(top_arc, residual_digraph), boost::source(top_arc, residual_digraph), flow_digraph).first;
      flow_digraph[flow_arc].flow -= min_residual_capacity;
    } else {
      flow_arc = boost::edge(boost::source(top_arc, residual_digraph), boost::target(top_arc, residual_digraph), flow_digraph).first;
      flow_digraph[flow_arc].flow += min_residual_capacity;
    }

    std::cout << flow_digraph[flow_arc].index * arc_orientation << " ";
    min_st_path.pop();
  }

  std::cout << std::endl;
}

int calculate_flow_value(const Digraph& flow_digraph) {

}

int main(int argc, char** argv)
{
  Digraph flow_digraph{read_flow_digraph(std::cin)};

  edmonds_karp_max_integral_flow(flow_digraph);

  return EXIT_SUCCESS;
}
