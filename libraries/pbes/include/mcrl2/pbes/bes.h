// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/bes.h
/// \brief Boolean expressions.

#ifndef MCRL2_PBES_BES_H
#define MCRL2_PBES_BES_H

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/exception.h"

//--------------------------------------------------------------------//
//                    boolean variable
//--------------------------------------------------------------------//
namespace mcrl2 {
  
/// \brief The main namespace for boolean equation systems.
namespace bes {

  typedef pbes_system::fixpoint_symbol fixpoint_symbol;

  /// \brief boolean variable
  class boolean_variable: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      boolean_variable()
        : atermpp::aterm_appl(core::detail::constructBooleanVariable())
      {}

      /// \brief Constructor.
      /// \param term A term
      boolean_variable(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Constructor.
      /// \param name A string
      explicit boolean_variable(core::identifier_string name)
        : atermpp::aterm_appl(core::detail::gsMakeBooleanVariable(name))
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Constructor.
      /// \param name A string
      boolean_variable(const std::string& name)
        : atermpp::aterm_appl(core::detail::gsMakeBooleanVariable(core::detail::gsString2ATermAppl(name.c_str())))
      {
        assert(core::detail::check_rule_BooleanVariable(m_term));
      }

      /// \brief Returns the name of the boolean variable.
      /// \return The name of the boolean variable.
      core::identifier_string name() const
      {
        return atermpp::arg1(*this);
      }

      /// \brief Applies a substitution to this boolean variable and returns the result.
      /// \param f A substitution function.
      /// The Substitution function must supply the method aterm operator()(aterm).
      /// \return The result of applying the substitution.
      template <typename Substitution>
      boolean_variable substitute(Substitution f) const
      {
        return boolean_variable(f(*this));
      }
  };

  /// \brief Singly linked list of boolean variables
  ///
  typedef atermpp::term_list<boolean_variable> boolean_variable_list;

  /// \brief Pretty print function
  inline
  std::string pp(boolean_variable v)
  {
    return core::pp(v.name());
  }

} // namespace bes
} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::bes::boolean_variable)
/// \endcond

//--------------------------------------------------------------------//
//                    boolean expression
//--------------------------------------------------------------------//
namespace mcrl2 {
namespace bes {

  /// \brief boolean expression
  ///
  class boolean_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      boolean_expression()
        : atermpp::aterm_appl(core::detail::constructBooleanExpression())
      {}

      /// \brief Constructor.
      /// \param term A term
      boolean_expression(ATermAppl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanExpression(m_term));
      }

      /// \brief Constructor.
      /// \param term A term
      boolean_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_BooleanExpression(m_term));
      }

      /// \brief Applies a substitution to this boolean expression and returns the result.
      /// \param f A substitution function.
      /// The Substitution function must supply the method aterm operator()(aterm).
      /// \return The result of applying the substitution.
      template <typename Substitution>
      boolean_expression substitute(Substitution f) const
      {
        return boolean_expression(f(*this));
      }
  };

  /// \brief singly linked list of boolean expressions
  typedef atermpp::term_list<boolean_expression> boolean_expression_list;

} // namespace bes
} // namespace mcrl2

namespace mcrl2 {
namespace core {

  template <>
  struct term_traits<bes::boolean_expression>
  {
    /// The term type
    typedef bes::boolean_expression term_type;
    typedef bes::boolean_variable variable_type;
    typedef core::identifier_string string_type;

    static inline
    term_type true_() { return core::detail::gsMakeBooleanTrue(); }

    static inline
    term_type false_() { return core::detail::gsMakeBooleanFalse(); }

    static inline
    term_type not_(term_type p) { return core::detail::gsMakeBooleanNot(p); }

    static inline
    term_type and_(term_type p, term_type q) { return core::detail::gsMakeBooleanAnd(p, q); }

    static inline
    term_type or_(term_type p, term_type q) { return core::detail::gsMakeBooleanOr(p, q); }

    static inline
    term_type imp(term_type p, term_type q) { return core::detail::gsMakeBooleanImp(p, q); }

    static inline
    bool is_true(term_type t) { return core::detail::gsIsBooleanTrue(t); }

    static inline
    bool is_false(term_type t) { return core::detail::gsIsBooleanFalse(t); }

