/* this file MUST NOT be edited by the students */

#ifndef ASGT_H
#define ASGT_H

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <stack>

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::vertices_size_type Size;

class SATDigraph {
private:
  int debug_level, variable_quantity, clause_quantity;
  Digraph sat_digraph;
  int map_variable_to_vertex(int variable);
  int map_vertex_to_negative_variable_vertex(int vertex);
  std::set<std::pair<int, int>> build_digraph_arcs(std::set<std::pair<int, int>> clauses);
public:
  SATDigraph(std::istream& in);
  void print_digraph();
  int get_debug_level() { return debug_level; }
  Digraph get_sat_digraph() { return sat_digraph; }
};

class SATSolver {
private:
  int variables_amount;
  std::vector<int> discovery_time, finish_time;
  std::stack<int> vertices_stack;
};

void process_vertex(Digraph &arb, Vertex current_vertex, std::vector<int> &discovery_time, std::vector<int> &finish_time, std::vector<int> &visited_vertices);

#endif 
