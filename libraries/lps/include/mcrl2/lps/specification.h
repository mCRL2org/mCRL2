// Author(s): Wieger Wesselink
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
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/detail/sequence_algorithm.h"
#include "mcrl2/lps/detail/action_utility.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::read_from_named_file;
using atermpp::aterm_traits;

/// \brief Linear process specification.
///
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
//<Spec>         ::= SpecV1(<DataSpec>, <ActSpec>, <ProcEqnSpec>, <Init>)
class specification: public aterm_appl
{
  protected:
    data_specification   m_data;
    action_label_list    m_action_labels;
    linear_process       m_process;
    process_initializer  m_initial_process;

    /// Initialize the specification with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      m_term = aterm_traits<aterm_appl>::term(t);
      aterm_appl::iterator i = t.begin();
      m_data            = aterm_appl(*i++);
      m_action_labels   = aterm_appl(*i++)(0);
      m_process         = aterm_appl(*i++);
      m_initial_process = aterm_appl(*i);
    }

  public:
    /// Constructor.
    ///
    specification()
      : aterm_appl(mcrl2::core::detail::constructProcSpec())
    { }

    /// Constructor.
    ///
    specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_ProcSpec(m_term));
      init_term(t);
    }

    /// Constructor.
    ///
    specification(
        data_specification  data         ,
        action_label_list   action_labels,
        linear_process      lps          ,
        process_initializer initial_process
       )
      :
        m_data(data),
        m_action_labels(action_labels),
        m_process(lps),
        m_initial_process(initial_process)
    {
      m_term = reinterpret_cast<ATerm>(
        gsMakeSpecV1(
          data,
          gsMakeActSpec(action_labels),
          lps,
          initial_process
        )
      );
    }

    /// Reads the LPS from file. Returns true if the operation succeeded.
    ///
    void load(const std::string& filename)
    {
      aterm t = atermpp::read_from_named_file(filename);
      if (!t || t.type() != AT_APPL || !check_rule_ProcSpec(aterm_appl(t)))
        throw std::runtime_error(std::string("Error in specification::load(): could not read from file " + filename));
      init_term(aterm_appl(t));
      if (!is_well_typed())
        throw std::runtime_error("Error in specification::load(): term is not well typed");
    }

    /// \brief Writes the linear process to file and returns true if the operation succeeded.
    /// \param binary If binary is true the linear process is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    ///
    bool save(const std::string& filename, bool binary = true)
    {
      if (!is_well_typed())
        throw std::runtime_error("Error in specification::save(): term is not well typed");
      if (binary)
      {
        return atermpp::write_to_named_saf_file(m_term, filename);
      }
      else
      {
        return atermpp::write_to_named_text_file(m_term, filename);
      }
    }

    /// Returns the LPS of the specification.
    ///
    linear_process process() const
    {
      return m_process;
    }

    /// Returns the data specification.
    ///
    data_specification data() const
    { return m_data; }

    /// Returns a sequence of action labels. This sequence includes all
    /// action labels occurring in this specification, but it can have more.
    ///
    action_label_list action_labels() const
    { return m_action_labels; }

    /// Returns the initial process.
    ///
    process_initializer initial_process() const
    {
      return m_initial_process;
    }

    /// Returns true if
    /// <ul>
    /// <li>the sorts occurring in the summation variables are declared in the data specification</li>
    /// <li>the sorts occurring in the process parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the free variables are declared in the data specification     </li>
    /// <li>the sorts occurring in the action labels are declared in the data specification      </li>
    ///
    /// <li>the action labels occurring in the process are contained in action_labels()          </li>
    ///
    /// <li>the process is well typed                                                            </li>
    /// <li>the data specification is well typed                                                 </li>
    /// <li>the initial process is well typed                                                    </li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      std::set<sort_expression> declared_sorts = detail::make_set(data().sorts());
      std::set<action_label> declared_labels = detail::make_set(action_labels());

      // check 1)
      for (summand_list::iterator i = process().summands().begin(); i != process().summands().end(); ++i)
      {
        if (!(detail::check_variable_sorts(i->summation_variables(), declared_sorts)))
        {
          std::cerr << "specification::is_well_typed() failed: some of the sorts of the summation variables " << pp(i->summation_variables()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
          return false;
        }
      }

      // check 2)
      if (!(detail::check_variable_sorts(process().process_parameters(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts of the process parameters " << pp(process().process_parameters()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 3)
      if (!(detail::check_variable_sorts(process().free_variables(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts of the free variables " << pp(process().free_variables()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 4)
      if (!(detail::check_action_label_sorts(action_labels(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts occurring in the action labels " << pp(action_labels()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 5)
      for (summand_list::iterator i = process().summands().begin(); i != process().summands().end(); ++i)
      {
        if (!(detail::check_action_labels(i->actions(), declared_labels)))
        {
          std::cerr << "specification::is_well_typed() failed: some of the labels occurring in the actions " << pp(i->actions()) << " are not declared in the action specification " << pp(action_labels()) << std::endl;
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

/// \brief Sets the data specification of spec and returns the result
inline
specification set_data_specification(specification spec, data_specification data)
{
  return specification(data,
                       spec.action_labels(),
                       spec.process(),
                       spec.initial_process()
                      );
}

/// \brief Sets the action labels of spec and returns the result
inline
specification set_action_labels(specification spec, action_label_list action_labels)
{
  return specification(spec.data(),
                       action_labels,
                       spec.process(),
                       spec.initial_process()
                      );
}

/// \brief Sets the linear process of spec and returns the result
inline
specification set_lps(specification spec, linear_process lps)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       lps,
                       spec.initial_process()
                      );
}

/// \brief Sets the initial process of spec and returns the result
inline
specification set_initial_process(specification spec, process_initializer initial_process)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       spec.process(),
                       initial_process
                      );
}

/// \brief Replaces the free variables of the process and the initial state by the union of them.
inline
specification repair_free_variables(const specification& spec)
{
  data_variable_list fv1 = spec.process().free_variables();
  data_variable_list fv2 = spec.initial_process().free_variables();
  std::set<data_variable> freevars(fv1.begin(), fv1.end());
  freevars.insert(fv2.begin(), fv2.end());
  data_variable_list new_free_vars(freevars.begin(), freevars.end());

  linear_process      new_process = set_free_variables(spec.process(), new_free_vars);
  process_initializer new_init(new_free_vars, spec.initial_process().assignments());

  specification result = set_lps(spec, new_process);
  result = set_initial_process(result, new_init);
  assert(result.is_well_typed());
  return result;
}

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::specification;

template<>
struct aterm_traits<specification>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::specification t)   { t.protect(); }
  static void unprotect(lps::specification t) { t.unprotect(); }
  static void mark(lps::specification t)      { t.mark(); }
  static ATerm term(lps::specification t)     { return t.term(); }
  static ATerm* ptr(lps::specification& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_LPS_SPECIFICATION_H