    static inline
    bool is_not(term_type t) { return core::detail::gsIsBooleanNot(t); }

    static inline
    bool is_and(term_type t) { return core::detail::gsIsBooleanAnd(t); }

    static inline
    bool is_or(term_type t) { return core::detail::gsIsBooleanOr(t); }

    static inline
    bool is_imp(term_type t) { return core::detail::gsIsBooleanImp(t); }

    static inline
    bool is_variable(term_type t) { return core::detail::gsIsBooleanVariable(t); }

    static inline
    term_type arg(term_type t)
    {
      assert(is_not(t));
      return atermpp::arg1(t);
    }

    static inline
    term_type left(term_type t)
    {
      assert(is_and(t) || is_or(t) || is_imp(t));
      return atermpp::arg1(t);
    }

    static inline
    term_type right(term_type t)
    {
      assert(is_and(t) || is_or(t) || is_imp(t));
      return atermpp::arg2(t);
    }

    static inline
    string_type name(term_type t)
    {
      assert(is_variable(t));
      return atermpp::arg1(t);
    }

    static inline
    bool is_constant(term_type t)
    {
      return atermpp::find_if(t, is_variable) != atermpp::aterm_appl();
    }

    static inline
    term_type variable2term(variable_type v) { return v; }

    static inline
    variable_type term2variable(term_type v) { return v; }
  };

} // namespace core
} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::bes::boolean_expression)
/// \endcond


namespace mcrl2 {
namespace bes {

