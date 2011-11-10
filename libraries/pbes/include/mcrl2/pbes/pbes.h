// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/detail/quantifier_visitor.h"
#include "mcrl2/pbes/detail/occurring_variable_visitor.h"
#include "mcrl2/pbes/detail/pbes_functional.h"

namespace mcrl2
{

/// \brief The main namespace for the PBES library.
namespace pbes_system
{

template <typename Container = atermpp::vector<pbes_equation> > class pbes;
template <typename Container> void complete_data_specification(pbes<Container>&);

// template function overloads
std::string pp(const pbes<>& x);
void normalize_sorts(pbes<>& x, const data::data_specification& dataspec);
void translate_user_notation(pbes_system::pbes<>& x);
std::set<data::sort_expression> find_sort_expressions(const pbes_system::pbes<>& x);
std::set<data::variable> find_variables(const pbes_system::pbes<>& x);
std::set<data::variable> find_free_variables(const pbes_system::pbes<>& x);
std::set<data::function_symbol> find_function_symbols(const pbes_system::pbes<>& x);

template <typename Container>
atermpp::aterm_appl pbes_to_aterm(const pbes<Container>& p);

/// \brief Computes the quantifier variables that occur in the sequence [first, last) of pbes equations.
/// \param first Start of a range of pbes equations
/// \param last End of a range of pbes equations
/// \return The quantifier variables in the sequence [first, last) of pbes equations.
template <typename Iterator>
std::set<data::variable> compute_quantifier_variables(Iterator first, Iterator last)
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
// <PBES>         ::= PBES(<DataSpec>, <GlobVarSpec>, <PBEqnSpec>, <PBInit>)
// <PBEqnSpec>    ::= PBEqnSpec(<PBEqn>*)
template <typename Container>
class pbes
{
    friend struct atermpp::aterm_traits<pbes<Container> >;

  protected:
    /// \brief The data specification
    data::data_specification m_data;

    /// \brief The sequence of pbes equations
    Container m_equations;

    /// \brief The set of global variables
    atermpp::set<data::variable> m_global_variables;

    /// \brief The initial state
    propositional_variable_instantiation m_initial_state;

    /// \brief Initialize the pbes from an ATerm
    /// \param t A term
    void init_term(atermpp::aterm_appl t)
    {
      atermpp::aterm_appl::iterator i = t.begin();
      m_data = atermpp::aterm_appl(*i++);

      data::variable_list global_variables = atermpp::aterm_appl(*i++)(0);
      m_global_variables = atermpp::convert<atermpp::set<data::variable> >(global_variables);

      atermpp::aterm_appl eqn_spec = *i++;
      atermpp::aterm_list eqn = eqn_spec(0);
      m_equations.clear();
      for (atermpp::aterm_list::iterator j = eqn.begin(); j != eqn.end(); ++j)
      {
        m_equations.push_back(pbes_equation(*j));
      }

      atermpp::aterm_appl init = atermpp::aterm_appl(*i);
      m_initial_state = atermpp::aterm_appl(init(0));
    }

    /// \brief Returns the predicate variables appearing in the left hand side of an equation.
    /// \return The predicate variables appearing in the left hand side of an equation.
    atermpp::set<propositional_variable> compute_declared_variables() const
    {
      atermpp::set<propositional_variable> result;
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        result.insert(i->variable());
      }
      return result;
    }

    /// \brief Checks if the sorts of the variables/expressions in both lists are equal.
    /// \param v A sequence of data variables
    /// \param w A sequence of data expressions
    /// \return True if the sorts match pairwise
    bool equal_sorts(data::variable_list v, data::data_expression_list w) const
    {
      if (v.size() != w.size())
      {
        return false;
      }
      data::variable_list::iterator i = v.begin();
      data::data_expression_list::iterator j = w.begin();
      for (; i != v.end(); ++i, ++j)
      {
        if (!m_data.equal_sorts(i->sort(), j->sort()))
        {
          return false;
        }
      }
      return true;
    }

