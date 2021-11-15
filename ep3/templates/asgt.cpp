#include "asgt.h"

#include <utility>              // for std::get
#include <tuple>
#include <vector>
#include <limits>
#include <math.h>
#include <stack>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

using boost::add_edge;
using boost::num_vertices;
using boost::out_edges;
using std::vector;

int bellman_ford_negative_cycle_source(Digraph& digraph, Vertex source);
Walk build_negative_cycle_walk(Digraph& digraph, Vertex source);
vector<double> get_feasible_potential(Digraph& digraph);

Digraph build_digraph(const Digraph& market)
{
  Digraph digraph(num_vertices(market));

  for (auto vp = boost::vertices(market); vp.first != vp.second; ++vp.first) {
    for (auto ep = boost::out_edges(*vp.first, market); ep.first != ep.second; ++ep.first) {
      Vertex source_vertex = *vp.first;
      Vertex target_vertex = boost::target(*ep.first, market);
      Arc new_arc;

      std::tie(new_arc, std::ignore) = add_edge(source_vertex, target_vertex, digraph);
      digraph[new_arc].cost = -log(market[*ep.first].cost);
    }
  }

  return digraph;
}

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  Vertex aux_vertex = boost::add_vertex(digraph);

  for (const auto& vertex : boost::make_iterator_range(boost::vertices(digraph))) {
      Arc new_arc;

      if (vertex != aux_vertex) {
        std::tie(new_arc, std::ignore) = add_edge(aux_vertex, vertex, digraph);
        digraph[new_arc].cost = 0.0;
      }
  }

  int negative_cycle_source = bellman_ford_negative_cycle_source(digraph, aux_vertex);

  if (negative_cycle_source >= 0) {
    Walk walk = build_negative_cycle_walk(digraph, negative_cycle_source);

    return {true, NegativeCycle(walk), boost::none};
  }

  boost::remove_vertex(aux_vertex, digraph);
  vector<double> y(get_feasible_potential(digraph));

  return {false, boost::none, FeasiblePotential(digraph, y)};
}

Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market)
{
  vector<Arc> cycle_arcs = negcycle.get();
  Walk walk(market, boost::source(cycle_arcs[0], market));

  for (const auto& arc : cycle_arcs)
    walk.extend(boost::edge(boost::source(arc, aux_digraph), boost::target(arc, aux_digraph), market).first);

  return Loophole(walk);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market)
{
  int vertices_quantity = num_vertices(market);
  vector<double> feasible_multiplier(vertices_quantity);
  vector<double> feasible_potential = feaspot.potential();

  for (int i = 0; i < vertices_quantity; i++)
    feasible_multiplier[i] = 1 / exp(feasible_potential[i]);

  return FeasibleMultiplier(market, feasible_multiplier);
}

int bellman_ford_negative_cycle_source(Digraph& digraph, Vertex source) {
  for (const auto& vertex : boost::make_iterator_range(boost::vertices(digraph))) {
    digraph[vertex].distance_cost = std::numeric_limits<double>::infinity();
    digraph[vertex].predecessor = -1;
  }

  digraph[source].distance_cost = 0;
  digraph[source].predecessor = source;

  int vertices_quantity = num_vertices(digraph);

  for (int l = 0; l < vertices_quantity; l++) {
    for (auto vp = boost::vertices(digraph); vp.first != vp.second; ++vp.first) {
      for (auto ep = boost::out_edges(*vp.first, digraph); ep.first != ep.second; ++ep.first) {
        Vertex target_vertex = boost::target(*ep.first, digraph);
        double new_distance_cost = digraph[*vp.first].distance_cost + digraph[*ep.first].cost;

        if (digraph[target_vertex].distance_cost > new_distance_cost) {
          digraph[target_vertex].distance_cost = new_distance_cost;
          digraph[target_vertex].predecessor = *vp.first;
        }
      }
    }
  }

  for (const auto& arc : boost::make_iterator_range(boost::edges(digraph))) {
    Vertex source_vertex = boost::source(arc, digraph);
    Vertex target_vertex = boost::target(arc, digraph);

    if (digraph[source_vertex].distance_cost + digraph[arc].cost < digraph[target_vertex].distance_cost)
      return source_vertex;
  }

  return -1;
}

Walk build_negative_cycle_walk(Digraph& digraph, Vertex source) {
  std::vector<bool> visited_vertices(num_vertices(digraph), false);

  visited_vertices[source] = true;
  Vertex parent = digraph[source].predecessor;

  while (!visited_vertices[parent]) {
    visited_vertices[parent] = true;
    parent = digraph[parent].predecessor;
  }

  std::stack<Vertex> cycle_stack;

  Vertex current_vertex = parent;
  cycle_stack.push(current_vertex);
  current_vertex = digraph[current_vertex].predecessor;

  while (current_vertex != parent) {
    cycle_stack.push(current_vertex);
    current_vertex = digraph[current_vertex].predecessor;
  }

  current_vertex = parent;
  Walk walk(digraph, current_vertex);

  while (!cycle_stack.empty()) {
    Arc arc = boost::edge(current_vertex, cycle_stack.top(), digraph).first;

    walk.extend(arc);

    current_vertex = cycle_stack.top();
    cycle_stack.pop();
  }

  return walk;
}

vector<double> get_feasible_potential(Digraph& digraph) {
  vector<double> feasible_potential;

  for (const auto& vertex : boost::make_iterator_range(boost::vertices(digraph)))
    feasible_potential.push_back(digraph[vertex].distance_cost);

  return feasible_potential;
}
