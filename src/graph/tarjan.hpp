#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <stack>
#include <utility>
#include <vector>

#include "utils.hpp"

using index_t = uint32_t;

/** Returns (m, c) s.t.
 *   1. 0 <= c_i < m
 *   2. c_u = c_v iff u, v are mutually reachable
 *   3. for edge (u, v), if c_u != c_v, then c_u > c_v
 */
[[nodiscard]] std::pair<index_t, std::vector<index_t>>
tarjan(const graph_t &graph) {
  assert(internal::check_graph(graph));
  const auto index_t_max = std::numeric_limits<index_t>::max();

  size_t n = graph.size();
  index_t m = 0, time = 0;
  std::vector<index_t> time_in(n, index_t_max), min_time(n),
      component_id(n, index_t_max);
  std::vector<bool> on(n);
  std::stack<graph_node_t> stack;

  auto dfs = [&](this auto &&self, graph_node_t u) -> void {
    time_in[u] = min_time[u] = time++;
    stack.push(u);
    on[u] = true;

    for (auto v : graph[u]) {
      if (time_in[v] == index_t_max) {
        self(v);
        min_time[u] = std::min(min_time[u], min_time[v]);
      } else if (on[v]) {
        min_time[u] = std::min(min_time[u], min_time[v]);
      }
    }

    if (min_time[u] == time_in[u]) {
      graph_node_t v;
      do {
        v = stack.top();
        stack.pop();
        on[v] = false;
        component_id[v] = m;
      } while (u != v);
      m++;
    }
  });

  for (graph_node_t u = 0; u < static_cast<graph_node_t>(n); u++)
    if (time_in[u] == index_t_max)
      dfs(u);
  return {m, component_id};
}