  /// pretty print function (N.B. the implementation is not very efficient)
  inline
  std::string pp(boolean_expression e, bool add_parens = false)
  {
    typedef core::term_traits<boolean_expression> tr;

    if (tr::is_variable(e))
    {
      return pp(tr::term2variable(e));
    }
    else if (tr::is_true(e))
    {
      return "true";
    }
    else if (tr::is_false(e))
    {
      return "false";
    }
    else if (tr::is_not(e))
    {
      return std::string("!") + (add_parens ? "(" : "") + pp(tr::arg(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_and(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " && " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_or(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " || " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    else if (tr::is_imp(e))
    {
      return (add_parens ? "(" : "") + pp(tr::left(e), true) + " => " + pp(tr::right(e), true) + (add_parens ? ")" : "");
    }
    throw mcrl2::runtime_error("error in mcrl2::bes::pp: encountered unknown boolean expression " + e.to_string());
    return "";
  }

} // namespace bes
} // namespace mcrl2

//--------------------------------------------------------------------//
//                    boolean equation
//--------------------------------------------------------------------//

namespace mcrl2 {
namespace bes {

/// \brief boolean equation.
///
// boolean equation
// <BooleanEquation>   ::= BooleanEquation(<FixPoint>, <BooleanVariable>, <BooleanExpression>)
class boolean_equation: public atermpp::aterm_appl
{
  protected:
    fixpoint_symbol m_symbol;
    boolean_variable   m_variable;
    boolean_expression m_formula;  // the right hand side

  public:
    /// \brief Constructor.
    boolean_equation()
      : atermpp::aterm_appl(core::detail::constructBooleanEquation())
    {}

    /// \brief Constructor.
    boolean_equation(atermpp::aterm_appl t)
      : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_BooleanEquation(m_term));
      iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      atermpp::aterm_appl var(*i++);
      m_variable = boolean_variable(var);
      m_formula  = boolean_expression(*i);
    }

    /// \brief Constructor.
    boolean_equation(fixpoint_symbol symbol, boolean_variable variable, boolean_expression expr)
      : atermpp::aterm_appl(core::detail::gsMakeBooleanEquation(symbol, variable, expr)),
        m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    /// \brief Returns the fixpoint symbol of the equation.
    fixpoint_symbol symbol() const
    {
      return m_symbol;
    }

    /// \brief Returns the boolean_equation_system variable of the equation.
    boolean_variable variable() const
    {
      return m_variable;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    boolean_expression formula() const
    {
      return m_formula;
    }
};

/// \brief singly linked list of data expressions
///
typedef atermpp::term_list<boolean_equation> boolean_equation_list;

  /// \brief Pretty print function
  inline
  std::string pp(boolean_equation e)
  {
    return core::pp(e.symbol()) + " " + pp(e.variable()) + " = " + pp(e.formula());
  }

} // namespace bes
} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::bes::boolean_equation)
/// \endcond

//--------------------------------------------------------------------//
//                    boolean equation system
//--------------------------------------------------------------------//

namespace mcrl2 {
namespace bes {

  // <BES>          ::= BES(<BooleanEquation>*, <BooleanExpression>)
  template <typename Container = atermpp::vector<boolean_equation> >
  class boolean_equation_system
  {
    friend struct atermpp::aterm_traits<boolean_equation_system>;

    protected:
      Container m_equations;
      boolean_expression m_initial_state;

      ATerm term() const
      {
        return reinterpret_cast<ATerm>(ATermAppl(*this));
      }

      /// \brief Initialize the boolean_equation_system with an atermpp::aterm_appl.
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
      const Container& equations() const
      {
        return m_equations;
      }

      /// \brief Returns the equations.
      Container& equations()
      {
        return m_equations;
      }

      /// \brief Returns the initial state.
      const boolean_expression& initial_state() const
      {
        return m_initial_state;
      }

      /// \brief Returns the initial state.
      boolean_expression& initial_state()
      {
        return m_initial_state;
      }

      /// \brief Returns true.
      bool is_well_typed() const
      {
        return true;
      }

      /// \brief Reads the boolean equation system from file.
      /// \param[in] filename
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
      void save(const std::string& filename, bool binary = true) // const
      {
        if (!is_well_typed())
        {
          throw mcrl2::runtime_error("boolean equation system is not well typed (boolean_equation_system::save())");
        }
        atermpp::aterm t = ATermAppl(*this);
        core::detail::save_aterm(t, filename, binary);
      }

      /// \brief Conversion to ATermAppl.
      operator ATermAppl() const
      {
        boolean_equation_list equations(m_equations.begin(), m_equations.end());
        return core::detail::gsMakeBES(equations, m_initial_state);
      }

      /// \brief Returns the set of binding variables of the boolean_equation_system, i.e. the
      /// variables that occur on the left hand side of an equation.
      atermpp::set<boolean_variable> binding_variables() const
      {
        atermpp::set<boolean_variable> result;
        for (typename Container::const_iterator i = equations().begin(); i != equations().end(); ++i)
        {
          result.insert(i->variable());
        }
        return result;
      }

      /// \brief Returns the set of occurring variables of the boolean_equation_system, i.e.
      /// the variables that occur in the right hand side of an equation or in the
      /// initial state.
      atermpp::set<boolean_variable> occurring_variables() const
      {
        atermpp::set<boolean_variable> result;
        for (typename Container::const_iterator i = m_equations.begin(); i != m_equations.end(); ++i)
        {
          atermpp::find_all_if(i->formula(), &core::term_traits<boolean_expression>::is_variable, std::inserter(result, result.end()));
        }
        atermpp::find_all_if(m_initial_state, &core::term_traits<boolean_expression>::is_variable, std::inserter(result, result.end()));
        return result;
      }

      /// \brief Returns true if all occurring variables are binding variables.
      bool is_closed() const
      {
        atermpp::set<boolean_variable> bnd = binding_variables();
        atermpp::set<boolean_variable> occ = occurring_variables();
        return std::includes(bnd.begin(), bnd.end(), occ.begin(), occ.end()) && is_declared_in(bnd.begin(), bnd.end(), initial_state());
      }

      /// \brief Applies a substitution to this boolean equation system and returns the result.
      /// \param f A substitution function.
      /// The Substitution function must supply the method aterm operator()(aterm).
      /// \return The result of applying the substitution.
      template <typename Substitution>
      void substitute(Substitution f)
      {
        std::transform(equations().begin(), equations().end(), equations().begin(), f);
      }

      /// Protects the term from being freed during garbage collection.
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
  template <typename Container>
  std::string pp(const boolean_equation_system<Container>& p)
  {
    std::ostringstream out;
    BOOST_FOREACH(const boolean_equation& eq, p.equations())
    {
      out << pp(eq) << std::endl;
    }
    out << "\ninit " << pp(p.initial_state()) << std::endl;
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

#endif // MCRL2_PBES_BES_H
