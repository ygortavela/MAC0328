#include "asgt.h"

int current_time;

SATDigraph::SATDigraph(std::istream& in) {
  std::set<std::pair<int, int>> clauses, arcs;

  in >> debug_level >> variable_quantity >> clause_quantity;

  for (int i = 0; i < clause_quantity; i++) {
    int u, v;

    in >> u >> v;
    clauses.insert(std::make_pair(map_variable_to_vertex(u), map_variable_to_vertex(v)));
  }

  arcs = build_digraph_arcs(clauses);
  sat_digraph = Digraph(arcs.begin(), arcs.end(), 2 * variable_quantity);
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
  for (auto vp = boost::vertices(sat_digraph); vp.first != vp.second; ++vp.first) {
    std::cout << "arcs that leave " << *vp.first << " go to:";
    for (auto ep = boost::out_edges(*vp.first, sat_digraph); ep.first != ep.second; ++ep.first) {
      Vertex target_vertex = boost::target(*ep.first, sat_digraph);

      std::cout << " " << target_vertex;
    }
    std::cout << std::endl;
  }
}

// Digraph read_arb(std::istream& in)
// {
//   std::vector<std::pair<int, int>> arb_arcs;
//   Size vertices_amount;

//   in >> vertices_amount;

//   for (int i = vertices_amount; i > 1; i--) {
//     int u, v; in >> u >> v;

//     arb_arcs.push_back(std::make_pair(--u, --v));
//   }

//   return Digraph(arb_arcs.begin(), arb_arcs.end(), vertices_amount);
// }

// HeadStart preprocess(Digraph &arb, const Vertex& root)
// {
//   Size vertices_amount = boost::num_vertices(arb);

//   std::vector<int> discovery_time(vertices_amount), finish_time(vertices_amount), visited_vertices(vertices_amount, 0);

//   current_time = 0;

//   process_vertex(arb, root, discovery_time, finish_time, visited_vertices);

//   return HeadStart(discovery_time, finish_time);
// }

// void process_vertex(Digraph &arb, Vertex current_vertex, std::vector<int> &discovery_time, std::vector<int> &finish_time, std::vector<int> &visited_vertices) {
//   visited_vertices[current_vertex] = 1;
//   discovery_time[current_vertex] = ++current_time;

//   for (auto ep = boost::out_edges(current_vertex, arb); ep.first != ep.second; ++ep.first) {
//     Vertex target_vertex = boost::target(*ep.first, arb);

//     if (!visited_vertices[target_vertex]) process_vertex(arb, target_vertex, discovery_time, finish_time, visited_vertices);
//   }

//   finish_time[current_vertex] = ++current_time;
// }


// bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
// {
//   if (data.discovery_time[u] <= data.discovery_time[v] && data.finish_time[u] >= data.finish_time[v])
//     return true;

//   return false;
// }
