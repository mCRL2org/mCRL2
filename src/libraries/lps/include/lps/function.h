///////////////////////////////////////////////////////////////////////////////
/// \file function.h
/// Contains function data structures for the LPS Library.

#ifndef LPS_FUNCTION_H
#define LPS_FUNCTION_H

#include <iostream>
#include <cassert>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_access.h"
#include "lps/sort.h"
#include "lps/pretty_print.h"
#include "lps/identifier_string.h"

namespace lps {

using atermpp::term_list;
using atermpp::aterm_appl;

///////////////////////////////////////////////////////////////////////////////
// function
/// \brief Represents mappings and constructors of a mCRL2 specification.
///
// <OpId>         ::= OpId(<String>, <SortExpr>)
class function: public aterm_appl
{
  public:
    function()
      : aterm_appl(detail::constructOpId())
    {}

    function(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_OpId(m_term));
    }

    function(std::string name, sort s)
      : aterm_appl(gsMakeOpId(gsString2ATermAppl(name.c_str()), s))
    {
    }

    identifier_string name() const
    {
      return arg1(*this);
    }
      
    sort type() const
    {
      return arg2(*this);
    }

    sort_list domain_sorts() const
    {
      return type().domain_sorts();
    }

    sort range_sort() const
    {
      return type().range_sort();
    }

    /// Applies a substitution to this function and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    function substitute(Substitution f)
    {
      return function(f(*this));
    }     
};

typedef term_list<function> function_list;

inline
bool is_function(aterm_appl t)
{
  return gsIsOpId(t);
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::function;

template<>
struct aterm_traits<function>
{
  typedef ATermAppl aterm_type;
  static void protect(function t)   { t.protect(); }
  static void unprotect(function t) { t.unprotect(); }
  static void mark(function t)      { t.mark(); }
  static ATerm term(function t)     { return t.term(); }
  static ATerm* ptr(function& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_FUNCTION_H
