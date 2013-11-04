// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/alias.h
/// \brief The class alias.

#ifndef MCRL2_DATA_ALIAS_H
#define MCRL2_DATA_ALIAS_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/basic_sort.h"

namespace mcrl2
{

namespace data
{

//--- start generated class alias ---//
/// \brief A sort alias
class alias: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    alias()
      : atermpp::aterm_appl(core::detail::constructSortRef())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit alias(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_SortRef(*this));
    }

    /// \brief Constructor.
    alias(const basic_sort& name, const sort_expression& reference)
      : atermpp::aterm_appl(core::detail::gsMakeSortRef(name, reference))
    {}

    const basic_sort& name() const
    {
      return atermpp::aterm_cast<const basic_sort>((*this)[0]);
    }

    const sort_expression& reference() const
    {
      return atermpp::aterm_cast<const sort_expression>((*this)[1]);
    }
};

/// \brief list of aliass
typedef atermpp::term_list<alias> alias_list;

/// \brief vector of aliass
typedef std::vector<alias>    alias_vector;

/// \brief Test for a alias expression
/// \param x A term
/// \return True if \a x is a alias expression
inline
bool is_alias(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSortRef(x);
}

/// \brief swap overload
inline void swap(alias& t1, alias& t2)
{
  t1.swap(t2);
}

//--- end generated class alias ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_EXPRESSION_H

