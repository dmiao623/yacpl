#define PROBLEM "https://judge.yosupo.jp/problem/shortest_path"

#include <algorithm>
#include <iostream>
#include <print>
#include <vector>

#include "../../src/graph/dijkstra.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  uint32_t n, m, s, t;
  std::cin >> n >> m >> s >> t;
  weighted_graph_t<uint64_t> graph(n);
  for (uint32_t i = 0; i < m; i++) {
    graph_node_t u, v;
    uint64_t w;
    std::cin >> u >> v >> w;
    graph[u].emplace_back(v, w);
  }

  auto distances = dijkstra(graph, s);

  if (distances[t] == std::numeric_limits<uint64_t>::max()) {
    std::println("-1");
    return 0;
  }

  auto sssp_graph = dijkstra_shortest_path_graph(graph, s, distances);

  std::vector<graph_node_t> parent(n, n);

  [&graph = std::as_const(sssp_graph), &parent, n](this auto &&self,
                                                   graph_node_t u) -> void {
    for (const auto &[v, _] : graph[u]) {
      if (parent[v] != n)
        continue;
      parent[v] = u;
      self(v);
    }
  }(s);

  std::vector<graph_node_t> path;
  path.reserve(n);
  for (graph_node_t u = t; u != s; u = parent[u])
    path.push_back(u);
  auto path_len = path.size();
  path.push_back(s);
  std::ranges::reverse(path);
  std::println("{} {}", distances[t], path_len);
  for (uint32_t i = 0; i < path_len; i++)
    std::println("{} {}", path[i], path[i + 1]);
}
