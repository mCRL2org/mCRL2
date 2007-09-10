// Author(s): Wieger Wesselink
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
#include <stdexcept>

#include "atermpp/aterm_list.h"
#include "atermpp/set.h"
#include "atermpp/vector.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/basic/pretty_print.h"
#include "mcrl2/lps/detail/utility.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_initializer.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"
#include "mcrl2/pbes/detail/free_variables.h"

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;

///////////////////////////////////////////////////////////////////////////////
// equation_system
/// \brief equation_system.
///
class equation_system: public atermpp::vector<pbes_equation>
{
  protected:
    atermpp::set<data_variable> m_free_variables;
    
  public:
    equation_system()
    {}

    /// Constructs an equation_system containing equation e.
    ///
    equation_system(const pbes_equation& e, atermpp::set<data_variable> free_variables = atermpp::set<data_variable>())
      : m_free_variables(free_variables)
    {
      push_back(e);
    }

    equation_system(const pbes_equation_list& l, atermpp::set<data_variable> free_variables = atermpp::set<data_variable>())
      : atermpp::vector<pbes_equation>(l.begin(), l.end()),
        m_free_variables(free_variables)
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
    
    /// Returns the free variables of this equation system.
    const atermpp::set<data_variable>& free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the free variables of this equation system.
    atermpp::set<data_variable>& free_variables()
    {
      return m_free_variables;     
    }

    /// Returns true if
    /// <ul>
    /// <li>all binding predicate variables of the equation_system are unique. Note that this does not imply that the names of the binding predicate variables are unique.</li>
    /// <li>the free variables occurring in the equations are contained in free_variables()       </li>
    /// <li></li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      // check )
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
};

/// Computes the free variables that occur in the pbes equation system.
///
inline
std::set<data_variable> compute_free_variables(const equation_system& eqn)
{
  return detail::free_pbes_variables(eqn.begin(), eqn.end());
}

///////////////////////////////////////////////////////////////////////////////
// pbes
/// \brief parameterized boolean equation system
///
// <PBES>         ::= PBES(<DataSpec>, <PBEqnSpec>, <PBInit>)
// <PBEqnSpec>    ::= PBEqnSpec(<DataVarId>*, <PBEqn>*)

class pbes
{
  friend struct atermpp::aterm_traits<pbes>;

  protected:
    data_specification m_data;
    equation_system m_equations;
    pbes_initializer m_initial_state;

    ATerm term() const
    {
      return reinterpret_cast<ATerm>(ATermAppl(*this));
    }

