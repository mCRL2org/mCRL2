// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_specification.h
/// \brief The class data_specification.

#ifndef MCRL2_DATA_DATA_SPECIFICATION_H
#define MCRL2_DATA_DATA_SPECIFICATION_H

#include <set>
#include <boost/iterator/filter_iterator.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/map.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/sort_utility1.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/sequence_algorithm.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm_appl;
using atermpp::aterm_list;

/// \cond INTERNAL_DOCS

namespace detail {

  struct has_result_sort
  {
    sort_expression m_result;
  
    has_result_sort(sort_expression target)
      : m_result(target)
    {}
  
    bool operator()(data_operation f)
    {
      return result_sort(f.sort()) == m_result;
    }
  };
} // namespace detail

/// \endcond

/// \brief A data specification is merely a struct containing
/// sequences of sorts, constructors, mappings and equations.
///
// <DataSpec>     ::= DataSpec(SortSpec(<SortDecl>*), ConsSpec(<OpId>*),
//                      MapSpec(<OpId>*), DataEqnSpec(<DataEqn>*))
class data_specification: public aterm_appl
{

  protected:
    sort_expression_list m_sorts;
    data_operation_list  m_constructors;
    data_operation_list  m_mappings;
    data_equation_list   m_equations;

    // The following map contains for each sort a default term of that particular
    // sort. Each default term remains valid, as long as no constructors or mappings
    // are removed from the specification.
    atermpp::map < sort_expression, data_expression > default_expression_map;

    /// Caches if a sort is finite or not.
    std::map<data::sort_expression, bool> m_finite_sorts;  

  public:
    /// Iterator for the sequence of sorts.
    typedef sort_expression_list::iterator sort_iterator;
      
    /// Iterator for the sequences of constructors and mappings.
    typedef data_operation_list::iterator  function_iterator;

    /// Iterator for the sequence of equations.
    typedef data_equation_list::iterator   equation_iterator;

    /// Constructor.
    ///             
    data_specification()
      : aterm_appl(core::detail::constructDataSpec())
    {}

