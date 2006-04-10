///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains data data structures for the LPE Library.

#ifndef LPE_DATA_DECLARATION_H
#define LPE_DATA_DECLARATION_H

#include "atermpp/aterm.h"
#include "lpe/data.h"
#include "lpe/sort.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// DataDeclaration
/// \brief a data declaration of a mCRL specification.
///
class DataDeclaration
{
  // N.B. A DataDeclaration is not explicitly represented in the specification.

  protected:
    aterm_list m_sorts;                 // elements are of type SortDeclaration
    aterm_list m_constructors;          // elements are of type sort
    aterm_list m_mappings;              // elements are of type sort
    aterm_list m_equations;             // elements are of type data_equation

  public:
    typedef sort_list::iterator     sort_iterator;
    typedef equation_list::iterator equation_iterator;

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

#endif // LPE_DATA_DECLARATION_H
