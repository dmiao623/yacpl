#pragma once

#include <array>
#include <functional>
#include <memory_resource>
#include <stack>
#include <unordered_set>
#include <utility>

#include "utils.hpp"

using index_t = uint32_t;

template <internal::alphabet A> class suffix_automaton {
  using symbol_t = A::symbol_t;

  struct state {
    index_t length;
    state *link;
    std::array<state *, A::size> next;

    state(index_t length_, state *link_ = nullptr)
        : length{length_}, link{link_}, next{} {}
  };

public:
  class state_view {
    friend class suffix_automaton;

    const state *s;

    explicit state_view(const state *s_) : s{s_} {}

  public:
    state_view() : s(nullptr) {}

    [[nodiscard]] bool is_valid() const { return s != nullptr; }

    [[nodiscard]] state_view next(symbol_t c) const {
      if (!s)
        return state_view{};
      return state_view{s->next[A::index(c)]};
    }

    [[nodiscard]] state_view suffix_link() const {
      if (!s)
        return state_view{};
      return state_view{s->link};
    }

    [[nodiscard]] index_t length() const { return s ? s->length : 0; }

    bool operator==(const state_view &other) const = default;

    struct hasher {
      size_t operator()(const state_view &view) const {
        return std::hash<const state *>{}(view.s);
      }
    };
  };

  suffix_automaton(size_t max_length)
      : buffer{(2 * max_length + 1) * sizeof(state)},
        pool{buffer.data(), buffer.size()}, alloc{&pool}, root{new_state(0)},
        last{root} {}

  template <internal::string_like_over_alphabet<A> T>
  suffix_automaton(const T &string)
      : suffix_automaton{std::ranges::size(string)} {
    for (const auto character : string)
      extend(character);
  }

  void extend(const symbol_t character) {
    const index_t index = A::index(character);
    state *s = new_state(last->length + 1), *p = last;
    for (; p != nullptr && p->next[index] == nullptr; p = p->link)
      p->next[index] = s;
    if (p == nullptr) {
      s->link = root;
    } else {
      state *q = p->next[index];
      if (p->length + 1 == q->length) {
        s->link = q;
      } else {
        state *d = new_state(*q);
        d->length = p->length + 1;
        for (; p != nullptr && p->next[index] == q; p = p->link)
          p->next[index] = d;
        s->link = q->link = d;
      }
    }
    last = s;
  }

  [[nodiscard]] state_view get_root() const { return state_view{root}; }
  [[nodiscard]] state_view get_last() const { return state_view{last}; }

  template <internal::string_like_over_alphabet<A> T>
  [[nodiscard]] state_view walk(const T &string) const {
    state *s = root;
    for (const auto character : string) {
      s = s->next[A::index(character)];
      if (!s)
        break;
    }
    return state_view(s);
  }

  template <internal::string_like_over_alphabet<A> T>
  [[nodiscard]] bool contains(const T &string) const {
    return walk(string).is_valid();
  }

  [[nodiscard]] std::vector<state_view> graph() const {
    std::unordered_set<state *> visited;
    std::stack<state *> stack;
    std::vector<state_view> order;

    visited.insert(root);
    stack.push(root);

    while (!stack.empty()) {
      state *s = stack.top();
      stack.pop();
      order.push_back(state_view{s});

      for (state *t : s->next)
        if (t != nullptr && visited.insert(t).second)
          stack.push(t);
    }

    std::ranges::sort(order, std::less{}, &state_view::length);

    return order;
  }

private:
  std::vector<std::byte> buffer;
  std::pmr::monotonic_buffer_resource pool;
  std::pmr::polymorphic_allocator<state> alloc;

  template <typename... Ts> state *new_state(Ts &&...args) {
    return alloc.template new_object<state>(std::forward<Ts>(args)...);
  }

  state *root, *last;
};
