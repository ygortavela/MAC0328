#include "asgt.h"
#include <stack>
#include <iostream>

class BCCGraph {
private:
  Graph& graph;
  int current_time, bcc;
  std::vector<int> discovery_time, low;
  std::vector<int> parent;
  std::stack<Edge> edge_stack;
  void process_vertex(Vertex current_vertex);
public:
  BCCGraph(Graph &g) : graph(g) {};
  void run_dfs();
};

void BCCGraph::run_dfs() {
  boost::graph_traits<Graph>::vertices_size_type vertices_amount = boost::num_vertices(graph);

  current_time = 1;
  bcc = 1;

  discovery_time = std::vector<int>(vertices_amount, -1);
  low = std::vector<int>(vertices_amount);
  parent = std::vector<int>(vertices_amount, -1);

  for (auto vp = boost::vertices(graph); vp.first != vp.second; ++vp.first) {
    Vertex current_vertex = *vp.first;

    if (discovery_time[current_vertex] == -1) process_vertex(current_vertex);
  }
}

void BCCGraph::process_vertex(Vertex current_vertex) {
  int child_amount = 0;

  discovery_time[current_vertex] = current_time;
  low[current_vertex] = current_time;
  current_time++;

  for (auto ep = boost::out_edges(current_vertex, graph); ep.first != ep.second; ++ep.first) {
    Vertex target_vertex = boost::target(*ep.first, graph);

    if (discovery_time[target_vertex] == -1) {
      parent[target_vertex] = current_vertex;
      edge_stack.push(*ep.first);
      child_amount++;
      process_vertex(target_vertex);
      low[current_vertex] = std::min(low[current_vertex], low[target_vertex]);

      if (low[target_vertex] >= discovery_time[current_vertex]) {
        Edge top_edge;

        if (parent[current_vertex] != -1 || child_amount > 1) graph[current_vertex].cutvertex = true;

        if (low[target_vertex] != discovery_time[current_vertex]) graph[*ep.first].bridge = true;

        do {
          top_edge = edge_stack.top();
          graph[top_edge].bcc = bcc;
          edge_stack.pop();
        } while (top_edge != *ep.first);

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
  BCCGraph bcc_graph(g);
  bcc_graph.run_dfs();
}

