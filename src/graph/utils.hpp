#pragma once

#include <cassert>
#include <cstdint>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../utility/hash-utils.hpp"

using index_t = uint32_t;
using graph_node_t = uint32_t;

using graph_t = std::vector<std::vector<graph_node_t>>;

using edge_list_t = std::vector<std::pair<graph_node_t, graph_node_t>>;

template <typename T>
  requires std::is_arithmetic_v<T>
using weighted_graph_t = std::vector<std::vector<std::pair<graph_node_t, T>>>;

template <typename T>
  requires std::is_arithmetic_v<T>
using weighted_edge_list_t =
    std::vector<std::tuple<graph_node_t, graph_node_t, T>>;

namespace internal {

template <typename T>
graph_t unweight_graph(const weighted_graph_t<T> &weighted_graph) {
  size_t n = weighted_graph.size();
  graph_t graph(n);
  for (graph_node_t u = 0; u < static_cast<graph_node_t>(n); u++)
    for (const auto &[v, _] : weighted_graph[u])
      graph[u].push_back(v);
  return graph;
}

template <bool undirected = false> bool check_graph(const graph_t &graph) {
  size_t n = graph.size();
  if constexpr (undirected) {
    std::unordered_map<std::pair<graph_node_t, graph_node_t>, index_t,
                       hash_pair>
        edge_count;
    for (const auto &[u, adj_u] : std::views::enumerate(graph)) {
      for (graph_node_t v : adj_u) {
        if (static_cast<graph_node_t>(n) <= v)
          return false;
        edge_count[{u, v}] += 1;
      }
    }
    for (const auto [edge, count] : edge_count) {
      auto reverse = edge_count.find({edge.second, edge.first});
      if (reverse == edge_count.end() || reverse->second != count)
        return false;
    }
  } else {
    for (const auto &adj_u : graph)
      for (graph_node_t v : adj_u)
        if (static_cast<graph_node_t>(n) <= v)
          return false;
  }
  return true;
}

template <bool undirected = false, typename T>
bool check_weighted_graph(const weighted_graph_t<T> &graph) {
  return check_graph<undirected>(internal::unweight_graph(graph));
}

inline bool check_valid_node(const graph_t &graph, graph_node_t node) {
  return node < static_cast<graph_node_t>(graph.size());
}

template <typename T>
bool check_valid_node(const weighted_graph_t<T> &graph, graph_node_t node) {
  return node < static_cast<graph_node_t>(graph.size());
}

} // namespace internal

template <typename T> graph_t unweight_graph(const weighted_graph_t<T> &graph) {
  assert(internal::check_weighted_graph(graph));
  return internal::unweight_graph(graph);
}

inline edge_list_t graph_to_edge_list(const graph_t &graph) {
  assert(internal::check_graph(graph));
  edge_list_t edges;
  for (const auto &[u, adj_u] : std::views::enumerate(graph))
    for (auto v : adj_u)
      edges.emplace_back(u, v);
  return edges;
}

template <typename T>
weighted_edge_list_t<T>
weighted_graph_to_edge_list(const weighted_graph_t<T> &graph) {
  assert(internal::check_weighted_graph(graph));
  weighted_edge_list_t<T> edges;
  for (const auto &[u, adj_u] : std::views::enumerate(graph))
    for (auto [v, w] : adj_u)
      edges.emplace_back(u, v, w);
  return edges;
}
