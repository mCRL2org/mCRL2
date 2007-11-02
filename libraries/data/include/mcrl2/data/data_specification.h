// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_specification.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DATA_SPECIFICATION_H
#define MCRL2_DATA_DATA_SPECIFICATION_H

#include <set>
#include <boost/iterator/filter_iterator.hpp>
#include "atermpp/aterm.h"
#include "atermpp/vector.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data.h"
#include "mcrl2/lps/detail/data_utility.h"
#include "mcrl2/lps/detail/sequence_algorithm.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::aterm_list;

/// \internal
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

///////////////////////////////////////////////////////////////////////////////
// data_specification
/// \brief data specification
///
// <DataSpec>     ::= DataSpec(SortSpec(<SortDecl>*), ConsSpec(<OpId>*),
//                      MapSpec(<OpId>*), DataEqnSpec(<DataEqn>*))
class data_specification: public aterm_appl
{

  protected:
    sort_expression_list          m_sorts;
    data_operation_list      m_constructors;
    data_operation_list      m_mappings;
    data_equation_list m_equations;

  public:
    typedef sort_expression_list::iterator          sort_iterator;
    typedef data_operation_list::iterator      function_iterator;
    typedef data_equation_list::iterator equation_iterator;

    data_specification()
      : aterm_appl(detail::constructDataSpec())
    {}

    data_specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_DataSpec(m_term));
      aterm_appl::iterator i = t.begin();
      m_sorts        = sort_expression_list(aterm_appl(*i++).argument(0));
      m_constructors = data_operation_list(aterm_appl(*i++).argument(0));
      m_mappings     = data_operation_list(aterm_appl(*i++).argument(0));
      m_equations    = data_equation_list(aterm_appl(*i++).argument(0));
    }

    data_specification(sort_expression_list sorts, data_operation_list constructors, data_operation_list mappings, data_equation_list equations)
      : aterm_appl(gsMakeDataSpec(
                      gsMakeSortSpec(sorts),
                      gsMakeConsSpec(constructors),
                      gsMakeMapSpec(mappings),
                      gsMakeDataEqnSpec(equations)
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
      atermpp::vector<lps::data_operation> result;

      typedef boost::filter_iterator<has_result_sort, data_operation_list::iterator> FilterIter;
      has_result_sort predicate(s);
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

    /// Returns the equations of the data specification.
    ///
    data_equation_list equations() const
    {
      return m_equations;
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
        std::cerr << "data_specification::is_well_typed() failed: not all of the sorts appearing in the constructors " << pp(constructors()) << " are declared in " << m_sorts << std::endl;
        return false;
      }

      // check 2)
      if (!detail::check_data_spec_sorts(mappings(), sorts))
      {
        std::cerr << "data_specification::is_well_typed() failed: not all of the sorts appearing in the mappings " << pp(mappings()) << " are declared in " << m_sorts << std::endl;
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

} // namespace lps

/// \internal
namespace atermpp
{
using lps::data_specification;

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

#endif // MCRL2_DATA_DATA_SPECIFICATION_H