    /// \brief Checks if the propositional variable instantiation v appears with the right type in the
    /// sequence of propositional variable declarations [first, last).
    /// \param first Start of a sequence of propositional variable declarations
    /// \param last End of a sequence of propositional variable declarations
    /// \return True if the type of \p v is matched correctly
    /// \param v A propositional variable instantiation
    template <typename Iter>
    bool is_declared_in(Iter first, Iter last, propositional_variable_instantiation v) const
    {
      for (Iter i = first; i != last; ++i)
      {
        if (i->name() == v.name() && equal_sorts(i->parameters(), v.parameters()))
        {
          return true;
        }
      }
      return false;
    }


    /// \brief Checks if the propositional variable instantiation v has a conflict with the
    /// sequence of propositional variable declarations [first, last).
    /// \param first Start of a sequence of propositional variable declarations
    /// \param last End of a sequence of propositional variable declarations
    /// \return True if a conflict has been detected
    /// \param v A propositional variable instantiation
    template <typename Iter>
    bool has_conflicting_type(Iter first, Iter last, propositional_variable_instantiation v) const
    {
      for (Iter i = first; i != last; ++i)
      {
        if (i->name() == v.name() && !equal_sorts(i->parameters(), v.parameters()))
        {
          return true;
        }
      }
      return false;
    }

  public:
    /// \brief The container type for the equations
    typedef Container container_type;

    /// \brief Constructor.
    pbes()
    {}

    /// \brief Constructor.
    /// \param t An aterm representing a pbes_specification.
    /// \param data_specification_is_type_checked A boolean that indicates whether the
    ///         data specification in the term has been type checked. If so, the internal data specification
    ///         data structures will be set up. Otherwise, the function
    ///         declare_data_specification_to_be_type_checked must be invoked after type checking,
    ///         before the data specification can be used.

