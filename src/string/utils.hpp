#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <vector>

namespace internal {

template <typename A>
concept alphabet = requires(typename A::symbol_t c) {
  { A::size } -> std::convertible_to<size_t>;
  { A::index(c) } -> std::convertible_to<size_t>;
};

template <typename T>
concept char_like = std::equality_comparable<T> && std::copyable<T>;

template <typename T>
concept string_like =
    (std::ranges::contiguous_range<T> && std::ranges::sized_range<T> &&
     requires { typename std::ranges::range_value_t<T>; } &&
     char_like<std::ranges::range_value_t<T>>);

template <typename T, typename A>
concept string_like_over_alphabet =
    alphabet<A> && string_like<T> &&
    std::same_as<typename A::symbol_t, std::ranges::range_value_t<T>>;

} // namespace internal

using index_t = uint32_t;

[[nodiscard]] std::vector<index_t> str_pi(const internal::string_like auto &s) {
  index_t n = static_cast<index_t>(std::ranges::size(s));
  std::vector<index_t> pi(n);
  for (index_t i = 1, j; i < n; i++) {
    for (j = pi[i - 1]; j > 0 && s[j] != s[i]; j = pi[j - 1])
      ;
    if (s[i] == s[j])
      j++;
    pi[i] = j;
  }
  return pi;
}

[[nodiscard]] std::vector<index_t> str_z(const internal::string_like auto &s) {
  int n = static_cast<index_t>(std::ranges::size(s));
  std::vector<index_t> z(n);
  for (index_t i = 1, l = 0, r = 0; i < n; i++) {
    if (i <= r)
      z[i] = std::min(z[i - l], r - i + 1);
    while (i + z[i] < n && s[z[i]] == s[i + z[i]])
      z[i]++;
    if (i + z[i] - 1 > r)
      l = i, r = i + z[i] - 1;
  }
  return z;
}
