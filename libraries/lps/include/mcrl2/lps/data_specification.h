///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/lps/data_specification.h
/// Add your file description here.
//
//  Copyright 2007 Wieger Wesselink. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPS_DATA_SPECIFICATION_H
#define LPS_DATA_SPECIFICATION_H

#include <set>

#include "atermpp/aterm.h"
#include "mcrl2/lps/sort.h"
#include "mcrl2/lps/function.h"
#include "mcrl2/lps/data.h"
#include "mcrl2/lps/detail/utility.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::aterm_list;

///////////////////////////////////////////////////////////////////////////////
// data_specification
/// \brief data specification
///
// <DataSpec>     ::= DataSpec(SortSpec(<SortDecl>*), ConsSpec(<OpId>*),
//                      MapSpec(<OpId>*), DataEqnSpec(<DataEqn>*))
class data_specification: public aterm_appl
{

  protected:
    sort_list          m_sorts;       
    function_list      m_constructors;
    function_list      m_mappings;    
    data_equation_list m_equations;   

  public:
    typedef sort_list::iterator          sort_iterator;
    typedef function_list::iterator      function_iterator;
    typedef data_equation_list::iterator equation_iterator;

    data_specification()
      : aterm_appl(detail::constructDataSpec())
    {}

    data_specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_DataSpec(m_term));
      aterm_appl::iterator i = t.begin();
      m_sorts        = sort_list(aterm_appl(*i++).argument(0));
      m_constructors = function_list(aterm_appl(*i++).argument(0));
      m_mappings     = function_list(aterm_appl(*i++).argument(0));
      m_equations    = data_equation_list(aterm_appl(*i++).argument(0));
    }

    data_specification(sort_list sorts, function_list constructors, function_list mappings, data_equation_list equations)
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
    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Returns the constructors of the data specification.
    ///
    function_list constructors() const
    {
      return m_constructors;
    }

    /// Returns the mappings of the data specification.
    ///
    function_list mappings() const
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
      std::set<lps::sort> sorts = detail::make_set(m_sorts);

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
data_specification set_sorts(data_specification s, sort_list sorts)
{
  return data_specification(sorts,
                            s.constructors(),
                            s.mappings(),
                            s.equations()
                           );
}

/// \brief Sets the sequence of constructors
inline
data_specification set_constructors(data_specification s, function_list constructors)
{
  return data_specification(s.sorts(),
                            constructors,
                            s.mappings(),
                            s.equations()
                           );
}

/// \brief Sets the sequence of mappings
inline
data_specification set_mappings(data_specification s, function_list mappings)
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

/// INTERNAL ONLY
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

#endif // LPS_DATA_SPECIFICATION_H
