///////////////////////////////////////////////////////////////////////////////
/// \file pbes.h
/// Contains pbes data structures for the LPE Library.

#ifndef LPE_PBES_H
#define LPE_PBES_H

#include <functional>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cassert>
#include <set>
#include <iterator>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/set.h"
#include "atermpp/vector.h"
#include "lpe/action.h"
#include "lpe/data.h"
#include "lpe/data_specification.h"
#include "lpe/pretty_print.h"
#include "lpe/mucalculus_init.h"
#include "lpe/detail/utility.h"
#include "libstruct.h"

namespace lpe {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;
using lpe::detail::parse_variable;

    struct is_state_variable
    {
      bool operator()(aterm_appl t) const
      {
        return gsIsStateVar(t);
      }
    };

///////////////////////////////////////////////////////////////////////////////
// pbes_expression
/// \brief pbes expression.
///
// <PBExpr>       ::= <DataExpr>
//                  | PBESTrue
//                  | PBESFalse
//                  | PBESAnd(<PBExpr>, <PBExpr>)
//                  | PBESOr(<PBExpr>, <PBExpr>)
//                  | PBESForall(<DataVarId>+, <PBExpr>)
//                  | PBESExists(<DataVarId>+, <PBExpr>)
//                  | <PropVarInst>
class pbes_expression: public aterm_appl
{
  public:
    pbes_expression()
    {}

    pbes_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(gsIsPBExpr(term));
    }

