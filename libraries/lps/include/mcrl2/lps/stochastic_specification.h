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

namespace mcrl2 {

namespace lps {

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
      : specification_base(other)
    { }

    /// \brief Constructor.
    /// \param t A term
    stochastic_specification(const atermpp::aterm_appl &t)
      : specification_base(t)
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
      : specification_base(data, action_labels, global_variables, lps, initial_process)
    { }

    void load(std::istream& stream, bool binary=true)
    {
      super::load(stream, binary);
      complete_data_specification();
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
