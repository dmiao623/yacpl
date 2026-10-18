#define PROBLEM "https://judge.yosupo.jp/problem/suffixarray"

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../src/string/suffix-automaton.hpp"

struct lowercase_alphabet {
  using symbol_t = char;
  static constexpr size_t size = 26;
  static constexpr size_t index(char c) { return c - 'a'; }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string s;
  std::cin >> s;
  std::ranges::reverse(s);
  const int n = static_cast<int>(s.size());
  using automaton_type = suffix_automaton<lowercase_alphabet>;
  using state_view = automaton_type::state_view;
  automaton_type automaton(s.size());
  std::vector<state_view> prefixes;
  prefixes.reserve(n);
  for (char c : s) {
    automaton.extend(c);
    prefixes.push_back(automaton.get_last());
  }

  const auto states = automaton.graph(); // Increasing maximum length.
  const int count = static_cast<int>(states.size());
  std::unordered_map<state_view, int, state_view::hasher> ids;
  ids.reserve(count);
  for (int i = 0; i < count; i++)
    ids.emplace(states[i], i);

  std::vector<int> end_position(count, -1), suffix_start(count, -1);
  for (int i = 0; i < n; i++) {
    const int id = ids.at(prefixes[i]);
    end_position[id] = i;
    suffix_start[id] = n - 1 - i;
  }

  // Every state needs one occurrence, including clones. Propagate occurrences
  // along suffix links from longer states to shorter states.
  std::vector<std::vector<int>> children(count);
  for (int i = count - 1; i > 0; i--) {
    const int parent = ids.at(states[i].suffix_link());
    children[parent].push_back(i);
    if (end_position[parent] == -1)
      end_position[parent] = end_position[i];
  }

  // The reversed string's suffix-link tree is a compact trie of the original
  // suffixes. Its edge labels start immediately before the parent's substring.
  for (int parent = 0; parent < count; parent++) {
    const int length = states[parent].length();
    std::ranges::sort(children[parent], [&](int a, int b) {
      return s[end_position[a] - length] < s[end_position[b] - length];
    });
  }

  // Emit a terminal before its children: a shorter suffix sorts first.
  // Use an explicit stack so repeated characters cannot overflow the call stack.
  std::vector<int> stack{ids.at(automaton.get_root())};
  bool first = true;
  while (!stack.empty()) {
    const int id = stack.back();
    stack.pop_back();
    if (suffix_start[id] != -1) {
      if (!first)
        std::cout << ' ';
      std::cout << suffix_start[id];
      first = false;
    }
    for (auto it = children[id].rbegin(); it != children[id].rend(); ++it)
      stack.push_back(*it);
  }
  std::cout << '\n';
}
