// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/specification.h
/// \brief The class specification.

#ifndef MCRL2_LPS_SPECIFICATION_H
#define MCRL2_LPS_SPECIFICATION_H

#include <iostream>
#include <utility>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <set>
#include "mcrl2/utilities/exception.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/process_initializer.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2
{

/// \brief The main namespace for the LPS library.
namespace lps
{

// prototype definitions
template <typename Object> bool is_well_typed(const Object& o);
template <typename LinearProcess, typename InitialProcessExpression> class specification_base;
template <typename LinearProcess, typename InitialProcessExpression> atermpp::aterm_appl specification_to_aterm(const specification_base<LinearProcess, InitialProcessExpression>& spec);
class specification;

// template function overloads
bool is_well_typed(const specification& spec);

/// \brief Test for a specification expression
/// \param x A term
/// \return True if \a x is a specification expression
inline
bool is_specification(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::LinProcSpec;
}

template <typename LinearProcess, typename InitialProcessExpression>
class specification_base
{
  protected:
    /// \brief The data specification of the specification
    data::data_specification m_data;

    /// \brief The action specification of the specification
    process::action_label_list m_action_labels;

    /// \brief The set of global variables
    std::set<data::variable> m_global_variables;

    /// \brief The linear process of the specification
    LinearProcess m_process;

    /// \brief The initial state of the specification
    InitialProcessExpression m_initial_process;

    /// \brief Returns the i-th element of t, converted to aterm_appl
    const atermpp::aterm_appl& get(const atermpp::aterm_appl& t, std::size_t i)
    {
      return atermpp::down_cast<atermpp::aterm_appl>(t[i]);
    }

    /// \brief Initializes the specification with an aterm.
    /// \param t A term
    void construct_from_aterm(const atermpp::aterm_appl& t)
    {
      using atermpp::down_cast;
      assert(core::detail::check_term_LinProcSpec(t));
      m_data             = data::data_specification(get(t, 0));
      m_action_labels    = down_cast<process::action_label_list>(get(t, 1)[0]);
      data::variable_list global_variables = down_cast<data::variable_list>(get(t, 2)[0]);
      m_global_variables = std::set<data::variable>(global_variables.begin(),global_variables.end());
      m_process          = LinearProcess(get(t, 3));
      m_initial_process  = InitialProcessExpression(get(t, 4));
      m_data.declare_data_specification_to_be_type_checked();
    }

  public:
    /// \brief Constructor.
    specification_base()
    { }

    specification_base(const specification_base<LinearProcess, InitialProcessExpression>& other)
    {
      m_data = other.m_data;
      m_action_labels = other.m_action_labels;
      m_global_variables = other.m_global_variables;
      m_process = other.m_process;
      m_initial_process = other.m_initial_process;
    }

    /// \brief Constructor.
    /// \param t A term
    specification_base(const atermpp::aterm_appl &t)
    {
      assert(core::detail::check_rule_LinProcSpec(t));
      construct_from_aterm(t);
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param action_labels A sequence of action labels
    /// \param global_variables A set of global variables
    /// \param lps A linear process
    /// \param initial_process A process initializer
    specification_base(const data::data_specification& data,
                       const process::action_label_list& action_labels,
                       const std::set<data::variable>& global_variables,
                       const LinearProcess& lps,
                       const InitialProcessExpression& initial_process)
       :
      m_data(data),
      m_action_labels(action_labels),
      m_global_variables(global_variables),
      m_process(lps),
      m_initial_process(initial_process)
    {
    }

    /// \brief Reads the specification from file.
    /// \param filename A string
    /// If filename is nonempty, input is read from the file named filename.
    /// If filename is empty, input is read from standard input.
    void load(std::istream& stream, bool binary=true)
    {
      atermpp::aterm t = binary ? atermpp::read_term_from_binary_stream(stream)
                                : atermpp::read_term_from_text_stream(stream);
      t = data::detail::add_index(t);
      if (!t.type_is_appl() || !is_specification(atermpp::down_cast<const atermpp::aterm_appl>(t)))
      {
        throw mcrl2::runtime_error("Input stream does not contain an LPS");
      }
      construct_from_aterm(atermpp::aterm_appl(t));
      // The well typedness check is only done in debug mode, since for large LPSs it takes too much
      // time
    }

    /// \brief Writes the specification to file.
    /// \param filename A string
    /// If filename is nonempty, output is written to the file named filename.
    /// If filename is empty, output is written to stdout.
    /// \param binary
    /// If binary is true the linear process is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    void save(std::ostream& stream, bool binary=true) const
    {
      // The well typedness check is only done in debug mode, since for large
      // LPSs it takes too much time
      atermpp::aterm t = specification_to_aterm(*this);
      t = data::detail::remove_index(t);
      if (binary)
      {
        atermpp::write_term_to_binary_stream(t, stream);
      }
      else
      {
        atermpp::write_term_to_text_stream(t, stream);
      }
    }

    /// \brief Returns the linear process of the specification.
    /// \return The linear process of the specification.
    const LinearProcess& process() const
    {
      return m_process;
    }

    /// \brief Returns a reference to the linear process of the specification.
    /// \return The linear process of the specification.
    LinearProcess& process()
    {
      return m_process;
    }

    /// \brief Returns the data specification.
    /// \return The data specification.
    const data::data_specification& data() const
    {
      return m_data;
    }

    /// \brief Returns a reference to the data specification.
    /// \return The data specification.
    data::data_specification& data()
    {
      return m_data;
    }

    /// \brief Returns a sequence of action labels.
    /// This sequence contains all action labels occurring in the specification (but it can have more).
    /// \return A sequence of action labels.
    const process::action_label_list& action_labels() const
    {
      return m_action_labels;
    }

    /// \brief Returns a sequence of action labels.
    /// This sequence contains all action labels occurring in the specification (but it can have more).
    /// \return A sequence of action labels.
    process::action_label_list& action_labels()
    {
      return m_action_labels;
    }

    /// \brief Returns the declared free variables of the LPS.
    /// \return The declared free variables of the LPS.
    const std::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    /// \brief Returns the declared free variables of the LPS.
    /// \return The declared free variables of the LPS.
    std::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    /// \brief Returns the initial process.
    /// \return The initial process.
    const InitialProcessExpression& initial_process() const
    {
      return m_initial_process;
    }

    /// \brief Returns a reference to the initial process.
    /// \return The initial process.
    InitialProcessExpression& initial_process()
    {
      return m_initial_process;
    }
};

/// \brief Linear process specification.
class specification: public specification_base<linear_process, process_initializer>
{
  protected:
    typedef specification_base<linear_process, process_initializer> super;

  public:
    /// \brief Constructor.
    specification()
    { }

    specification(const specification &other)
      : super(other)
    { }

    /// \brief Constructor.
    /// \param t A term
    specification(const atermpp::aterm_appl &t)
      : super(t)
    { }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param action_labels A sequence of action labels
    /// \param global_variables A set of global variables
    /// \param lps A linear process
    /// \param initial_process A process initializer
    specification(const data::data_specification& data,
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
      assert(is_well_typed(*this));
    }
};

//--- start generated class specification ---//
// prototype declaration
std::string pp(const specification& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const specification& x)
{
  return out << lps::pp(x);
}
//--- end generated class specification ---//

// template function overloads
std::string pp_with_summand_numbers(const specification& x);
std::set<data::sort_expression> find_sort_expressions(const lps::specification& x);
std::set<data::variable> find_all_variables(const lps::specification& x);
std::set<data::variable> find_free_variables(const lps::specification& x);
std::set<data::function_symbol> find_function_symbols(const lps::specification& x);
std::set<core::identifier_string> find_identifiers(const lps::specification& x);

/// \brief Conversion to aterm_appl.
/// \return The specification converted to aterm format.
template <typename LinearProcess, typename InitialProcessExpression>
atermpp::aterm_appl specification_to_aterm(const specification_base<LinearProcess, InitialProcessExpression>& spec)
{
  return atermpp::aterm_appl(core::detail::function_symbol_LinProcSpec(),
           data::detail::data_specification_to_aterm_data_spec(spec.data()),
           atermpp::aterm_appl(core::detail::function_symbol_ActSpec(), spec.action_labels()),
           atermpp::aterm_appl(core::detail::function_symbol_GlobVarSpec(), data::variable_list(spec.global_variables().begin(),spec.global_variables().end())),
           linear_process_to_aterm(spec.process()),
           spec.initial_process()
         );
}

/// \brief Equality operator
inline
bool operator==(const specification& spec1, const specification& spec2)
{
  return specification_to_aterm(spec1) == specification_to_aterm(spec2);
}

/// \brief Inequality operator
inline
bool operator!=(const specification& spec1, const specification& spec2)
{
  return !(spec1 == spec2);
}

/// \brief Adds all sorts that appear in the process of l to the data specification of l.
/// \param spec A linear process specification
inline
void complete_data_specification(specification& spec)
{
  std::set<data::sort_expression> s = lps::find_sort_expressions(spec);
  spec.data().add_context_sorts(s);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SPECIFICATION_H
