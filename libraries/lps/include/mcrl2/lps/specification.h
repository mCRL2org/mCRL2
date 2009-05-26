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
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/detail/action_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/sequence_algorithm.h"

namespace mcrl2 {

/// \brief The main namespace for the LPS library.
namespace lps {

class specification;
ATermAppl specification_to_aterm(const specification&);
void complete_data_specification(lps::specification&);

/// \brief Linear process specification.
// sort ...;
//
// cons ...;
//
// map ...;
//
// eqn ...;
//
// proc P(b: Bool, n: Nat) = a(b).P() + sum c: Bool. b -\> e@1.P(b := c);
//
// init P(true, 0);
//
//<Spec>         ::= LinProcSpec(<DataSpec>, <ActSpec>, <ProcEqnSpec>, <Init>)
class specification
{
  protected:
    /// \brief The data specification of the specification
    data::data_specification m_data;

    /// \brief The action specification of the specification
    action_label_list m_action_labels;

    /// \brief The linear process of the specification
    linear_process m_process;

    /// \brief The initial state of the specification
    process_initializer m_initial_process;

    /// \brief Computes the used free variables, and puts them in the free
    /// variable declarations of the process and the initial_process.
    void repair_free_variables()
    {
      std::set<data::variable> free_variables = compute_free_variables(process());
      std::set<data::variable> v = compute_free_variables(initial_process());
      free_variables.insert(v.begin(), v.end());
      data::variable_list freevars = data::convert<data::variable_list>(free_variables);
      m_process.free_variables() = freevars;
      m_initial_process = process_initializer(freevars, m_initial_process.assignments());
    }
    
    /// \brief Initializes the specification with an ATerm.
    /// \param t A term
    void construct_from_aterm(atermpp::aterm_appl t)
    {
      atermpp::aterm_appl::iterator i = t.begin();
      m_data            = atermpp::aterm_appl(*i++);
      m_action_labels   = atermpp::aterm_appl(*i++)(0);
      m_process         = atermpp::aterm_appl(*i++);
      m_initial_process = atermpp::aterm_appl(*i);
      repair_free_variables();
    }

    /// \brief Conversion to ATermAppl.
    /// \return The specification converted to ATerm format.
    operator ATermAppl() const
    {
      return core::detail::gsMakeLinProcSpec(
          data::detail::data_specification_to_aterm_data_spec(m_data),
          core::detail::gsMakeActSpec(m_action_labels),
          linear_process_to_aterm(m_process),
          m_initial_process
      );
    }

  public:
    /// \brief Constructor.
    specification()
    {}

