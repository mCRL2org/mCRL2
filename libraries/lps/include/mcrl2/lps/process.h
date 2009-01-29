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

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/detail/algorithms.h"

namespace mcrl2 {

namespace lps {

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
      
      core::identifier_string name() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::sort_expression_list sorts() const
      {
        using namespace atermpp;
        return list_arg2(*this);
      }
  };

  /// \brief Process equation
  // <ProcEqn> ::= ProcEqn(<DataVarId>*, <ProcVarId>, <DataVarId>*, <ProcExpr>)
  class process_equation: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_equation(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcEqn(m_term));
      }

      data::data_variable_list variables1() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      process_identifier name() const
      {
        using namespace atermpp;
        return arg2(*this);
      }     

      data::data_variable_list variables2() const
      {
        using namespace atermpp;
        return list_arg3(*this);
      }   

      process_expression expression() const
      {
        using namespace atermpp;
        return arg4(*this);
      }     
  };

  /// \brief Read-only singly linked list of process equations
  typedef atermpp::term_list<process_equation> process_equation_list;

  /// \brief Process initialization
  //<ProcInit>     ::= ProcessInit(<DataVarId>*, <ProcExpr>)
  class process_initialization: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_initialization(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcessInit(m_term));
      }

      data::data_variable_list variables() const
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

  /// \brief Process specification
  //<ProcSpec>     ::= ProcSpec(<DataSpec>, <ActSpec>, <ProcEqnSpec>, <ProcInit>)
  class process_specification: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_specification(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(core::detail::check_term_ProcSpec(m_term));
      }

      data::data_specification data() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      action_list actions() const
      {
        using namespace atermpp;
        return list_arg1(arg2(*this));
      }

      process_equation_list equations() const
      {
        using namespace atermpp;
        return list_arg1(arg3(*this));
      }
      
      process_initialization init() const
      {
        using namespace atermpp;
        return arg4(*this);       
      }
  };

  inline
  process_specification parse_process_specification(const std::string& spec)
  {
    std::stringstream spec_stream;
    spec_stream << spec;
    ATermAppl result = detail::parse_specification(spec_stream);
    result           = detail::type_check_specification(result);
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

      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      data::data_expression_list expressions() const
      {
        using namespace atermpp;
        return list_arg2(*this);
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

      data::data_variable_list variables() const
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

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::process)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::process_assignment)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::delta)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::tau)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::sum)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::block)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::hide)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::rename)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::comm)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::allow)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::sync)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::at_time)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::seq)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::if_then)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::if_then_else)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::binit)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::merge)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::lmerge)
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::choice)
/// \endcond

#endif // MCRL2_LPS_PROCESS_H
