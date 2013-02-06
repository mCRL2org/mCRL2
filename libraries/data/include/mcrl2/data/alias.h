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
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/basic_sort.h"

namespace mcrl2
{

namespace data
{

/// \brief Returns true iff this expression is a sort alias.
inline
bool is_alias(atermpp::aterm_appl const& e)
{
  return core::detail::gsIsSortRef(e);
}

/// \brief alias.
///
/// An alias introduces another name for a sort.
/// An example of an alias is L = List(S), where
/// L is an alternative name for List(S).
class alias: public atermpp::aterm_appl
{
  public:

    /// \brief Default constructor for alias, this does not entail
    ///        a valid expression.
    ///
    alias()
      : atermpp::aterm_appl(core::detail::constructSortRef())
    {}

    /// \brief Construct an alias from a term.
    ///
    /// \param[in] s The term from which the alias is
    ///            constructed.
    /// \pre is_alias(s)
    ///
    alias(const atermpp::aterm_appl& s)
      : atermpp::aterm_appl(s)
    {
      assert(is_alias(s));
    }

    /// \brief Constructor
    ///
    /// \param[in] b The name of the alias that is created.
    /// \param[in] s The sort for which an alias is created.
    /// \post n and s describe the same sort.
    alias(const basic_sort& b, const sort_expression s)
      : atermpp::aterm_appl(mcrl2::core::detail::gsMakeSortRef(atermpp::arg1(static_cast< atermpp::aterm_appl >(b)), s))
    {}

    /// \brief Returns the name of this sort.
    /// \details For an alias defined as alias(S,s) with S a basic
    /// sort and s the basic sort S is returned.
    inline
    basic_sort name() const
    {
      return basic_sort(atermpp::aterm_string(atermpp::arg1(*this)));
    }

    /// \brief Returns the sort to which the name refers.
    /// \details For an alias defined as alias(S,s) with S a basic
    /// sort and s a sort_expression s, the sort expression
    /// s is delivered.
    inline
    sort_expression reference() const
    {
      return atermpp::arg2(*this);
    }

}; // class alias

/// \brief list of aliases
typedef atermpp::term_list< alias >    alias_list;

/// \brief vector of aliases
typedef std::vector< alias > alias_vector;

} // namespace data

} // namespace mcrl2
namespace std {
template <>
inline void swap(mcrl2::data::alias& t1, mcrl2::data::alias& t2)
{
  t1.swap(t2);
}
} // namespace std

#endif // MCRL2_DATA_SORT_EXPRESSION_H

