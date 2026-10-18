#define PROBLEM "https://judge.yosupo.jp/problem/scc"

#include <iostream>
#include <vector>

#include "../../src/graph/tarjan.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  graph_t graph(n);
  for (int i = 0; i < m; i++) {
    graph_node_t u, v;
    std::cin >> u >> v;
    graph[u].push_back(v);
  }

  auto [count, component_ids] = tarjan(graph);
  std::vector<std::vector<graph_node_t>> components(count);
  for (graph_node_t u = 0; u < n; u++)
    components[component_ids[u]].push_back(u);

  std::cout << count << '\n';
  for (int id = count - 1; id >= 0; id--) {
    std::cout << components[id].size();
    for (auto u : components[id])
      std::cout << ' ' << u;
    std::cout << '\n';
  }
}
