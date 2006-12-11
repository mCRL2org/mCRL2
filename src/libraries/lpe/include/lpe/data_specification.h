///////////////////////////////////////////////////////////////////////////////
/// \file data_specification.h

#ifndef LPE_DATA_SPECIFICATION_H
#define LPE_DATA_SPECIFICATION_H

#include "atermpp/aterm.h"
#include "lpe/data_specification.h"
#include "lpe/sort.h"
#include "lpe/function.h"
#include "lpe/data.h"

namespace lpe {

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
    {}

    data_specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsDataSpec(t));
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

} // namespace lpe

#endif // LPE_DATA_SPECIFICATION_H
