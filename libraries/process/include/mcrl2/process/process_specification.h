// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_specification.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_SPECIFICATION_H
#define MCRL2_PROCESS_PROCESS_SPECIFICATION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/lps/action_label.h"
#include "mcrl2/process/process_equation.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/detail/linear_process_expression_visitor.h"

namespace mcrl2 {

/// \brief The main namespace for the Process library.
namespace process {

  template <typename Object, typename OutIter>
  void traverse_sort_expressions(const Object& o, OutIter dest);

  class process_specification;
  ATermAppl process_specification_to_aterm(const process_specification& spec);
  void complete_data_specification(process_specification&);

  /// \brief Process specification consisting of a data specification, action labels, a sequence of process equations and a process initialization.
  //<ProcSpec>     ::= ProcSpec(<DataSpec>, <ActSpec>, <GlobVarSpec>, <ProcEqnSpec>, <ProcInit>)
  class process_specification
  {
    protected:
      /// \brief The data specification of the specification
      data::data_specification m_data;
      
      /// \brief The action specification of the specification
      lps::action_label_list m_action_labels;

      /// \brief The equations of the specification
      atermpp::vector<process_equation> m_equations;
      
      /// \brief The set of global variables
      atermpp::set<data::variable> m_global_variables;

      /// \brief The initial state of the specification
      process_expression m_initial_process;
      
      /// \brief Initializes the specification with an ATerm.
      /// \param t A term
      void construct_from_aterm(atermpp::aterm_appl t)
      {
        atermpp::aterm_appl::iterator i = t.begin();
        m_data            = atermpp::aterm_appl(*i++);
        m_action_labels   = atermpp::aterm_appl(*i++)(0);
        data::variable_list global_variables = atermpp::aterm_appl(*i++)(0);
        m_global_variables = data::convert<atermpp::set<data::variable> >(global_variables);
        process_equation_list l = atermpp::aterm_appl(*i++)(0);
        m_initial_process = atermpp::aterm_appl(*i);
        m_equations       = atermpp::vector<process_equation>(l.begin(), l.end());
        complete_data_specification(*this);
      }

    public:
      /// \brief Constructor.
      process_specification()
      {}

      /// \brief Constructor.
      /// \param term A term
      /// \param t A term
      process_specification(atermpp::aterm_appl t)
      {
        assert(core::detail::check_term_ProcSpec(t));
        construct_from_aterm(t);
      }

      process_specification(data::data_specification data, lps::action_label_list action_labels, process_equation_list equations, process_expression init)
        : m_data(data),
          m_action_labels(action_labels),
          m_equations(equations.begin(), equations.end()),
          m_initial_process(init)
      {}

      /// \brief Returns the data specification
      /// \return The data specification
      const data::data_specification& data() const
      {
        return m_data;
      }

      /// \brief Returns the data specification
      /// \return The data specification
      data::data_specification& data()
      {
        using namespace atermpp;
        return m_data;
      }

      /// \brief Returns the action label specification
      /// \return The action label specification
      const lps::action_label_list& action_labels() const
      {
        return m_action_labels;
      }

      /// \brief Returns the action label specification
      /// \return The action label specification
      lps::action_label_list& action_labels()
      {
        return m_action_labels;
      }

      /// \brief Returns the declared free variables of the process specification.
      /// \return The declared free variables of the process specification.
      const atermpp::set<data::variable>& global_variables() const
      {
        return m_global_variables;
      }

      /// \brief Returns the declared free variables of the process specification.
      /// \return The declared free variables of the process specification.
      atermpp::set<data::variable>& global_variables()
      {
        return m_global_variables;
      }
    
      /// \brief Returns the equations of the process specification
      /// \return The equations of the process specification
      const atermpp::vector<process_equation>& equations() const
      {
        return m_equations;
      }

      /// \brief Returns the equations of the process specification
      /// \return The equations of the process specification
      atermpp::vector<process_equation>& equations()
      {
        return m_equations;
      }

      /// \brief Returns the initialization of the process specification
      /// \return The initialization of the process specification
      const process_expression& init() const
      {
        return m_initial_process;
      }

      /// \brief Returns the initialization of the process specification
      /// \return The initialization of the process specification
      process_expression& init()
      {
        return m_initial_process;
      }
  };

  /// \brief Adds all sorts that appear in the process specification spec
  ///  to the data specification of spec.
  /// \param spec A process specification
  inline
  void complete_data_specification(process_specification& spec)
  {
    std::set<data::sort_expression> s;
    traverse_sort_expressions(spec, std::inserter(s, s.end()));
    for (std::set<data::sort_expression>::iterator i = s.begin(); i != s.end(); ++i)
    {
      if (i->is_system_defined())
      {
        spec.data().import_system_defined_sort(*i);
      }
    }
  }

  /// \brief Conversion to ATermAppl.
  /// \return The specification converted to ATerm format.
  /// \param spec A process specification
  inline
  ATermAppl process_specification_to_aterm(const process_specification& spec)
  {
    return core::detail::gsMakeProcSpec(
        data::detail::data_specification_to_aterm_data_spec(spec.data()),
        core::detail::gsMakeActSpec(spec.action_labels()),
        core::detail::gsMakeGlobVarSpec(data::convert<data::variable_list>(spec.global_variables())),
        core::detail::gsMakeProcEqnSpec(process_equation_list(spec.equations().begin(), spec.equations().end())),
        spec.init()
    );
  }
  
  /// \brief Pretty print function
  /// \param spec A process specification
  /// \return A pretty print representation of the specification
  inline std::string pp(const process_specification& spec)
  {
    return core::pp(process_specification_to_aterm(spec));
  }
  
  /// \brief Equality operator
  inline
  bool operator==(const process_specification& spec1, const process_specification& spec2)
  {
    return process_specification_to_aterm(spec1) == process_specification_to_aterm(spec2);
  }
  
  /// \brief Inequality operator
  inline
  bool operator!=(const process_specification& spec1, const process_specification& spec2)
  {
    return !(spec1 == spec2);
  }

  /// \brief Returns true if the process specification is linear.
  /// \param p A process specification
  /// \return True if the process specification is linear.
  inline
  bool is_linear(const process_specification& p)
  {
    if (p.equations().size() != 1)
    {
      return false;
    }
    detail::linear_process_expression_visitor visitor;
    if (!visitor.is_linear(*p.equations().begin()))
    {
      return false;
    }
    if (!is_process_instance(p.init()))
    {
      return false;
    }
    return true;
  }
  
} // namespace process

} // namespace mcrl2

#ifndef MCRL2_PROCESS_TRAVERSE_H
#include "mcrl2/process/traverse.h"
#endif

#endif // MCRL2_PROCESS_PROCESS_SPECIFICATION_H


