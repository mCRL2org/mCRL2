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

  /// \brief Process instance
  // Process(<ProcVarId>, <DataExpr>*)
  class process_instance: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_instance(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_Process(m_term));
      }

      /// \brief Constructor.
      /// \param pi A process identifier
      /// \param v A sequence of data expressions
      process_instance(const process_identifier pi, const data::data_expression_list& v)
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

      /// \brief Returns the arguments of the process instance
      /// \return The arguments of the process expression
      data::data_expression_list actual_parameters() const
      {
        using namespace atermpp;
        return data::data_expression_list(
          atermpp::term_list_iterator<data::data_expression>(list_arg2(*this)),
          atermpp::term_list_iterator<data::data_expression>());
      }
  };

  /// \brief Process instance assignment
  //ProcessAssignment(<ProcVarId>, <DataVarIdInit>*)
  class process_instance_assignment: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      process_instance_assignment(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_ProcessAssignment(m_term));
      }

      /// \brief Constructor.
      /// \param pi A process identifier
      /// \param v A sequence of assignments to data variables
      process_instance_assignment(const process_identifier& pi, const data::assignment_list& v)
        : process_expression(core::detail::gsMakeProcessAssignment(pi,
                        atermpp::term_list<data::data_expression>(v.begin(), v.end())))
      {}

      /// \brief Returns the process identifier of the instance
      /// \return The process identifier of the instance
      process_identifier identifier() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the assignments of the instance
      /// \return The assignments of the instance
      data::assignment_list assignments() const
      {
        using namespace atermpp;
        return data::assignment_list(
          atermpp::term_list_iterator<data::assignment>(list_arg2(*this)),
          atermpp::term_list_iterator<data::assignment>());
      }
  };

  /// \brief Deadlock process
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

  /// \brief The silent step process expression tau
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

  /// \brief Sum operator for process expressions
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

      /// \brief Returns the bound variables of the sum
      /// \return The bound variables of the sum
      data::variable_list bound_variables() const
      {
        using namespace atermpp;
        return data::variable_list(
          atermpp::term_list_iterator<data::variable>(list_arg1(*this)),
          atermpp::term_list_iterator<data::variable>());
      }

      /// \brief Returns the operand of the sum
      /// \return The operand of the sum
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Blocking operator for process expressions
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

      /// \brief Returns the set of blocked names
      /// \return The set of blocked names
      core::identifier_string_list block_set() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief Returns the operand of the block expression
      /// \return The operand of the block expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Hiding operator for process expressions
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

      /// \brief Returns the set of hidden names
      /// \return The set of hidden names
      core::identifier_string_list hide_set() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief Returns the operand of the hide expression
      /// \return The operand of the hide expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Rename operator for process expressions
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

      /// \brief Returns the set of rename rules
      /// \return The set of rename rules
      rename_expression_list rename_set() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief Returns the operand of the rename expression
      /// \return The operand of the rename expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Communication operator for process expressions
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

      /// \brief Returns the set of communications
      /// \return The set of communications
      communication_expression_list comm_set() const
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief Returns the operand of the comm expression
      /// \return The operand of the comm expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Allow operator for process expressions
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
      allow(const action_name_multiset_list& s, const process_expression& right)
        : process_expression(core::detail::gsMakeAllow(s, right))
      {}

      /// \brief Returns the set of allowed multi-actions
      /// \return The set of allowed multi-actions
      action_name_multiset_list allow_set()
      {
        using namespace atermpp;
        return list_arg1(*this);
      }

      /// \brief Returns the operand of the allow expression
      /// \return The operand of the allow expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  /// \brief Synchronization operator for process expressions
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

      /// \brief Returns the left operand of the sync expression
      /// \return The left operand of the sync expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the sync expression
      /// \return The right operand of the sync expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // AtTime(<ProcExpr>, <DataExpr>)
  /// \brief At operator for process expressions
  class at: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      at(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_AtTime(m_term));
      }

      /// \brief Constructor.
      /// \param left A process expression
      /// \param d A data expression
      at(const process_expression& left, const data::data_expression& d)
        : process_expression(core::detail::gsMakeAtTime(left, d))
      {}

      /// \brief Returns the operand of the at expression
      /// \return The operand of the at expression
      process_expression operand() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the time stamp of the at expression
      /// \return The time stamp of the at expression
      data::data_expression time_stamp() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Seq(<ProcExpr>, <ProcExpr>)
  /// \brief Sequential composition operator for process expressions
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

      /// \brief Returns the left operand of the seq expression
      /// \return The left operand of the seq expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the seq expression
      /// \return The right operand of the seq expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // IfThen(<DataExpr>, <ProcExpr>)
  /// \brief If-then operator for process expressions
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

      /// \brief Returns the condition of the if-then expression
      /// \return The condition of the if-then expression
      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the then case of the if-then expression
      /// \return The then case of the if-then expression
      process_expression then_case() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // IfThenElse(<DataExpr>, <ProcExpr>, <ProcExpr>)
  /// \brief If-then-else operator for process expressions
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

      /// \brief Returns the condition of the if-then-else
      /// \return The condition of the if-then-else
      data::data_expression condition() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the then case of the if-then-else expression
      /// \return The then case of the if-then-else expression
      process_expression then_case() const
      {
        using namespace atermpp;
        return arg2(*this);
      }

      /// \brief Returns the then case of the if-then-else expression
      /// \return The then case of the if-then-else expression
      process_expression else_case() const
      {
        using namespace atermpp;
        return arg3(*this);
      }
  };

  // BInit(<ProcExpr>, <ProcExpr>)
  /// \brief Bounded initialization operator for process expressions
  class bounded_init: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      bounded_init(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_BInit(m_term));
      }

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      bounded_init(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeBInit(left, right))
      {}

      /// \brief Returns the left operand of the bounded initialization expression
      /// \return The left operand of the bounded initialization expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the bounded initialization expression
      /// \return The right operand of the bounded initialization expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Merge(<ProcExpr>, <ProcExpr>)
  /// \brief Merge operator for process expressions
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

      /// \brief Returns the left operand of the merge expression
      /// \return The left operand of the merge expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the merge expression
      /// \return The right operand of the merge expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // LMerge(<ProcExpr>, <ProcExpr>)
  /// \brief Left-merge operator for process expressions
  class left_merge: public process_expression
  {
    public:
      /// \brief Constructor.
      /// \param term A term
      left_merge(atermpp::aterm_appl term)
        : process_expression(term)
      {
        assert(core::detail::check_term_LMerge(m_term));
      }

      /// \brief Constructor.
      /// \param left A process expression
      /// \param right A process expression
      left_merge(const process_expression& left, const process_expression& right)
        : process_expression(core::detail::gsMakeLMerge(left, right))
      {}

      /// \brief Returns the left operand of the left-merge expression
      /// \return The left operand of the left-merge expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the left-merge expression
      /// \return The right operand of the left-merge expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

  // Choice(<ProcExpr>, <ProcExpr>)
  /// \brief Choice operator for process expressions
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

      /// \brief Returns the left operand of the choice expression
      /// \return The left operand of the choice expression
      process_expression left() const
      {
        using namespace atermpp;
        return arg1(*this);
      }

      /// \brief Returns the right operand of the choice expression
      /// \return The right operand of the choice expression
      process_expression right() const
      {
        using namespace atermpp;
        return arg2(*this);
      }
  };

