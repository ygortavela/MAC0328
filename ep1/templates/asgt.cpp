#include "asgt.h"

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
  arcs_quantity = arcs.size();

  digraph = Digraph(arcs.begin(), arcs.end(), digraph_size);
}

/*
 * Here I'm using a specific mapping function which seemed the most intuitive to me.
 * Remembering that the way I chose this mapping affects the output of this program.
 *
 * For a given input of 3 variables, we have:
 *
 * not x3 -> vertex 0
 * not x2 -> vertex 1
 * not x1 -> vertex 2
 * x1 -> vertex 3
 * x2 -> vertex 4
 * x3 -> vertex 5
 *
*/
int SATDigraph::map_variable_to_vertex(int variable) {
  int vertex = variable + variable_quantity;

  return variable > 0 ? --vertex : vertex;
}

int SATDigraph::map_vertex_to_variable(int vertex) {
  int variable = vertex - variable_quantity;

  return vertex >= variable_quantity ? ++variable : variable;
}

int SATDigraph::map_vertex_to_negative_variable_vertex(int vertex) {
  return (2 * variable_quantity) - vertex - 1;
}

std::set<std::pair<int, int>> SATDigraph::build_digraph_arcs(std::set<std::pair<int, int>> clauses) {
  std::set<std::pair<int, int>> arcs;
  int u, v, not_u, not_v;

  for (auto clauses_iterator = clauses.begin(); clauses_iterator != clauses.end(); ++clauses_iterator) {
    u = (*clauses_iterator).first;
    v = (*clauses_iterator).second;
    not_u = map_vertex_to_negative_variable_vertex(u);
    not_v = map_vertex_to_negative_variable_vertex(v);

    arcs.insert(std::make_pair(not_u, v));
    arcs.insert(std::make_pair(not_v, u));
  }

  return arcs;
}

void SATDigraph::print_digraph() {
  std::cout << variable_quantity << " " << arcs_quantity << std::endl;

  for (auto vp = boost::vertices(digraph); vp.first != vp.second; ++vp.first) {
    for (auto ep = boost::out_edges(*vp.first, digraph); ep.first != ep.second; ++ep.first) {
      Vertex target_vertex = boost::target(*ep.first, digraph);

      std::cout << map_vertex_to_variable(*vp.first) << " " << map_vertex_to_variable(target_vertex) << std::endl;
    }
  }
}

SATSolver::SATSolver(SATDigraph& built_sat_digraph) {
  sat_digraph = &built_sat_digraph;

  int digraph_size = sat_digraph->get_digraph_size();

  current_time = 0;
  nscc = 0;
  discovery_time = std::vector<int>(digraph_size, -1);
  lowlink = std::vector<int>(digraph_size);
  strong_component = std::vector<int>(digraph_size, -1);
  on_stack = std::vector<bool>(digraph_size, false);

  run_dfs();

  switch (sat_digraph->get_debug_level()) {
    case 0:
      satisfiability_check();
      break;
    case 1:
      print_strong_components();
      break;
    case 2:
      sat_digraph->print_digraph();
      break;
  }
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
      process_vertex(target_vertex);
      lowlink[current_vertex] = std::min(lowlink[current_vertex], lowlink[target_vertex]);
    } else if (on_stack[target_vertex]) {
      lowlink[current_vertex] = std::min(lowlink[current_vertex], discovery_time[target_vertex]);
    }
  }

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

void SATSolver::satisfiability_check() {
  std::vector<int> solution(sat_digraph->get_digraph_size());
  bool has_solution = true;
  int negative_vertex, vertex_component, negative_vertex_component;

  for (int i = sat_digraph->get_digraph_size() / 2; i < sat_digraph->get_digraph_size(); i++) {
    negative_vertex = sat_digraph->get_negative_variable_of(i);
    vertex_component = strong_component[i];
    negative_vertex_component = strong_component[negative_vertex];

    if (vertex_component == negative_vertex_component) {
      has_solution = false;

      break;
    } else if (vertex_component < negative_vertex_component) {
      solution[i] = 1;
      solution[negative_vertex] = 0;
    } else {
      solution[i] = 0;
      solution[negative_vertex] = 1;
    }
  }

  std::cout << (has_solution ? "YES" : "NO");

  for (int i = sat_digraph->get_digraph_size() / 2; has_solution && i < sat_digraph->get_digraph_size(); i++)
    std::cout << " " << solution[i];

  std::cout << std::endl;
}

void SATSolver::print_strong_components() {
  for (int i = sat_digraph->get_digraph_size() / 2; i < sat_digraph->get_digraph_size(); i++)
    std::cout << strong_component[i] + 1 << " ";

  for (int i = (sat_digraph->get_digraph_size() / 2) - 1; i >= 0; i--)
    std::cout << strong_component[i] + 1 << " ";

  std::cout << std::endl;
}

