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

#include <iostream> //  FOR DEBUGGING. 
#include <limits>
// #include <type_traits>
#include "mcrl2/utilities/type_traits.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

// Forward declaration.
class unprotected_aterm;
class aterm;

namespace detail
{

class _aterm;
inline void debug_print(std::ostream& o, const _aterm* t, const std::size_t d = 3);

/// Can be used to check whether all elements in the parameter pack are derived from aterms.
template<typename ...Terms>
using are_terms = mcrl2::utilities::forall<std::is_convertible<Terms, unprotected_aterm>...>;

/// Check whether all arguments of a parameter pack are terms, constant functions yielding a term
/// or a function putting a term in a result parameter. 

template <typename Term>
using is_term_or_function = std::disjunction<std::is_convertible<Term, unprotected_aterm>,
                            std::disjunction<mcrl2::utilities::is_applicable<Term, unprotected_aterm&>,
                                             mcrl2::utilities::is_constant_function_yielding<Term, unprotected_aterm> > >;

template<typename ...Terms>
using are_terms_or_functions = mcrl2::utilities::forall<is_term_or_function<Terms>...>;

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

  /// \brief Mark this term to be garbage collected.
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

#ifdef MCRL2_ATERMPP_REFERENCE_COUNTED
const std::size_t offset = 2;
#else
const std::size_t offset = 2;
#endif

inline void debug_print(std::ostream& out, const _aterm* t, const std::size_t d)
{
  if (d==0) { out << "..."; return; }
  if (!t->function().defined()) { out << "UNDEFFUNC"; return; }

  out << t->function().name();
  std::string separator="(";

  for(std::size_t i=0; i< t->function().arity() ; ++i)
  {
    out << separator;
    debug_print(out, *((reinterpret_cast<_aterm *const*>(t))+offset+i),d-1);
    separator = ",";
  }
  if (t->function().arity()>0)
  {
    out << ")";
  }
}


} // namespace detail
} // namespace atermpp

#endif /* MCRL2_ATERMPP_DETAIL_ATERM_H */