    /// Constructor.
    ///             
    data_specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_DataSpec(m_term));
      aterm_appl::iterator i = t.begin();
      m_sorts        = sort_expression_list(aterm_appl(*i++).argument(0));
      m_constructors = data_operation_list(aterm_appl(*i++).argument(0));
      m_mappings     = data_operation_list(aterm_appl(*i++).argument(0));
      m_equations    = data_equation_list(aterm_appl(*i++).argument(0));
    }

    /// Constructor.
    ///             
    data_specification(sort_expression_list sorts, data_operation_list constructors, data_operation_list mappings, data_equation_list equations)
      : aterm_appl(core::detail::gsMakeDataSpec(
                      core::detail::gsMakeSortSpec(sorts),
                      core::detail::gsMakeConsSpec(constructors),
                      core::detail::gsMakeMapSpec(mappings),
                      core::detail::gsMakeDataEqnSpec(equations)
                     )
                   ),
        m_sorts(sorts),
        m_constructors(constructors),
        m_mappings(mappings),
        m_equations(equations)
    {}

    /// Returns the sorts of the data specification.
    ///
    sort_expression_list sorts() const
    {
      return m_sorts;
    }

    /// Returns the constructors of the data specification.
    ///
    data_operation_list constructors() const
    {
      return m_constructors;
    }

    /// Returns the constructors of the data specification that have s as their target.
    data_operation_list constructors(sort_expression s) const
    {
      atermpp::vector<data_operation> result;

      typedef boost::filter_iterator<detail::has_result_sort, data_operation_list::iterator> FilterIter;
      detail::has_result_sort predicate(s);
      FilterIter first(predicate, m_constructors.begin(), m_constructors.end());
      FilterIter last(predicate, m_constructors.end(), m_constructors.end());
      std::copy(first, last, std::back_inserter(result));
      return data_operation_list(result.begin(), result.end());
    }

    /// Returns the mappings of the data specification.
    ///
    data_operation_list mappings() const
    {
      return m_mappings;
    }

    /// Returns the mappings of the data specification that have s as their target.
    ///
    data_operation_list mappings(sort_expression s) const
    {
      atermpp::vector<data_operation> result;
      typedef boost::filter_iterator<detail::has_result_sort, data_operation_list::iterator> FilterIter;
      detail::has_result_sort predicate(s);
      FilterIter first(predicate, m_mappings.begin(), m_mappings.end());
      FilterIter last(predicate, m_mappings.end(), m_mappings.end());
      std::copy(first, last, std::back_inserter(result));
      return data_operation_list(result.begin(), result.end());
    }

    /// Returns the equations of the data specification.
    ///
    data_equation_list equations() const
    {
      return m_equations;
    }

    /// Returns a valid data expression according to this data specification
    /// of the given sort s. If no valid expression can be found, data_expression()
    /// is returned. It returns a minimal term with by default at most three nested function
    /// symbols. When selecting function symbols, constructor symbols have a
    /// preference over mappings. For each sort, the same term is returned.
    /// Currently, only expressions for basic sorts are delivered. For function
    /// sorts data_expression() is returned.
    /// <P>
    /// Terms that are generated are stored in a map such that they do not have to be
    /// generated more than once. When generating a term the nesting depth is taken into
    /// account. When the term is taken from the map, the nesting depth is ignored.
    /// So, generating a term with nesting depth 10, and subsequentely with nesting depth
    /// 1 can still yield a term of nesting depth larger than 1, because the earlier
    /// generated term is returned.
    data_expression default_expression(sort_expression s, const unsigned int max_recursion_depth=3) 
    // data_expression default_expression(sort_expression s) const
    {
      // first check whether a term has already been constructed for this sort.

      data_expression result;
      atermpp::map < sort_expression,data_expression >::iterator l=default_expression_map.find(s);
      
      if (l!=default_expression_map.end())
      { // a default data_expression is found.
        return l->second;
      }

      // check if there is a constant constructor for s
      data_operation_list::iterator i = std::find_if(constructors(s).begin(), 
                                                     constructors(s).end(), 
                                                     detail::is_constant_operation());
      if (i != constructors().end())
      { 
        result=data_expression((aterm_appl)*i);
        default_expression_map.insert(std::make_pair(s,result));
        return result;
      }
      
      // check if there is a constant mapping for s
      i = std::find_if(mappings(s).begin(), mappings(s).end(), detail::is_constant_operation());
      if (i != mappings().end())
      { 
        result=data_expression((aterm_appl)*i);
        default_expression_map.insert(std::make_pair(s,result));
        return result;
      }
      
      if (max_recursion_depth>0)
      { // recursively traverse constructor functions
        for(data_operation_list::iterator i=constructors(s).begin();
            i!=constructors(s).end(); i++)
        { if (i->sort().is_arrow())
          { sort_expression_list argument_sorts=domain_sorts((*i).sort());
            data_expression_list arguments;
            sort_expression_list::iterator j;
            for(j=argument_sorts.begin();j!=argument_sorts.end(); j++)
            { data_expression t=default_expression(*j,max_recursion_depth-1);
              if (t==data_expression())
              { break;
              }
              else arguments=push_front(arguments,t);
            }
            if (j==argument_sorts.end())
            { // a suitable set of arguments is found
              arguments=reverse(arguments);
              result=data_application((aterm_appl)*i,arguments);
              default_expression_map.insert(std::make_pair(s,result));
              return result;
            }
          }
        }
  
        // recursively traverse mappings
        for(data_operation_list::iterator i=mappings(s).begin();
            i!=mappings(s).end(); i++)
        { if (i->sort().is_arrow())                 
          { sort_expression_list argument_sorts=domain_sorts((*i).sort());
            data_expression_list arguments;
            sort_expression_list::iterator j;
            for(j=argument_sorts.begin();j!=argument_sorts.end(); j++)
            { data_expression t=default_expression(*j,max_recursion_depth-1);
              if (t==data_expression())
              { break;
              }
              else arguments=push_front(arguments,t);
            }
            if (j==argument_sorts.end())
            { // a suitable set of arguments is found
              arguments=reverse(arguments);
              result=data_application((aterm_appl)*i,arguments);
              default_expression_map.insert(std::make_pair(s,result));
              return result;
            }
          }
        }
      }

      return data_expression();
    }

    /// Returns true if the sort s has a finite number of values.
    /// For efficiency, the results of this function are cached.
    bool is_finite(sort_expression s)
    {
      std::map<sort_expression, bool>::const_iterator i = m_finite_sorts.find(s);
      if (i != m_finite_sorts.end())
      {
        return i->second;
      }
      bool b = data::is_finite(constructors(), s);
      m_finite_sorts[s] = b;
      return m_finite_sorts[s];
    }

    /// Returns true if
    /// <ul>
    /// <li>the domain and range sorts of constructors are contained in the list of sorts</li>
    /// <li>the domain and range sorts of mappings are contained in the list of sorts</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      std::set<sort_expression> sorts = detail::make_set(m_sorts);

      // check 1)
      if (!detail::check_data_spec_sorts(constructors(), sorts))
      {
        std::cerr << "data_specification::is_well_typed() failed: not all of the sorts appearing in the constructors " << mcrl2::core::pp(constructors()) << " are declared in " << m_sorts << std::endl;
        return false;
      }

      // check 2)
      if (!detail::check_data_spec_sorts(mappings(), sorts))
      {
        std::cerr << "data_specification::is_well_typed() failed: not all of the sorts appearing in the mappings " << mcrl2::core::pp(mappings()) << " are declared in " << m_sorts << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief Sets the sequence of sorts
inline
data_specification set_sorts(data_specification s, sort_expression_list sorts)
{
  return data_specification(sorts,
                            s.constructors(),
                            s.mappings(),
                            s.equations()
                           );
}

/// \brief Sets the sequence of constructors
inline
data_specification set_constructors(data_specification s, data_operation_list constructors)
{
  return data_specification(s.sorts(),
                            constructors,
                            s.mappings(),
                            s.equations()
                           );
}

/// \brief Sets the sequence of mappings
inline
data_specification set_mappings(data_specification s, data_operation_list mappings)
{
  return data_specification(s.sorts(),
                            s.constructors(),
                            mappings,
                            s.equations()
                           );
}

/// \brief Sets the sequence of data equations
inline
data_specification set_equations(data_specification s, data_equation_list equations)
{
  return data_specification(s.sorts(),
                            s.constructors(),
                            s.mappings(),
                            equations
                           );
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::data_specification;

template<>
struct aterm_traits<data_specification>
{
  typedef ATermAppl aterm_type;
  static void protect(data_specification t)   { t.protect(); }
  static void unprotect(data_specification t) { t.unprotect(); }
  static void mark(data_specification t)      { t.mark(); }
  static ATerm term(data_specification t)     { return t.term(); }
  static ATerm* ptr(data_specification& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_DATA_SPECIFICATION_H
