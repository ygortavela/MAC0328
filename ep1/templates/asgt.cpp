#include "asgt.h"

int current_time;

SATDigraph::SATDigraph(std::istream& in) {
  std::set<std::pair<int, int>> clauses, arcs;

  in >> debug_level >> variable_quantity >> clause_quantity;

  DigraphSize digraph_size = 2 * variable_quantity;

  for (int i = 0; i < clause_quantity; i++) {
    int u, v;

    in >> u >> v;
    clauses.insert(std::make_pair(map_variable_to_vertex(u), map_variable_to_vertex(v)));
  }

  arcs = build_digraph_arcs(clauses);
  digraph = Digraph(arcs.begin(), arcs.end(), digraph_size);
}

int SATDigraph::map_variable_to_vertex(int variable) {
  int vertex = variable + variable_quantity;

  if (variable > 0) vertex--;

  return vertex;
}

int SATDigraph::map_vertex_to_negative_variable_vertex(int vertex) {
  int variable = vertex - variable_quantity;

  if (vertex >= variable_quantity) variable++;

  return map_variable_to_vertex(variable * -1);
}

std::set<std::pair<int, int>> SATDigraph::build_digraph_arcs(std::set<std::pair<int, int>> clauses) {
  std::set<std::pair<int, int>> arcs;
  int u, v, not_u, not_v;

  for (auto clauses_iterator = clauses.begin(); clauses_iterator != clauses.end(); ++clauses_iterator) {
    u = (*clauses_iterator).first;
    v = (*clauses_iterator).second;
    not_u = map_vertex_to_negative_variable_vertex(u);
    not_v = map_vertex_to_negative_variable_vertex(v);

    std::cout << "u: " << u << " v: " << v << " not_u: " << not_u << " not_v: " << not_v << "\n";

    arcs.insert(std::make_pair(not_u, v));
    arcs.insert(std::make_pair(not_v, u));
  }

  return arcs;
}

void SATDigraph::print_digraph() {
  for (auto vp = boost::vertices(digraph); vp.first != vp.second; ++vp.first) {
    std::cout << "arcs that leave " << *vp.first << " go to:";
    for (auto ep = boost::out_edges(*vp.first, digraph); ep.first != ep.second; ++ep.first) {
      Vertex target_vertex = boost::target(*ep.first, digraph);

      std::cout << " " << target_vertex;
    }
    std::cout << std::endl;
  }
}

SATSolver::SATSolver(SATDigraph& built_sat_digraph) {
  sat_digraph = &built_sat_digraph;

  int digraph_size = sat_digraph->get_digraph_size();

  current_time = 0;
  nscc = 0;
  discovery_time = std::vector<int>(digraph_size, -1);
  finish_time = std::vector<int>(digraph_size);
  lowlink = std::vector<int>(digraph_size);
  strong_component = std::vector<int>(digraph_size, -1);
  on_stack = std::vector<bool>(digraph_size, false);
  parent = std::vector<Vertex>(digraph_size, -1);

  run_dfs();
}

void SATSolver::run_dfs() {
  for (auto vp = boost::vertices(sat_digraph->get_digraph()); vp.first != vp.second; ++vp.first) {
    Vertex current_vertex = *vp.first;

    if (discovery_time[current_vertex] == -1) process_vertex(current_vertex);
  }
}

void SATSolver::process_vertex(Vertex current_vertex) {
  discovery_time[current_vertex] = current_time;
  lowlink[current_vertex] = current_time;
  current_time++;
  vertices_stack.push(current_vertex);
  on_stack[current_vertex] = true;

  for (auto ep = boost::out_edges(current_vertex, sat_digraph->get_digraph()); ep.first != ep.second; ++ep.first) {
    Vertex target_vertex = boost::target(*ep.first, sat_digraph->get_digraph());

    if (discovery_time[target_vertex] == -1) {
      parent[target_vertex] = current_vertex;
      process_vertex(target_vertex);
      lowlink[current_vertex] = std::min(lowlink[current_vertex], lowlink[target_vertex]);
    } else if (on_stack[target_vertex]) {
      lowlink[current_vertex] = std::min(lowlink[current_vertex], discovery_time[target_vertex]);
    }
  }

  finish_time[current_vertex] = ++current_time;

  if (lowlink[current_vertex] == discovery_time[current_vertex]) {
    Vertex popped_vertex;

    do {
      popped_vertex = vertices_stack.top();
      vertices_stack.pop();
      strong_component[popped_vertex] = nscc;
      on_stack[popped_vertex] = false;
    } while (popped_vertex != current_vertex);

    nscc++;
  }
}

void SATSolver::print_strong_components() {
  for (int i = 0; i < sat_digraph->get_digraph_size(); i++) {
    std::cout << i << ": " << strong_component[i] << std::endl;
  }
}

