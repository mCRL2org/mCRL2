// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_expression.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_EXPRESSION_H
#define MCRL2_PROCESS_PROCESS_EXPRESSION_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/term_traits.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/process/process_identifier.h"
#include "mcrl2/process/rename_expression.h"
#include "mcrl2/process/communication_expression.h"

namespace mcrl2 {

namespace process {

// Make the LPS action visible.
using lps::action;

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

      /// \brief Constructor.
      /// \param pi A process identifier
      /// \param v A sequence of data expressions
      process(const process_identifier pi, const data::data_expression_list& v)
        : process_expression(core::detail::gsMakeProcess(pi,
                        atermpp::term_list<data::data_expression>(v.begin(), v.end())))
      {}

      /// \brief Returns the identifier of the process
      /// \return The identifier of the process
      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the arguments of the process expression
      /// \return The arguments of the process expression
      data::data_expression_list expressions() const
      {
        using namespace atermpp;
        return data::data_expression_list(
          atermpp::term_list_iterator<data::data_expression>(list_arg2(*this)),
          atermpp::term_list_iterator<data::data_expression>());
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

      /// \brief Constructor.
      /// \param pi A process identifier
      /// \param v A sequence of assignments to data variables
      process_assignment(const process_identifier& pi, const data::assignment_list& v)
        : process_expression(core::detail::gsMakeProcessAssignment(pi,
                        atermpp::term_list<data::data_expression>(v.begin(), v.end())))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::assignment_list assignments() const
      {
        using namespace atermpp;
        return data::assignment_list(
          atermpp::term_list_iterator<data::assignment>(list_arg2(*this)),
          atermpp::term_list_iterator<data::assignment>());
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

      /// \brief Constructor.
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

      /// \brief Constructor.
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

      /// \brief Constructor.
      /// \param v A sequence of data variables
      /// \param right A process expression
      sum(const data::variable_list& v, const process_expression& right)
        : process_expression(core::detail::gsMakeSum(atermpp::term_list<data::variable>(v.begin(), v.end()), right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::variable_list variables() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator<data::variable>(list_arg1(*this)),
          atermpp::term_list_iterator<data::variable>());
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param s A sequence of identifiers
      /// \param right A process expression
      block(const core::identifier_string_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeBlock(s, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param s A sequence of identifiers
      /// \param right A process expression
      hide(const core::identifier_string_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeHide(s, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      core::identifier_string_list names() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param r A sequence of rename expressions
      /// \param right A process expression
      rename(const rename_expression_list& r, const process_expression& right)
        : process_expression(core::detail::gsMakeRename(r, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      rename_expression_list rename_expressions() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param c A sequence of communication expressions
      /// \param right A process expression
      comm(const communication_expression_list& c, const process_expression& right)
        : process_expression(core::detail::gsMakeComm(c, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      communication_expression_list communication_expressions() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param s A sequence of multi-action names
      /// \param right A process expression
      allow(const multi_action_name_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeAllow(s, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      multi_action_name_list names()
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      sync(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeSync(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param d A data expression
      at_time(const process_expression& left, const data::data_expression& d)
        : process_expression(core::detail::gsMakeAtTime(left, d))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression expression() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      seq(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeSeq(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param d A data expression
      /// \param right A process expression
      if_then(const data::data_expression& d, const process_expression& right)
        : process_expression(core::detail::gsMakeIfThen(d, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param d A data expression
      /// \param left A process expression
      /// \param right A process expression
      if_then_else(const data::data_expression& d, const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeIfThenElse(d, left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg2(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      binit(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeBInit(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      merge(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeMerge(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      lmerge(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeLMerge(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
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

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      choice(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeChoice(left, right))
      {}

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief FUNCTION_DESCRIPTION
      /// \return RETURN_DESCRIPTION
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

} // namespace process

namespace core {

  /// \brief Contains type information for process expressions.
  template <>
  struct term_traits<process::process_expression>
  {
    typedef process::process_expression process_expression;

    //--- start generated text ---//
    
    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_action(const process_expression& t)
    {
      return core::detail::gsIsAction(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_process(const process_expression& t)
    {
      return core::detail::gsIsProcess(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_process_assignment(const process_expression& t)
    {
      return core::detail::gsIsProcessAssignment(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_delta(const process_expression& t)
    {
      return core::detail::gsIsDelta(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_tau(const process_expression& t)
    {
      return core::detail::gsIsTau(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_sum(const process_expression& t)
    {
      return core::detail::gsIsSum(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_block(const process_expression& t)
    {
      return core::detail::gsIsBlock(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_hide(const process_expression& t)
    {
      return core::detail::gsIsHide(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_rename(const process_expression& t)
    {
      return core::detail::gsIsRename(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_comm(const process_expression& t)
    {
      return core::detail::gsIsComm(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_allow(const process_expression& t)
    {
      return core::detail::gsIsAllow(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_sync(const process_expression& t)
    {
      return core::detail::gsIsSync(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_at_time(const process_expression& t)
    {
      return core::detail::gsIsAtTime(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_seq(const process_expression& t)
    {
      return core::detail::gsIsSeq(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_if_then(const process_expression& t)
    {
      return core::detail::gsIsIfThen(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_if_then_else(const process_expression& t)
    {
      return core::detail::gsIsIfThenElse(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_binit(const process_expression& t)
    {
      return core::detail::gsIsBInit(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_merge(const process_expression& t)
    {
      return core::detail::gsIsMerge(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
    /// \return True if it is the value \p true
    static inline
    bool is_lmerge(const process_expression& t)
    {
      return core::detail::gsIsLMerge(t);
    }

    /// \brief Test for the value true
    /// \param t A process expression
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

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_H
