// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/process.h
/// \brief Process classes and process expressions.

#ifndef MCRL2_LPS_PROCESS_H
#define MCRL2_LPS_PROCESS_H

#include <functional>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/detail/algorithms.h"

namespace mcrl2 {

namespace lps {

  class process_specification;
  ATermAppl process_specification_to_aterm(const process_specification& spec);
  void complete_data_specification(process_specification&);

//<ProcExpr>     ::= <ParamId>                                             [- tc]
//                 | IdAssignment(<String>, <IdInit>*)                     [- tc]
//                 | <Action>                                              [+ tc]
//                 | Process(<ProcVarId>, <DataExpr>*)                     [+ tc]
//                 | ProcessAssignment(<ProcVarId>, <DataVarIdInit>*)      [+ tc]
//                 | Delta
//                 | Tau
//                 | Sum(<DataVarId>+, <ProcExpr>)
//                 | Block(<String>*, <ProcExpr>)
//                 | Hide(<String>*, <ProcExpr>)
//                 | Rename(<RenameExpr>*, <ProcExpr>)
//                 | Comm(<CommExpr>*, <ProcExpr>)
//                 | Allow(<MultActName>*, <ProcExpr>)
//                 | Sync(<ProcExpr>, <ProcExpr>)
//                 | AtTime(<ProcExpr>, <DataExpr>)
//                 | Seq(<ProcExpr>, <ProcExpr>)
//                 | IfThen(<DataExpr>, <ProcExpr>)
//                 | IfThenElse(<DataExpr>, <ProcExpr>, <ProcExpr>)
//                 | BInit(<ProcExpr>, <ProcExpr>)
//                 | Merge(<ProcExpr>, <ProcExpr>)
//                 | LMerge(<ProcExpr>, <ProcExpr>)
//                 | Choice(<ProcExpr>, <ProcExpr>)
  /// \brief Process expression
  class process_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      process_expression()
        : atermpp::aterm_appl(core::detail::constructProcExpr())
      {}

