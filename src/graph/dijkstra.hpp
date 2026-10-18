#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include "graph.hpp"
#include "../data-structures/min-priority-queue.hpp"

namespace internal {

template <typename T>
bool check_nonnegative_edges(const weighted_graph_t<T> &graph) {
  if constexpr (std::is_unsigned_v<T>)
    return true;

  for (const auto &adj_u : graph)
    for (auto [_, w] : adj_u)
      if (w < 0)
        return false;
  return true;
}

} // namespace internal

/** Returns array d s.t. d_u represents the shortest path source -> u. We assume
 *  that diameter(graph) <= T::max(), i.e. path length never overflows. */
template <typename T>
std::vector<T> dijkstra(const weighted_graph_t<T> &graph, graph_node_t source) {
  assert(internal::check_weighted_graph(graph));
  assert(internal::check_nonnegative_edges(graph));
  assert(internal::check_valid_node(graph, source));

  size_t n = graph.size();
  std::vector<T> distances(n, std::numeric_limits<T>::max());
  min_priority_queue<std::pair<T, graph_node_t>> pq;

  distances[source] = 0;
  pq.emplace(0, source);

  while (!pq.empty()) {
    auto [distance, u] = pq.top();
    pq.pop();
    if (distance != distances[u])
      continue;
    for (auto [v, w] : graph[u]) {
      if (distance + w < distances[v]) {
        distances[v] = distance + w;
        pq.emplace(distances[v], v);
      }
    }
  }

  return distances;
}

/** Retains reachable shortest-path edges. Zero-weight cycles are preserved,
 *  so this is a DAG only when no such cycles exist.
 *  `distances` must be the result of dijkstra(graph, source).
 */
template <typename T>
weighted_graph_t<T>
dijkstra_shortest_path_graph(const weighted_graph_t<T> &graph,
                             graph_node_t source,
                             const std::vector<T> &distances) {
  assert(internal::check_weighted_graph(graph));
  assert(internal::check_nonnegative_edges(graph));
  assert(internal::check_valid_node(graph, source));
  assert(distances.size() == graph.size());

  size_t n = graph.size();
  weighted_graph_t<T> dag(n);
  for (const auto &[u, adj_u] : std::views::enumerate(graph)) {
    if (distances[u] == std::numeric_limits<T>::max())
      continue;
    for (auto [v, w] : adj_u) {
      if (distances[v] == std::numeric_limits<T>::max() ||
          w > std::numeric_limits<T>::max() - distances[u])
        continue;
      if (distances[v] == distances[u] + w)
        dag[u].emplace_back(v, w);
    }
  }
  return dag;
}

/** Compatibility name: zero-weight cycles mean the result may not be a DAG. */
template <typename T>
weighted_graph_t<T> dijkstra_dag(const weighted_graph_t<T> &graph,
                                 graph_node_t source,
                                 const std::vector<T> &distances) {
  return dijkstra_shortest_path_graph(graph, source, distances);
}
