#pragma once

#include "boost/variant.hpp"
#include <utility>

namespace Chess::utils {

struct Position {
  int x;
  int y;
};

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

template <class QueriedType, class Variant>
bool variant_contains(Variant const& variant)
{
  return visit_variant(variant, 
    [](QueriedType const&)
    {
      return true;
    },
    [](auto const&)
    {
      return false;
    }
  );
}

}

