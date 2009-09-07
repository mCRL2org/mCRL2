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
#include "mcrl2/data/assignment.h"
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

//--- start generated expression classes ---//
/// \brief An action
class process_action: public process_expression
{
  public:
    /// \brief Constructor.
    /// \param term A term
    process_action(atermpp::aterm_appl term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Action(m_term));
    }

    /// \brief Constructor.
    process_action(const lps::action_label& label, const data::data_expression_list& arguments)
      : process_expression(core::detail::gsMakeAction(label, arguments))
    {}

    lps::action_label label() const
    {
      return atermpp::arg1(*this);
    }

    data::data_expression_list arguments() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief A process
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
    process_instance(const process_identifier identifier, const data::data_expression_list& actual_parameters)
      : process_expression(core::detail::gsMakeProcess(identifier, actual_parameters))
    {}

    process_identifier identifier() const
    {
      return atermpp::arg1(*this);
    }

    data::data_expression_list actual_parameters() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief A process assignment
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
    process_instance_assignment(const process_identifier& identifier, const data::assignment_list& assignments)
      : process_expression(core::detail::gsMakeProcessAssignment(identifier, assignments))
    {}

    process_identifier identifier() const
    {
      return atermpp::arg1(*this);
    }

    data::assignment_list assignments() const
    {
      return atermpp::list_arg2(*this);
    }
};

/// \brief The value delta
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

/// \brief The value tau
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

/// \brief The sum operator
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
    sum(const data::variable_list& bound_variables, const process_expression& operand)
      : process_expression(core::detail::gsMakeSum(bound_variables, operand))
    {}

    data::variable_list bound_variables() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The block operator
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
    block(const core::identifier_string_list& block_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeBlock(block_set, operand))
    {}

    core::identifier_string_list block_set() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The hide operator
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
    hide(const core::identifier_string_list& hide_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeHide(hide_set, operand))
    {}

    core::identifier_string_list hide_set() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The rename operator
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
    rename(const rename_expression_list& rename_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeRename(rename_set, operand))
    {}

    rename_expression_list rename_set() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The communication operator
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
    comm(const communication_expression_list& comm_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeComm(comm_set, operand))
    {}

    communication_expression_list comm_set() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The allow operator
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
    allow(const action_name_multiset_list& allow_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeAllow(allow_set, operand))
    {}

    action_name_multiset_list allow_set() const
    {
      return atermpp::list_arg1(*this);
    }

    process_expression operand() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The synchronization operator
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
    sync(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeSync(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The at operator
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
    at(const process_expression& operand, const data::data_expression& time_stamp)
      : process_expression(core::detail::gsMakeAtTime(operand, time_stamp))
    {}

    process_expression operand() const
    {
      return atermpp::arg1(*this);
    }

    data::data_expression time_stamp() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The sequential composition
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
    seq(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeSeq(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The if-then operator
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
    if_then(const data::data_expression& condition, const process_expression& then_case)
      : process_expression(core::detail::gsMakeIfThen(condition, then_case))
    {}

    data::data_expression condition() const
    {
      return atermpp::arg1(*this);
    }

    process_expression then_case() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The if-then-else operator
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
    if_then_else(const data::data_expression& condition, const process_expression& then_case, const process_expression& else_case)
      : process_expression(core::detail::gsMakeIfThenElse(condition, then_case, else_case))
    {}

    data::data_expression condition() const
    {
      return atermpp::arg1(*this);
    }

    process_expression then_case() const
    {
      return atermpp::arg2(*this);
    }

    process_expression else_case() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief The bounded initialization
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
    bounded_init(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeBInit(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The merge operator
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
    merge(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeMerge(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The left merge operator
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
    left_merge(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeLMerge(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief The choice operator
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
    choice(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeChoice(left, right))
    {}

    process_expression left() const
    {
      return atermpp::arg1(*this);
    }

    process_expression right() const
    {
      return atermpp::arg2(*this);
    }
};
//--- end generated expression classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a process_action expression
    /// \param t A term
    /// \return True if it is a process_action expression
    inline
    bool is_process_action(const process_expression& t)
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
//--- end generated is-functions ---//

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_H
