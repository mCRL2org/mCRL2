// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_equation_system.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
#define MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <boost/foreach.hpp>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/exception.h"
#include "mcrl2/bes/boolean_equation.h"

namespace mcrl2 {

namespace bes {

// forward declarations
template <typename Container, typename OutputIterator>
void find_variables(Container const& container, OutputIterator o);

/// \brief boolean equation system
  // <BES>          ::= BES(<BooleanEquation>*, <BooleanExpression>)
  template <typename Container = atermpp::vector<boolean_equation> >
  class boolean_equation_system
  {
    friend struct atermpp::aterm_traits<boolean_equation_system>;

    protected:
      /// \brief The equations
      Container m_equations;

      /// \brief The initial state
      boolean_expression m_initial_state;

      /// \brief Conversion to ATerm
      /// \return The converted term
      ATerm term() const
      {
        return reinterpret_cast<ATerm>(ATermAppl(*this));
      }

      /// \brief Initialize the boolean_equation_system with an atermpp::aterm_appl.
      /// \param t A term
      void init_term(atermpp::aterm_appl t)
      {
        atermpp::aterm_appl::iterator i = t.begin();
        boolean_equation_list eqn = *i++;
        m_initial_state = boolean_expression(*i);
        m_equations = Container(eqn.begin(), eqn.end());
      }

    public:
      /// \brief Constructor.
      boolean_equation_system()
        : m_initial_state(core::term_traits<boolean_expression>::true_())
      {}

      /// \brief Constructor.
      /// \param equations A sequence of boolean equations
      /// \param initial_state An initial state
      boolean_equation_system(
          const Container& equations,
          boolean_expression initial_state)
        :
          m_equations(equations),
          m_initial_state(initial_state)
      {
        assert(core::detail::check_rule_BES(term()));
      }

      /// \brief Returns the equations.
      /// \return The equations
      const Container& equations() const
      {
        return m_equations;
      }

      /// \brief Returns the equations.
      /// \return The equations
      Container& equations()
      {
        return m_equations;
      }

      /// \brief Returns the initial state.
      /// \return The initial state.
      const boolean_expression& initial_state() const
      {
        return m_initial_state;
      }

      /// \brief Returns the initial state.
      /// \return The initial state.
      boolean_expression& initial_state()
      {
        return m_initial_state;
      }

      /// \brief Returns true.
      /// Some checks will be added later.
      /// \return The value true.
      bool is_well_typed() const
      {
        return true;
      }

      /// \brief Reads the boolean equation system from file.
      /// \param filename A string
      /// If filename is nonempty, input is read from the file named filename.
      /// If filename is empty, input is read from standard input.
      void load(const std::string& filename)
      {
        atermpp::aterm t = core::detail::load_aterm(filename);
        if (!t || t.type() != AT_APPL || !core::detail::check_rule_BES(atermpp::aterm_appl(t)))
        {
          throw mcrl2::runtime_error(((filename.empty())?"stdin":("'" + filename + "'")) + " does not contain a boolean equation system");
        }
        init_term(atermpp::aterm_appl(t));
        if (!is_well_typed())
        {
          throw mcrl2::runtime_error("boolean equation system is not well typed (boolean_equation_system::load())");
        }
      }

      /// \brief Writes the boolean equation system to file.
      /// \param binary If binary is true the boolean equation system is saved in compressed binary format.
      /// Otherwise an ascii representation is saved. In general the binary format is
      /// much more compact than the ascii representation.
      /// \param filename A string
      /// \param binary If true, the file is saved in binary format
      void save(const std::string& filename, bool binary = true) const
      {
        if (!is_well_typed())
        {
          throw mcrl2::runtime_error("boolean equation system is not well typed (boolean_equation_system::save())");
        }
        atermpp::aterm_appl t = ATermAppl(*this);
        core::detail::save_aterm(t, filename, binary);
      }

      /// \brief Conversion to ATermAppl.
      /// \return An ATerm representation of the boolean equation system
      operator ATermAppl() const
      {
        boolean_equation_list equations(m_equations.begin(), m_equations.end());
        return core::detail::gsMakeBES(equations, m_initial_state);
      }

      /// \brief Returns the set of binding variables of the boolean_equation_system, i.e. the
      /// variables that occur on the left hand side of an equation.
      /// \return The binding variables of the equation system
      std::set<boolean_variable> binding_variables() const
      {
        std::set<boolean_variable> result;
        for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
        {
          result.insert(i->variable());
        }
        return result;
      }

      /// \brief Returns the set of occurring variables of the boolean_equation_system, i.e.
      /// the variables that occur in the right hand side of an equation or in the
      /// initial state.
      /// \return The occurring variables of the equation system
      std::set<boolean_variable> occurring_variables() const
      {
        std::set<boolean_variable> result;
        for (typename Container::const_iterator i = m_equations.begin(); i != m_equations.end(); ++i)
        {
          find_variables(i->formula(), std::inserter(result, result.end()));
        }
        find_variables(m_initial_state, std::inserter(result, result.end()));
        return result;
      }

      /// \brief Returns true if all occurring variables are binding variables.
      /// \return True if the equation system is closed
      bool is_closed() const
      {
        std::set<boolean_variable> bnd = binding_variables();
        std::set<boolean_variable> occ = occurring_variables();
        return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end()) && bnd.find(initial_state()) != bnd.end();
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

  /// \brief Pretty print function
  /// \param p A boolean equation system
  /// \return A pretty printed representation of the boolean equation system
  template <typename Container>
  std::string pp(const boolean_equation_system<Container>& p)
  {
    std::ostringstream out;
    out << "pbes\n";
    BOOST_FOREACH(const boolean_equation& eq, p.equations())
    {
      out << "    " << pp(eq) << ";" << std::endl;
    }
    out << "\ninit " << pp(p.initial_state()) << ";" << std::endl;
    return out.str();
  }

} // namespace bes

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp {
template<typename Container>
struct aterm_traits<mcrl2::bes::boolean_equation_system<Container> >
{
  typedef ATermAppl aterm_type;
  static void protect(mcrl2::bes::boolean_equation_system<Container> t)   { t.protect(); }
  static void unprotect(mcrl2::bes::boolean_equation_system<Container> t) { t.unprotect(); }
  static void mark(mcrl2::bes::boolean_equation_system<Container> t)      { t.mark(); }
  static ATerm term(mcrl2::bes::boolean_equation_system<Container> t)     { return t.term(); }
  static ATerm* ptr(mcrl2::bes::boolean_equation_system<Container>& t)    { return &t.term(); }
};
}
/// \endcond

#ifndef MCRL2_BES_FIND_H
#include "mcrl2/bes/find.h"
#endif

#endif // MCRL2_BES_BOOLEAN_EQUATION_SYSTEM_H
