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

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/typecheck.h"

namespace mcrl2 {

namespace data {

namespace detail {

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

template <typename Container>
data::sort_expression_list parameter_sorts(const Container& parameters)
{
  data::sort_expression_list sorts;
  for (const data::data_expression& e: parameters)
  {
    sorts.push_front(e.sort());
  }
  return atermpp::reverse(sorts);
}

inline
bool contains_untyped_sorts(const data::sort_expression_list& sorts)
{
  for (const data::sort_expression& s: sorts)
  {
    if (data::is_untyped_sort(s) || data::is_untyped_possible_sorts(s))
    {
      return true;
    }
  }
  return false;
}

// This class extends the public interface of the data_type_checker with some
// functions that are needed for typecheckers of other libraries.
class data_typechecker: protected data::data_type_checker
{
  private:
    data_expression UpCastNumericType(const data_expression& Par, const sort_expression& NeededType, const std::map<core::identifier_string,sort_expression>& variables)
    {
      std::map<core::identifier_string,data::sort_expression> dummy_table;
      data_expression Par1 = Par;
      sort_expression s = data::data_type_checker::UpCastNumericType(NeededType, Par.sort(), Par1, variables, variables, dummy_table, false, false, false);
      assert(s == Par1.sort());
      return data::normalize_sorts(Par1, get_sort_specification());
    }