    /// \brief Constructor.
    /// \param t A term
    specification(atermpp::aterm_appl t)
    {
      assert(core::detail::check_rule_LinProcSpec(t));
      construct_from_aterm(t);
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param action_labels A sequence of action labels
    /// \param lps A linear process
    /// \param initial_process A process initializer
    specification(data::data_specification const& data, action_label_list action_labels, linear_process lps, process_initializer initial_process)
      :
        m_data(data),
        m_action_labels(action_labels),
        m_process(lps),
        m_initial_process(initial_process)
    {}

    /// \brief Reads the specification from file.
    /// \param filename A string
    /// If filename is nonempty, input is read from the file named filename.
    /// If filename is empty, input is read from standard input.
    void load(const std::string& filename)
    {
      atermpp::aterm t = core::detail::load_aterm(filename);
      if (!t || t.type() != AT_APPL || !core::detail::gsIsLinProcSpec(atermpp::aterm_appl(t)))
      {
        throw mcrl2::runtime_error(((filename.empty())?"stdin":("'" + filename + "'")) + " does not contain an LPS");
      }
      //store the term locally
      construct_from_aterm(atermpp::aterm_appl(t));
      // The well typedness check is only done in debug mode, since for large
      // LPSs it takes too much time                                        
      assert(is_well_typed());
      //if (!is_well_typed())
      //{
      //  throw mcrl2::runtime_error("specification is not well typed (specification::load())");
      //}
      complete_data_specification(*this);
    }

    /// \brief Writes the specification to file.
    /// \param filename A string
    /// If filename is nonempty, output is written to the file named filename.
    /// If filename is empty, output is written to stdout.
    /// \param binary
    /// If binary is true the linear process is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    void save(const std::string& filename, bool binary = true) const
    {
      // The well typedness check is only done in debug mode, since for large
      // LPSs it takes too much time                                        
      assert(is_well_typed());
      specification tmp(*this);
      tmp.data() = data::remove_all_system_defined(tmp.data());
      core::detail::save_aterm(reinterpret_cast<ATerm>(specification_to_aterm(tmp)), filename, binary);
    }

    /// \brief Returns the linear process of the specification.
    /// \return The linear process of the specification.
    const linear_process& process() const
    {
      return m_process;
    }

    /// \brief Returns a reference to the linear process of the specification.
    /// \return The linear process of the specification.
    linear_process& process()
    {
      return m_process;
    }

    /// \brief Returns the data specification.
    /// \return The data specification.
    const data::data_specification& data() const
    { return m_data; }

    /// \brief Returns a reference to the data specification.
    /// \return The data specification.
    data::data_specification& data()
    { return m_data; }

    /// \brief Returns a sequence of action labels.
    /// This sequence contains all action labels occurring in the specification (but it can have more).
    /// \return A sequence of action labels.
    const action_label_list& action_labels() const
    { return m_action_labels; }

    /// \brief Returns a sequence of action labels.
    /// This sequence contains all action labels occurring in the specification (but it can have more).
    /// \return A sequence of action labels.
    action_label_list& action_labels()
    { return m_action_labels; }

    /// \brief Returns the initial process.
    /// \return The initial process.
    const process_initializer& initial_process() const
    {
      return m_initial_process;
    }

    /// \brief Returns a reference to the initial process.
    /// \return The initial process.
    process_initializer& initial_process()
    {
      return m_initial_process;
    }

    /// \brief Indicates whether the specification is well typed.
    /// \return True if
    /// <ul>
    /// <li>the sorts occurring in the summation variables are declared in the data specification</li>
    /// <li>the sorts occurring in the process parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the free variables are declared in the data specification     </li>
    /// <li>the sorts occurring in the action labels are declared in the data specification      </li>
    /// <li>the action labels occurring in the process are contained in action_labels()          </li>
    /// <li>the process is well typed                                                            </li>
    /// <li>the data specification is well typed                                                 </li>
    /// <li>the initial process is well typed                                                    </li>
    /// </ul>
    bool is_well_typed() const
    {
      std::set<data::sort_expression> declared_sorts = mcrl2::data::detail::make_set(data().sorts());
      std::set<action_label> declared_labels = mcrl2::data::detail::make_set(action_labels());
      summand_list summands = process().summands();

      // check 1)
      for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
      {
        if (!(mcrl2::data::detail::check_variable_sorts(i->summation_variables(), declared_sorts)))
        {
          std::cerr << "specification::is_well_typed() failed: some of the sorts of the summation variables " << data::pp(i->summation_variables()) << " are not declared in the data specification " << data::pp(data().sorts()) << std::endl;
          return false;
        }
      }

      // check 2)
      if (!(mcrl2::data::detail::check_variable_sorts(process().process_parameters(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts of the process parameters " << data::pp(process().process_parameters()) << " are not declared in the data specification " << data::pp(data().sorts()) << std::endl;
        return false;
      }

      // check 3)
      if (!(mcrl2::data::detail::check_variable_sorts(process().free_variables(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts of the free variables " << data::pp(process().free_variables()) << " are not declared in the data specification " << data::pp(data().sorts()) << std::endl;
        return false;
      }

      // check 4)
      if (!(detail::check_action_label_sorts(action_labels(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts occurring in the action labels " << mcrl2::core::pp(action_labels()) << " are not declared in the data specification " << data::pp(data().sorts()) << std::endl;
        return false;
      }

      // check 5)
      for (summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
      {
        if (!(detail::check_action_labels(i->actions(), declared_labels)))
        {
          std::cerr << "specification::is_well_typed() failed: some of the labels occurring in the actions " << mcrl2::core::pp(i->actions()) << " are not declared in the action specification " << mcrl2::core::pp(action_labels()) << std::endl;
          return false;
        }
      }

      // check 6)
      if (!process().is_well_typed())
      {
        return false;
      }

      // check 7)
      if (!data().is_well_typed())
      {
        return false;
      }

      // check 8)
      if (!initial_process().is_well_typed())
      {
        return false;
      }

      return true;
    }
};

/// \brief Replaces the free variables of the process and the initial state by the union of them.
/// \param spec A linear process specification
/// \return The modified specification
inline
specification repair_free_variables(const specification& spec)
{
  data::variable_list fv1 = spec.process().free_variables();
  data::variable_list fv2 = spec.initial_process().free_variables();
  std::set<data::variable> freevars(fv1.begin(), fv1.end());
  freevars.insert(fv2.begin(), fv2.end());
  data::variable_list new_free_vars(freevars.begin(), freevars.end());

  linear_process new_process = spec.process();
  new_process.free_variables() = new_free_vars;
  process_initializer new_init(new_free_vars, spec.initial_process().assignments());

  specification result(spec.data(), spec.action_labels(), new_process, new_init);
  assert(result.is_well_typed());
  return result;
}

/// \brief Traverses the linear process specification, and writes all sort expressions
/// that are encountered to the output range [dest, ...).
template <typename OutIter>
void traverse_sort_expressions(const specification& spec, OutIter dest)
{
  // action labels
  data::traverse_sort_expressions(spec.action_labels(), dest);

  // linear process
  traverse_sort_expressions(spec.process(), dest);

  // initial process
  traverse_sort_expressions(spec.initial_process(), dest);
}

/// \brief Adds all sorts that appear in the process of l to the data specification of l.
/// \param l A linear process specification
inline
void complete_data_specification(lps::specification& spec)
{
  std::set<data::sort_expression> s;
  traverse_sort_expressions(spec, std::inserter(s, s.end()));
  spec.data().make_complete(boost::make_iterator_range(s));
}

/// \brief Conversion to ATermAppl.
/// \return The specification converted to ATerm format.
inline
ATermAppl specification_to_aterm(const specification& spec)
{
  return core::detail::gsMakeLinProcSpec(
      data::detail::data_specification_to_aterm_data_spec(spec.data()),
      core::detail::gsMakeActSpec(spec.action_labels()),
      linear_process_to_aterm(spec.process()),
      spec.initial_process()
  );
}

/// \brief Pretty print function
inline std::string pp(const specification& spec)
{
  return core::pp(specification_to_aterm(spec));
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

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SPECIFICATION_H
