/*
 * Copyright (c) 2020 Mateusz Nowak
 * Under MIT License: https://github.com/noqqaqq/utilities/blob/master/LICENSE
 */

#ifndef NOQQ_IS_TUPLE_CONCEPT_H_
#define NOQQ_IS_TUPLE_CONCEPT_H_

#include <concepts>
#include <tuple>

namespace noqq {
namespace internal {
template <typename> struct is_tuple_s : std::false_type {};
template <typename... T>
struct is_tuple_s<std::tuple<T...>> : std::true_type {};
} // namespace internal

template <typename V> concept is_tuple = requires {
  {
    internal::is_tuple_s<V> {}
  }
  ->std::convertible_to<std::true_type>;
};

} // namespace noqq

#endif // NOQQ_IS_TUPLE_CONCEPT_H_