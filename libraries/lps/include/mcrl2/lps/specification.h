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
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/process_initializer.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/sequence_algorithm.h"

namespace mcrl2 {

/// \brief The main namespace for the LPS library.
namespace lps {
                     
template <typename Object, typename OutIter>
void traverse_sort_expressions(const Object& o, OutIter dest);

template <typename Object>
bool is_well_typed(const Object& o);

template <typename Container>
std::set<data::variable> find_free_variables(Container const& container);

class specification;   
atermpp::aterm_appl specification_to_aterm(const specification&, bool compatible = true);
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
//<LinProcSpec>   ::= LinProcSpec(<DataSpec>, <ActSpec>, <GlobVarSpec>,
//                      <LinearProcess>, <LinearProcessInit>)
class specification
{
  protected:
    /// \brief The data specification of the specification
    data::data_specification m_data;

    /// \brief The action specification of the specification
    action_label_list m_action_labels;

    /// \brief The set of global variables
    atermpp::set<data::variable> m_global_variables;

    /// \brief The linear process of the specification
    linear_process m_process;

    /// \brief The initial state of the specification
    process_initializer m_initial_process;

    /// \brief Initializes the specification with an ATerm.
    /// \param t A term
    void construct_from_aterm(atermpp::aterm_appl t)
    {
      atermpp::aterm_appl::iterator i = t.begin();
      m_data             = atermpp::aterm_appl(*i++);
      m_action_labels    = atermpp::aterm_appl(*i++)(0);
      data::variable_list global_variables = atermpp::aterm_appl(*i++)(0);
      m_global_variables = data::convert<atermpp::set<data::variable> >(global_variables);
      m_process          = atermpp::aterm_appl(*i++);
      m_initial_process  = atermpp::aterm_appl(*i);
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
    specification(const data::data_specification& data,
                  const action_label_list& action_labels,
                  const atermpp::set<data::variable>& global_variables,
                  const linear_process& lps,
                  const process_initializer& initial_process)
      :
        m_data(data),
        m_action_labels(action_labels),
        m_global_variables(global_variables),
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
      assert(is_well_typed(*this));
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
      assert(is_well_typed(*this));
      specification tmp(*this);
      tmp.data() = data::remove_all_system_defined(tmp.data());
      core::detail::save_aterm(specification_to_aterm(tmp, false), filename, binary);
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

    
    /// \brief Returns the declared free variables of the LPS.
    /// \return The declared free variables of the LPS.
    const atermpp::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    /// \brief Returns the declared free variables of the LPS.
    /// \return The declared free variables of the LPS.
    atermpp::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }
    
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
};

/// \brief Adds all sorts that appear in the process of l to the data specification of l.
/// \param l A linear process specification
inline
void complete_data_specification(lps::specification& spec)
{
  std::set<data::sort_expression> s;
  traverse_sort_expressions(spec, std::inserter(s, s.end()));
  spec.data().make_complete(s);
}

/// \brief Conversion to ATermAppl.
/// \return The specification converted to ATerm format.
inline
atermpp::aterm_appl specification_to_aterm(const specification& spec, bool compatible)
{
  if (compatible)
  {
    atermpp::aterm_appl specification_term(core::detail::gsMakeLinProcSpec(
      data::detail::data_specification_to_aterm_data_spec(data::data_specification()),
      core::detail::gsMakeActSpec(spec.action_labels()),
      core::detail::gsMakeGlobVarSpec(data::convert<data::variable_list>(spec.global_variables())),
      linear_process_to_aterm(spec.process()),
      spec.initial_process()
    ));

    specification_term = data::detail::apply_compatibility_renamings(spec.data(), specification_term);

    return core::detail::gsMakeLinProcSpec(
        data::detail::data_specification_to_aterm_data_spec(spec.data(), compatible),
        atermpp::aterm_appl(specification_term(1)),
        atermpp::aterm_appl(specification_term(2)),
        atermpp::aterm_appl(specification_term(3)),
        atermpp::aterm_appl(specification_term(4))
    );
  }

  return core::detail::gsMakeLinProcSpec(
      data::detail::data_specification_to_aterm_data_spec(spec.data(), compatible),
      core::detail::gsMakeActSpec(spec.action_labels()),
      core::detail::gsMakeGlobVarSpec(data::convert<data::variable_list>(spec.global_variables())),
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

#ifndef MCRL2_LPS_TRAVERSE_H
#include "mcrl2/lps/traverse.h"
#endif

#ifndef MCRL2_LPS_WELL_TYPED_H
#include "mcrl2/lps/well_typed.h"
#endif

#ifndef MCRL2_LPS_FIND_H
#include "mcrl2/lps/find.h"
#endif

#endif // MCRL2_LPS_SPECIFICATION_H                                                                                       
