#pragma once

#include "boost/variant.hpp"
#include <utility>

// TODO Simplify make_visitor

namespace Chess::utils {

struct Position {
  int x;
  int y;
};

inline bool operator==(Position const lhs, Position const rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline Position operator-(Position const lhs, Position const rhs)
{
  return Position {
    lhs.x - rhs.x,
    lhs.y - rhs.y
  };
}

inline Position operator+(Position const lhs, Position const rhs)
{
  return Position{
    lhs.x + rhs.x,
    lhs.y + rhs.y
  };
}

template <class ResultType, class Lambda, class... Lambdas>
struct Made_visitor: Lambda, Made_visitor<ResultType, Lambdas...> {
    using Lambda::operator();
    using Made_visitor<ResultType, Lambdas...>::operator();
    using result_type = ResultType;
    
    template <class L, class... Ls>
    Made_visitor(L&& l, Ls&&... ls)
        : Lambda(std::forward<L>(l))
        , Made_visitor<ResultType, Lambdas...>(std::forward<Ls>(ls)...)
    {}
};

template <class ResultType, class Lambda>
struct Made_visitor<ResultType, Lambda> : Lambda {
    using Lambda::operator();
    using result_type = ResultType;
    
    template <class L>
    explicit Made_visitor(L&& l)
        : Lambda(std::forward<L>(l))
    {}
};

template <class ResultType, class... Lambdas>
auto make_visitor(Lambdas&&... lambdas)
{
    return Made_visitor<ResultType, Lambdas...>(
      std::forward<Lambdas>(lambdas)...);
}

template <class ResultType, class Variant, class... Lambdas>
ResultType visit_variant(Variant& variant, Lambdas&&... lambdas)
{
  return boost::apply_visitor(
    make_visitor<ResultType>(std::forward<Lambdas>(lambdas)...),
    variant
  );
}

namespace optionalized {

template <class Container, class Pred>
auto find_if(Container& c, Pred pred)
  -> boost::optional<typename Container::value_type>
{
  const auto i = std::find_if(c.begin(), c.end(), pred);
  if (i == c.end())
    return boost::none;
  return *i;
}

}

}
