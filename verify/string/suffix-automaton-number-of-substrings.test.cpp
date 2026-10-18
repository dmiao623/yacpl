#define PROBLEM "https://judge.yosupo.jp/problem/number_of_substrings"

#include <iostream>
#include <string>

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
  suffix_automaton<lowercase_alphabet> automaton(s);

  long long answer = 0;
  for (auto state : automaton.graph()) {
    auto link = state.suffix_link();
    if (link.is_valid())
      answer += state.length() - link.length();
  }
  std::cout << answer << '\n';
}