    // allow assignment to aterms
    pbes_expression& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns true if the pbes expression equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      return *this == gsMakePBESTrue();
    }     

    /// Returns true if the pbes expression equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      return *this == gsMakePBESFalse();
    }

    /// Applies a substitution to this pbes expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    pbes_expression substitute(Substitution f)
    {
      return pbes_expression(f(*this));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// pbes_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_expression> pbes_expression_list;

///////////////////////////////////////////////////////////////////////////////
/// \brief A propositional variable declaration
///
// <PropVarDecl>  ::= PropVarDecl(<String>, <DataVarId>*)
class propositional_variable: public aterm_appl
{
  protected:
    aterm_string m_name;
    data_variable_list m_variables;

  public:
    propositional_variable()
    {}

    // example: "X(d:D,e:E)"
    propositional_variable(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_name      = aterm_string(p.first);
      m_variables = p.second;
      m_term      = reinterpret_cast<ATerm>(gsMakePropVarDecl(m_name, m_variables));
    }

    propositional_variable(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsPropVarDecl(t));
      iterator i = t.begin();
      m_name = *i++;
      m_variables = *i;
    }

    propositional_variable(aterm_string name, data_variable_list variables)
      : aterm_appl(gsMakePropVarDecl(name, variables)),
        m_name(name),
        m_variables(variables)
    {
    }

    operator pbes_expression() const
    {
      return pbes_expression(m_term);
    }

    /// Returns the term containing the name of the propositional variable.
    aterm_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    data_variable_list parameters() const
    {
      return m_variables;
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief A propositional variable instantiation
///
// <PropVarInst>  ::= PropVarInst(<String>, <DataExpr>*)
class propositional_variable_instantiation: public aterm_appl
{
  protected:
    aterm_string m_name;
    data_expression_list m_expressions;

  public:
    propositional_variable_instantiation()
    {}

    // example: "X(d:D,e:E)"
    propositional_variable_instantiation(std::string s)
    {
      std::pair<std::string, data_expression_list> p = parse_variable(s);
      m_name        = aterm_string(p.first);
      m_expressions = p.second;
      m_term        = reinterpret_cast<ATerm>(gsMakePropVarInst(m_name, m_expressions));
    }

    propositional_variable_instantiation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsPropVarInst(t));
      iterator i = t.begin();
      m_name = *i++;
      m_expressions = *i;
    }

    propositional_variable_instantiation(aterm_string name, data_expression_list expressions)
      : aterm_appl(gsMakePropVarInst(name, expressions)),
        m_name(name),
        m_expressions(expressions)
    {
    }

    operator pbes_expression() const
    {
      return pbes_expression(m_term);
    }

    /// Returns the term containing the name of the propositional variable.
    aterm_string name() const
    {
      return m_name;
    }

    /// Returns the parameters of the propositional variable.
    data_expression_list parameters() const
    {
      return m_expressions;
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_fixpoint_symbol
/// \brief pbes fixpoint symbol (mu or nu).
///
// <FixPoint>     ::= Mu
//                  | Nu
class pbes_fixpoint_symbol: public aterm_appl
{
  public:
    pbes_fixpoint_symbol()
    {}

    pbes_fixpoint_symbol(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsMu(t) || gsIsNu(t));
    }
    
    // allow assignment to aterms
    pbes_fixpoint_symbol& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns true if the symbol is mu.
    bool is_mu() const
    {
      return gsIsMu(*this);
    }

    /// Returns true if the symbol is nu.
    bool is_nu() const
    {
      return gsIsNu(*this);
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_equation
/// \brief pbes equation.
///
class pbes_equation: public aterm_appl
{
  protected:
    pbes_fixpoint_symbol   m_symbol;
    propositional_variable m_variable;
    pbes_expression        m_formula;  // the right hand side

  public:
    static pbes_fixpoint_symbol mu()
    {
      return pbes_fixpoint_symbol(gsMakeMu());
    }

    static pbes_fixpoint_symbol nu()
    {
      return pbes_fixpoint_symbol(gsMakeNu());
    }

    pbes_equation()
    {}

    pbes_equation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(gsIsPBEqn(t));
      iterator i = t.begin();
      m_symbol   = pbes_fixpoint_symbol(*i++);
      m_variable = propositional_variable(*i++);
      m_formula  = pbes_expression(*i);
    }

    pbes_equation(pbes_fixpoint_symbol symbol, propositional_variable variable, pbes_expression expr)
      : aterm_appl(gsMakePBEqn(symbol, variable, expr)),
        m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    // allow assignment to aterms
    pbes_equation& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// Returns the fixpoint symbol of the equation.
    pbes_fixpoint_symbol symbol() const
    {
      return m_symbol;
    }

    /// Returns the pbes variable of the equation.
    propositional_variable variable() const
    {
      return m_variable;
    }

    /// Returns the predicate formula on the right hand side of the equation.
    pbes_expression formula() const
    {
      return m_formula;
    }
    
    /// Returns true if the predicate formula on the right hand side contains no
    /// predicate variables. (COMMENT Wieger: is_const would be a better name)
    bool is_solved() const
    {
      aterm t = atermpp::find_if(ATermAppl(m_formula), is_state_variable());
      return t == aterm(); // true if nothing was found
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_equation_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_equation> pbes_equation_list;

/*
template <typename T, typename Container<T> >
struct container_inserter
{
  Container& m_container;
  
  container_inserter(Container& container)
    : m_container(container)
  {}
  
  void operator()(T t)
  {
    m_container.insert(t);
  }
};

template <typename UnaryPredicate>
void tmp_impl(aterm t, UnaryPredicate pred, atermpp::set<propositional_variable>& s)
{
  aterm_list arguments;
  if (t.type() == AT_LIST) {
    arguments = t;
  }
  else if (t.type() == AT_APPL) {
    if (pred(t)) // only check elements of type aterm_appl...
      s.insert(propositional_variable(aterm_appl(t)));
    arguments = aterm_appl(t).argument_list();
  }
  else {
    return;
  }
  for (aterm_list::iterator i = arguments.begin(); i != arguments.end(); ++i)
  {
    tmp_impl(*i, pred, s);
  }
}

/// Finds all subterms of t that match the predicate op, and applies the given
/// function to it.
template <typename UnaryPredicate>
void tmp(ATermAppl t, UnaryPredicate pred, atermpp::set<propositional_variable>& s)
{
  tmp_impl(t, pred, s);
} 
*/

///////////////////////////////////////////////////////////////////////////////
// equation_system
/// \brief equation_system.
///
class equation_system: public atermpp::vector<pbes_equation>
{
  public:
    equation_system()
    {}

    /// Returns a equation_system containing equation e.
    equation_system(pbes_equation e)
    {
      push_back(e);
    }

    equation_system(pbes_equation_list l)
      : atermpp::vector<pbes_equation>(l.begin(), l.end())
    {}
  
    /// Applies a substitution to this LPE and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    equation_system substitute(Substitution f)
    {
      // todo: implement substitution
      assert(false);
      return *this;
    }     

    /// Returns a equation_system which is the concatenation of the equations of this equation_system and
    /// the other.
    equation_system operator+(equation_system other) const
    {
      equation_system result(*this);
      result.insert(result.end(), other.begin(), other.end());
      return result;
    }

    /// Returns a equation_system which is the concatenation of the equations of this equation_system and
    /// the equation e.
    equation_system operator+(pbes_equation e) const
    {
      equation_system result(*this);
      result.push_back(e);
      return result;
    }

    /// Returns the set of binding variables of the equation_system, i.e. the variables that
    /// occur on the left hand side of an equation.
    atermpp::set<propositional_variable> binding_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// Returns the set of occurring variables of the equation_system, i.e. the variables that
    /// occur in the right hand side of an equation.
    atermpp::set<propositional_variable> occurring_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
//        atermpp::find_all_if(i->formula(), is_state_variable(), std::inserter(result, result.end()));
      }
      return result;
    }

    /// Returns true if all occurring variables are binding variables.
    bool is_closed() const
    {
      atermpp::set<propositional_variable> bnd = binding_variables();
      atermpp::set<propositional_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end());
    }

    /// Returns true if all binding predicate variables of the equation_system are unique.
    /// Note that this does not imply that the names of the binding predicate
    /// variables are unique.
    bool is_well_formed() const
    {
      atermpp::set<propositional_variable> variables;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        propositional_variable p = i->variable();
        atermpp::set<propositional_variable>::iterator j = variables.find(p);
        if (j != variables.end())
          return false;
        variables.insert(p);
      }
      return true;
    }

    /// Returns a representation of the term.
    ///
    std::string to_string() const
    {
      pbes_equation_list eqn(begin(), end());
      return "jammer!";
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes
/// \brief pbes.
///
// <PBES>         ::= PBES(<DataSpec>, <PBEqn>*, <PropVarInst>)
class pbes
{
  protected:
    data_specification m_data;
    equation_system m_equations;
    propositional_variable_instantiation m_initial_state;

  public:
    pbes()
    {}

    pbes(data_specification data,
         equation_system equations, 
         propositional_variable_instantiation initial_state)
      :
        m_data(data),
        m_equations(equations),
        m_initial_state(initial_state)
    {}

    /// Reads the pbes from file. Returns true if the operation succeeded.
    ///
    bool load(const std::string& filename)
    {
      aterm_appl t = atermpp::read_from_named_file(filename);
      assert(gsIsPBES(t));
      if (!t)
        return false;
      aterm_appl::iterator i = t.begin();
      m_data          = data_specification(*i++);
      m_equations     = equation_system(pbes_equation_list(*i++));
      m_initial_state = propositional_variable_instantiation(*i);
      return true;
    }

    /// Writes the pbes to file. Returns true if the operation succeeded.
    ///
    bool save(const std::string& filename, bool binary = true)
    {
      aterm t = ATermAppl(*this);
      if (binary)
      {
        return atermpp::write_to_named_binary_file(t, filename);
      }
      else
      {
        return atermpp::write_to_named_text_file(t, filename);
      }
    }

    /// Returns the equations.
    ///
    const equation_system& equations() const
    {
      return m_equations;
    }

    /// Returns the equations.
    ///
    equation_system& equations()
    {
      return m_equations;
    }

    operator ATermAppl() const
    {
      pbes_equation_list l(m_equations.begin(), m_equations.end());
      return gsMakePBES(m_data, l, m_initial_state);
    }

    ATerm term() const
    {
      return reinterpret_cast<ATerm>(ATermAppl(*this));
    }

    /// Returns the set of binding variables of the pbes, i.e. the variables that
    /// occur on the left hand side of an equation.
    atermpp::set<propositional_variable> binding_variables() const
    {
      return m_equations.binding_variables();
    }

    /// Returns the set of occurring variables of the pbes, i.e. the variables that
    /// occur in the right hand side of an equation.
    atermpp::set<propositional_variable> occurring_variables() const
    {
      return m_equations.occurring_variables();
    }

    /// Returns true if all occurring variables are binding variables.
    bool is_closed() const
    {
      return m_equations.is_closed();
    }

    /// Returns true if all binding predicate variables of the pbes are unique.
    /// Note that this does not imply that the names of the binding predicate
    /// variables are unique.
    bool is_well_formed() const
    {
      return m_equations.is_well_formed();
    }

    /// Returns a representation of the term.
    ///
    std::string to_string() const
    {
      // TODO: implement to_string
      return "pbes::to_string";
    }

    /// Protect the term.
    /// Protects the term from being freed at garbage collection.
    ///
    void protect()
    {
      m_initial_state.protect();
    }

    /// Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    ///
    void unprotect()
    {
      m_initial_state.unprotect();
    }

    /// Mark the term for not being garbage collected.
    ///
    void mark()
    {
      m_initial_state.mark();
    }
};

inline
std::ostream& operator<<(std::ostream& to, const equation_system& p)
{
  // return to << p.to_string();
  to << "# equations: " << p.size() << std::endl;
  for (atermpp::vector<pbes_equation>::const_iterator i = p.begin(); i != p.end(); ++i)
  {
    to << "(" << *i << ")" << std::endl;
  }
  return to;
}

} // namespace lpe

namespace atermpp {

using lpe::pbes_equation;

template <>
struct term_list_iterator_traits<pbes_equation>
{
  typedef ATermAppl value_type;
};

} // namespace atermpp

namespace atermpp
{
using lpe::pbes_expression;
using lpe::propositional_variable;
using lpe::propositional_variable_instantiation;
using lpe::pbes_fixpoint_symbol;
using lpe::pbes_equation;
using lpe::equation_system;
using lpe::pbes;

template<>
struct aterm_traits<pbes_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::pbes_expression t)   { t.protect(); }
  static void unprotect(lpe::pbes_expression t) { t.unprotect(); }
  static void mark(lpe::pbes_expression t)      { t.mark(); }
  static ATerm term(lpe::pbes_expression t)     { return t.term(); }
  static ATerm* ptr(lpe::pbes_expression& t)    { return &t.term(); }
};

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

