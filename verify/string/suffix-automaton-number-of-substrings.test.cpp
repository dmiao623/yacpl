#define PROBLEM "https://judge.yosupo.jp/problem/number_of_substrings"

#include <iostream>
#include <print>
#include <string>

#include "../../src/string/suffix-automaton.hpp"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string s;
  std::cin >> s;
  suffix_automaton<lowercase_alphabet> automaton(s);

  uint64_t answer = 0;
  for (auto state : automaton.graph()) {
    auto link = state.suffix_link();
    if (link.is_valid())
      answer += state.length() - link.length();
  }
  std::println("{}", answer);
}
