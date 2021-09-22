/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"

int current_time;

Arb read_arb(std::istream& in)
{
  std::vector<std::pair<int, int>> arb_arcs;
  ArbSize vertices_amount;

  in >> vertices_amount;

  for (int i = vertices_amount; i > 1; i--) {
    int u, v; in >> u >> v;

    arb_arcs.push_back(std::make_pair(--u, --v));
  }

  return Arb(arb_arcs.begin(), arb_arcs.end(), vertices_amount);
}

HeadStart preprocess(Arb &arb, const Vertex& root)
{
  ArbSize vertices_amount = boost::num_vertices(arb);

  std::vector<int> discovery_time(vertices_amount), finish_time(vertices_amount), visited_vertices(vertices_amount, 0);

  current_time = 0;

  process_vertex(arb, root, discovery_time, finish_time, visited_vertices);

  return HeadStart(discovery_time, finish_time);
}

void process_vertex(Arb &arb, Vertex current_vertex, std::vector<int> &discovery_time, std::vector<int> &finish_time, std::vector<int> &visited_vertices) {
  visited_vertices[current_vertex] = 1;
  discovery_time[current_vertex] = ++current_time;

  for (auto ep = boost::out_edges(current_vertex, arb); ep.first != ep.second; ++ep.first) {
    Vertex target_vertex = boost::target(*ep.first, arb);

    if (!visited_vertices[target_vertex]) process_vertex(arb, target_vertex, discovery_time, finish_time, visited_vertices);
  }

  finish_time[current_vertex] = ++current_time;
}


bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
{
  if (data.discovery_time[u] <= data.discovery_time[v] && data.finish_time[u] >= data.finish_time[v])
    return true;

  return false;
}
