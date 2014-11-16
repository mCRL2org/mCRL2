// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/propositional_variable.h
/// \brief The classes propositional_variable and propositional_variable_instantiation.

#ifndef MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
#define MCRL2_PBES_PROPOSITIONAL_VARIABLE_H

#include <cassert>
#include <string>
#include <utility>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2
{

namespace pbes_system
{

//--- start generated class propositional_variable ---//
/// \brief A propositional variable declaration
class propositional_variable: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    propositional_variable()
      : atermpp::aterm_appl(core::detail::default_values::PropVarDecl)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit propositional_variable(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_PropVarDecl(*this));
    }

    /// \brief Constructor.
    propositional_variable(const core::identifier_string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::function_symbol_PropVarDecl(), name, parameters)
    {}

    /// \brief Constructor.
    propositional_variable(const std::string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::function_symbol_PropVarDecl(), core::identifier_string(name), parameters)
    {}

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::variable_list& parameters() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[1]);
    }
//--- start user section propositional_variable ---//
    /// \brief Type of the parameters.
    typedef data::variable parameter_type;

    /// \brief Constructor.
    /// \param s A string
    propositional_variable(const std::string& s)
    {
      std::pair<std::string, data::data_expression_list> p = data::detail::parse_variable(s);
      copy_term(atermpp::aterm_appl(core::detail::function_symbol_PropVarDecl(), core::identifier_string(p.first), atermpp::container_cast< data::variable_list >(p.second)));
    }
//--- end user section propositional_variable ---//
};

/// \brief list of propositional_variables
typedef atermpp::term_list<propositional_variable> propositional_variable_list;

/// \brief vector of propositional_variables
typedef std::vector<propositional_variable>    propositional_variable_vector;

/// \brief Test for a propositional_variable expression
/// \param x A term
/// \return True if \a x is a propositional_variable expression
inline
bool is_propositional_variable(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PropVarDecl;
}

// prototype declaration
std::string pp(const propositional_variable& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const propositional_variable& x)
{
  return out << pbes_system::pp(x);
}

/// \brief swap overload
inline void swap(propositional_variable& t1, propositional_variable& t2)
{
  t1.swap(t2);
}
//--- end generated class propositional_variable ---//

// template function overloads
std::string pp(const propositional_variable_list& x);
std::string pp(const propositional_variable_vector& x);

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