    data::data_expression upcast_numeric_type(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables, const core::identifier_string& name, const data::data_expression_list& parameters)
    {
      try
      {
        return UpCastNumericType(d, expected_sort, variables);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(d) + " as type " + data::pp(expand_numeric_types_down(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
      }
    }

    sort_expression expand_numeric_types_down(const sort_expression& x)
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

    //we get: List of Lists of SortExpressions
    //Outer list: possible parameter types 0..nPosParsVectors-1
    //inner lists: parameter types vectors 0..n-1
    //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
    //0..n-1
    data::sort_expression_list GetNotInferredList(const sorts_list& sorts)
    {
      if (sorts.size() == 1)
      {
        return sorts.front();
      }
      data::sort_expression_list result;
      size_t n = sorts.front().size();
      std::vector<data::sort_expression_list> parameter_lists(n, data::sort_expression_list());

      for (data::sort_expression_list s: sorts)
      {
        for (size_t i = 0; i < n; s = s.tail(), i++)
        {
          parameter_lists[i] = insert_type(parameter_lists[i], s.front());
        }
      }

      for (size_t i = n; i > 0; i--)
      {
        data::sort_expression sort;
        if (parameter_lists[i - 1].size() == 1)
        {
          sort = parameter_lists[i - 1].front();
        }
        else
        {
          sort = data::untyped_possible_sorts(data::sort_expression_list(atermpp::reverse(parameter_lists[i - 1])));
        }
        result.push_front(sort);
      }
      return result;
    }

    std::pair<bool, data::sort_expression_list> AdjustNotInferredList(const data::sort_expression_list& possible_sorts, const sorts_list& sorts)
    {
      // possible_sorts -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
      // sorts -- List of (Lists of sorts)
      // returns: possible_sorts, adjusted to the elements of sorts
      // NULL if cannot be ajusted.

      //if possible_sorts has only normal types -- check if it is in sorts,
      //if so return possible_sorts, otherwise return false.
      if (!contains_untyped_sorts(possible_sorts))
      {
        if (is_contained_in(possible_sorts,sorts))
        {
          return std::make_pair(true, possible_sorts);
        }
        else
        {
          return std::make_pair(false, data::sort_expression_list());
        }
      }

      //Filter sorts to contain only compatible with TypeList lists of parameters.
      sorts_list new_sorts;
      for (const data::sort_expression_list& s: sorts)
      {
        if (is_allowed_sort_list(s, possible_sorts))
        {
          new_sorts.push_front(s);
        }
      }
      if (new_sorts.empty())
      {
        return std::make_pair(false, data::sort_expression_list());
      }
      if (new_sorts.size() == 1)
      {
        return std::make_pair(true, new_sorts.front());
      }

      // otherwise return not inferred.
      return std::make_pair(true, GetNotInferredList(atermpp::reverse(new_sorts)));
    }

    sort_expression normalize_sorts (const sort_expression& x) const
    {
      return data::normalize_sorts(x, get_sort_specification());
    }

    // returns true if s1 and s2 are equal after normalization
    bool equal_sorts(const sort_expression& s1, const sort_expression& s2) const
    {
      if (s1 == s2)
      {
        return true;
      }
      return normalize_sorts(s1) == normalize_sorts(s2);
    }

    // returns true if s matches with an element of sorts after normalization
    bool match_sort(const sort_expression& s, const sort_expression_list& sorts) const
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
    bool is_allowed_sort(const sort_expression& sort, const sort_expression& allowed_sort) const
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
    bool is_allowed_sort_list(const sort_expression_list& sorts, const sort_expression_list& allowed_sorts) const
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

    sort_expression_list insert_type(const sort_expression_list TypeList, const sort_expression Type) const
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

    bool equal_sort_lists(const sort_expression_list& x1, const sort_expression_list& x2) const
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
    bool is_contained_in(const sort_expression_list& l, const sorts_list& sorts) const
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

  public:
    /** \brief     make a data type checker.
     *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
     *  \param[in] data_spec A data specification that does not need to have been type checked.
     *  \return    a data expression where all untyped identifiers have been replace by typed ones.
     **/
    data_typechecker(const data_specification& dataspec)
      : data_type_checker(dataspec)
    {}

    bool unique_variables(const variable_list& x)
    {
      return data_type_checker::VarsUnique(x);
    }

    void print_variables(const std::map<core::identifier_string, data::sort_expression>& variables) const
    {
      std::cout << "--- variables ---" << std::endl;
      for (auto i = variables.begin(); i != variables.end(); ++i)
      {
        std::cout << i->first << " -> " << i->second << std::endl;
      }
    }

    void print_context() const
    {
      auto const& sortspec = get_sort_specification();
      std::cout << "--- basic sorts ---" << std::endl;
      for (auto const& x: sortspec.user_defined_sorts())
      {
        std::cout << x << std::endl;
      }
      std::cout << "--- aliases ---" << std::endl;
      for (auto const& x: sortspec.user_defined_aliases())
      {
        std::cout << x << std::endl;
      }
      std::cout << "--- user constants ---" << std::endl;
      for (auto i = user_constants.begin(); i != user_constants.end(); ++i)
      {
        std::cout << i->first << " -> " << i->second << std::endl;
      }
      std::cout << "--- user functions ---" << std::endl;
      for (auto i = user_functions.begin(); i != user_functions.end(); ++i)
      {
        std::cout << i->first << " -> " << i->second << std::endl;
      }
    }

    void check_sort_list_is_declared(const sort_expression_list& x) const
    {
      return sort_type_checker::check_sort_list_is_declared(x);
    }

    void check_sort_is_declared(const sort_expression& x) const
    {
      return sort_type_checker::check_sort_is_declared(x);
    }

    data_specification typechecked_data_specification() const
    {
      return type_checked_data_spec;
    }

    data::data_expression typecheck_data_expression(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables)
    {
      mCRL2log(log::debug) << "--- Typechecking " << d << " (" << atermpp::aterm(d) << ") with expected sort = " << expected_sort << std::endl;
      // print_context();
      // print_variables(variables);
      data::data_expression result = (*this)(d, expected_sort, variables);
      mCRL2log(log::debug) << "--- Typechecking result = " << result << std::endl;
      assert(data::normalize_sorts(result, get_sort_specification()) == result);
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
      assert(data::normalize_sorts(result, get_sort_specification()) == result);
      return result;
    }

    std::pair<data::data_expression_list, data::sort_expression_list> match_parameters(const data::data_expression_list& parameters,
                                                                                       const sorts_list& parameter_list,
                                                                                       const std::map<core::identifier_string, data::sort_expression>& variables,
                                                                                       const core::identifier_string& name,
                                                                                       const std::string& msg
                                                                                      )
    {
      if (parameter_list.empty())
      {
        throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                        + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                        + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
      }
      sort_expression_list expected_sorts = GetNotInferredList(parameter_list);
      data::sort_expression_list possible_sorts = expected_sorts;
      data::data_expression_vector new_parameters(parameters.begin(), parameters.end());
      auto p1 = new_parameters.begin();
      auto p2 = possible_sorts.begin();
      for (; p1 != new_parameters.end(); ++p1, ++p2)
      {
        data::data_expression& e = *p1;
        const data::sort_expression& expected_sort = *p2;
        e = typecheck_data_expression_nothrow(e, expected_sort, variables, name, parameters);
      }

      std::pair<bool, data::sort_expression_list> p = AdjustNotInferredList(parameter_sorts(new_parameters), parameter_list);
      possible_sorts = p.second;

      if (!p.first)
      {
        possible_sorts = expected_sorts;
        auto q1 = new_parameters.begin();
        auto q2 = possible_sorts.begin();
        for (; q1 != new_parameters.end(); ++q1, ++q2)
        {
          data::data_expression& e = *q1;
          data::sort_expression expected_sort = *q2;
          e = upcast_numeric_type(e, expected_sort, variables, name, parameters);
        }

        std::pair<bool, data::sort_expression_list> p = AdjustNotInferredList(parameter_sorts(new_parameters), parameter_list);
        possible_sorts = p.second;

        if (!p.first)
        {
          throw mcrl2::runtime_error("no " + msg + " " + core::pp(name) + "with type " + data::pp(parameter_sorts(new_parameters)) + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
        }
      }
      if (contains_untyped_sorts(possible_sorts))
      {
        throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(name));
      }
      data::data_expression_list result(new_parameters.begin(), new_parameters.end());
      assert(data::normalize_sorts(result, get_sort_specification()) == result);
      return std::make_pair(result, possible_sorts);
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
      assert(data::normalize_sorts(result, get_sort_specification()) == result);
      return result;
    }

    data::assignment_list typecheck_assignments(const data::assignment_list& assignments, const std::map<core::identifier_string, data::sort_expression>& variables)
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
          rhs_sort = TraverseVarConsTypeD(variables, variables, rhs, ExpandNumTypesDown(lhs_sort));
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + ".");
        }

