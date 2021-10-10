#ifndef ASGT_H
#define ASGT_H

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::vertices_size_type DigraphSize;

#include <iostream>
#include <stack>

class SATDigraph {
private:
  int debug_level, variable_quantity, clause_quantity;
  Digraph digraph;
  int map_variable_to_vertex(int variable);
  int map_vertex_to_negative_variable_vertex(int vertex);
  std::set<std::pair<int, int>> build_digraph_arcs(std::set<std::pair<int, int>> clauses);
public:
  SATDigraph(std::istream& in);
  void print_digraph();
  int get_debug_level() { return debug_level; }
  Digraph& get_digraph() { return digraph; }
  int get_digraph_size() { return 2 * variable_quantity; }
  int get_negative_variable_of(int vertex) { return map_vertex_to_negative_variable_vertex(vertex); }
};

class SATSolver {
private:
  SATDigraph* sat_digraph;
  int current_time, nscc;
  std::vector<int> discovery_time, finish_time, lowlink, strong_component;
  std::vector<bool> on_stack;
  std::vector<Vertex> parent;
  std::stack<Vertex> vertices_stack;
  void run_dfs();
  void process_vertex(Vertex current_vertex);
public:
  SATSolver(SATDigraph& built_sat_digraph);
  void satisfiability_check();
  void print_strong_components();
};


#endif
