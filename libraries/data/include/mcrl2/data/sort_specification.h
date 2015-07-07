// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_specification.h
/// \brief This file describes the a sort_specification, 
//         which contains declared sorts and sort aliases.

#ifndef MCRL2_DATA_SORT_SPECIFICATION_H
#define MCRL2_DATA_SORT_SPECIFICATION_H

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/data/structured_sort.h"

namespace mcrl2
{
namespace data
{

/// \cond INTERNAL_DOC
namespace detail
{

  /* template < typename Container, typename T >
  inline
  void remove(Container& container, const T& t)
  {
    typename Container::iterator i = std::find(container.begin(), container.end(), t);
    if(i != container.end())
    {
      container.erase(i);
    }
  } */

} // end namespace detail
/// \endcond

class sort_specification;
sort_expression normalize_sorts(const sort_expression& x, const data::sort_specification& sortspec);

class sort_specification
{
  protected:
    // The function below recalculates m_normalised_aliases, such that
    // it forms a confluent terminating rewriting system using which
    // sorts can be normalised.
    void reconstruct_m_normalised_aliases() const;
    
    
    // The function below checks whether there is an alias loop, e.g. aliases
    // of the form A=B; B=A; or more complex A=B->C; B=Set(D); D=List(A); Loops
    // through structured sorts are allowed. If a loop is detected, an exception
    // is thrown.
    void check_for_alias_loop(
      const sort_expression s,
      std::set<sort_expression> sorts_already_seen,
      const bool toplevel=true) const;

    /// \brief This boolean indicates whether the variables
    /// m_normalised_constructors, m_mappings, m_equations, m_normalised_sorts,
    /// m_normalised_aliases.
    mutable bool m_normalised_sorts_are_up_to_date;

    /// \brief The basic sorts and structured sorts in the specification.
    basic_sort_vector     m_sorts;

    /// \brief Set containing all the sorts, including the system defined ones.
    mutable sort_expression_vector         m_normalised_sorts;

    /// \brief The basic sorts and structured sorts in the specification.
    alias_vector                     m_aliases;
     
    /// \brief Table containing how sorts should be mapped to normalised sorts.
    mutable std::map< sort_expression, sort_expression > m_normalised_aliases;

  public:
    
    /// \brief Gets the user defined aliases.
    /// \details The time complexity is constant.
    inline
    const alias_vector& user_defined_aliases() const
    {
      return m_aliases;
    }

}
;

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_SPECIFICATION_H
