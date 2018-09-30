// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file move_constants_to_substitution.h
/// \brief This file contains the function move_constants_to_substitution. This
///        function is used to replace non trivial constants in an expression by
///        variables and add an appropriate assignment to a substitution. When
///        rewriting these expressions repeatedly, this saves time, as the
///        expressions in the substitution are taken as normal forms. 


#ifndef MCRL2_DATA_MOVE_CONSTANT_TO_SUBSTITUTION_H
#define MCRL2_DATA_MOVE_CONSTANT_TO_SUBSTITUTION_H


#include <unordered_map>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief This function first splits the given condition e into real conditions and
///        non real conditions.
/// \detail This function first uses split_condition_aux to split the condition e. Then
//          it merges equal real conditions by merging the non-real conditions. No further
//          calculations take place with the non-real conditions, but if the non-real conditions
//          lead to unnecessary copying, this may lead to a huge overhead in removing the
//          real conditions.
template <class SUBSTITUTION>
data_expression move_constants_to_substitution(const data_expression& t,
                                               rewriter& r,
                                               SUBSTITUTION& sigma,
                                               std::unordered_map<data_expression, variable>& expression_to_variable_map,
                                               set_identifier_generator& identifier_generator)
{
  if (is_application(t))
  {
    if (find_free_variables(t).size()==0)
    {
      std::unordered_map<data_expression, variable>::const_iterator i=expression_to_variable_map.find(t);
      if (i==expression_to_variable_map.end()) // expression is not found.
      {
        const variable v(identifier_generator("@rewr_var"),t.sort()); 
        sigma[v]=r(t,sigma);  // sigma is here not necessary. 
        expression_to_variable_map[t]=v;
        return v;
      }
      else
      {
        return i->second;
      }
    }
    else
    {
      const application& ta=atermpp::down_cast<application>(t);
      const data_expression h=move_constants_to_substitution(ta.head(),r,sigma,expression_to_variable_map,identifier_generator);
      return application(h,
                         ta.begin(),
                         ta.end(),
                         [&](const data_expression& t)
                              { return move_constants_to_substitution(t,
                                                                      r,
                                                                      sigma,
                                                                      expression_to_variable_map,
                                                                      identifier_generator); });
    }
  }
  
  if (is_abstraction(t))
  {
    const abstraction& ta=atermpp::down_cast<abstraction>(t);
    return abstraction(ta.binding_operator(),
                       ta.variables(),
                       move_constants_to_substitution(ta.body(),r,sigma,expression_to_variable_map,identifier_generator));
  }

  // For variables and constants no substitution is provided. 
  assert(is_variable(t) || is_constant(t));
  return t;
}

} // end namespace detail


} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MOVE_CONSTANT_TO_SUBSTITUTION_H
