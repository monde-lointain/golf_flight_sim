#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

template <typename Container,
          typename T = typename std::decay<
              decltype(*std::begin(std::declval<Container>()))>::type>
T stdev_s(Container &&c) {

  auto b = std::begin(c), e = std::end(c);
  auto size = std::distance(b, e);

  auto sum = std::accumulate(b, e, T());
  auto mean = sum / size;

  T accum = T();
  for (const auto d : c)
    accum += (d - mean) * (d - mean);

  return static_cast<T>(std::sqrt(accum / (size - 1)));
}