    /// Initialize the pbes with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      aterm_appl::iterator i = t.begin();
      m_data          = aterm_appl(*i++);
      aterm_appl eqn_spec = *i++;
      data_variable_list freevars = eqn_spec(0);
      pbes_equation_list eqn = eqn_spec(1);
      atermpp::set<data_variable> v;
      std::copy(freevars.begin(), freevars.end(), std::inserter(v, v.begin()));
      m_equations     = equation_system(eqn, v);
      m_initial_state = pbes_initializer(*i);
    }

  public:
    pbes()
    {}

    pbes(data_specification data,
         equation_system equations, 
         pbes_initializer initial_state)
      :
        m_data(data),
        m_equations(equations),
        m_initial_state(initial_state)
    {
      assert(detail::check_rule_PBES(term()));
    }

    pbes(data_specification data,
         equation_system equations, 
         propositional_variable_instantiation initial_state)
      :
        m_data(data),
        m_equations(equations),
        m_initial_state(pbes_initializer(data_variable_list(), initial_state))
    {
      assert(detail::check_rule_PBES(term()));
    }

    /// Returns the data specification.
    data_specification data() const
    {
      return m_data;
    }

    /// Returns the initial state.
    pbes_initializer initial_state() const
    {
      return m_initial_state;
    }

    /// Reads the pbes from file. Returns true if the operation succeeded.
    ///
    void load(const std::string& filename)
    {
      aterm t = atermpp::read_from_named_file(filename);
      if (!t || t.type() != AT_APPL || !detail::check_rule_PBES(aterm_appl(t)))
        throw std::runtime_error(std::string("Error in pbes::load(): could not read from file " + filename));

      init_term(aterm_appl(t));

      if (!is_well_typed())
        throw std::runtime_error("Error in pbes::load(): term is not well typed");
    }

    /// Writes the pbes to file and returns true if the operation succeeded.
    /// \param binary If binary is true the pbes is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    ///
    bool save(const std::string& filename, bool binary = true) const
    {
      if (!is_well_typed())
        throw std::runtime_error("Error in pbes::save(): term is not well typed");

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
      // convert the equation system to ATerm format
      data_variable_list free_variables(m_equations.free_variables().begin(), m_equations.free_variables().end());
      pbes_equation_list equations(m_equations.begin(), m_equations.end());
      return gsMakePBES(m_data, gsMakePBEqnSpec(free_variables, equations), m_initial_state);
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

/*
    /// Computes the set of free variables.
    ///
    atermpp::set<data_variable> free_variables() const
    {
      // collect the free variables of the equations
      atermpp::set<data_variable> result = m_equations.free_variables();
        
      // add the (free) variables appearing in the initial state
      const data_expression_list parameters = m_initial_state.variable().parameters();
      for (data_expression_list::iterator i = parameters.begin(); i != parameters.end(); ++i)
        atermpp::for_each(*i, data_variable_collector(data_variable_list(), atermpp::vector<data_variable>(), result));

      return result;
    }
*/

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

    /// Returns true if
    /// <ul>
    /// <li>the sorts occurring in the free variables of the equations are declared in the data specification</li>
    /// <li>the sorts occurring in the binding variable parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the quantifier variables of the equations are declared in the data specification </li>
    ///
    /// <li>instances of free variables in the equations with the same name are identical</li>
    /// <li>the binding variables of the equations have unique names (well formedness)</li>
    ///
    /// <li>the data specification is well typed</li>
    /// <li>the equations are well typed</li>
    /// <li>the initial state is well typed</li>
    /// </ul>
    ///
    /// N.B. Conflicts between the types of instantiations and declarations of binding variables are not checked!
    bool is_well_typed() const
    {
      std::set<lps::sort> declared_sorts = detail::make_set(data().sorts());

      // check 1)
      if (!detail::is_subset_of(boost::make_transform_iterator(equations().free_variables().begin(), detail::data_variable_sort()),
                                boost::make_transform_iterator(equations().free_variables().end()  , detail::data_variable_sort()),
                                declared_sorts
                               )
         )
      {
        data_variable_list free_variables(equations().free_variables().begin(), equations().free_variables().end());
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the free variables " << pp(free_variables) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 2)
      for (equation_system::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        const data_variable_list& variables = i->variable().parameters();
        if (!detail::is_subset_of(boost::make_transform_iterator(variables.begin(), detail::data_variable_sort()),
                                  boost::make_transform_iterator(variables.end()  , detail::data_variable_sort()),
                                  declared_sorts
                                 )
           )
        {
          std::cerr << "pbes::is_well_typed() failed: some of the sorts of the binding variable " << pp(i->variable()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
          return false;
        }
      }

      // check 3)
      // collect the set of all quantifier variables in qvisitor
      detail::quantifier_visitor qvisitor;
      for (equation_system::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        qvisitor.visit(i->formula());
      }
      // check the existence of the sorts
      if (!detail::is_subset_of(boost::make_transform_iterator(qvisitor.variables.begin(), detail::data_variable_sort()),
                                boost::make_transform_iterator(qvisitor.variables.end()  , detail::data_variable_sort()),
                                declared_sorts
                               )
         )
      {
        data_variable_list quantifier_variables(qvisitor.variables.begin(), qvisitor.variables.end());
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the quantifier variables " << pp(quantifier_variables) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 6)
      if (!data().is_well_typed())
      {
        return false;
      }

      // check 7)
      if (!equations().is_well_typed())
      {
        return false;
      }

      // check 8)
      if (!initial_state().is_well_typed())
      {
        return false;
      }

      return true;
    }
};

/// Computes the free variables that occur in the pbes.
///
inline
std::set<data_variable> compute_free_variables(const pbes& p)
{
  return compute_free_variables(p.equations());
}

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
