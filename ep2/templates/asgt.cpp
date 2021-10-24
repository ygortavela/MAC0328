#include "asgt.h"
#include <stack>
#include <iostream>

void init(Graph &graph);
void run_dfs(Graph &graph);
void process_vertex(Graph &graph, Vertex current_vertex);

int current_time, bcc;
std::vector<int> discovery_time, low;
std::vector<int> parent;
std::stack<Edge> edge_stack;

void init(Graph &graph) {
  std::cout << &graph << std::endl;

  boost::graph_traits<Graph>::vertices_size_type vertices_amount = boost::num_vertices(graph);

  current_time = 1;
  bcc = 1;

  discovery_time = std::vector<int>(vertices_amount, -1);
  low = std::vector<int>(vertices_amount);
  parent = std::vector<int>(vertices_amount, -1);

  run_dfs(graph);

  for (const auto& edge : boost::make_iterator_range(boost::edges(graph))) {
    auto u = boost::source(edge, graph) + 1;
    auto v = boost::target(edge, graph) + 1;
    std::cout << "bcc[" << u << "," << v << "] = " << graph[edge].bcc << std::endl;
    std::cout << edge << std::endl;
  }
}

void run_dfs(Graph &graph) {
  for (auto vp = boost::vertices(graph); vp.first != vp.second; ++vp.first) {
    Vertex current_vertex = *vp.first;

    if (discovery_time[current_vertex] == -1) process_vertex(graph, current_vertex);
  }
}

void process_vertex(Graph &graph, Vertex current_vertex) {
  int child_amount = 0;

  discovery_time[current_vertex] = current_time;
  low[current_vertex] = current_time;
  current_time++;

  for (auto ep = boost::out_edges(current_vertex, graph); ep.first != ep.second; ++ep.first) {
    Vertex target_vertex = boost::target(*ep.first, graph);

    if (discovery_time[target_vertex] == -1) {
      child_amount++;
      parent[target_vertex] = current_vertex;
      edge_stack.push(*ep.first);
      process_vertex(graph, target_vertex);
      low[current_vertex] = std::min(low[current_vertex], low[target_vertex]);

      if (parent[current_vertex] == -1 && child_amount > 1) {
        graph[current_vertex].cutvertex = true;
      } else if (low[target_vertex] >= discovery_time[current_vertex]) {
        graph[current_vertex].cutvertex = true;
        graph[*ep.first].bridge = true;

        while (!edge_stack.empty()) {
          graph[edge_stack.top()].bcc = bcc;
          std::cout << "entrei " << edge_stack.top() << " bcc " << graph[edge_stack.top()].bcc << "\n";
          edge_stack.pop();
        }

        bcc++;
      }
    } else if ((int) target_vertex != parent[current_vertex] && discovery_time[target_vertex] < discovery_time[current_vertex]) {
      low[current_vertex] = std::min(low[current_vertex], discovery_time[target_vertex]);
      edge_stack.push(*ep.first);
    }
  }
}

void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges)
{
  init(g);
  std::cout << &g << std::endl;
}

