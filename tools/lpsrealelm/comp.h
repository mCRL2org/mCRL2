// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file comp.h
/// \brief Provides an implementation of sort Comp.

#ifndef MCRL2_LPSRTA_COMP_H
#define MCRL2_LPSRTA_COMP_H

#include <iostream>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data_application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/detail/data_implementation_concrete.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

/// \brief Name for the sort Comp.
/// Comp is used as sort indicating relative order in lpsrealelm.
inline identifier_string comp_name()
{
  return identifier_string("Comp");
}

/// \brief Name for the operator smaller on sort Comp.
inline
identifier_string smaller_name()
{
  return identifier_string("smaller");
}

/// \brief Name for the operator equal on sort Comp.
inline
identifier_string equal_name()
{
  return identifier_string("equation");
}

/// \brief Name for the operator larger on sort Comp.
inline
identifier_string larger_name()
{
  return identifier_string("larger");
}

/// \brief Name for the operator is_smaller on sort Comp.
inline
identifier_string is_smaller_name()
{
  return identifier_string("is_smaller");
}

/// \brief Name of the operator is_equal on sort Comp.
inline
identifier_string is_equal_name()
{
  return identifier_string("is_equal");
}

/// \brief Name of the operator is_larger on sort Comp.
inline
identifier_string is_larger_name()
{
  return identifier_string("is_larger");
}

/// \brief Sort expression for sort Comp.
inline
sort_expression comp()
{
  return sort_identifier(comp_name());
}

/// \brief Constructor smaller for sort Comp.
inline
data_operation smaller()
{
  return data_operation(smaller_name(), comp());
}

/// \brief Constructor equal for sort Comp.
inline
data_operation equal()
{
  return data_operation(equal_name(), comp());
}

/// \brief Constructor larger for sort Comp.
inline
data_operation larger()
{
  return data_operation(larger_name(), comp());
}

/// \brief Sort expression Comp -> Bool
inline
sort_expression comp2bool()
{
  return sort_arrow(make_list(comp()), sort_expr::bool_());
}

/// \brief Operation is_smaller for sort Comp.
inline
data_operation is_smaller()
{
  return data_operation(is_smaller_name(), comp2bool());
}

/// \brief Operation is_equal for sort Comp.
inline
data_operation is_equal()
{
  return data_operation(is_equal_name(), comp2bool());
}

/// \brief Operation is_larger for sort Comp.
inline
data_operation is_larger()
{
  return data_operation(is_larger_name(), comp2bool());
}

/// \brief Application of is_smaller on a data expression e.
inline
data_application is_smaller(const data_expression& e)
{
  return data_application(is_smaller(), make_list(e));
}

/// \brief Application of is_equal on a data expression e.
inline
data_application is_equal(const data_expression& e)
{
  return data_application(is_equal(), make_list(e));
}

/// \brief Application of is_larger on a data_expression e.
inline
data_application is_larger(const data_expression& e)
{
  return data_application(is_larger(), make_list(e));
}

/// \brief Add declarations for sort Comp to data specification s
/// \param s A data specification.
/// \ret s to which declarations for sort Comp have been added.
inline
data_specification add_comp_sort(const data_specification& s)
{
  // Constructors
  aterm_appl comp_smaller = gsMakeStructCons(smaller_name(), aterm_list(), is_smaller_name());
  aterm_appl comp_equal   = gsMakeStructCons(equal_name(), aterm_list(), is_equal_name());
  aterm_appl comp_larger  = gsMakeStructCons(larger_name(), aterm_list(), is_larger_name());
  aterm_list comp_constructors = make_list(comp_smaller, comp_equal, comp_larger);

  // Build up structured sort
  aterm_appl comp_struct = gsMakeSortStruct(comp_constructors);

  // Empty data declarations and substitutions
  t_data_decls data_decls;
  initialize_data_decls(&data_decls);
  ATermList substitutions = ATmakeList0();

  // Implement Comp as structured sort, reusing data implementation
  impl_sort_struct(comp_struct, comp(), &substitutions, &data_decls);

  // Add declarations in data_decls to the specification
  sort_expression_list sorts = ATconcat(s.sorts(), data_decls.sorts);
  data_operation_list constructors = ATconcat(s.constructors(), data_decls.cons_ops);
  data_operation_list mappings = ATconcat(s.mappings(), data_decls.ops);
  data_equation_list equations = ATconcat(s.equations(), data_decls.data_eqns);

  return data_specification(sorts, constructors, mappings, equations);
}

#endif //MCRL2_LPSRTA_COMP_H

