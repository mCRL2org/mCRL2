// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/variable.h
/// \brief The class variable.

#ifndef MCRL2_DATA_VARIABLE_H
#define MCRL2_DATA_VARIABLE_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/application.h"

namespace mcrl2
{

namespace data
{

//--- start generated class variable ---//
/// \brief A data variable
class variable: public data_expression
{
  public:
    /// \brief Default constructor.
    variable()
      : data_expression(core::detail::constructDataVarId())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit variable(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_DataVarId(*this));
    }

    /// \brief Constructor.
    variable(const core::identifier_string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeDataVarId(name, sort))
    {}

    /// \brief Constructor.
    variable(const std::string& name, const sort_expression& sort)
      : data_expression(core::detail::gsMakeDataVarId(core::identifier_string(name), sort))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const sort_expression& sort() const
    {
      return atermpp::aterm_cast<const sort_expression>((*this)[1]);
    }
};

/// \brief list of variables
typedef atermpp::term_list<variable> variable_list;

/// \brief vector of variables
typedef std::vector<variable>    variable_vector;

// prototype declaration
std::string pp(const variable& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const variable& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(variable& t1, variable& t2)
{
  t1.swap(t2);
}
//--- end generated class variable ---//

// template function overloads
std::string pp(const variable_list& x);
std::string pp(const variable_vector& x);
std::string pp(const std::set<variable>& x);
std::string pp(const std::set<variable>& x);
std::set<data::variable> find_all_variables(const data::variable& x);
std::set<data::variable> find_all_variables(const data::variable_list& x);
std::set<core::identifier_string> find_identifiers(const data::variable_list& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_VARIABLE_H

