// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_PBES_H
#define MCRL2_PBES_PBES_H

#include <functional>
#include <iostream>
#include <utility>
#include <string>
#include <cassert>
#include <set>
#include <iterator>
#include <algorithm>
#include <iterator>

#include "atermpp/aterm_list.h"
#include "atermpp/set.h"
#include "atermpp/vector.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/basic/pretty_print.h"
#include "mcrl2/lps/detail/utility.h"
#include "mcrl2/pbes/pbes_equation.h"

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;

/// INTERNAL ONLY
/// Computes the free variables in a data expression.
struct data_variable_collector
{
  const data_variable_list& m_bound_variables;
  const atermpp::vector<data_variable>& m_quantifier_stack;
  atermpp::set<data_variable>& m_result;
    
  data_variable_collector(const data_variable_list& bound_variables,
                          const atermpp::vector<data_variable>& quantifier_stack,
                          atermpp::set<data_variable>& result
                         )
    : m_bound_variables(bound_variables), m_quantifier_stack(quantifier_stack), m_result(result)
  {}
  
  bool operator()(aterm_appl t)
  {
    if (is_data_variable(t))
    {
      if (std::find(m_bound_variables.begin(), m_bound_variables.end(), t) == m_bound_variables.end()
          && std::find(m_quantifier_stack.begin(), m_quantifier_stack.end(), t) == m_quantifier_stack.end()
         )
      {
        m_result.insert(data_variable(t));
      }
      return false;
    }
    return true;
  }
};

/// INTERNAL ONLY
/// Computes the free variables in the pbes expression t.
///
inline
void collect_free_pbes_variables(pbes_expression t,
                                 const data_variable_list& bound_variables,
                                 atermpp::vector<data_variable>& quantifier_stack,
                                 atermpp::set<data_variable>& result
                                )
{
  using namespace lps::pbes_expr;

  if(is_and(t)) {
    collect_free_pbes_variables(lhs(t), bound_variables, quantifier_stack, result);
    collect_free_pbes_variables(rhs(t), bound_variables, quantifier_stack, result);
  }
  else if(is_or(t)) {
    collect_free_pbes_variables(lhs(t), bound_variables, quantifier_stack, result);
    collect_free_pbes_variables(rhs(t), bound_variables, quantifier_stack, result);
  }
  else if(is_forall(t)) {
    data_variable_list vars = quant_vars(t);   
    std::copy(vars.begin(), vars.end(), std::back_inserter(quantifier_stack));
    collect_free_pbes_variables(quant_expr(t), bound_variables, quantifier_stack, result);
    quantifier_stack.erase(quantifier_stack.end() - vars.size(), quantifier_stack.end());
  }
  else if(is_exists(t)) {
    data_variable_list vars = quant_vars(t);   
    std::copy(vars.begin(), vars.end(), std::back_inserter(quantifier_stack));
    collect_free_pbes_variables(quant_expr(t), bound_variables, quantifier_stack, result);
    quantifier_stack.erase(quantifier_stack.end() - vars.size(), quantifier_stack.end());
  }
  else if(is_propositional_variable_instantiation(t)) {
    data_expression_list l = propositional_variable_instantiation(t).parameters();
    for (data_expression_list::iterator i = l.begin(); i != l.end(); ++i)
      atermpp::for_each(*i, data_variable_collector(bound_variables, quantifier_stack, result));
  }
  else if(is_true(t)) {
  }
  else if(is_false(t)) {
  }
  else if(is_data(t)) {
    atermpp::for_each(t, data_variable_collector(bound_variables, quantifier_stack, result));
  }
}