    pbes(atermpp::aterm_appl t, const bool data_specification_is_type_checked=true)
    {
      init_term(t);
      if (data_specification_is_type_checked)
      {
        m_data.declare_data_specification_to_be_type_checked();
      }
      assert(core::detail::check_rule_PBES(pbes_to_aterm(*this)));
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param equations A sequence of pbes equations
    /// \param initial_state A propositional variable instantiation
    pbes(data::data_specification const& data,
         const Container& equations,
         propositional_variable_instantiation initial_state)
      :
      m_data(data),
      m_equations(equations),
      m_initial_state(initial_state)
    {
      m_global_variables = pbes_system::find_free_variables(*this);
      assert(core::detail::check_rule_PBES(pbes_to_aterm(*this)));
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param equations A sequence of pbes equations
    /// \param global_variables A sequence of free variables
    /// \param initial_state A propositional variable instantiation
    pbes(data::data_specification const& data,
         const Container& equations,
         const atermpp::set<data::variable>& global_variables,
         propositional_variable_instantiation initial_state)
      :
      m_data(data),
      m_equations(equations),
      m_global_variables(global_variables),
      m_initial_state(initial_state)
    {
      assert(core::detail::check_rule_PBES(pbes_to_aterm(*this)));
    }

    /// \brief Returns the data specification.
    /// \return The data specification of the pbes
    const data::data_specification& data() const
    {
      return m_data;
    }

    /// \brief Returns the data specification.
    /// \return The data specification of the pbes
    data::data_specification& data()
    {
      return m_data;
    }

    /// \brief Returns the equations.
    /// \return The equations.
    const Container& equations() const
    {
      return m_equations;
    }

    /// \brief Returns the equations.
    /// \return The equations.
    Container& equations()
    {
      return m_equations;
    }

    /// \brief Returns the declared free variables of the pbes.
    /// \return The declared free variables of the pbes.
    const atermpp::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    /// \brief Returns the declared free variables of the pbes.
    /// \return The declared free variables of the pbes.
    atermpp::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    const propositional_variable_instantiation& initial_state() const
    {
      return m_initial_state;
    }

    /// \brief Returns the initial state.
    /// \return The initial state.
    propositional_variable_instantiation& initial_state()
    {
      return m_initial_state;
    }

    /// \brief Reads the pbes from file.
    /// \param filename A string
    /// If filename is nonempty, input is read from the file named filename.
    /// If filename is empty, input is read from standard input.
    void load(const std::string& filename)
    {
      atermpp::aterm t = core::detail::load_aterm(filename);
      if (!t || t.type() != AT_APPL || !core::detail::check_rule_PBES(atermpp::aterm_appl(t)))
      {
        throw mcrl2::runtime_error(((filename.empty())?"stdin":("'" + filename + "'")) + " does not contain a PBES");
      }
      init_term(atermpp::aterm_appl(t));
      m_data.declare_data_specification_to_be_type_checked();
      complete_data_specification(*this); // Add all the sorts that are used in the specification
      // to the data specification. This is important for those
      // sorts that are built in, because these are not explicitly
      // declared.

      // The well typedness check is only done in debug mode, since for large
      // PBESs it takes too much time
      assert(is_well_typed());
      //if (!is_well_typed())
      //{
      //  throw mcrl2::runtime_error("PBES is not well typed (pbes::load())");
      //}
    }

    /// \brief Writes the pbes to file.
    /// \param binary If binary is true the pbes is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    /// \param filename A string
    /// \param binary If true the file is saved in binary format
    void save(const std::string& filename, bool binary = true, bool no_well_typedness_check = false) const
    {
      // The well typedness check is only done in debug mode, since for large
      // PBESs it takes too much time
      assert(no_well_typedness_check || is_well_typed());

      pbes<Container> tmp(*this);
      // tmp.data() = data::remove_all_system_defined(tmp.data());
      atermpp::aterm_appl t = pbes_to_aterm(tmp);
      core::detail::save_aterm(t, filename, binary);
    }

    /// \brief Returns the set of binding variables of the pbes.
    /// This is the set variables that occur on the left hand side of an equation.
    /// \return The set of binding variables of the pbes.
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

    /// \brief Returns the set of occurring propositional variable instantiations of the pbes.
    /// This is the set of variables that occur in the right hand side of an equation.
    /// \return The occurring propositional variable instantiations of the pbes
    atermpp::set<propositional_variable_instantiation> occurring_variable_instantiations() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      atermpp::set<propositional_variable_instantiation> result;
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        detail::occurring_variable_visitor visitor;
        visitor.visit(i->formula());
        result.insert(visitor.variables.begin(), visitor.variables.end());
      }
      return result;
    }

    /// \brief Returns the set of occurring propositional variable declarations of the pbes, i.e.
    /// the propositional variable declarations that occur in the right hand side of an equation.
    /// \return The occurring propositional variable declarations of the pbes
    atermpp::set<propositional_variable> occurring_variables() const
    {
      atermpp::set<propositional_variable> result;
      atermpp::set<propositional_variable_instantiation> occ = occurring_variable_instantiations();
      std::map<core::identifier_string, propositional_variable> declared_variables;
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        declared_variables[i->variable().name()] = i->variable();
      }
      for (atermpp::set<propositional_variable_instantiation>::iterator i = occ.begin(); i != occ.end(); ++i)
      {
        result.insert(declared_variables[i->name()]);
      }
      return result;
    }

