///////////////////////////////////////////////////////////////////////////////
/// \file pbes.h
/// Contains pbes data structures for the LPS Library.

#ifndef LPS_PBES_H
#define LPS_PBES_H

#include <functional>
#include <iostream>
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
#include "atermpp/aterm_traits.h"
#include "atermpp/set.h"
#include "atermpp/vector.h"
#include "lps/action.h"
#include "lps/data.h"
#include "lps/data_specification.h"
#include "lps/pbes_expression.h"
#include "lps/mucalculus.h"
#include "lps/pretty_print.h"
#include "lps/detail/utility.h"
#include "libstruct.h"

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;
using lps::detail::parse_variable;

///////////////////////////////////////////////////////////////////////////////
// pbes_fixpoint_symbol
/// \brief pbes fixpoint symbol (mu or nu)
///
// <FixPoint>     ::= Mu
//                  | Nu
class pbes_fixpoint_symbol: public aterm_appl
{
  public:
    pbes_fixpoint_symbol()
      : aterm_appl(detail::constructFixPoint())
    {}

    pbes_fixpoint_symbol(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_FixPoint(m_term));
    }
    
    // allow assignment to aterms
    pbes_fixpoint_symbol& operator=(aterm t)
    {
      m_term = t;
      return *this;
    }

    /// \brief Returns the mu symbol
    static pbes_fixpoint_symbol mu()
    {
      return pbes_fixpoint_symbol(gsMakeMu());
    }

    /// \brief Returns the nu symbol
    static pbes_fixpoint_symbol nu()
    {
      return pbes_fixpoint_symbol(gsMakeNu());
    }

    /// \brief Returns true if the symbol is mu
    bool is_mu() const
    {
      return gsIsMu(*this);
    }

    /// \brief Returns true if the symbol is nu
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
    pbes_equation()
      : aterm_appl(detail::constructPBEqn())
    {}

    pbes_equation(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_PBEqn(m_term));
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

    /// \brief Returns the fixpoint symbol of the equation
    pbes_fixpoint_symbol symbol() const
    {
      return m_symbol;
    }

    /// \brief Returns the pbes variable of the equation
    propositional_variable variable() const
    {
      return m_variable;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation
    pbes_expression formula() const
    {
      return m_formula;
    }
    
    /// \brief Returns true if the predicate formula on the right hand side contains no predicate variables.
    /// (Comment Wieger: is_const would be a better name)
    bool is_solved() const
    {
      aterm t = atermpp::find_if(ATermAppl(m_formula), state_frm::is_var);
      return t == aterm(); // true if nothing was found
    }

    /// \brief Returns true if the equation is a BES (boolean equation system)
    bool is_bes() const
    {
      return variable().parameters().empty() && formula().is_bes();
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes_equation_list
/// \brief singly linked list of data expressions
///
typedef term_list<pbes_equation> pbes_equation_list;

///////////////////////////////////////////////////////////////////////////////
// equation_system
/// \brief equation_system.
///
class equation_system: public atermpp::vector<pbes_equation>
{
  public:
    equation_system()
    {}

    /// \brief Constructs an equation_system containing equation e
    equation_system(pbes_equation e)
    {
      push_back(e);
    }

    equation_system(pbes_equation_list l)
      : atermpp::vector<pbes_equation>(l.begin(), l.end())
    {}
  
    /// \brief Applies a substitution to this equation system.
    /// The Substitution object must supply the method aterm operator()(aterm).
    template <typename Substitution>
    void substitute(Substitution f)
    {
      std::transform(begin(), end(), begin(), f);
    }     

    /// \brief Returns a equation_system which is the concatenation of the equations
    /// of this equation_system and the other
    equation_system operator+(equation_system other) const
    {
      equation_system result(*this);
      result.insert(result.end(), other.begin(), other.end());
      return result;
    }

    /// \brief Returns a equation_system which is the concatenation of the equations
    /// of this equation_system and the equation e
    equation_system operator+(pbes_equation e) const
    {
      equation_system result(*this);
      result.push_back(e);
      return result;
    }

    /// \brief Returns the set of binding variables of the equation_system, i.e. the
    /// variables that occur on the left hand side of an equation
    atermpp::set<propositional_variable> binding_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// \brief Returns the set of occurring variables of the equation_system, i.e. the
    /// variables that occur in the right hand side of an equation
    atermpp::set<propositional_variable> occurring_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        atermpp::find_all_if(i->formula(), state_frm::is_var, std::inserter(result, result.end()));
      }
      return result;
    }

    /// \brief Returns true if all occurring variables are binding variables
    bool is_closed() const
    {
      atermpp::set<propositional_variable> bnd = binding_variables();
      atermpp::set<propositional_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end());
    }

    /// \brief Returns true if all binding predicate variables of the equation_system are unique.
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

    /// \brief Returns an ascii representation of the equation system
    std::string to_string() const
    {
      return pbes_equation_list(begin(), end()).to_string();
    }

    /// \brief Returns true if the equation is a BES (boolean equation system)
    bool is_bes() const
    {
      for (const_iterator i = begin(); i != end(); ++i)
      {
        if (!i->is_bes())
          return false;
      }
      return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
// pbes
/// \brief parameterized boolean equation system
///
// <PBES>         ::= PBES(<DataSpec>, <PBEqn>*, <PropVarInst>)
class pbes
{
  friend struct atermpp::aterm_traits<pbes>;

  protected:
    data_specification m_data;
    equation_system m_equations;
    propositional_variable_instantiation m_initial_state;

    ATerm term() const
    {
      return reinterpret_cast<ATerm>(ATermAppl(*this));
    }

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
    {
      assert(detail::check_rule_PBES(term()));
    }

    /// Returns the data specification.
    data_specification data() const
    {
      return m_data;
    }

    /// Returns the initial state.
    propositional_variable_instantiation initial_state() const
    {
      return m_initial_state;
    }

    /// \brief Reads the pbes from file. Returns true if the operation succeeded.
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

    /// \brief Writes the pbes to file and returns true if the operation succeeded.
    /// \param binary If binary is true the pbes is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
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

    /// \brief Returns the equations
    const equation_system& equations() const
    {
      return m_equations;
    }

    /// \brief Returns the equations
    equation_system& equations()
    {
      return m_equations;
    }

    /// \brief Conversion to ATermAppl
    operator ATermAppl() const
    {
      pbes_equation_list l(m_equations.begin(), m_equations.end());
      return gsMakePBES(m_data, l, m_initial_state);
    }

    /// \brief Returns the set of binding variables of the pbes, i.e. the
    /// variables that occur on the left hand side of an equation
    atermpp::set<propositional_variable> binding_variables() const
    {
      return m_equations.binding_variables();
    }

    /// \brief Returns the set of occurring variables of the pbes, i.e.
    /// the variables that occur in the right hand side of an equation.
    atermpp::set<propositional_variable> occurring_variables() const
    {
      return m_equations.occurring_variables();
    }

    /// \brief Returns true if all occurring variables are binding variables
    bool is_closed() const
    {
      return m_equations.is_closed();
    }

    /// \brief Returns true if all binding predicate variables of the pbes are unique.
    /// Note that this does not imply that the names of the binding predicate
    /// variables are unique.
    bool is_well_formed() const
    {
      return m_equations.is_well_formed();
    }

    /// \brief Protects the term from being freed during garbage collection.
    void protect()
    {
      m_initial_state.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect()
    {
      m_initial_state.unprotect();
    }

    /// \brief Mark the term for not being garbage collected.
    void mark()
    {
      m_initial_state.mark();
    }
};

// inline
// std::ostream& operator<<(std::ostream& to, const equation_system& p)
// {
//   // return to << p.to_string();
//   to << "# equations: " << p.size() << std::endl;
//   for (atermpp::vector<pbes_equation>::const_iterator i = p.begin(); i != p.end(); ++i)
//   {
//     to << "(" << *i << ")" << std::endl;
//   }
//   return to;
// }

} // namespace lps

namespace atermpp {

using lps::pbes_equation;

template <>
struct term_list_iterator_traits<pbes_equation>
{
  typedef ATermAppl value_type;
};

} // namespace atermpp

/// INTERNAL ONLY
namespace atermpp
{
using lps::pbes_expression;
using lps::pbes_fixpoint_symbol;
using lps::pbes_equation;
using lps::equation_system;
using lps::pbes;

template<>
struct aterm_traits<pbes_fixpoint_symbol>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::pbes_fixpoint_symbol t)   { t.protect(); }
  static void unprotect(lps::pbes_fixpoint_symbol t) { t.unprotect(); }
  static void mark(lps::pbes_fixpoint_symbol t)      { t.mark(); }
  static ATerm term(lps::pbes_fixpoint_symbol t)     { return t.term(); }
  static ATerm* ptr(lps::pbes_fixpoint_symbol& t)    { return &t.term(); }
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

#endif // LPS_PBES_H