      /// \brief Constructor.
      /// \param term A term
      process_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_rule_ProcExpr(m_term));
      }
  };

  /// \brief process identifier
  //<ProcVarId>    ::= ProcVarId(<String>, <SortExpr>*)
  class process_identifier: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_identifier(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcVarId(m_term));
      }

      process_identifier(core::identifier_string name, data::sort_expression_list sorts)
        : atermpp::aterm_appl(core::detail::gsMakeProcVarId(name, atermpp::term_list< data::sort_expression >(sorts.begin(), sorts.end())))
      {}

      core::identifier_string name() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::sort_expression_list sorts() const
      {
        using namespace atermpp;
        return data::sort_expression_list(
          atermpp::term_list_iterator< data::sort_expression >(list_arg2(*this)),
          atermpp::term_list_iterator< data::sort_expression >());
      }
  };

  /// \brief Traverses the process identifier, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  template <typename OutIter>
  void traverse_sort_expressions(const process_identifier& pi, OutIter dest)
  {
    data::traverse_sort_expressions(pi.sorts(), dest);
  }

  /// \brief Process equation
  // <ProcEqn> ::= ProcEqn(<DataVarId>*, <ProcVarId>, <DataVarId>*, <ProcExpr>)
  class process_equation: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      process_equation()
        : atermpp::aterm_appl(core::detail::constructProcEqn())
      {}

      /// \brief Constructor.
      /// \param term A term
      process_equation(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcEqn(m_term));
      }

      process_equation(data::variable_list variables1, process_identifier name, data::variable_list variables2, process_expression expression)
        : atermpp::aterm_appl(core::detail::gsMakeProcEqn(
                                atermpp::term_list< data::variable >(variables1.begin(), variables1.end()),
                                name,
                                atermpp::term_list< data::variable >(variables2.begin(), variables2.end()),
                                expression))
      {}

      data::variable_list variables1() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator< data::variable >(list_arg1(*this)),
          atermpp::term_list_iterator< data::variable >());
      }

      process_identifier name() const
      {
        using namespace atermpp;
        return arg2(*this);
      }

      data::variable_list variables2() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator< data::variable >(list_arg3(*this)),
          atermpp::term_list_iterator< data::variable >());
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg4(*this);
      }
  };

  /// \brief Traverses the process equation, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  template <typename OutIter>
  void traverse_sort_expressions(const process_equation& eq, OutIter dest)
  {
    data::traverse_sort_expressions(eq.variables1(), dest);
    data::traverse_sort_expressions(eq.variables2(), dest);
  }

  /// \brief Read-only singly linked list of process equations
  typedef atermpp::term_list<process_equation> process_equation_list;

  /// \brief Process initialization
  //<ProcInit>     ::= ProcessInit(<DataVarId>*, <ProcExpr>)
  class process_initialization: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_initialization()
        : atermpp::aterm_appl(mcrl2::core::detail::constructProcessInit())
      {}

      /// \brief Constructor.
      /// \param term A term
      process_initialization(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcessInit(m_term));
      }

      process_initialization(data::variable_list variables, process_expression expression)
        : atermpp::aterm_appl(core::detail::gsMakeProcessInit(
              atermpp::term_list< data::variable >(variables.begin(), variables.end()), expression))
      {}

      data::variable_list variables() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator< data::variable >(list_arg1(*this)),
          atermpp::term_list_iterator< data::variable >());
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Traverses the process equation, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  template <typename OutIter>
  void traverse_sort_expressions(const process_initialization& init, OutIter dest)
  {
    data::traverse_sort_expressions(init.variables(), dest);
  }

  /// \brief Process specification
  //<ProcSpec>     ::= ProcSpec(<DataSpec>, <ActSpec>, <ProcEqnSpec>, <ProcInit>)
  class process_specification
  {
    protected:
      /// \brief The data specification of the specification
      data::data_specification m_data;
      
      /// \brief The action specification of the specification
      action_label_list m_action_labels;
      
      /// \brief The equations of the specification
      atermpp::vector<process_equation> m_equations;
      
      /// \brief The initial state of the specification
      process_initialization m_initial_process;
      
      /// \brief Initializes the specification with an ATerm.
      /// \param t A term
      void construct_from_aterm(atermpp::aterm_appl t)
      {
        atermpp::aterm_appl::iterator i = t.begin();
        m_data            = atermpp::aterm_appl(*i++);
        m_action_labels   = atermpp::aterm_appl(*i++)(0);
        process_equation_list l = atermpp::aterm_appl(*i++)(0);
        m_initial_process = atermpp::aterm_appl(*i);
        m_equations       = atermpp::vector<process_equation>(l.begin(), l.end());
      }

    public:
      /// \brief Constructor.
      process_specification()
      {}

      /// \brief Constructor.
      /// \param term A term
      process_specification(atermpp::aterm_appl t)
      {
        assert(core::detail::check_term_ProcSpec(t));
        construct_from_aterm(t);
      }

      process_specification(data::data_specification data, action_label_list action_labels, process_equation_list equations, process_initialization init)
        : m_data(data),
          m_action_labels(action_labels),
          m_equations(equations.begin(), equations.end()),
          m_initial_process(init)
      {}

      const data::data_specification& data() const
      {
        return m_data;
      }

      data::data_specification& data()
      {
        using namespace atermpp;
        return m_data;
      }

      const action_label_list& action_labels() const
      {
        return m_action_labels;
      }

      action_label_list& action_labels()
      {
        return m_action_labels;
      }

      const atermpp::vector<process_equation>& equations() const
      {
        return m_equations;
      }

      atermpp::vector<process_equation>& equations()
      {
        return m_equations;
      }

      const process_initialization& init() const
      {
        return m_initial_process;
      }

      process_initialization& init()
      {
        return m_initial_process;
      }
  };

  /// \brief Traverses the process specification, and writes all sort expressions
  /// that are encountered to the output range [dest, ...).
  template <typename OutIter>
  void traverse_sort_expressions(const process_specification& spec, OutIter dest)
  {
    data::traverse_sort_expressions(spec.action_labels(), dest);
    data::traverse_sort_expressions(spec.equations(), dest);
    traverse_sort_expressions(spec.init(), dest);
  }

  inline
  process_specification parse_process_specification(const std::string& spec)
  {
    std::stringstream spec_stream;
    spec_stream << spec;
    ATermAppl result = detail::parse_specification(spec_stream);
    result           = detail::type_check_specification(result);
    result           = detail::alpha_reduce(result);
    result           = data::detail::internal_format_conversion(result);
    return atermpp::aterm_appl(result);
  }

  /// \brief Renaming expression
  //<RenameExpr>   ::= RenameExpr(<String>, <String>)
  class rename_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      rename_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_RenameExpr(m_term));
      }

      rename_expression(core::identifier_string source, core::identifier_string target)
        : atermpp::aterm_appl(core::detail::gsMakeRenameExpr(source, target))
      {}

      core::identifier_string source() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      core::identifier_string target() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Read-only singly linked list of rename expressions
  typedef atermpp::term_list<rename_expression> rename_expression_list;

  //<MultActName>  ::= MultActName(<String>+)
  class multi_action_name: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      multi_action_name(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_MultActName(m_term));
      }

      multi_action_name(core::identifier_string_list names)
        : atermpp::aterm_appl(core::detail::gsMakeMultActName(names))
      {}

      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(arg1(*this));
      }
  };

  /// \brief Read-only singly linked list of multi_action_name expressions
  typedef atermpp::term_list<multi_action_name> multi_action_name_list;

  //<CommExpr>     ::= CommExpr(<MultActName>, <StringOrNil>)
  class communication_expression: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      communication_expression(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_CommExpr(m_term));
      }

      communication_expression(multi_action_name action_name, core::identifier_string name)
        : atermpp::aterm_appl(core::detail::gsMakeCommExpr(action_name, name))
      {}

      multi_action_name action_name() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      core::identifier_string name() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Read-only singly linked list of communication expressions
  typedef atermpp::term_list<rename_expression> communication_expression_list;

  /// \brief Process
  // Process(<ProcVarId>, <DataExpr>*)
  class process: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Process(m_term));
      }

      process(const process_identifier pi, const data::data_expression_list& v)
        : process_expression(core::detail::gsMakeProcess(pi,
                        atermpp::term_list< data::data_expression >(v.begin(), v.end())))
      {}

      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::data_expression_list expressions() const
      {
        using namespace atermpp;
        return data::data_expression_list(
          atermpp::term_list_iterator< data::data_expression >(list_arg2(*this)),
          atermpp::term_list_iterator< data::data_expression >());
      }
  };

  /// \brief ProcessAssignment
  //ProcessAssignment(<ProcVarId>, <DataVarIdInit>*)
  class process_assignment: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_assignment(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_ProcessAssignment(m_term));
      }

      process_assignment(const process_identifier& pi, const data::assignment_list& v)
        : process_expression(core::detail::gsMakeProcessAssignment(pi,
                        atermpp::term_list< data::data_expression >(v.begin(), v.end())))
      {}

      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::assignment_list assignments() const
      {
        using namespace atermpp;
        return data::assignment_list(
          atermpp::term_list_iterator< data::assignment >(list_arg2(*this)),
          atermpp::term_list_iterator< data::assignment >());
      }
  };

  /// \brief Delta
  // Delta
  class delta: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      delta(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Delta(m_term));
      }

      delta()
        : process_expression(core::detail::gsMakeDelta())
      {}
  };

  /// \brief Tau
  // Tau
  class tau: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      tau(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Tau(m_term));
      }

      tau()
        : process_expression(core::detail::gsMakeTau())
      {}
  };

  /// \brief Sum
  // Sum(<DataVarId>+, <ProcExpr>)
  class sum: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      sum(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Sum(m_term));
      }

      sum(const data::variable_list& v, const process_expression& right)
        : process_expression(core::detail::gsMakeSum(atermpp::term_list< data::variable >(v.begin(), v.end()), right))
      {}

      data::variable_list variables() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator< data::variable >(list_arg1(*this)),
          atermpp::term_list_iterator< data::variable >());
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Block
  // Block(<String>*, <ProcExpr>)
  class block: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      block(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Block(m_term));
      }

      block(const core::identifier_string_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeBlock(s, right))
      {}

      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Hide
  // Hide(<String>*, <ProcExpr>)
  class hide: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      hide(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Hide(m_term));
      }

      hide(const core::identifier_string_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeHide(s, right))
      {}

      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Rename
  // Rename(<RenameExpr>*, <ProcExpr>)
  class rename: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      rename(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Rename(m_term));
      }

      rename(const rename_expression_list& r, const process_expression& right)
        : process_expression(core::detail::gsMakeRename(r, right))
      {}

      rename_expression_list rename_expressions() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Comm
  // Comm(<CommExpr>*, <ProcExpr>)
  class comm: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      comm(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Comm(m_term));
      }

      comm(const communication_expression_list& c, const process_expression& right)
        : process_expression(core::detail::gsMakeComm(c, right))
      {}

      communication_expression_list communication_expressions() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Allow
  // Allow(<MultActName>*, <ProcExpr>)
  class allow: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      allow(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Allow(m_term));
      }

      allow(const multi_action_name_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeAllow(s, right))
      {}

      multi_action_name_list names()
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_expression expression() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Sync
  // Sync(<ProcExpr>, <ProcExpr>)
  class sync: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      sync(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Sync(m_term));
      }

      sync(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeSync(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // AtTime(<ProcExpr>, <DataExpr>)
  /// \brief AtTime
  class at_time: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      at_time(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_AtTime(m_term));
      }

      at_time(const process_expression& left, const data::data_expression& d)
        : process_expression(core::detail::gsMakeAtTime(left, d))
      {}

      process_expression expression() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::data_expression time() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Seq(<ProcExpr>, <ProcExpr>)
  /// \brief Seq
  class seq: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      seq(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Seq(m_term));
      }

      seq(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeSeq(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // IfThen(<DataExpr>, <ProcExpr>)
  /// \brief IfThen
  class if_then: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      if_then(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_IfThen(m_term));
      }

      if_then(const data::data_expression& d, const process_expression& right)
        : process_expression(core::detail::gsMakeIfThen(d, right))
      {}

      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression left() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // IfThenElse(<DataExpr>, <ProcExpr>, <ProcExpr>)
  /// \brief IfThenElse
  class if_then_else: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      if_then_else(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_IfThenElse(m_term));
      }

      if_then_else(const data::data_expression& d, const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeIfThenElse(d, left, right))
      {}

      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression left() const
      {
        using namespace atermpp;
        return arg2(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg3(*this);
      }
  };

  // BInit(<ProcExpr>, <ProcExpr>)
  /// \brief BInit
  class binit: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      binit(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_BInit(m_term));
      }

      binit(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeBInit(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Merge(<ProcExpr>, <ProcExpr>)
  /// \brief Merge
  class merge: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      merge(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Merge(m_term));
      }

      merge(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeMerge(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // LMerge(<ProcExpr>, <ProcExpr>)
  /// \brief LMerge
  class lmerge: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      lmerge(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_LMerge(m_term));
      }

      lmerge(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeLMerge(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Choice(<ProcExpr>, <ProcExpr>)
  /// \brief Choice
  class choice: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      choice(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Choice(m_term));
      }

      choice(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeChoice(left, right))
      {}

      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
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
      if (i->is_standard())
      {
        spec.data().import_system_defined_sort(*i);
      }
    }
  }

  /// \brief Conversion to ATermAppl.
  /// \return The specification converted to ATerm format.
  inline
  ATermAppl process_specification_to_aterm(const process_specification& spec)
  {
    return core::detail::gsMakeProcSpec(
        data::detail::data_specification_to_aterm_data_spec(spec.data()),
        core::detail::gsMakeActSpec(spec.action_labels()),
        core::detail::gsMakeProcEqnSpec(process_equation_list(spec.equations().begin(), spec.equations().end())),
        spec.init()
    );
  }
  
  /// \brief Pretty print function
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

} // namespace lps

} // namespace mcrl2

