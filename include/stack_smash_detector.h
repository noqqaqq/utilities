/*
 * Copyright (c) 2019 Mateusz Nowak
 * Under MIT License: https://github.com/noqqaqq/utilities/blob/master/LICENSE
 */

#ifndef NOQQ_STACK_SMASH_DETECTOR_H_
#define NOQQ_STACK_SMASH_DETECTOR_H_

#include "function_t.h"
#include <iostream>
#include <type_traits>
#include <utility>

namespace noqq
{

template <class ObjectT>
struct stack_smash_detector
{
  template <typename OTi = ObjectT,
            typename = typename std::enable_if_t<std::is_void_v<OTi>>>
  stack_smash_detector() : object_{nullptr} {}

  template <typename OTi = ObjectT,
            typename = typename std::enable_if_t<!std::is_void_v<OTi>>,
            typename = OTi>
  stack_smash_detector(OTi &object) : object_{&object} {}

  stack_smash_detector(stack_smash_detector const &) = delete;
  stack_smash_detector &operator=(stack_smash_detector const &) = delete;
  stack_smash_detector(stack_smash_detector &&) = delete;
  stack_smash_detector &operator=(stack_smash_detector) = delete;
  ~stack_smash_detector() = default;

  template <typename ReturnT, typename... Args>
  struct caller
  {
    using function_concrete_t =
        typename noqq::function_t<ObjectT>::template type<ReturnT, Args...>;

  private:
    function_concrete_t func_;
    stack_smash_detector &parent_;

  public:
    caller(stack_smash_detector &parent, function_concrete_t func)
        : parent_{parent}, func_{func} {}

    ReturnT __attribute__((noinline, optimize("O0"))) operator()(Args... args)
    {
      constexpr uint32_t CANARY__V{0xDEADBEEF};
      uint32_t CANARY__[4]{CANARY__V, CANARY__V, CANARY__V, CANARY__V};
#define CANARY_CHECK                                          \
  if (CANARY__[0] != CANARY__V || CANARY__[1] != CANARY__V || \
      CANARY__[2] != CANARY__V || CANARY__[3] != CANARY__V)   \
  {                                                           \
    std::cout << "Stack Smash Error" << std::endl;            \
    std::abort();                                             \
  }
      if constexpr (std::is_void_v<ReturnT>)
      {
        if constexpr (std::is_same_v<void, ObjectT>)
        {
          func_(std::forward<Args>(args)...);
        }
        else
        {
          (parent_.object_->*func_)(std::forward<Args>(args)...);
        }
        CANARY_CHECK;
      }
      else
      {
        if constexpr (std::is_same_v<void, ObjectT>)
        {
          auto &&retval{func_(std::forward<Args>(args)...)};
          CANARY_CHECK;
          return retval;
        }
        else
        {
          auto &&retval{(parent_.object_->*func_)(std::forward<Args>(args)...)};
          CANARY_CHECK;
          return retval;
        }
      }
    }
  };

  template <typename ReturnT, typename... Args>
  caller<ReturnT, Args...> operator()(
      typename function_t<ObjectT>::template type<ReturnT, Args...> func)
  {
    return caller{*this, func};
  }

private:
  ObjectT *const object_;
};

} // namespace noqq

#define func_caller_o(i, f) noqq::stack_smash_detector<decltype(i)>{i}(&decltype(i)::f)
#define func_caller(f) noqq::stack_smash_detector<void>()(f)

#endif