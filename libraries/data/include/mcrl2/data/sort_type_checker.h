// Author(s): Wieger Wesselink, Jan Friso Groote
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

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/sort_specification.h"
#include "mcrl2/data/find.h"

namespace mcrl2
{

namespace data
{

class sort_type_checker
{
  protected:
    sort_specification m_sort_specification; // Intentionally a copy. If this is a reference or pointer the
                                             // object to which this referred may disappear.

  public:
    /// \brief constructs a sort expression checker.
    sort_type_checker(const sort_specification& sort_spec, bool must_check_aliases = true)
      : m_sort_specification(sort_spec)
    {
      check_sorts();
      if (must_check_aliases)
      {
        check_aliases();
      }
    }

    const sort_specification& get_sort_specification() const
    {
      return m_sort_specification;
    }

    /** \brief     Type check a sort expression.
    *  Throws an exception if the expression is not well typed.
    *  \param[in] s A sort expression that has not been type checked.
    **/
    void operator()(const sort_expression& x) const
    {
      check_sort_is_declared(x);
    }

  protected:
    // Throws a runtime_error if a rewriting loop is detected via
    // basic sorts, function sorts or sort containers.
    void check_alias_circularity(
               const data::basic_sort& lhs,
               const data::sort_expression& rhs,
               std::set<basic_sort> sort_already_seen,
               const std::map < basic_sort, sort_expression >& alias_map)
    {
      if (data::is_basic_sort(rhs))
      {
        const basic_sort& sort=atermpp::down_cast<basic_sort>(rhs);
        if (sort == lhs)
        {
          throw mcrl2::runtime_error("Sort alias " + data::pp(lhs) + " is circularly defined.\n");
        }
        if (sort_already_seen.insert(sort).second && alias_map.count(sort)>0) // sort is newly added and there is an alias for this sort.
        {
          check_alias_circularity(lhs,alias_map.at(sort),sort_already_seen, alias_map);
        }
        sort_already_seen.erase(sort);
        return;
      }
      if (data::is_container_sort(rhs))
      {
        const container_sort& c=atermpp::down_cast<container_sort>(rhs);
        check_alias_circularity(lhs,c.element_sort(),sort_already_seen, alias_map);
        return;
      }
      if (data::is_function_sort(rhs))
      {
        const function_sort sort=atermpp::down_cast<function_sort>(rhs);
        for(const sort_expression& s: sort.domain())
        {
          check_alias_circularity(lhs,s,sort_already_seen, alias_map);
        }
        check_alias_circularity(lhs,sort.codomain(),sort_already_seen, alias_map);
        return;
      }
      // Intentionally no further search is done through a structured sort, because aliases
      // can be circularly defined through structured sorts. Example: sort Tree = struct leaf | node(Tree,Tree);
      assert(is_structured_sort(rhs));
    }

    void check_sorts()
    {
      // Check for double occurrences of sorts, as well as overlap with the five predefined basic sorts.
      std::set<basic_sort> defined_sorts={ sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() };
      for(const basic_sort& sort: get_sort_specification().user_defined_sorts())
      {
        if (!defined_sorts.insert(sort).second) // Sort did already exist.
        {
           throw mcrl2::runtime_error("Attempt to redeclare sort " + core::pp(sort.name()));
        }
      }
      for(const alias& a: get_sort_specification().user_defined_aliases())
      {
        if (!defined_sorts.insert(a.name()).second) // Sort did already exist.
        {
           throw mcrl2::runtime_error("Attempt to redeclare sort in alias " + data::pp(a));
        }
      }
    }


    // Throws an exception if there is a problem with the alias
    void check_aliases()
    {
      std::map < basic_sort, sort_expression > alias_map;
      for(const alias& a: get_sort_specification().user_defined_aliases())
      {
        alias_map[a.name()]=a.reference();
      }

      std::set<data::basic_sort> sort_already_seen;
      for(const alias& a: get_sort_specification().user_defined_aliases())
      {
        check_for_sort_alias_loop_through_function_sort(a.name(),a.reference(), sort_already_seen, false, alias_map);
        assert(sort_already_seen.size()==0);
        check_alias_circularity(a.name(), a.reference(),sort_already_seen, alias_map);
        assert(sort_already_seen.size()==0);
      }

        try
        {
          for(const alias& a: get_sort_specification().user_defined_aliases())
          {
            (*this)(a.reference()); // Type check sort expression.
          }
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of aliases failed");
        }
    }

