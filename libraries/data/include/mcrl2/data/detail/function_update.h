// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/function_update.h
/// \brief This files contains the implementation of the function "is_not_a_function_update"
///        which was declared as"defined_by_code" in the file build/code_generation/data_types/function_update.spec. 

#ifndef MCRL2_DATA_DETAIL_FUNCTION_UPDATE_H
#define MCRL2_DATA_DETAIL_FUNCTION_UPDATE_H

#include "mcrl2/data/find.h"

namespace mcrl2
{
namespace data
{

// Prototype. 
inline function_symbol function_update(const sort_expression& s, const sort_expression& t);
inline function_symbol function_update_stable(const sort_expression& s, const sort_expression& t);


inline data_expression is_not_a_function_update_manual_implementation(const data_expression& f)
{
  function_sort sort=atermpp::down_cast<function_sort>(f.sort());
  assert(sort.domain().front().size()==1);

  std::set<function_symbol> function_symbol_set = find_function_symbols(f);
  if (function_symbol_set.count(function_update_stable(sort.domain().front(), sort.codomain()))==0 &&
      function_symbol_set.count(function_update(sort.domain().front(), sort.codomain()))==0)
  {
    std::set<variable> variable_set = find_free_variables(f);
    if (variable_set.size()==0)
    {
      return sort_bool::true_();
    }
  }
  return sort_bool::false_();
}

inline data_expression if_always_else_manual_implementation(const data_expression& b, const data_expression& e1, const data_expression& e2)
{
  if (b==sort_bool::true_())
  {
    return e1;
  }
  return e2;
}

} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FUNCTION_UPDATE_H
