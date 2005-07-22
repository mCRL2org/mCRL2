///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains data data structures for the mcrl2 library.

#ifndef MCRL2_DATA_DECLARATION_H
#define MCRL2_DATA_DECLARATION_H

#include "atermpp/aterm.h"
#include "mcrl2/data.h"
#include "mcrl2/sort.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// DataDeclaration
/// \brief a data declaration of a mCRL specification.
///
/// A data declaration consists of a sequence of constructors, functions (mappings)
/// and data equations.
//
// sort S;
// 
// cons Ca: S;
//       Cb: Bool -\> S;
// 
// map f: S -\> S;
// 
// var b: Bool;
// eqn f(Cb(b)) = Ca;
// 
// SortSpec([SortId("S")])
// ConsSpec([OpId("Ca",SortId("S")),OpId("Cb",SortArrow(SortId("Bool"),SortId("S"))
// )])
// MapSpec([OpId("f",SortArrow(SortId("S"),SortId("S"))])
// DataEqnSpec([DataEqn([DataVarId("b",SortId("Bool"))],Nil,DataAppl(OpId("f",SortA
// rrow(SortId("S"),SortId("S")),DataAppl(OpId("Cb",SortArrow(SortId("Bool"),SortId
// ("S"))),DataVarId("b",SortId("Bool")))),OpId("Ca",SortId("S")))])
// 
// sorts -\> SortId("S")
// constructors -\> OpId("Ca",SortId("S")),
// OpId("Cb",SortArrow(SortId("Bool"),SortId("S")))
// mappings -\> OpId("f",SortArrow(SortId("S"),SortId("S"))
// equations -\>
// DataEqn([DataVarId("b",SortId("Bool"))],Nil,DataAppl(OpId("f",SortArrow(SortId("
// S"),SortId("S")),DataAppl(OpId("Cb",SortArrow(SortId("Bool"),SortId("S"))),DataV
// arId("b",SortId("Bool")))),OpId("Ca",SortId("S")))
//
class DataDeclaration
{
  // N.B. A DataDeclaration is not explicitly represented in the specification.

  protected:
    aterm_list m_sorts;                 // elements are of type SortDeclaration
    aterm_list m_constructors;          // elements are of type Sort
    aterm_list m_mappings;              // elements are of type Sort
    aterm_list m_equations;             // elements are of type DataEquation

  public:
    typedef list_iterator<Sort>          sort_iterator;
    typedef list_iterator<DataEquation>  equation_iterator;

    DataDeclaration()
    {}

    DataDeclaration(aterm_list sorts, aterm_list constructors, aterm_list mappings, aterm_list equations)
      : m_sorts(sorts),
        m_constructors(constructors),
        m_mappings(mappings),
        m_equations(equations)
    {}

    /// Returns a begin iterator to the sequence of sorts.
    ///
    sort_iterator sorts_begin() const
    {
      return sort_iterator(m_sorts);
    }

    /// Returns an end iterator to the sequence of sorts.
    ///
    sort_iterator sorts_end() const
    {
      return sort_iterator();
    }
    /// Returns a begin iterator to the sequence of constructors.

    ///
    sort_iterator constructors_begin() const
    {
      return sort_iterator(m_constructors);
    }

    /// Returns an end iterator to the sequence of constructors.
    ///
    sort_iterator constructors_end() const
    {
      return sort_iterator();
    }

    /// Returns a begin iterator to the sequence of mappings.
    ///
    sort_iterator mappings_begin() const
    {
      return sort_iterator(m_mappings);
    }

    /// Returns an end iterator to the sequence of mappings.
    ///
    sort_iterator mappings_end() const
    {
      return sort_iterator();
    }

    /// Returns a begin iterator to the sequence of equations.
    ///
    equation_iterator equations_begin() const
    {
      return equation_iterator(m_equations);
    }

    /// Returns an end iterator to the sequence of mappings.
    ///
    equation_iterator equations_end() const
    {
      return equation_iterator();
    }
};

} // namespace mcrl

#endif // MCRL2_DATA_DECLARATION_H