    void check_for_sort_alias_loop_through_function_sort(
      const basic_sort& end_search,
      const sort_expression& start_search,
      std::set < basic_sort >& visited,
      const bool observed_a_sort_constructor,
      const std::map < basic_sort, sort_expression >& alias_map)
    {
      if (is_basic_sort(start_search))
      {
        const basic_sort& s=atermpp::down_cast<basic_sort>(start_search);
        if (s==end_search)
        {
          // We found a loop.
          if (observed_a_sort_constructor)
          {
            throw mcrl2::runtime_error("sort " + data::pp(end_search) + " is recursively defined via a function sort, or a set or a bag type container");
          }
        }

        if (alias_map.count(s)==0 || visited.count(s)>0)
        {
          // start_search is not a sort alias, and hence not a recursive sort, or we have already seen sort s.
          return;
        }
        visited.insert(s);
        check_for_sort_alias_loop_through_function_sort(end_search,alias_map.at(s),visited,observed_a_sort_constructor,alias_map);
        visited.erase(s);
        return;
      }

      if (is_container_sort(start_search))
      {
        // A loop through a list container is allowed, but a loop through a set or bag container
        // is problematic.
        const container_sort start_search_container=atermpp::down_cast<container_sort>(start_search);
        check_for_sort_alias_loop_through_function_sort(
                       end_search,
                       start_search_container.element_sort(),
                       visited,
                       start_search_container.container_name()==set_container()||start_search_container.container_name()==bag_container(),
                       alias_map);
        return;
      }

      if (is_function_sort(start_search))
      {
        const function_sort f_start_search=atermpp::down_cast<function_sort>(start_search);
        check_for_sort_alias_loop_through_function_sort(end_search,f_start_search.codomain(),visited,true,alias_map);
        for(const sort_expression& s: f_start_search.domain())
        {
          check_for_sort_alias_loop_through_function_sort(end_search,s,visited,true,alias_map);
        }
        // end_search has not been found, so:
        return;
      }

      assert(is_structured_sort(start_search));

      const structured_sort struct_start_search=atermpp::down_cast<structured_sort>(start_search);
      for(const function_symbol& f: struct_start_search.constructor_functions())
      {
        if (is_function_sort(f.sort()))
        {
          const sort_expression_list domain_sorts=function_sort(f.sort()).domain();
          for(const sort_expression& s: domain_sorts)
          {
            check_for_sort_alias_loop_through_function_sort(end_search,s,visited,observed_a_sort_constructor,alias_map);
          }
        }
      }
    }


    // Throws an exception if the sort x is not declared
    void check_basic_sort_is_declared(const basic_sort& x) const
    {
      if (sort_bool::is_bool(x) ||
          sort_pos::is_pos(x) ||
          sort_nat::is_nat(x) ||
          sort_int::is_int(x) ||
          sort_real::is_real(x))
      {
        return;
      }
      if (std::find(get_sort_specification().user_defined_sorts().begin(), get_sort_specification().user_defined_sorts().end(),x)!=
                   get_sort_specification().user_defined_sorts().end())
      {
        return;
      }
      for(const alias& a: get_sort_specification().user_defined_aliases())
      {
        if (x==a.name())
        {
          return;
        }
      }
      throw mcrl2::runtime_error("basic or defined sort " + data::pp(x) + " is not declared");
    }

    void check_sort_list_is_declared(const sort_expression_list& SortExprList) const
    {
      for(const sort_expression& s: SortExprList)
      {
        check_sort_is_declared(s);
      }
    }

    void check_sort_is_declared(const sort_expression& x) const
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
        for(const sort_expression& s: fs.domain())
        {
          check_sort_is_declared(s);
        }
      }
      else if (is_structured_sort(x))
      {
        const structured_sort& ss = atermpp::down_cast<structured_sort>(x);
        for(const structured_sort_constructor& constructor: ss.constructors())
        {
          for(const structured_sort_constructor_argument& arg: constructor.arguments())
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

    void check_for_empty_constructor_domains(const function_symbol_vector& constructors)
    {
      std::set<sort_expression> possibly_empty_constructor_sorts;
      for(const function_symbol& constructor: constructors)
      {
        const sort_expression& s = constructor.sort();
        if (is_function_sort(s))
        {
          // if s is a constant sort, nothing needs to be added.
          possibly_empty_constructor_sorts.insert(normalize_sorts(function_sort(s).codomain(),get_sort_specification()));
        }
      }

      // Walk through the constructors removing constructor sorts that are not empty,
      // until no more constructors sorts can be removed.
      for(bool stable=false ; !stable ;)
      {
        stable=true;
        for(const function_symbol& constructor: constructors)
        {
          const sort_expression& s = constructor.sort();
          if (!is_function_sort(s))
          {
            if (possibly_empty_constructor_sorts.erase(normalize_sorts(s,get_sort_specification()))==1) // True if one element has been removed.
            {
              stable=false;
            }
          }
          else
          {
            bool has_a_domain_sort_possibly_empty_sorts=false;
            for(const sort_expression& sort: function_sort(s).domain())
            {
              if (possibly_empty_constructor_sorts.find(normalize_sorts(sort, get_sort_specification()))!=possibly_empty_constructor_sorts.end())
              {
                //
                has_a_domain_sort_possibly_empty_sorts=true;
                continue;
              }
            }
            if (!has_a_domain_sort_possibly_empty_sorts)
            {
              // Condition below is true if one element has been removed.
              if (possibly_empty_constructor_sorts.erase(normalize_sorts(function_sort(s).codomain(),get_sort_specification()))==1)
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
        for(const sort_expression& sort: possibly_empty_constructor_sorts)
        {
          reason = reason + "\n" + data::pp(sort);
        }
        throw mcrl2::runtime_error(reason);
      }
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_TYPE_CHECKER_H
