// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_H
#define MCRL2_ATERMPP_DETAIL_ATERM_H

#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/utilities/type_traits.h"

#include <limits>

namespace atermpp
{

// Forward declaration.
class unprotected_aterm;
class aterm;

namespace detail
{

/// \brief Can be used to check whether all elements in the parameter pack are derived from aterms.
template<typename ...Terms>
using are_terms = mcrl2::utilities::forall<std::is_convertible<Terms, unprotected_aterm>...>;

/// \brief This is the class to which an aterm points. Each _aterm consists
///        of a function symbol and a reference count used for garbage
///        collection.
class _aterm : public
#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
  mcrl2::utilities::shared_reference_counted<_aterm, GlobalThreadSafe>,
#endif
  mcrl2::utilities::noncopyable
{
public:
  /// \brief Create a term from a function symbol.
  _aterm(const function_symbol& symbol) :
    m_function_symbol(symbol)
  {}

  const function_symbol& function() const noexcept
  {
    return m_function_symbol;
  }

  /// \brief Mark this term to be reachable.
  void mark() const
  {
    m_function_symbol.m_function_symbol.tag();
  }

  /// \brief Remove the mark from a term.
  void unmark() const
  {
    m_function_symbol.m_function_symbol.untag();
  }

  /// \brief Check if the term is already marked.
  bool is_marked() const
  {
    return m_function_symbol.m_function_symbol.tagged();
  }

private:
  function_symbol m_function_symbol;
};

inline _aterm* address(const unprotected_aterm& t);

} // namespace detail
} // namespace atermpp

#endif /* MCRL2_ATERMPP_DETAIL_ATERM_H */
