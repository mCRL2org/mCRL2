// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/propositional_variable.h
/// \brief The classes propositional_variable and propositional_variable_instantiation.

#ifndef MCRL2_PBES_PROPOSITIONAL_VARIABLE_H
#define MCRL2_PBES_PROPOSITIONAL_VARIABLE_H

#include "mcrl2/core/print.h"
#include "mcrl2/data/parse.h"

namespace mcrl2
{

namespace pbes_system
{

//--- start generated class propositional_variable ---//
/// \\brief A propositional variable declaration
class propositional_variable: public atermpp::aterm_appl
{
  public:
    /// \\brief Default constructor.
    propositional_variable()
      : atermpp::aterm_appl(core::detail::default_values::PropVarDecl)
    {}

    /// \\brief Constructor.
    /// \\param term A term
    explicit propositional_variable(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_PropVarDecl(*this));
    }

    /// \\brief Constructor.
    propositional_variable(const core::identifier_string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::function_symbol_PropVarDecl(), name, parameters)
    {}

    /// \\brief Constructor.
    propositional_variable(const std::string& name, const data::variable_list& parameters)
      : atermpp::aterm_appl(core::detail::function_symbol_PropVarDecl(), core::identifier_string(name), parameters)
    {}

    /// Move semantics
    propositional_variable(const propositional_variable&) noexcept = default;
    propositional_variable(propositional_variable&&) noexcept = default;
    propositional_variable& operator=(const propositional_variable&) noexcept = default;
    propositional_variable& operator=(propositional_variable&&) noexcept = default;

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::variable_list& parameters() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[1]);
    }
};

/// \\brief Make_propositional_variable constructs a new term into a given address.
/// \\ \param t The reference into which the new propositional_variable is constructed. 
template <class... ARGUMENTS>
inline void make_propositional_variable(atermpp::aterm_appl& t, const ARGUMENTS&... args)
{
  atermpp::make_term_appl(t, core::detail::function_symbol_PropVarDecl(), args...);
}

/// \\brief list of propositional_variables
typedef atermpp::term_list<propositional_variable> propositional_variable_list;

/// \\brief vector of propositional_variables
typedef std::vector<propositional_variable>    propositional_variable_vector;

/// \\brief Test for a propositional_variable expression
/// \\param x A term
/// \\return True if \\a x is a propositional_variable expression
inline
bool is_propositional_variable(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::PropVarDecl;
}

// prototype declaration
std::string pp(const propositional_variable& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const propositional_variable& x)
{
  return out << pbes_system::pp(x);
}

/// \\brief swap overload
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
