// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/stochastic_specification.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_STOCHASTIC_SPECIFICATION_H
#define MCRL2_LPS_STOCHASTIC_SPECIFICATION_H

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/stochastic_linear_process.h"

namespace mcrl2 {

namespace lps {

class stochastic_specification;
// void complete_data_specification(stochastic_specification& spec);

// template function overloads
std::set<data::sort_expression> find_sort_expressions(const lps::stochastic_specification& x);
std::set<data::variable> find_all_variables(const lps::stochastic_specification& x);
std::set<data::variable> find_free_variables(const lps::stochastic_specification& x);
std::set<data::function_symbol> find_function_symbols(const lps::stochastic_specification& x);
std::set<core::identifier_string> find_identifiers(const lps::stochastic_specification& x);

// template function overloads
bool is_well_typed(const stochastic_specification& spec);

/// \brief Linear process specification.
class stochastic_specification: public specification_base<stochastic_linear_process, stochastic_process_initializer>
{
  protected:
    typedef specification_base<stochastic_linear_process, stochastic_process_initializer> super;

  public:
    /// \brief Constructor.
    stochastic_specification()
    { }

    stochastic_specification(const stochastic_specification &other)
      : super(other)
    { }

    /// \brief Constructor.
    /// \param t A term
    stochastic_specification(const atermpp::aterm_appl &t)
      : super(t)
    {
      // complete_data_specification(*this);
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param action_labels A sequence of action labels
    /// \param global_variables A set of global variables
    /// \param lps A linear process
    /// \param initial_process A process initializer
    stochastic_specification(const data::data_specification& data,
                             const process::action_label_list& action_labels,
                             const std::set<data::variable>& global_variables,
                             const stochastic_linear_process& lps,
                             const stochastic_process_initializer& initial_process)
      : super(data, action_labels, global_variables, lps, initial_process)
    { }

    void save(std::ostream& stream, bool binary=true) const
    {
      assert(is_well_typed(*this));
      super::save(stream, binary);
    }

    void load(std::istream& stream, bool binary=true)
    {
      super::load(stream, binary);
      // complete_data_specification(*this);
      assert(is_well_typed(*this));
    }
};

//--- start generated class stochastic_specification ---//
// prototype declaration
std::string pp(const stochastic_specification& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const stochastic_specification& x)
{
  return out << lps::pp(x);
}
//--- end generated class stochastic_specification ---//

/// \brief Adds all sorts that appear in the process of l to the data specification of l.
/// \param spec A linear process specification
inline
void complete_data_specification(stochastic_specification& spec)
{
  std::set<data::sort_expression> s = lps::find_sort_expressions(spec);
  spec.data().add_context_sorts(s);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_STOCHASTIC_SPECIFICATION_H
