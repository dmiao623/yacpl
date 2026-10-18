#pragma once

#include <functional>
#include <queue>
#include <vector>

template <typename T>
using min_priority_queue =
    std::priority_queue<T, std::vector<T>, std::greater<T>>;