    /// \brief True if the pbes is closed
    /// \return Returns true if all occurring variables are binding variables, and the initial state variable is a binding variable.
    bool is_closed() const
    {
      atermpp::set<propositional_variable> bnd = binding_variables();
      atermpp::set<propositional_variable> occ = occurring_variables();
      return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end()) && is_declared_in(bnd.begin(), bnd.end(), initial_state());
    }

    /// \brief Applies a low level substitution function to this term.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    template <typename Substitution>
    void substitute(Substitution f)
    {
      std::transform(equations().begin(), equations().end(), equations().begin(), f);
    }

    /// \brief Protects the term from being freed during garbage collection.
    void protect() const
    {
      m_initial_state.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect() const
    {
      m_initial_state.unprotect();
    }

    /// \brief Mark the term for not being garbage collected.
    void mark() const
    {
      m_initial_state.mark();
    }

    /// \brief Checks if the PBES is well typed
    /// \return True if
    /// <ul>
    /// <li>the sorts occurring in the free variables of the equations are declared in the data specification</li>
    /// <li>the sorts occurring in the binding variable parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the quantifier variables of the equations are declared in the data specification </li>
    /// <li>the binding variables of the equations have unique names (well formedness)</li>
    /// <li>the global variables occurring in the equations are declared in global_variables()</li>
    /// <li>the global variables occurring in the equations with the same name are identical</li>
    /// <li>the declared global variables and the quantifier variables occurring in the equations have different names</li>
    /// <li>the predicate variable instantiations occurring in the equations match with their declarations</li>
    /// <li>the predicate variable instantiation occurring in the initial state matches with the declaration</li>
    /// <li>the data specification is well typed</li>
    /// </ul>
    /// N.B. Conflicts between the types of instantiations and declarations of binding variables are not checked!
    bool is_well_typed() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      std::set<data::sort_expression> declared_sorts = data::detail::make_set(data().sorts());
      const atermpp::set<data::variable>& declared_global_variables = global_variables();
      std::set<data::variable> occurring_global_variables = pbes_system::find_free_variables(*this);
      std::set<data::variable> quantifier_variables = compute_quantifier_variables(equations().begin(), equations().end());
      atermpp::set<propositional_variable> declared_variables = compute_declared_variables();
      atermpp::set<propositional_variable_instantiation> occ = occurring_variable_instantiations();

      // check 1)
      if (!data::detail::check_sorts(
            boost::make_transform_iterator(declared_global_variables.begin(), data::detail::sort_of_variable()),
            boost::make_transform_iterator(declared_global_variables.end()  , data::detail::sort_of_variable()),
            declared_sorts
          )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the free variables "
                  << data::pp(declared_global_variables)
                  << " are not declared in the data specification "
                  << data::pp(data().sorts())
                  << std::endl;
        return false;
      }

      // check 2)
      for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
      {
        const data::variable_list& variables = i->variable().parameters();
        if (!data::detail::check_sorts(
              boost::make_transform_iterator(variables.begin(), data::detail::sort_of_variable()),
              boost::make_transform_iterator(variables.end()  , data::detail::sort_of_variable()),
              declared_sorts
            )
           )
        {
          std::cerr << "pbes::is_well_typed() failed: some of the sorts of the binding variable "
                    << data::pp(i->variable())
                    << " are not declared in the data specification "
                    << data::pp(data().sorts())
                    << std::endl;
          return false;
        }
      }

      // check 3)
      if (!data::detail::check_sorts(
            boost::make_transform_iterator(quantifier_variables.begin(), data::detail::sort_of_variable()),
            boost::make_transform_iterator(quantifier_variables.end()  , data::detail::sort_of_variable()),
            declared_sorts
          )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: some of the sorts of the quantifier variables "
                  << data::pp(quantifier_variables)
                  << " are not declared in the data specification "
                  << data::pp(data().sorts())
                  << std::endl;
        return false;
      }

      // check 4)
      if (data::detail::sequence_contains_duplicates(
            boost::make_transform_iterator(equations().begin(), detail::pbes_equation_variable_name()),
            boost::make_transform_iterator(equations().end()  , detail::pbes_equation_variable_name())
          )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: the names of the binding variables are not unique" << std::endl;
        return false;
      }

      // check 5)
      if (!std::includes(declared_global_variables.begin(),
                         declared_global_variables.end(),
                         occurring_global_variables.begin(),
                         occurring_global_variables.end()
                        )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: not all of the free variables are declared\n"
                  << "free variables: " << data::pp(occurring_global_variables) << "\n"
                  << "declared free variables: " << data::pp(declared_global_variables)
                  << std::endl;
        return false;
      }

      // check 6)
      if (data::detail::sequence_contains_duplicates(
            boost::make_transform_iterator(occurring_global_variables.begin(), data::detail::variable_name()),
            boost::make_transform_iterator(occurring_global_variables.end()  , data::detail::variable_name())
          )
         )
      {
        std::cerr << "pbes::is_well_typed() failed: the free variables have no unique names" << std::endl;
        return false;
      }

      // check 7)
      if (!data::detail::set_intersection(declared_global_variables, quantifier_variables).empty())
      {
        std::cerr << "pbes::is_well_typed() failed: the declared free variables and the quantifier variables have collisions" << std::endl;
        return false;
      }

      // check 8)
      for (atermpp::set<propositional_variable_instantiation>::iterator i = occ.begin(); i != occ.end(); ++i)
      {
        if (has_conflicting_type(declared_variables.begin(), declared_variables.end(), *i))
        {
          std::cerr << "pbes::is_well_typed() failed: the occurring variable " << data::pp(*i) << " conflicts with its declaration!" << std::endl;
          return false;
        }
      }

      // check 9)
      if (has_conflicting_type(declared_variables.begin(), declared_variables.end(), initial_state()))
      {
        std::cerr << "pbes::is_well_typed() failed: the initial state " << pbes_system::pp(initial_state()) << " conflicts with its declaration!" << std::endl;
        return false;
      }

      // check 10)
      if (!data().is_well_typed())
      {
        return false;
      }

      return true;
    }
};

