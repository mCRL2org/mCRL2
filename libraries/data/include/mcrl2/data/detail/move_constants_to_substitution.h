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

#include "mcrl2/data/rewriter.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief This function first splits the given condition e into real conditions and
///        non real conditions.
/// \detail This function first uses split_condition_aux to split the condition e. Then
///         it merges equal real conditions by merging the non-real conditions. No further
///         calculations take place with the non-real conditions, but if the non-real conditions
///         lead to unnecessary copying, this may lead to a huge overhead in removing the
///         real conditions.
/// \param  t The data expression in which closed expressions are replaced by variables.
/// \param  r The rewriter used to rewrite closed expressions to normal form. 
/// \param  sigma The substitution into which the mapping from variables to expressions are inserted.
/// \param  expression_to_variable_map A map used to recall which expressions are replaced by which variables.
/// \param  identifier_generator An identifier generator, used to generate fresh variable names. 
/// \return The data expression t in which closed expressions are replaced by variables. 
template <class SUBSTITUTION>
data_expression move_constants_to_substitution(const data_expression& t,
                                               rewriter& r,
                                               SUBSTITUTION& sigma,
                                               std::unordered_map<data_expression, variable>& expression_to_variable_map,
                                               set_identifier_generator& identifier_generator)
{
  if (is_application(t))
  {
    // The line below where all variables are obtained from an expression to find out that it is closed 
    // is not particularly efficient, as it is quadratic in the size of the term. If need be, this can be
    // replaced by one pass through the term, determining whether it is closed, and replacing variables at
    // the same time. 
    if (find_free_variables(t).size()==0)
    {
      std::unordered_map<data_expression, variable>::const_iterator i=expression_to_variable_map.find(t);
      if (i==expression_to_variable_map.end()) 
      {
        // expression is not found.
        const variable v(identifier_generator("@rewr_var"),t.sort()); 
        sigma[v]=r(t,sigma);  // sigma is here not necessary. 
        expression_to_variable_map[t]=v;
        return v;
      }
      else 
      {
        // A variable for the current expression already exists. 
        return i->second;
      }
    }
    else
    {
      const application& ta=atermpp::down_cast<application>(t);
      return application(ta.head(),
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
