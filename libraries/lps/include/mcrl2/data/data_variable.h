// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_variable.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DATA_VARIABLE_H
#define MCRL2_DATA_DATA_VARIABLE_H

#include <cassert>
#include <string>
#include "atermpp/aterm_appl.h"
#include "atermpp/aterm_traits.h"
#include "mcrl2/basic/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/basic/detail/soundness_checks.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;
using atermpp::arg1;

///////////////////////////////////////////////////////////////////////////////
// data_variable
/// \brief data variable
///
// DataVarId(<String>, <SortExpr>)
class data_variable: public data_expression
{
  public:
    data_variable()
      : data_expression(detail::constructDataVarId())
    {}

    data_variable(aterm_appl t)
     : data_expression(t)
    {
      assert(detail::check_rule_DataVarId(m_term));
    }

    /// Constructor for strings like "d:D".
    /// Only works for constant sorts.
    ///
    data_variable(const std::string& s)
    {
      std::string::size_type idx = s.find(':');
      assert (idx != std::string::npos);
      std::string name = s.substr(0, idx);
      std::string type = s.substr(idx+1);
      m_term = reinterpret_cast<ATerm>(gsMakeDataVarId(gsString2ATermAppl(name.c_str()), lps::sort(type)));
    }

    data_variable(identifier_string name, const lps::sort& s)
     : data_expression(gsMakeDataVarId(name, s))
    {}

    data_variable(const std::string& name, const lps::sort& s)
     : data_expression(gsMakeDataVarId(gsString2ATermAppl(name.c_str()), s))
    {}

    /// Returns the name of the data_variable.
    ///
    identifier_string name() const
    {
      return arg1(*this);
    }
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_variable_list
/// \brief singly linked list of data variables
///
typedef term_list<data_variable> data_variable_list;

/// \brief Returns true if the term t is a data variable
inline
bool is_data_variable(aterm_appl t)
{
  return gsIsDataVarId(t);
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::data_variable;

template<>
struct aterm_traits<data_variable>
{
  typedef ATermAppl aterm_type;
  static void protect(data_variable t)   { t.protect(); }
  static void unprotect(data_variable t) { t.unprotect(); }
  static void mark(data_variable t)      { t.mark(); }
  static ATerm term(data_variable t)     { return t.term(); }
  static ATerm* ptr(data_variable& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_DATA_DATA_VARIABLE_H
