// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_typechecker.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_TYPECHECKER_H
#define MCRL2_DATA_DETAIL_DATA_TYPECHECKER_H

#include "mcrl2/data/typecheck.h"

namespace mcrl2 {

namespace data {

namespace detail {

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

// This class extends the public interface of the data_type_checker with some
// functions that are needed for typecheckers of other libraries.
struct data_typechecker: protected data::data_type_checker
{
  /** \brief     make a data type checker.
   *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
   *  \param[in] data_spec A data specification that does not need to have been type checked.
   *  \return    a data expression where all untyped identifiers have been replace by typed ones.
   **/
  data_typechecker(const data_specification& dataspec)
    : data_type_checker(dataspec)
  {}

  void check_sort_list_is_declared(const sort_expression_list& x)
  {
    return sort_type_checker::check_sort_list_is_declared(x);
  }

  void check_sort_is_declared(const sort_expression& x)
  {
    return sort_type_checker::check_sort_is_declared(x);
  }

  bool VarsUnique(const variable_list& VarDecls)
  {
    return data_type_checker::VarsUnique(VarDecls);
  }

  sort_expression normalize_sorts(const sort_expression& x)
  {
    return data::normalize_sorts(x, get_sort_specification());
  }

  data_expression UpCastNumericType(const data_expression& Par, const sort_expression& NeededType, const std::map<core::identifier_string,sort_expression>& variables)
  {
    std::map<core::identifier_string,data::sort_expression> dummy_table;
    data_expression Par1 = Par;
    sort_expression s = data::data_type_checker::UpCastNumericType(NeededType, Par.sort(), Par1, variables, variables, dummy_table, false, false, false);
    assert(s == Par1.sort());
    return data::normalize_sorts(Par1, get_sort_specification());
  }

  // returns true if s1 and s2 are equal after normalization
  bool equal_sorts(const sort_expression& s1, const sort_expression& s2)
  {
    if (s1 == s2)
    {
      return true;
    }
    return normalize_sorts(s1) == normalize_sorts(s2);
  }

  // returns true if s matches with an element of sorts after normalization
  bool match_sort(const sort_expression& s, const sort_expression_list& sorts)
  {
    for (const sort_expression& s1: sorts)
    {
      if (equal_sorts(s, s1))
      {
        return true;
      }
    }
    return false;
  }

  // returns true if s is allowed by allowed_sort, meaning that allowed_sort is an untyped sort,
  // or allowed_sort is a sequence that contains a matching sort
  bool is_allowed_sort(const sort_expression& sort, const sort_expression& allowed_sort)
  {
    if (is_untyped_sort(data::sort_expression(allowed_sort)))
    {
      return true;
    }
    if (is_untyped_possible_sorts(allowed_sort))
    {
      return match_sort(sort, atermpp::down_cast<const untyped_possible_sorts>(allowed_sort).sorts());
    }

    //PosType is a normal type
    return equal_sorts(sort, allowed_sort);
  }

  // returns true if all elements of sorts are allowed by the corresponding entries of allowed_sorts
  bool is_allowed_sort_list(const sort_expression_list& sorts, const sort_expression_list& allowed_sorts)
  {
    assert(sorts.size() == allowed_sorts.size());
    auto j = allowed_sorts.begin();
    for (auto i = sorts.begin(); i != sorts.end(); ++i,++j)
    {
      if (!is_allowed_sort(*i, *j))
      {
        return false;
      }
    }
    return true;
  }

  sort_expression_list insert_type(const sort_expression_list TypeList, const sort_expression Type)
  {
    for (sort_expression_list OldTypeList = TypeList; !OldTypeList.empty(); OldTypeList = OldTypeList.tail())
    {
      if (equal_sorts(OldTypeList.front(), Type))
      {
        return TypeList;
      }
    }
    sort_expression_list result = TypeList;
    result.push_front(Type);
    return result;
  }

  bool equal_sort_lists(const sort_expression_list& x1, const sort_expression_list& x2)
  {
    if (x1 == x2)
    {
      return true;
    }
    if (x1.size() != x2.size())
    {
      return false;
    }
    return std::equal(x1.begin(), x1.end(), x2.begin(), [&](const sort_expression& s1, const sort_expression& s2) { return equal_sorts(s1, s2); });
  }

  // returns true if l is (after unwinding) contained in sorts
  bool is_contained_in(const sort_expression_list& l, const sorts_list& sorts)
  {
    for (const sort_expression_list& m: sorts)
    {
      if (equal_sort_lists(l, m))
      {
        return true;
      }
    }
    return false;
  }

  sort_expression ExpandNumTypesDown(const sort_expression& x)
  {
    return data::normalize_sorts(data_type_checker::ExpandNumTypesDown(x), get_sort_specification());
  }

  /** \brief     Type check a data expression.
   *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
   *  \param[in] x A data expression that has not been type checked.
   *  \param[in] expected_sort The expected sort of the data expression.
   *  \param[in] variable_constext a mapping of variable names to their types.
   *  \return the type checked data expression.
   **/
  data_expression operator()(const data_expression& x,
                             const sort_expression& expected_sort,
                             const std::map<core::identifier_string, sort_expression>& variable_context
                            )
  {
    data_expression x1 = x;
    TraverseVarConsTypeD(variable_context, variable_context, x1, expected_sort);
    return data::normalize_sorts(x1, get_sort_specification());
  }

  data_specification typechecked_data_specification()
  {
    return type_checked_data_spec;
  }
};

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_TYPECHECKER_H
