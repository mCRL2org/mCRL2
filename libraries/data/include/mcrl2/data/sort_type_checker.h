// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_type_checker.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_SORT_TYPE_CHECKER_H
#define MCRL2_DATA_SORT_TYPE_CHECKER_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/utilities/logger.h"

#ifdef MCRL2_USE_NEW_TYPE_CHECKER

namespace mcrl2
{

namespace data
{

namespace detail
{
  inline
  sort_expression mapping(sort_expression s, std::map<sort_expression, basic_sort>& m)
  {
    if (m.find(s)==m.end())
    {
      return s;
    }
    return m[s];
  }

}; // namespace detail

class sort_type_checker
{
  protected:
    std::set<basic_sort> m_basic_sorts;
    std::map<basic_sort, sort_expression> m_aliases;
    std::map<sort_expression, basic_sort> m_normalized_aliases;

  public:
    /// \brief constructs a sort expression checker.
    sort_type_checker(const basic_sort_vector& sorts, const alias_vector& aliases, bool check_aliases = true)
    {
      for (const basic_sort& s: sorts)
      {
        add_basic_sort(s);
      }

      for (const alias& a: aliases)
      {
        add_alias(a);
      }

      if (check_aliases)
      {
        for (auto i = m_aliases.begin(); i != m_aliases.end(); ++i)
        {
          check_alias(i->first, i->second);
        }

        try
        {
          for (const alias& a: aliases)
          {
            (*this)(a.reference()); // Type check sort expression.
          }
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of aliases failed");
        }
      }
    }

    /** \brief     Type check a sort expression.
    *  Throws an exception if the expression is not well typed.
    *  \param[in] s A sort expression that has not been type checked.
    **/
    void operator()(const sort_expression& x)
    {
      check_sort_is_declared(x);
    }

  protected:
    // Normalizes alias x, and adds it to m_normalized_aliases
    // Fill m_normalized_aliases. Simple aliases are stored from left to
    // right. If the right hand side is non trivial (struct, list, set or bag)
    // the alias is stored from right to left.
    void normalize_alias(const data::alias& x)
    {
      const data::basic_sort& lhs = x.name();
      const data::sort_expression& rhs = x.reference();
      if (data::is_structured_sort(rhs) || data::is_function_sort(rhs) || data::is_container_sort(rhs))
      {
        // We deal here with a declaration of the shape sort A=ComplexType.
        // Rewrite every occurrence of ComplexType to A. Suppose that there are
        // two declarations of the shape sort A=ComplexType; B=ComplexType then
        // ComplexType is rewritten to A and B is also rewritten to A.

        auto j = m_normalized_aliases.find(rhs);
        if (j != m_normalized_aliases.end())
        {
          m_normalized_aliases[lhs] = j->second;
        }
        else
        {
          m_normalized_aliases[rhs] = lhs;
        }
      }
      else
      {
        // We are dealing with a sort declaration of the shape sort A=B.
        // Every occurrence of sort A is normalised to sort B.
        m_normalized_aliases[lhs] = atermpp::down_cast<data::basic_sort>(rhs);
      }
    }

    void add_alias(const data::alias& x)
    {
      mCRL2log(log::debug) << "Add sort alias " << x.name() << "  " << x.reference() << "" << std::endl;
      add_basic_sort(x.name());
      m_aliases[x.name()] = x.reference();
      normalize_alias(x);
    }

    // Throws a runtime_error if a rewriting loop is detected.
    // Also closes the mapping m_normalized_aliases under itself(!)
    void check_alias_circularity(const data::basic_sort& lhs, const data::sort_expression& rhs)
    {
      std::set<data::sort_expression> sort_already_seen;
      data::sort_expression result_sort = rhs;

      std::set<data::sort_expression> all_sorts;
      if (data::is_container_sort(rhs) || data::is_function_sort(rhs))
      {
        all_sorts = data::find_sort_expressions(lhs);
      }
      auto i = m_normalized_aliases.find(result_sort);
      while (i != m_normalized_aliases.end())
      {
        sort_already_seen.insert(result_sort);
        result_sort = i->second;
        if (sort_already_seen.count(result_sort))
        {
          throw mcrl2::runtime_error("Sort alias " + data::pp(result_sort) + " is defined in terms of itself.");
        }

        for (const data::sort_expression& sort: all_sorts)
        {
          if (sort == result_sort)
          {
            throw mcrl2::runtime_error("Sort alias " + data::pp(lhs) + " depends on sort " + data::pp(result_sort) + ", which is circularly defined.\n");
          }
        }
        i = m_normalized_aliases.find(result_sort);
      }

      // So the normalised sort of lhs is result_sort.
      m_normalized_aliases[lhs] = atermpp::down_cast<basic_sort>(result_sort);
    }

