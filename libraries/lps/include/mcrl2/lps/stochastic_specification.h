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

// template function overloads
std::set<data::sort_expression> find_sort_expressions(const lps::stochastic_specification& x);
std::set<data::variable> find_all_variables(const lps::stochastic_specification& x);
std::set<data::variable> find_free_variables(const lps::stochastic_specification& x);
std::set<data::function_symbol> find_function_symbols(const lps::stochastic_specification& x);
std::set<core::identifier_string> find_identifiers(const lps::stochastic_specification& x);

/// \brief Linear process specification.
class stochastic_specification: public specification_base<linear_process, process_initializer>
{
  protected:
    typedef specification_base<linear_process, process_initializer> super;

    /// \brief Adds all sorts that appear in the process to the data specification.
    inline
    void complete_data_specification()
    {
      std::set<data::sort_expression> s = lps::find_sort_expressions(*this);
      data().add_context_sorts(s);
    }

  public:
    /// \brief Constructor.
    stochastic_specification()
    { }

    stochastic_specification(const specification &other)
      : super(other)
    { }

    /// \brief Constructor.
    /// \param t A term
    stochastic_specification(const atermpp::aterm_appl &t)
      : super(t)
    {
      complete_data_specification();
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
                             const linear_process& lps,
                             const process_initializer& initial_process)
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
      complete_data_specification();
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

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_STOCHASTIC_SPECIFICATION_H
