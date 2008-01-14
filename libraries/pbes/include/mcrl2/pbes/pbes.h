// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes.h
/// \brief The class pbes.

#ifndef MCRL2_PBES_PBES_H
#define MCRL2_PBES_PBES_H

#include <functional>
#include <iostream>
#include <utility>
#include <string>
#include <cassert>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lps/detail/data_utility.h"
#include "mcrl2/lps/detail/sequence_algorithm.h"
#include "mcrl2/lps/detail/sorted_sequence_algorithm.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_initializer.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"
#include "mcrl2/pbes/detail/pbes_functional.h"

namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::read_from_named_file;

/// \cond INTERNAL_DOCS
struct normalize_pbes_equation
{
  pbes_equation operator()(const pbes_equation& e) const
  {
    return normalize(e);
  }
};
/// \endcond

/// Computes the free variables that occur in the sequence [first, last[
/// of pbes equations.
template <typename Iterator>
std::set<data_variable> compute_free_variables(Iterator first, Iterator last)
{
  using namespace std::rel_ops; // for definition of operator!= in terms of operator==

  detail::free_variable_visitor visitor;

  for (Iterator i = first; i != last; ++i)
  {
    visitor.bound_variables = i->variable().parameters();
    visitor.visit(i->formula());
  }

  return visitor.result;
}

/// Computes the quantifier variables that occur in the sequence [first, last[
/// of pbes equations.
///
template <typename Iterator>
std::set<data_variable> compute_quantifier_variables(Iterator first, Iterator last)
{
  using namespace std::rel_ops; // for definition of operator!= in terms of operator==

  // collect the set of all quantifier variables in visitor
  detail::quantifier_visitor visitor;
  for (Iterator i = first; i != last; ++i)
  {
    visitor.visit(i->formula());
  }
  return visitor.variables;
}

/// \brief parameterized boolean equation system
///
// <PBES>         ::= PBES(<DataSpec>, <PBEqnSpec>, <PBInit>)
// <PBEqnSpec>    ::= PBEqnSpec(<DataVarId>*, <PBEqn>*)

template <typename Container = atermpp::vector<pbes_equation> >
class pbes
{
  friend struct atermpp::aterm_traits<pbes>;

  protected:
    data_specification m_data;
    Container m_equations;
    atermpp::set<data_variable> m_free_variables;
    propositional_variable_instantiation m_initial_state;

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
      pbes_initializer init = pbes_initializer(*i);

      m_initial_state = init.variable();
      data_variable_list freevars = eqn_spec(0);
      data_variable_list init_freevars = init.free_variables();
      pbes_equation_list eqn = eqn_spec(1);

      // combine the free variables of the equations and the initial state
      m_free_variables.clear();
      m_free_variables.insert(freevars.begin(), freevars.end());
      m_free_variables.insert(init_freevars.begin(), init_freevars.end());