    // Throws an exception if the alias lhs -> rhs is recursively defined
    void check_alias_recursion(const data::basic_sort& lhs, const data::sort_expression& rhs)
    {
      std::set<data::basic_sort> visited;
      if (check_for_sort_alias_loop_through_function_sort_via_expression(rhs, lhs, visited, false))
      {
        throw mcrl2::runtime_error("sort " + data::pp(lhs) + " is recursively defined via a function sort, or a set or a bag type container");
      }
    }

    // Throws an exception if there is a problem with the alias
    void check_alias(const data::basic_sort& lhs, const data::sort_expression& rhs)
    {
      check_alias_recursion(lhs, rhs);
      check_alias_circularity(lhs, rhs);
    }

    bool check_for_sort_alias_loop_through_function_sort(
      const basic_sort& start_search,
      const basic_sort& end_search,
      std::set < basic_sort > &visited,
      const bool observed_a_sort_constructor)
    {
      const std::map<basic_sort, sort_expression>::const_iterator i=m_aliases.find(start_search.name());

      if (i==m_aliases.end())
      {
        // start_search is not a sort alias, and hence not a recursive sort.
        return false;
      }
      sort_expression aterm_reference= i->second;

      if (start_search==end_search)
      {
        // We found a loop.
        return observed_a_sort_constructor;
      }
      if (visited.find(start_search)!=visited.end())
      {
        // start_search has already been encountered. end_search will not be found via this path.
        return false;
      }

      visited.insert(start_search);
      const sort_expression reference(aterm_reference);
      return check_for_sort_alias_loop_through_function_sort_via_expression(reference,end_search,visited,observed_a_sort_constructor);
    }

    bool check_for_sort_alias_loop_through_function_sort_via_expression(
      const sort_expression& sort_expression_start_search,
      const basic_sort& end_search,
      std::set < basic_sort > &visited,
      const bool observed_a_sort_constructor)
    {
      if (is_basic_sort(sort_expression_start_search))
      {
        const basic_sort start_search(sort_expression_start_search);
        if (end_search==start_search)
        {
          return observed_a_sort_constructor;
        }
        else
        {
          return check_for_sort_alias_loop_through_function_sort(start_search,end_search,visited,observed_a_sort_constructor);
        }
      }

      if (is_container_sort(sort_expression_start_search))
      {
        // A loop through a list container is allowed, but a loop through a set or bag container
        // is problematic.
        const container_sort start_search_container(sort_expression_start_search);
        return check_for_sort_alias_loop_through_function_sort_via_expression(
                 start_search_container.element_sort(),end_search,visited,
                 start_search_container.container_name()!=list_container());
      }

      if (is_function_sort(sort_expression_start_search))
      {
        const function_sort f_start_search(sort_expression_start_search);
        if (check_for_sort_alias_loop_through_function_sort_via_expression(f_start_search.codomain(),end_search,visited,true))
        {
          return true;
        }
        for (const sort_expression& s: f_start_search.domain())
        {
          if (check_for_sort_alias_loop_through_function_sort_via_expression(s, end_search,visited,true))
          {
            return true;
          }
        }
        // end_search has not been found, so:
        return false;
      }

      if (is_structured_sort(sort_expression_start_search))
      {
        const structured_sort struct_start_search(sort_expression_start_search);
        for (const function_symbol& f: struct_start_search.constructor_functions())
        {
          if (is_function_sort(f.sort()))
          {
            const sort_expression_list domain_sorts=function_sort(f.sort()).domain();
            for (const sort_expression& s: domain_sorts)
            {
              if (check_for_sort_alias_loop_through_function_sort_via_expression(s, end_search,visited,observed_a_sort_constructor))
              {
                return true;
              }
            }
          }
        }
        return false;

      }

      assert(0); // start_search cannot be a untyped_possible_sorts, or an unknown sort.
      return false;
    }

    void add_basic_sort(const basic_sort &sort)
    {
      if (sort_bool::is_bool(sort))
      {
        throw mcrl2::runtime_error("attempt to redeclare sort Bool");
      }
      if (sort_pos::is_pos(sort))
      {
        throw mcrl2::runtime_error("attempt to redeclare sort Pos");
      }
      if (sort_nat::is_nat(sort))
      {
        throw mcrl2::runtime_error("attempt to redeclare sort Nat");
      }
      if (sort_int::is_int(sort))
      {
        throw mcrl2::runtime_error("attempt to redeclare sort Int");
      }
      if (sort_real::is_real(sort))
      {
        throw mcrl2::runtime_error("attempt to redeclare sort Real");
      }
      if (m_basic_sorts.count(sort)>0 || m_aliases.count(sort)>0)
      {
        throw mcrl2::runtime_error("double declaration of sort " + core::pp(sort.name()));
      }
      m_basic_sorts.insert(sort);
    }