namespace mcrl2 {

namespace core {

  /// \brief Contains type information for process expressions.
  template <>
  struct term_traits<lps::process_expression>
  {
    typedef lps::process_expression process_expression;

//--- start generated text ---//

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_action(const process_expression& t)
    {
      return core::detail::gsIsAction(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_process(const process_expression& t)
    {
      return core::detail::gsIsProcess(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_process_assignment(const process_expression& t)
    {
      return core::detail::gsIsProcessAssignment(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_delta(const process_expression& t)
    {
      return core::detail::gsIsDelta(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_tau(const process_expression& t)
    {
      return core::detail::gsIsTau(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_sum(const process_expression& t)
    {
      return core::detail::gsIsSum(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_block(const process_expression& t)
    {
      return core::detail::gsIsBlock(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_hide(const process_expression& t)
    {
      return core::detail::gsIsHide(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_rename(const process_expression& t)
    {
      return core::detail::gsIsRename(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_comm(const process_expression& t)
    {
      return core::detail::gsIsComm(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_allow(const process_expression& t)
    {
      return core::detail::gsIsAllow(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_sync(const process_expression& t)
    {
      return core::detail::gsIsSync(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_at_time(const process_expression& t)
    {
      return core::detail::gsIsAtTime(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_seq(const process_expression& t)
    {
      return core::detail::gsIsSeq(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_if_then(const process_expression& t)
    {
      return core::detail::gsIsIfThen(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_if_then_else(const process_expression& t)
    {
      return core::detail::gsIsIfThenElse(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_binit(const process_expression& t)
    {
      return core::detail::gsIsBInit(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_merge(const process_expression& t)
    {
      return core::detail::gsIsMerge(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_lmerge(const process_expression& t)
    {
      return core::detail::gsIsLMerge(t);
    }

    /// \brief Test for the value true
    /// \param t A term
    /// \return True if it is the value \p true
    static inline
    bool is_choice(const process_expression& t)
    {
      return core::detail::gsIsChoice(t);
    }
//--- end generated text ---//
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_LPS_PROCESS_H