//--- start generated text ---//

    /// \brief Test for a action expression
    /// \param t A term
    /// \return True if it is a action expression
    inline
    bool is_action(const process_expression& t)
    {
      return core::detail::gsIsAction(t);
    }

    /// \brief Test for a process_instance expression
    /// \param t A term
    /// \return True if it is a process_instance expression
    inline
    bool is_process_instance(const process_expression& t)
    {
      return core::detail::gsIsProcess(t);
    }

    /// \brief Test for a process_instance_assignment expression
    /// \param t A term
    /// \return True if it is a process_instance_assignment expression
    inline
    bool is_process_instance_assignment(const process_expression& t)
    {
      return core::detail::gsIsProcessAssignment(t);
    }

    /// \brief Test for a delta expression
    /// \param t A term
    /// \return True if it is a delta expression
    inline
    bool is_delta(const process_expression& t)
    {
      return core::detail::gsIsDelta(t);
    }

    /// \brief Test for a tau expression
    /// \param t A term
    /// \return True if it is a tau expression
    inline
    bool is_tau(const process_expression& t)
    {
      return core::detail::gsIsTau(t);
    }

    /// \brief Test for a sum expression
    /// \param t A term
    /// \return True if it is a sum expression
    inline
    bool is_sum(const process_expression& t)
    {
      return core::detail::gsIsSum(t);
    }

    /// \brief Test for a block expression
    /// \param t A term
    /// \return True if it is a block expression
    inline
    bool is_block(const process_expression& t)
    {
      return core::detail::gsIsBlock(t);
    }

    /// \brief Test for a hide expression
    /// \param t A term
    /// \return True if it is a hide expression
    inline
    bool is_hide(const process_expression& t)
    {
      return core::detail::gsIsHide(t);
    }

    /// \brief Test for a rename expression
    /// \param t A term
    /// \return True if it is a rename expression
    inline
    bool is_rename(const process_expression& t)
    {
      return core::detail::gsIsRename(t);
    }

    /// \brief Test for a comm expression
    /// \param t A term
    /// \return True if it is a comm expression
    inline
    bool is_comm(const process_expression& t)
    {
      return core::detail::gsIsComm(t);
    }

    /// \brief Test for a allow expression
    /// \param t A term
    /// \return True if it is a allow expression
    inline
    bool is_allow(const process_expression& t)
    {
      return core::detail::gsIsAllow(t);
    }

    /// \brief Test for a sync expression
    /// \param t A term
    /// \return True if it is a sync expression
    inline
    bool is_sync(const process_expression& t)
    {
      return core::detail::gsIsSync(t);
    }

    /// \brief Test for a at expression
    /// \param t A term
    /// \return True if it is a at expression
    inline
    bool is_at(const process_expression& t)
    {
      return core::detail::gsIsAtTime(t);
    }

    /// \brief Test for a seq expression
    /// \param t A term
    /// \return True if it is a seq expression
    inline
    bool is_seq(const process_expression& t)
    {
      return core::detail::gsIsSeq(t);
    }

    /// \brief Test for a if_then expression
    /// \param t A term
    /// \return True if it is a if_then expression
    inline
    bool is_if_then(const process_expression& t)
    {
      return core::detail::gsIsIfThen(t);
    }

    /// \brief Test for a if_then_else expression
    /// \param t A term
    /// \return True if it is a if_then_else expression
    inline
    bool is_if_then_else(const process_expression& t)
    {
      return core::detail::gsIsIfThenElse(t);
    }

    /// \brief Test for a bounded_init expression
    /// \param t A term
    /// \return True if it is a bounded_init expression
    inline
    bool is_bounded_init(const process_expression& t)
    {
      return core::detail::gsIsBInit(t);
    }

    /// \brief Test for a merge expression
    /// \param t A term
    /// \return True if it is a merge expression
    inline
    bool is_merge(const process_expression& t)
    {
      return core::detail::gsIsMerge(t);
    }

    /// \brief Test for a left_merge expression
    /// \param t A term
    /// \return True if it is a left_merge expression
    inline
    bool is_left_merge(const process_expression& t)
    {
      return core::detail::gsIsLMerge(t);
    }

    /// \brief Test for a choice expression
    /// \param t A term
    /// \return True if it is a choice expression
    inline
    bool is_choice(const process_expression& t)
    {
      return core::detail::gsIsChoice(t);
    }
//--- end generated text ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_H