        data::sort_expression temp;
        if (!TypeMatchA(lhs_sort, rhs_sort, temp))
        {
          //upcasting
          try
          {
            std::map<core::identifier_string,data::sort_expression> dummy_table;
            rhs_sort = data_type_checker::UpCastNumericType(lhs_sort, rhs_sort, rhs, variables, variables, dummy_table, false);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + data::pp(rhs) + " to type " + data::pp(lhs_sort));
          }
        }
        result.push_front(data::assignment(a.lhs(), rhs));
        sorts.push_front(lhs_sort);
      }
      result == atermpp::reverse(result);
      result = data::normalize_sorts(result, get_sort_specification());
      // assert(data::normalize_sorts(result, get_sort_specification()) == result);
      return result;
    }

    data_expression typecheck_untyped_data_parameter(const core::identifier_string& name, const data_expression_list& parameters, const std::map<core::identifier_string, data::sort_expression>& variables, const data::sort_expression& expected_sort = untyped_sort())
    {
      data_expression result;
      if (parameters.empty())
      {
        result = (*this)(untyped_identifier(name), expected_sort, variables);
      }
      else
      {
        result = (*this)(application(untyped_identifier(name), parameters), expected_sort, variables);
      }
      assert(data::normalize_sorts(result, get_sort_specification()) == result);
      return result;
    }
};

} // namespace detail

// Adds the elements of variables to variable_map
// Throws an exception if a typecheck error is encountered
template <typename VariableContainer>
void add_context_variables(std::map<core::identifier_string, data::sort_expression>& variable_map, const VariableContainer& variables, const data::detail::data_typechecker& data_typechecker)
{
  for (const data::variable& v: variables)
  {
    data_typechecker.check_sort_is_declared(v.sort());
    auto i = variable_map.find(v.name());
    if (i == variable_map.end())
    {
      variable_map[v.name()] = v.sort();
    }
    else
    {
      throw mcrl2::runtime_error("attempt to overload global variable " + core::pp(v.name()));
    }
  }
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_TYPECHECKER_H