/// \brief Conversion to ATermAppl.
/// \return The PBES converted to ATerm format.
template <typename Container>
atermpp::aterm_appl pbes_to_aterm(const pbes<Container>& p)
{
  ATermAppl global_variables = core::detail::gsMakeGlobVarSpec(atermpp::convert<data::variable_list>(p.global_variables()));

  atermpp::aterm_list eqn_list;
  const Container& eqn = p.equations();
  for (typename Container::const_reverse_iterator i = eqn.rbegin(); i != eqn.rend(); ++i)
  {
    atermpp::aterm a = pbes_equation_to_aterm(*i);
    eqn_list = atermpp::push_front(eqn_list, a);
  }
  ATermAppl equations = core::detail::gsMakePBEqnSpec(eqn_list);
  ATermAppl initial_state = core::detail::gsMakePBInit(p.initial_state());
  atermpp::aterm_appl result;

  result = core::detail::gsMakePBES(
             data::detail::data_specification_to_aterm_data_spec(p.data()),
             global_variables,
             equations,
             initial_state);

  return result;
}

/// \brief Adds all sorts that appear in the process of l to the data specification of l.
/// \param l A linear process specification
template <typename Container>
void complete_data_specification(pbes<Container>& p)
{
  std::set<data::sort_expression> s = pbes_system::find_sort_expressions(p);
  p.data().add_context_sorts(s);
}

/// \brief Equality operator on PBESs
/// \return True if the PBESs have exactly the same internal representation. Note
/// that this is in general not a very useful test.
// TODO: improve the comparison
template <typename Container1, typename Container2>
bool operator==(const pbes<Container1>& p1, const pbes<Container2>& p2)
{
  return pbes_to_aterm(p1) == pbes_to_aterm(p2);
}

} // namespace pbes_system

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
template<typename Container>
struct aterm_traits<mcrl2::pbes_system::pbes<Container> >
{
  static void protect(const mcrl2::pbes_system::pbes<Container>& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::pbes_system::pbes<Container>& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::pbes_system::pbes<Container>& t)
  {
    t.mark();
  }
  static ATerm term(const mcrl2::pbes_system::pbes<Container>& t)
  {
    atermpp::aterm x = pbes_to_aterm(t);
    return x;
  }
};
}
/// \endcond

#endif // MCRL2_PBES_PBES_H