/// INTERNAL ONLY
/// Computes the free variables in the sequence of pbes equations [first, last[.
///
template <typename EquationIterator>
atermpp::set<data_variable> free_pbes_variables(EquationIterator first, EquationIterator last)
{
  atermpp::set<data_variable> result;
  for (EquationIterator i = first; i != last; ++i)
  {
    atermpp::vector<data_variable> quantifier_stack;
    collect_free_pbes_variables(i->formula(), i->variable().parameters(), quantifier_stack, result);
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// equation_system
/// \brief equation_system.
///
class equation_system: public atermpp::vector<pbes_equation>
{
  public:
    equation_system()
    {}

    /// Constructs an equation_system containing equation e.
    ///
    equation_system(pbes_equation e)
    {
      push_back(e);
    }

    equation_system(pbes_equation_list l)
      : atermpp::vector<pbes_equation>(l.begin(), l.end())
    {}
  
    /// Applies a substitution to this equation system.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    void substitute(Substitution f)
    {
      std::transform(begin(), end(), begin(), f);
    }     

    /// Returns a equation_system which is the concatenation of the equations
    /// of this equation_system and the other.
    ///
    equation_system operator+(equation_system other) const
    {
      equation_system result(*this);
      result.insert(result.end(), other.begin(), other.end());
      return result;
    }

    /// Returns a equation_system which is the concatenation of the equations
    /// of this equation_system and the equation e.
    equation_system operator+(pbes_equation e) const
    {
      equation_system result(*this);
      result.push_back(e);
      return result;
    }

    /// Returns the set of binding variables of the equation_system, i.e. the
    /// variables that occur on the left hand side of an equation.
    ///
    atermpp::set<propositional_variable> binding_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// Returns the set of occurring variables of the equation_system, i.e. the
    /// variables that occur in the right hand side of an equation.
    ///
    atermpp::set<propositional_variable> occurring_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (const_iterator i = begin(); i != end(); ++i)
      {
        atermpp::find_all_if(i->formula(), state_frm::is_var, std::inserter(result, result.end()));
      }
      return result;
    }

    /// Returns true if all occurring variables are binding variables.
    ///
    bool is_closed() const
    {
      atermpp::set<propositional_variable> bnd = binding_variables();
      atermpp::set<propositional_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end());
    }

    /// Returns true if all binding predicate variables of the equation_system are unique.
    /// Note that this does not imply that the names of the binding predicate
    /// variables are unique.
    ///
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

    /// Returns an ascii representation of the equation system.
    ///
    std::string to_string() const
    {
      return pbes_equation_list(begin(), end()).to_string();
    }

    /// Returns true if the equation is a BES (boolean equation system).
    ///
    bool is_bes() const
    {
      for (const_iterator i = begin(); i != end(); ++i)
      {
        if (!i->is_bes())
          return false;
      }
      return true;
    }
    
    /// Computes the set of free variables.
    ///
    atermpp::set<data_variable> free_variables() const
    {
      return free_pbes_variables(begin(), end());
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

    /// Writes the pbes to file and returns true if the operation succeeded.
    /// \param binary If binary is true the pbes is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
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

    /// Conversion to ATermAppl.
    ///
    operator ATermAppl() const
    {
      pbes_equation_list l(m_equations.begin(), m_equations.end());
      return gsMakePBES(m_data, l, m_initial_state);
    }

    /// Returns the set of binding variables of the pbes, i.e. the
    /// variables that occur on the left hand side of an equation.
    ///
    atermpp::set<propositional_variable> binding_variables() const
    {
      return m_equations.binding_variables();
    }

    /// Returns the set of occurring variables of the pbes, i.e.
    /// the variables that occur in the right hand side of an equation.
    ///
    atermpp::set<propositional_variable> occurring_variables() const
    {
      return m_equations.occurring_variables();
    }

    /// Returns true if all occurring variables are binding variables.
    ///
    bool is_closed() const
    {
      return m_equations.is_closed();
    }

    /// Returns true if all binding predicate variables of the pbes are unique.
    /// Note that this does not imply that the names of the binding predicate
    /// variables are unique.
    ///
    bool is_well_formed() const
    {
      return m_equations.is_well_formed();
    }

    /// Computes the set of free variables.
    ///
    atermpp::set<data_variable> free_variables() const
    {
      // collect the free variables of the equations
      atermpp::set<data_variable> result = m_equations.free_variables();
        
      // add the (free) variables appearing in the initial state
      for (data_expression_list::iterator i = m_initial_state.parameters().begin(); i != m_initial_state.parameters().end(); ++i)
        atermpp::for_each(*i, data_variable_collector(data_variable_list(), atermpp::vector<data_variable>(), result));

      return result;
    }

    /// Protects the term from being freed during garbage collection.
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

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::pbes;

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

#endif // MCRL2_PBES_PBES_H