      m_equations = Container(eqn.begin(), eqn.end());
    }

  public:
    /// Constructor.
    ///
    pbes()
    {}

    /// Constructor.
    ///
    pbes(data_specification data,
         const Container& equations,
         propositional_variable_instantiation initial_state)
      :
        m_data(data),
        m_equations(equations),
        m_initial_state(initial_state)
    {
      m_free_variables = compute_free_variables(m_equations.begin(), m_equations.end());
      assert(detail::check_rule_PBES(term()));
    }

    /// Constructor.
    ///
    pbes(data_specification data,
         const Container& equations,
         const atermpp::set<data_variable>& free_variables,
         propositional_variable_instantiation initial_state)
      :
        m_data(data),
        m_equations(equations),
        m_free_variables(free_variables),
        m_initial_state(initial_state)
    {
      assert(detail::check_rule_PBES(term()));
    }

    /// Returns the data specification.
    ///
    const data_specification& data() const
    {
      return m_data;
    }

    /// Returns the data specification.
    ///
    data_specification& data()
    {
      return m_data;
    }

    /// Returns the equations.
    ///
    const Container& equations() const
    {
      return m_equations;
    }

    /// Returns the equations.
    ///
    Container& equations()
    {
      return m_equations;
    }

    /// Returns the declared free variables of the pbes.
    ///
    const atermpp::set<data_variable>& free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the declared free variables of the pbes.
    ///
    atermpp::set<data_variable>& free_variables()
    {
      return m_free_variables;
    }

    /// Returns the initial state.
    ///
    propositional_variable_instantiation initial_state() const
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

    /// Returns true if the PBES is a BES (boolean equation system).
    ///
    bool is_bes() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        if (!i->is_bes())
          return false;
      }
      return true;
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
        return atermpp::write_to_named_saf_file(t, filename);
      }
      else
      {
        return atermpp::write_to_named_text_file(t, filename);
      }
    }

    /// Conversion to ATermAppl.
    ///
    operator ATermAppl() const
    {
      // convert the equation system to ATerm format
      data_variable_list free_variables(m_free_variables.begin(), m_free_variables.end());
      pbes_equation_list equations(m_equations.begin(), m_equations.end());
      return gsMakePBES(m_data, gsMakePBEqnSpec(free_variables, equations), pbes_initializer(free_variables, m_initial_state));
    }

    /// Returns the set of binding variables of the pbes, i.e. the
    /// variables that occur on the left hand side of an equation.
    ///
    atermpp::set<propositional_variable> binding_variables() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      atermpp::set<propositional_variable> result;
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// Returns the set of occurring variables of the pbes, i.e.
    /// the variables that occur in the right hand side of an equation.
    ///
    atermpp::set<propositional_variable> occurring_variables() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      atermpp::set<propositional_variable> result;
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
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

    /// Applies normalization to the equations of the pbes.
    /// 
    void normalize()
    {
      std::transform(equations().begin(), equations().end(), equations().begin(), normalize_pbes_equation());
    }

    /// Returns true if the pbes is normalized.
    ///
    bool is_normalized() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        is_normalized_visitor visitor;
        visitor.visit(i->formula());
        if (visitor.result == false)
        {
          return false;
        }
      }
      return true;
    }

    /// Applies a substitution to the pbes equations.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    void substitute(Substitution f)
    {
      std::transform(equations().begin(), equations().end(), equations().begin(), f);
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

    /// Returns true if
    /// <ul>
    /// <li>the sorts occurring in the free variables of the equations are declared in the data specification</li>
    /// <li>the sorts occurring in the binding variable parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the quantifier variables of the equations are declared in the data specification </li>
    ///
    /// <li>the binding variables of the equations have unique names (well formedness)</li>
    /// <li>the free variables occurring in the equations are declared in free_variables()</li>
    /// <li>the free variables occurring in the equations with the same name are identical</li>
    /// <li>the declared free variables and the quantifier variables occurring in the equations have different names</li>
    ///
    /// <li>the data specification is well typed</li>
    /// </ul>
    ///
    /// N.B. Conflicts between the types of instantiations and declarations of binding variables are not checked!
    bool is_well_typed() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      std::set<sort_expression> declared_sorts = detail::make_set(data().sorts());
      const atermpp::set<data_variable>& declared_free_variables = free_variables();
      std::set<data_variable> occurring_free_variables = compute_free_variables(equations().begin(), equations().end());
      std::set<data_variable> quantifier_variables = compute_quantifier_variables(equations().begin(), equations().end());

      // check 1)
      if (!detail::check_sorts(
              boost::make_transform_iterator(declared_free_variables.begin(), detail::data_variable_sort()),
              boost::make_transform_iterator(declared_free_variables.end()  , detail::data_variable_sort()),
              declared_sorts
             )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the free variables "
                  << pp(data_variable_list(declared_free_variables.begin(), declared_free_variables.end()))
                  << " are not declared in the data specification "
                  << pp(data().sorts())
                  << std::endl;
        return false;
      }

      // check 2)
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        const data_variable_list& variables = i->variable().parameters();
        if (!detail::check_sorts(
               boost::make_transform_iterator(variables.begin(), detail::data_variable_sort()),
               boost::make_transform_iterator(variables.end()  , detail::data_variable_sort()),
               declared_sorts
              )
           )
        {
          std::cerr << "pbes::is_well_typed() failed: some of the sorts of the binding variable "
                    << pp(i->variable())
                    << " are not declared in the data specification "
                    << pp(data().sorts())
                    << std::endl;
          return false;
        }
      }

      // check 3)
      if (!detail::check_sorts(
              boost::make_transform_iterator(quantifier_variables.begin(), detail::data_variable_sort()),
              boost::make_transform_iterator(quantifier_variables.end()  , detail::data_variable_sort()),
              declared_sorts
             )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the quantifier variables "
                  << pp(data_variable_list(quantifier_variables.begin(), quantifier_variables.end()))
                  << " are not declared in the data specification "
                  << pp(data().sorts())
                  << std::endl;
        return false;
      }

      // check 4)
      if (detail::sequence_contains_duplicates(
               boost::make_transform_iterator(equations().begin(), detail::pbes_equation_variable_name()),
               boost::make_transform_iterator(equations().end()  , detail::pbes_equation_variable_name())
              )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: the names of the binding variables are not unique" << std::endl;
        return false;
      }

      // check 5)
      if (!std::includes(declared_free_variables.begin(),
                         declared_free_variables.end(),
                         occurring_free_variables.begin(),
                         occurring_free_variables.end()
                        )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: not all of the free variables are declared\n"
                  << "free variables: " << pp(data_variable_list(occurring_free_variables.begin(), occurring_free_variables.end())) << "\n"
                  << "declared free variables: " << pp(data_variable_list(declared_free_variables.begin(), declared_free_variables.end()))
                  << std::endl;
        return false;
      }

      // check 6)
      if (detail::sequence_contains_duplicates(
               boost::make_transform_iterator(occurring_free_variables.begin(), detail::data_variable_name()),
               boost::make_transform_iterator(occurring_free_variables.end()  , detail::data_variable_name())
              )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: the free variables have no unique names" << std::endl;
        return false;
      }

      // check 7)
      if (!detail::set_intersection(declared_free_variables, quantifier_variables).empty())
      {
        std::cerr << "pbes::is_well_typed() failed: the declared free variables and the quantifier variables have collisions" << std::endl;
        return false;
      }

      // check 8)
      if (!data().is_well_typed())
      {
        return false;
      }

      return true;
    }
};

/// Computes the free variables that occur in the pbes.
template <typename Container>
std::set<data_variable> compute_free_variables(const pbes<Container>& p)
{
  return compute_free_variables(p.equations().begin(), p.equations().end());
}

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::pbes;

template<typename Container>
struct aterm_traits<pbes<Container> >
{
  typedef ATermAppl aterm_type;
  static void protect(pbes<Container> t)   { t.protect(); }
  static void unprotect(pbes<Container> t) { t.unprotect(); }
  static void mark(pbes<Container> t)      { t.mark(); }
  static ATerm term(pbes<Container> t)     { return t.term(); }
  // static ATerm* ptr(pbes& t) undefined for pbes!
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_PBES_PBES_H