template<>
struct aterm_traits<pbes_fixpoint_symbol>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::pbes_fixpoint_symbol t)   { t.protect(); }
  static void unprotect(lpe::pbes_fixpoint_symbol t) { t.unprotect(); }
  static void mark(lpe::pbes_fixpoint_symbol t)      { t.mark(); }
  static ATerm term(lpe::pbes_fixpoint_symbol t)     { return t.term(); }
  static ATerm* ptr(lpe::pbes_fixpoint_symbol& t)    { return &t.term(); }
};

template<>
struct aterm_traits<pbes_equation>
{
  typedef ATermAppl aterm_type;
  static void protect(pbes_equation t)   { t.protect(); }
  static void unprotect(pbes_equation t) { t.unprotect(); }
  static void mark(pbes_equation t)      { t.mark(); }
  static ATerm term(pbes_equation t)     { return t.term(); }
  static ATerm* ptr(pbes_equation& t)    { return &t.term(); }
};

template<>
struct aterm_traits<pbes>
{
  typedef ATermAppl aterm_type;
  static void protect(pbes t)   { t.protect(); }
  static void unprotect(pbes t) { t.unprotect(); }
  static void mark(pbes t)      { t.mark(); }
  static ATerm term(pbes t)     { return t.term(); }
  // static ATerm* ptr(pbes& t) undefined for pbes!
};

} // namespace atermpp

#endif // LPE_PBES_H
