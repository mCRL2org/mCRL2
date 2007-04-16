///////////////////////////////////////////////////////////////////////////////
/// \file data_specification.h

#ifndef LPS_DATA_SPECIFICATION_H
#define LPS_DATA_SPECIFICATION_H

#include <set>

#include "atermpp/aterm.h"
#include "lps/sort.h"
#include "lps/function.h"
#include "lps/data.h"
#include "lps/detail/utility.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::aterm_list;

///////////////////////////////////////////////////////////////////////////////
// data_specification
/// \brief a data declaration of a mCRL specification.
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

    /// Returns the list of sorts.
    ///
    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Returns the list of constructors.
    ///
    function_list constructors() const
    {
      return m_constructors;
    }

    /// Returns the list of mappings.
    ///
    function_list mappings() const
    {
      return m_mappings;
    }

    /// Returns the list of equations.
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

/// Sets the sequence of sorts.
///
inline
data_specification set_sorts(data_specification s, sort_list sorts)
{
  return data_specification(sorts,
                            s.constructors(),
                            s.mappings(),
                            s.equations()
                           );
}

/// Sets the sequence of constructors.
///
inline
data_specification set_constructors(data_specification s, function_list constructors)
{
  return data_specification(s.sorts(),
                            constructors,
                            s.mappings(),
                            s.equations()
                           );
}

/// Sets the sequence of mappings.
///
inline
data_specification set_mappings(data_specification s, function_list mappings)
{
  return data_specification(s.sorts(),
                            s.constructors(),
                            mappings,
                            s.equations()
                           );
}

/// Sets the sequence of data equations.
///
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
