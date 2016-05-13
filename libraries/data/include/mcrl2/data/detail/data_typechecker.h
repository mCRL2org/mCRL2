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

#include <set>
#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/detail/variable_context.h"

namespace mcrl2 {

namespace data {

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

namespace detail {

inline
bool unique_variables(const variable_list& x)
{
  std::set<core::identifier_string> names;
  for (const variable& v: x)
  {
    if (!names.insert(v.name()).second) // The variable name is already in the set.
    {
      return false;
    }
  }
  return true;
}

// This class extends the public interface of the data_type_checker with some
// functions that are needed for typecheckers of other libraries.
class data_typechecker
{
  private:
    data::data_type_checker m_data_type_checker;

    sort_expression expand_numeric_types_down(const sort_expression& x)
    {
      return data::normalize_sorts(m_data_type_checker.expand_numeric_types_down(x), m_data_type_checker.get_sort_specification());
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
                               const std::map<core::identifier_string, sort_expression>& variables
                              )
    {
      data_expression x1 = x;
      m_data_type_checker.visit_data_expression(variables, x1, expected_sort);
      return data::normalize_sorts(x1, m_data_type_checker.get_sort_specification());
    }

    // Apparently this approach is too simple
    data::assignment_list typecheck_assignments_does_not_work(const data::assignment_list& assignments, const std::map<core::identifier_string, data::sort_expression>& variables)
    {
      data::assignment_list result;
      for (const data::assignment& a: assignments)
      {
        data::sort_expression expected_sort = expand_numeric_types_down(a.lhs().sort());
        data::data_expression rhs = (*this)(a.rhs(), expected_sort, variables);
        result.push_front(data::assignment(a.lhs(), rhs));
      }
      result = atermpp::reverse(result);
      assert(data::normalize_sorts(result, m_data_type_checker.get_sort_specification()) == result);
      return result;
    }

  public:
    /** \brief     make a data type checker.
     *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
     *  \param[in] data_spec A data specification that does not need to have been type checked.
     *  \return    a data expression where all untyped identifiers have been replace by typed ones.
     **/
    data_typechecker(const data_specification& dataspec)
      : m_data_type_checker(dataspec)
    {}

    const sort_type_checker& get_sort_type_checker() const
    {
      return m_data_type_checker;
    }

    void check_sort_is_declared(const sort_expression& x) const
    {
      get_sort_type_checker()(x);
    }

    void check_sort_list_is_declared(const sort_expression_list& x) const
    {
      for (const sort_expression& s: x)
      {
        check_sort_is_declared(s);
      }
    }

    data_specification typechecked_data_specification() const
    {
      return m_data_type_checker.typechecked_data_specification();
    }

    data::data_expression typecheck_data_expression(const data::data_expression& d, const data::sort_expression& expected_sort, const detail::variable_context& variable_context)
    {
      mCRL2log(log::debug) << "--- Typechecking " << d << " (" << atermpp::aterm(d) << ") with expected sort = " << expected_sort << std::endl;
      data::data_expression result = (*this)(d, expected_sort, variable_context.context());
      mCRL2log(log::debug) << "--- Typechecking result = " << result << std::endl;
      assert(data::normalize_sorts(result, m_data_type_checker.get_sort_specification()) == result);
      return result;
    }

    data::data_expression typecheck_data_expression_nothrow(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables, const core::identifier_string& name, const data::data_expression_list& parameters)
    {
      data::data_expression result;
      try
      {
        result = (*this)(d, expected_sort, variables);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(d) + " as type " + data::pp(expand_numeric_types_down(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
      }
      assert(data::normalize_sorts(result, m_data_type_checker.get_sort_specification()) == result);
      return result;
    }

    data::data_expression upcast_numeric_type(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables, const core::identifier_string& name, const data::data_expression_list& parameters)
    {
      try
      {
        return upcast_numeric_type(d, expected_sort, variables);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(d) + " as type " + data::pp(expand_numeric_types_down(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
      }
    }

    data_expression upcast_numeric_type(const data_expression& Par, const sort_expression& NeededType, const std::map<core::identifier_string,sort_expression>& variables)
    {
      std::map<core::identifier_string,data::sort_expression> dummy_table;
      data_expression Par1 = Par;
      sort_expression s = m_data_type_checker.upcast_numeric_type(NeededType, Par.sort(), Par1, variables, dummy_table, false, false, false);
      assert(s == Par1.sort());
      return data::normalize_sorts(Par1, m_data_type_checker.get_sort_specification());
    }

    data::assignment_list typecheck_assignments(const data::assignment_list& assignments, const detail::variable_context& variable_context)
    {
      std::map<core::identifier_string, data::sort_expression> sort_map;
      data::assignment_list result;
      data::sort_expression_list sorts;
      for (const data::assignment& a: assignments)
      {
        const core::identifier_string& name = a.lhs().name();
        if (sort_map.count(name) > 0)
        {
          throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(name) + ")");
        }

        data::sort_expression lhs_sort = a.lhs().sort();
        check_sort_is_declared(lhs_sort);

        sort_map[name] = lhs_sort;

        data::data_expression rhs = a.rhs();
        data::sort_expression rhs_sort;
        try
        {
          rhs_sort = m_data_type_checker.visit_data_expression(variable_context.context(), rhs, m_data_type_checker.expand_numeric_types_down(lhs_sort));
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + ".");
        }

        data::sort_expression temp;
        if (!m_data_type_checker.type_match(lhs_sort, rhs_sort, temp))
        {
          //upcasting
          try
          {
            std::map<core::identifier_string,data::sort_expression> dummy_table;
            rhs_sort = m_data_type_checker.upcast_numeric_type(lhs_sort, rhs_sort, rhs, variable_context.context(), dummy_table, false);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + data::pp(rhs) + " to type " + data::pp(lhs_sort));
          }
        }
        result.push_front(data::assignment(a.lhs(), rhs));
        sorts.push_front(lhs_sort);
      }
      result = atermpp::reverse(result);
      result = data::normalize_sorts(result, m_data_type_checker.get_sort_specification());
      return result;
    }
};

} // namespace detail

inline
data_expression typecheck_untyped_data_parameter(detail::data_typechecker& typechecker,
                                                 const core::identifier_string& name,
                                                 const data_expression_list& parameters,
                                                 const data::sort_expression& expected_sort,
                                                 const detail::variable_context& variable_context
                                                )
{
  if (parameters.empty())
  {
    return typechecker.typecheck_data_expression(untyped_identifier(name), expected_sort, variable_context);
  }
  else
  {
    return typechecker.typecheck_data_expression(application(untyped_identifier(name), parameters), expected_sort, variable_context);
  }
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_TYPECHECKER_H