    // Throws an exception if the sort x is not declared
    void check_basic_sort_is_declared(const basic_sort& x)
    {
      if (sort_bool::is_bool(x) ||
          sort_pos::is_pos(x) ||
          sort_nat::is_nat(x) ||
          sort_int::is_int(x) ||
          sort_real::is_real(x))
      {
        return;
      }
      if (m_basic_sorts.count(x) > 0)
      {
        return;
      }
      if (m_aliases.count(x) > 0)
      {
        return;
      }
      throw mcrl2::runtime_error("basic or defined sort " + data::pp(x) + " is not declared");
    }

    void check_sort_list_is_declared(const sort_expression_list &SortExprList)
    {
      for (const sort_expression& s: SortExprList)
      {
        check_sort_is_declared(s);
      }
    }

    void check_sort_is_declared(const sort_expression& x)
    {
      if (is_basic_sort(x))
      {
        const basic_sort& bs = atermpp::down_cast<basic_sort>(x);
        check_basic_sort_is_declared(bs);
      }
      else if (is_container_sort(x))
      {
        const container_sort& cs = atermpp::down_cast<container_sort>(x);
        check_sort_is_declared(cs.element_sort());
      }
      else if (is_function_sort(x))
      {
        const function_sort& fs = atermpp::down_cast<function_sort>(x);
        check_sort_is_declared(fs.codomain());
        for (const sort_expression& s: fs.domain())
        {
          check_sort_is_declared(s);
        }
      }
      else if (is_structured_sort(x))
      {
        const structured_sort& ss = atermpp::down_cast<structured_sort>(x);
        for (const structured_sort_constructor& constructor: ss.constructors())
        {
          for (const structured_sort_constructor_argument& arg: constructor.arguments())
          {
            check_sort_is_declared(arg.sort());
          }
        }
      }
      else
      {
        throw mcrl2::runtime_error("this is not a sort expression " + data::pp(x));
      }
    }

    void check_for_empty_constructor_domains(function_symbol_list constructors)
    {
      // First add the constructors for structured sorts to the constructor list;
      try
      {
        std::map<sort_expression, basic_sort> normalised_aliases = m_normalized_aliases;
        std::set<sort_expression> all_sorts;
        for (std::map<basic_sort, sort_expression>::const_iterator i=m_aliases.begin(); i!=m_aliases.end(); ++i)
        {
          const sort_expression& reference = i->second;
          data::find_sort_expressions<sort_expression>(reference, std::inserter(all_sorts, all_sorts.end()));
        }

        for (const sort_expression& sort: all_sorts)
        {
          if (is_structured_sort(sort))
          {
            for (const function_symbol& f: structured_sort(sort).constructor_functions())
            {
              constructors.push_front(f);
            }
          }
        }

        std::set<sort_expression> possibly_empty_constructor_sorts;
        for (const function_symbol& constructor: constructors)
        {
          const sort_expression& s = constructor.sort();
          if (is_function_sort(s))
          {
            // if s is a constant sort, nothing needs to be added.
            possibly_empty_constructor_sorts.insert(detail::mapping(function_sort(s).codomain(),normalised_aliases));
          }
        }

        // Walk through the constructors removing constructor sorts that are not empty,
        // until no more constructors sorts can be removed.
        for (bool stable=false ; !stable ;)
        {
          stable=true;
          for (const function_symbol& constructor: constructors)
          {
            const sort_expression& s = constructor.sort();
            if (!is_function_sort(s))
            {
              if (possibly_empty_constructor_sorts.erase(detail::mapping(s,normalised_aliases))==1) // True if one element has been removed.
              {
                stable=false;
              }
            }
            else
            {
              bool has_a_domain_sort_possibly_empty_sorts=false;
              for (const sort_expression& sort: function_sort(s).domain())
              {
                if (possibly_empty_constructor_sorts.find(detail::mapping(sort, normalised_aliases))!=possibly_empty_constructor_sorts.end())
                {
                  //
                  has_a_domain_sort_possibly_empty_sorts=true;
                  continue;
                }
              }
              if (!has_a_domain_sort_possibly_empty_sorts)
              {
                // Condition below is true if one element has been removed.
                if (possibly_empty_constructor_sorts.erase(detail::mapping(function_sort(s).codomain(),normalised_aliases))==1)
                {
                  stable=false;
                }
              }
            }
          }
        }
        // Print the sorts remaining in possibly_empty_constructor_sorts, as they must be empty
        if (possibly_empty_constructor_sorts.empty())
        {
          return; // There are no empty sorts
        }
        else
        {
          std::string reason="the following domains are empty due to recursive constructors:";
          for (const sort_expression& sort: possibly_empty_constructor_sorts)
          {
            reason = reason + "\n" + data::pp(sort);
          }
          throw mcrl2::runtime_error(reason);
        }
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(e.what());
      }
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_USE_NEW_TYPE_CHECKER

#endif // MCRL2_DATA_SORT_TYPE_CHECKER_H
