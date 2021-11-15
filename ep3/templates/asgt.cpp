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

bool bellman_ford(Digraph& digraph, Vertex source);
Walk build_negative_cycle_walk(Digraph& digraph);

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

      // PRINT EXCLUIR DEPOIS
      std::cout << new_arc << " " << digraph[new_arc].cost << " market: " << market[*ep.first].cost << std::endl;
    }
  }

  return digraph;
}

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  bool digraph_has_no_negative_cycle = bellman_ford(digraph, 0);

  std::cout << " tem ciclo negativo? " << !digraph_has_no_negative_cycle << "\n";

  /* Replace `NegativeCycle(walk)` with `boost::none` in the next
   * command to trigger "negative cycle reported but not computed".
   * Comment the whole `return` and uncomment the remaining lines to
   * exercise construction of a feasible potential. */

  // encourage RVO
  if (!digraph_has_no_negative_cycle) {
    Walk walk = build_negative_cycle_walk(digraph);


    return {true, NegativeCycle(walk), boost::none};
  }

  /* Replace `FeasiblePotential(digraph, y)` with `boost::none` in the
   * next command to trigger "feasible potential reported but not
   * computed". */

  // encourage RVO
  vector<double> y(num_vertices(digraph), 0.0);
  return {false, boost::none, FeasiblePotential(digraph, y)};
}

Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market)
{
  Walk walk(market, 0);

  for (const auto& arc : negcycle.get())
    walk.extend(boost::edge(boost::source(arc, aux_digraph), boost::target(arc, aux_digraph), market).first);

  return Loophole(walk);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market)
{
  vector<double> z(num_vertices(market), 1.0);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}

bool bellman_ford(Digraph& digraph, Vertex source) {
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

    if (digraph[source_vertex].distance_cost + digraph[arc].cost < digraph[target_vertex].distance_cost) return false;
  }

  return true;
}

Walk build_negative_cycle_walk(Digraph& digraph) {
  std::stack<Vertex> cycle_stack;

  Vertex current_vertex = 0;
  cycle_stack.push(current_vertex);
  current_vertex = digraph[current_vertex].predecessor;

  while (current_vertex != 0) {
    cycle_stack.push(current_vertex);
    current_vertex = digraph[current_vertex].predecessor;
  }

  current_vertex = 0;
  Walk walk(digraph, current_vertex);

  while (!cycle_stack.empty()) {
    Arc arc = boost::edge(current_vertex, cycle_stack.top(), digraph).first;

    walk.extend(arc);

    current_vertex = cycle_stack.top();
    cycle_stack.pop();
  }

  return walk;
}
