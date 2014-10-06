// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/process_initializer.h
/// \brief The class process_initializer.

#ifndef MCRL2_LPS_PROCESS_INITIALIZER_H
#define MCRL2_LPS_PROCESS_INITIALIZER_H

#include <algorithm>
#include <cassert>
#include <iterator>
#include <string>
#include <utility>
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/substitutions/assignment_sequence_substitution.h"
#include "mcrl2/lps/stochastic_distribution.h"

namespace mcrl2
{

namespace lps
{

/// \brief A process initializer
class process_initializer: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    process_initializer()
      : atermpp::aterm_appl(core::detail::default_values::LinearProcessInit)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_initializer(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_term_LinearProcessInit(*this));
      const lps::stochastic_distribution& dist = atermpp::down_cast<lps::stochastic_distribution>(atermpp::down_cast<atermpp::aterm_appl>(term)[1]);
      if (dist.is_defined())
      {
        throw mcrl2::runtime_error("cannot handle stochastic processes!");
      }
    }

    /// \brief Constructor.
    process_initializer(const data::assignment_list& assignments)
      : atermpp::aterm_appl(core::detail::function_symbol_LinearProcessInit(), assignments, stochastic_distribution())
    {}

    const data::assignment_list& assignments() const
    {
      return atermpp::down_cast<data::assignment_list>((*this)[0]);
    }

    /// \brief Returns the initial state of the LPS.
    /// \param process_parameters The parameters of the correponding linear process
    /// \return The initial state of the LPS.
    data::data_expression_list state(const data::variable_list& process_parameters) const
    {
      return data::replace_variables(atermpp::container_cast<data::data_expression_list>(process_parameters), data::assignment_sequence_substitution(assignments()));
    }
};

//--- start generated class process_initializer ---//
/// \brief list of process_initializers
typedef atermpp::term_list<process_initializer> process_initializer_list;

/// \brief vector of process_initializers
typedef std::vector<process_initializer>    process_initializer_vector;

/// \brief Test for a process_initializer expression
/// \param x A term
/// \return True if \a x is a process_initializer expression
inline
bool is_process_initializer(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::LinearProcessInit;
}

// prototype declaration
std::string pp(const process_initializer& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const process_initializer& x)
{
  return out << lps::pp(x);
}

/// \brief swap overload
inline void swap(process_initializer& t1, process_initializer& t2)
{
  t1.swap(t2);
}
//--- end generated class process_initializer ---//

// template function overloads
std::set<data::variable> find_free_variables(const lps::process_initializer& x);

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PROCESS_INITIALIZER_H
