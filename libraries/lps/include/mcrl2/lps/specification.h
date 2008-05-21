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
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/lps/detail/action_utility.h"

namespace mcrl2 {

namespace lps {

using atermpp::aterm;
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
    data::data_specification   m_data;
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
    /// \brief Constructor.
    ///
    specification()
      : aterm_appl(mcrl2::core::detail::constructProcSpec())
    { }

    /// \brief Constructor.
    ///
    specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(core::detail::check_rule_ProcSpec(m_term));
      init_term(t);
    }

    /// \brief Constructor.
    ///
    specification(
        data::data_specification  data         ,
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
        core::detail::gsMakeSpecV1(
          data,
          core::detail::gsMakeActSpec(action_labels),
          lps,
          initial_process
        )
      );
    }

    /// \brief Reads the specification from file.
    /// \param[in] filename
    /// If filename is nonempty, input is read from the file named filename.
    /// If filename is empty, input is read from stdin.
    ///
    void load(const std::string& filename)
    {
      try {
        //open filename for reading as spec_stream
        FILE *spec_stream = NULL;
        if (filename.empty()) {
          spec_stream = stdin;
        } else {
          spec_stream = fopen(filename.c_str(), "rb");
        }
        if (spec_stream == NULL) {
          std::string err_msg(strerror(errno)); 
          if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n') {
            err_msg.replace(err_msg.length()-1, 1, "");
          }
          throw std::runtime_error("could not open input file '" + filename + "' for reading (" + err_msg + ")");
        }
        //read specification from spec_stream
        ATermAppl spec_term = (ATermAppl) ATreadFromFile(spec_stream);
        if (spec_stream != stdin) {
          fclose(spec_stream);
        }
        if (spec_term == NULL) {
          throw std::runtime_error("could not read LPS from " + ((spec_stream == stdin)?"stdin":("'" + filename + "'")));
        }
        if (!core::detail::gsIsSpecV1(spec_term)) {
          throw std::runtime_error(((spec_stream == stdin)?"stdin":("'" + filename + "'")) + " does not contain an LPS");
        }
        //store the term locally
        init_term(aterm_appl(spec_term));
        if (!is_well_typed())
          throw std::runtime_error("specification is not well typed (specification::load())");
      } catch (std::exception &e) {
        throw std::runtime_error(std::string("error: ") + e.what());
      }
    }

    /// \brief Writes the specification to file.
    /// \param[in] filename
    /// If filename is nonempty, output is written to the file named filename.
    /// If filename is empty, output is written to stdout.
    /// \param[in] binary
    /// If binary is true the linear process is saved in compressed binary format.
    /// Otherwise an ascii representation is saved. In general the binary format is
    /// much more compact than the ascii representation.
    /// \return true if the operation succeeded.
    ///
    void save(const std::string& filename, bool binary = true)
    {
      try {
        if (!is_well_typed())
          throw std::runtime_error("specification is not well typed (specification::save())");
        //open filename for writing as spec_stream
        FILE *spec_stream = NULL;
        if (filename.empty()) {
          spec_stream = stdout;
        } else {
          spec_stream = fopen(filename.c_str(), binary?"wb":"w");
        }
        if (spec_stream == NULL) {
          std::string err_msg(strerror(errno)); 
          if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n') {
            err_msg.replace(err_msg.length()-1, 1, "");
          }
          throw std::runtime_error("could not open output file '" + filename + "' for writing (" + err_msg + ")");
        }
        //write specification to spec_stream
        ATbool result;
        if (binary) {
          result = ATwriteToSAFFile(m_term, spec_stream);
        } else {
          result = ATwriteToTextFile(m_term, spec_stream);
        }
        if (spec_stream != stdout) {
          fclose(spec_stream);
        }
        if (result == ATfalse) {
          throw std::runtime_error("could not write LPS to " + ((spec_stream == stdout)?"stdout":("'" + filename + "'")));
        }
      } catch (std::exception &e) {
        throw std::runtime_error(std::string("error: ") + e.what());
      }
    }

    /// \brief Returns the linear process of the specification.
    ///
    linear_process process() const
    {
      return m_process;
    }

    /// \brief Returns the data specification.
    ///
    data::data_specification data() const
    { return m_data; }

    /// \brief Returns a sequence of action labels containing all action
    /// labels occurring in the specification (but it can have more).
    ///
    action_label_list action_labels() const
    { return m_action_labels; }

    /// \brief Returns the initial process.
    ///
    process_initializer initial_process() const
    {
      return m_initial_process;
    }

    /// \brief Indicates whether the specification is well typed.
    ///
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
      std::set<data::sort_expression> declared_sorts = mcrl2::data::detail::make_set(data().sorts());
      std::set<action_label> declared_labels = mcrl2::data::detail::make_set(action_labels());

      // check 1)
      for (summand_list::iterator i = process().summands().begin(); i != process().summands().end(); ++i)
      {
        if (!(mcrl2::data::detail::check_variable_sorts(i->summation_variables(), declared_sorts)))
        {
          std::cerr << "specification::is_well_typed() failed: some of the sorts of the summation variables " << pp(i->summation_variables()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
          return false;
        }
      }

      // check 2)
      if (!(mcrl2::data::detail::check_variable_sorts(process().process_parameters(), declared_sorts)))
      {
        std::cerr << "specification::is_well_typed() failed: some of the sorts of the process parameters " << pp(process().process_parameters()) << " are not declared in the data specification " << pp(data().sorts()) << std::endl;
        return false;
      }

      // check 3)
      if (!(mcrl2::data::detail::check_variable_sorts(process().free_variables(), declared_sorts)))
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
specification set_data_specification(specification spec, data::data_specification data)
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
  data::data_variable_list fv1 = spec.process().free_variables();
  data::data_variable_list fv2 = spec.initial_process().free_variables();
  std::set<data::data_variable> freevars(fv1.begin(), fv1.end());
  freevars.insert(fv2.begin(), fv2.end());
  data::data_variable_list new_free_vars(freevars.begin(), freevars.end());

  linear_process      new_process = set_free_variables(spec.process(), new_free_vars);
  process_initializer new_init(new_free_vars, spec.initial_process().assignments());

  specification result = set_lps(spec, new_process);
  result = set_initial_process(result, new_init);
  assert(result.is_well_typed());
  return result;
}

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::lps::specification;

template<>
struct aterm_traits<specification>
{
  typedef ATermAppl aterm_type;
  static void protect(specification t)   { t.protect(); }
  static void unprotect(specification t) { t.unprotect(); }
  static void mark(specification t)      { t.mark(); }
  static ATerm term(specification t)     { return t.term(); }
  static ATerm* ptr(specification& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_LPS_SPECIFICATION_H
