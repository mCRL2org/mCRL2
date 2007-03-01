///////////////////////////////////////////////////////////////////////////////
/// \file propositional_variable.h
/// Contains definitions of propositional variables.

#ifndef LPS_PROPOSITIONAL_VARIABLE
#define LPS_PROPOSITIONAL_VARIABLE

#include <cassert>
#include <string>
#include <utility>
#include "atermpp/aterm_appl.h"
#include "lps/identifier_string.h"
#include "lps/data.h"
#include "lps/pretty_print.h"
#include "libstruct.h"

namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;
using lps::detail::parse_variable;

///////////////////////////////////////////////////////////////////////////////
/// \brief A propositional variable declaration
///
// <PropVarDecl>  ::= PropVarDecl(<String>, <DataVarId>*)
class propositional_variable: public aterm_appl
{
  protected:
    identifier_string m_name;
    data_variable_list m_parameters;

  public:
    propositional_variable()
      : aterm_appl(detail::constructPropVarDecl())
    {}

    // example: "X(d:D,e:E)"
    propositional_variable(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_name      = identifier_string(p.first);
      m_parameters = p.second;
      m_term      = reinterpret_cast<ATerm>(gsMakePropVarDecl(m_name, m_parameters));
    }

    propositional_variable(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_PropVarDecl(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    propositional_variable(identifier_string name, data_variable_list parameters)
      : aterm_appl(gsMakePropVarDecl(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

    /// Returns the term containing the name of the propositional variable.
    identifier_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    data_variable_list parameters() const
    {
      return m_parameters;
    }
};

///////////////////////////////////////////////////////////////////////////////
// propositional_variable_list
/// \brief singly linked list of propositional variables
///
typedef term_list<propositional_variable> propositional_variable_list;

inline
bool is_propositional_variable(aterm_appl t)
{
  return gsIsPropVarDecl(t);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief A propositional variable instantiation
///
// <PropVarInst>  ::= PropVarInst(<String>, <DataExpr>*)
class propositional_variable_instantiation: public aterm_appl
{
  protected:
    identifier_string m_name;
    data_expression_list m_parameters;

  public:
    propositional_variable_instantiation()
      : aterm_appl(detail::constructPropVarInst())
    {}

    // example: "X(d:D,e:E)"
    propositional_variable_instantiation(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_name        = identifier_string(p.first);
      m_parameters = p.second;
      m_term        = reinterpret_cast<ATerm>(gsMakePropVarInst(m_name, m_parameters));
    }

    propositional_variable_instantiation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_PropVarInst(m_term));
      iterator i = t.begin();
      m_name = *i++;
      m_parameters = *i;
    }

    propositional_variable_instantiation(identifier_string name, data_expression_list parameters)
      : aterm_appl(gsMakePropVarInst(name, parameters)),
        m_name(name),
        m_parameters(parameters)
    {
    }

//    operator pbes_expression() const
//    {
//      return pbes_expression(m_term);
//    }

    /// Returns the term containing the name of the propositional variable.
    identifier_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    data_expression_list parameters() const
    {
      return m_parameters;
    }
};

///////////////////////////////////////////////////////////////////////////////
// propositional_variable_instantiation_list
/// \brief singly linked list of propositional variables instantiations
///
typedef term_list<propositional_variable_instantiation> propositional_variable_instantiation_list;

inline
bool is_propositional_variable_instantiation(aterm_appl t)
{
  return gsIsPropVarInst(t);
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::propositional_variable;
using lps::propositional_variable_instantiation;

template<>
struct aterm_traits<propositional_variable>
{
  typedef ATermAppl aterm_type;
  static void protect(propositional_variable t)   { t.protect(); }
  static void unprotect(propositional_variable t) { t.unprotect(); }
  static void mark(propositional_variable t)      { t.mark(); }
  static ATerm term(propositional_variable t)     { return t.term(); }
  static ATerm* ptr(propositional_variable& t)    { return &t.term(); }
};

template<>
struct aterm_traits<propositional_variable_instantiation>
{
  typedef ATermAppl aterm_type;
  static void protect(propositional_variable_instantiation t)   { t.protect(); }
  static void unprotect(propositional_variable_instantiation t) { t.unprotect(); }
  static void mark(propositional_variable_instantiation t)      { t.mark(); }
  static ATerm term(propositional_variable_instantiation t)     { return t.term(); }
  static ATerm* ptr(propositional_variable_instantiation& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_PROPOSITIONAL_VARIABLE
