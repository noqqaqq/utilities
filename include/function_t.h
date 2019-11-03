/*
 * Copyright (c) 2019 Mateusz Nowak
 * Under MIT License: https://github.com/noqqaqq/utilities/blob/master/LICENSE
 */

#ifndef NOQQ_FUNCTION_T_H_
#define NOQQ_FUNCTION_T_H_

namespace noqq
{

template <typename ObjectT>
struct function_t
{
  template <typename ReturnT, typename... Args>
  using type = ReturnT (ObjectT::*)(Args...);
};

template <>
struct function_t<void>
{
  template <typename ReturnT, typename... Args>
  using type = ReturnT (*)(Args...);
};

} // namespace noqq

#endif // NOQQ_FUNCTION_T_H_