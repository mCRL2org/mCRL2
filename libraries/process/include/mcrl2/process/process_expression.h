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

#include <set>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/down_cast.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/precedence.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/process/process_identifier.h"
#include "mcrl2/process/rename_expression.h"
#include "mcrl2/process/communication_expression.h"

namespace mcrl2
{

namespace process
{

// Make some LPS types visible. These should become part of the process library.
using lps::action;
using lps::action_name_set;
using lps::multi_action;
using lps::multi_action_name;
using lps::multi_action_name_set;

// Needed for argument dependent lookup (?)
using namespace core::detail::precedences;

//--- start generated classes ---//
/// \brief A process expression
class process_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    process_expression()
      : atermpp::aterm_appl(core::detail::constructProcExpr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_ProcExpr(*this));
    }

    /// \brief Constructor.
    process_expression(const lps::action& x)
      : atermpp::aterm_appl(x)
    {}
};

/// \brief list of process_expressions
typedef atermpp::term_list<process_expression> process_expression_list;

/// \brief vector of process_expressions
typedef std::vector<process_expression>    process_expression_vector;

// prototypes
inline bool is_process_instance(const atermpp::aterm_appl& x);
inline bool is_process_instance_assignment(const atermpp::aterm_appl& x);
inline bool is_delta(const atermpp::aterm_appl& x);
inline bool is_tau(const atermpp::aterm_appl& x);
inline bool is_sum(const atermpp::aterm_appl& x);
inline bool is_block(const atermpp::aterm_appl& x);
inline bool is_hide(const atermpp::aterm_appl& x);
inline bool is_rename(const atermpp::aterm_appl& x);
inline bool is_comm(const atermpp::aterm_appl& x);
inline bool is_allow(const atermpp::aterm_appl& x);
inline bool is_sync(const atermpp::aterm_appl& x);
inline bool is_at(const atermpp::aterm_appl& x);
inline bool is_seq(const atermpp::aterm_appl& x);
inline bool is_if_then(const atermpp::aterm_appl& x);
inline bool is_if_then_else(const atermpp::aterm_appl& x);
inline bool is_bounded_init(const atermpp::aterm_appl& x);
inline bool is_merge(const atermpp::aterm_appl& x);
inline bool is_left_merge(const atermpp::aterm_appl& x);
inline bool is_choice(const atermpp::aterm_appl& x);
inline bool is_untyped_parameter_identifier(const atermpp::aterm_appl& x);
inline bool is_untyped_process_assignment(const atermpp::aterm_appl& x);

/// \brief Test for a process_expression expression
/// \param x A term
/// \return True if \a x is a process_expression expression
inline
bool is_process_expression(const atermpp::aterm_appl& x)
{
  return process::is_process_instance(x) ||
         process::is_process_instance_assignment(x) ||
         process::is_delta(x) ||
         process::is_tau(x) ||
         process::is_sum(x) ||
         process::is_block(x) ||
         process::is_hide(x) ||
         process::is_rename(x) ||
         process::is_comm(x) ||
         process::is_allow(x) ||
         process::is_sync(x) ||
         process::is_at(x) ||
         process::is_seq(x) ||
         process::is_if_then(x) ||
         process::is_if_then_else(x) ||
         process::is_bounded_init(x) ||
         process::is_merge(x) ||
         process::is_left_merge(x) ||
         process::is_choice(x) ||
         process::is_untyped_parameter_identifier(x) ||
         process::is_untyped_process_assignment(x) ||
         lps::is_action(x);
}

/// \brief swap overload
inline void swap(process_expression& t1, process_expression& t2)
{
  t1.swap(t2);
}


/// \brief A process
class process_instance: public process_expression
{
  public:
    /// \brief Default constructor.
    process_instance()
      : process_expression(core::detail::constructProcess())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_instance(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Process(*this));
    }

    /// \brief Constructor.
    process_instance(const process_identifier& identifier, const data::data_expression_list& actual_parameters)
      : process_expression(core::detail::gsMakeProcess(identifier, actual_parameters))
    {}

    const process_identifier& identifier() const
    {
      return atermpp::aterm_cast<const process_identifier>(atermpp::arg1(*this));
    }

    const data::data_expression_list& actual_parameters() const
    {
      return atermpp::aterm_cast<const data::data_expression_list>(atermpp::list_arg2(*this));
    }
};

/// \brief Test for a process_instance expression
/// \param x A term
/// \return True if \a x is a process_instance expression
inline
bool is_process_instance(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsProcess(x);
}

/// \brief swap overload
inline void swap(process_instance& t1, process_instance& t2)
{
  t1.swap(t2);
}


/// \brief A process assignment
class process_instance_assignment: public process_expression
{
  public:
    /// \brief Default constructor.
    process_instance_assignment()
      : process_expression(core::detail::constructProcessAssignment())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_instance_assignment(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_ProcessAssignment(*this));
    }

    /// \brief Constructor.
    process_instance_assignment(const process_identifier& identifier, const data::assignment_list& assignments)
      : process_expression(core::detail::gsMakeProcessAssignment(identifier, assignments))
    {}

    const process_identifier& identifier() const
    {
      return atermpp::aterm_cast<const process_identifier>(atermpp::arg1(*this));
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const data::assignment_list>(atermpp::list_arg2(*this));
    }
};

/// \brief Test for a process_instance_assignment expression
/// \param x A term
/// \return True if \a x is a process_instance_assignment expression
inline
bool is_process_instance_assignment(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsProcessAssignment(x);
}

/// \brief swap overload
inline void swap(process_instance_assignment& t1, process_instance_assignment& t2)
{
  t1.swap(t2);
}


/// \brief The value delta
class delta: public process_expression
{
  public:
    /// \brief Default constructor.
    delta()
      : process_expression(core::detail::constructDelta())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit delta(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Delta(*this));
    }
};

/// \brief Test for a delta expression
/// \param x A term
/// \return True if \a x is a delta expression
inline
bool is_delta(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsDelta(x);
}

/// \brief swap overload
inline void swap(delta& t1, delta& t2)
{
  t1.swap(t2);
}


/// \brief The value tau
class tau: public process_expression
{
  public:
    /// \brief Default constructor.
    tau()
      : process_expression(core::detail::constructTau())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit tau(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Tau(*this));
    }
};

/// \brief Test for a tau expression
/// \param x A term
/// \return True if \a x is a tau expression
inline
bool is_tau(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsTau(x);
}

/// \brief swap overload
inline void swap(tau& t1, tau& t2)
{
  t1.swap(t2);
}


/// \brief The sum operator
class sum: public process_expression
{
  public:
    /// \brief Default constructor.
    sum()
      : process_expression(core::detail::constructSum())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit sum(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Sum(*this));
    }

    /// \brief Constructor.
    sum(const data::variable_list& bound_variables, const process_expression& operand)
      : process_expression(core::detail::gsMakeSum(bound_variables, operand))
    {}

    const data::variable_list& bound_variables() const
    {
      return atermpp::aterm_cast<const data::variable_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a sum expression
/// \param x A term
/// \return True if \a x is a sum expression
inline
bool is_sum(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSum(x);
}

/// \brief swap overload
inline void swap(sum& t1, sum& t2)
{
  t1.swap(t2);
}


/// \brief The block operator
class block: public process_expression
{
  public:
    /// \brief Default constructor.
    block()
      : process_expression(core::detail::constructBlock())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit block(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Block(*this));
    }

    /// \brief Constructor.
    block(const core::identifier_string_list& block_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeBlock(block_set, operand))
    {}

    const core::identifier_string_list& block_set() const
    {
      return atermpp::aterm_cast<const core::identifier_string_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a block expression
/// \param x A term
/// \return True if \a x is a block expression
inline
bool is_block(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsBlock(x);
}

/// \brief swap overload
inline void swap(block& t1, block& t2)
{
  t1.swap(t2);
}


/// \brief The hide operator
class hide: public process_expression
{
  public:
    /// \brief Default constructor.
    hide()
      : process_expression(core::detail::constructHide())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit hide(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Hide(*this));
    }

    /// \brief Constructor.
    hide(const core::identifier_string_list& hide_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeHide(hide_set, operand))
    {}

    const core::identifier_string_list& hide_set() const
    {
      return atermpp::aterm_cast<const core::identifier_string_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a hide expression
/// \param x A term
/// \return True if \a x is a hide expression
inline
bool is_hide(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsHide(x);
}

/// \brief swap overload
inline void swap(hide& t1, hide& t2)
{
  t1.swap(t2);
}


/// \brief The rename operator
class rename: public process_expression
{
  public:
    /// \brief Default constructor.
    rename()
      : process_expression(core::detail::constructRename())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit rename(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Rename(*this));
    }

    /// \brief Constructor.
    rename(const rename_expression_list& rename_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeRename(rename_set, operand))
    {}

    const rename_expression_list& rename_set() const
    {
      return atermpp::aterm_cast<const rename_expression_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a rename expression
/// \param x A term
/// \return True if \a x is a rename expression
inline
bool is_rename(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsRename(x);
}

/// \brief swap overload
inline void swap(rename& t1, rename& t2)
{
  t1.swap(t2);
}


/// \brief The communication operator
class comm: public process_expression
{
  public:
    /// \brief Default constructor.
    comm()
      : process_expression(core::detail::constructComm())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit comm(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Comm(*this));
    }

    /// \brief Constructor.
    comm(const communication_expression_list& comm_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeComm(comm_set, operand))
    {}

    const communication_expression_list& comm_set() const
    {
      return atermpp::aterm_cast<const communication_expression_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a comm expression
/// \param x A term
/// \return True if \a x is a comm expression
inline
bool is_comm(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsComm(x);
}

/// \brief swap overload
inline void swap(comm& t1, comm& t2)
{
  t1.swap(t2);
}


/// \brief The allow operator
class allow: public process_expression
{
  public:
    /// \brief Default constructor.
    allow()
      : process_expression(core::detail::constructAllow())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit allow(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Allow(*this));
    }

    /// \brief Constructor.
    allow(const action_name_multiset_list& allow_set, const process_expression& operand)
      : process_expression(core::detail::gsMakeAllow(allow_set, operand))
    {}

    const action_name_multiset_list& allow_set() const
    {
      return atermpp::aterm_cast<const action_name_multiset_list>(atermpp::list_arg1(*this));
    }

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a allow expression
/// \param x A term
/// \return True if \a x is a allow expression
inline
bool is_allow(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsAllow(x);
}

/// \brief swap overload
inline void swap(allow& t1, allow& t2)
{
  t1.swap(t2);
}


/// \brief The synchronization operator
class sync: public process_expression
{
  public:
    /// \brief Default constructor.
    sync()
      : process_expression(core::detail::constructSync())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit sync(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Sync(*this));
    }

    /// \brief Constructor.
    sync(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeSync(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a sync expression
/// \param x A term
/// \return True if \a x is a sync expression
inline
bool is_sync(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSync(x);
}

/// \brief swap overload
inline void swap(sync& t1, sync& t2)
{
  t1.swap(t2);
}


/// \brief The at operator
class at: public process_expression
{
  public:
    /// \brief Default constructor.
    at()
      : process_expression(core::detail::constructAtTime())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit at(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_AtTime(*this));
    }

    /// \brief Constructor.
    at(const process_expression& operand, const data::data_expression& time_stamp)
      : process_expression(core::detail::gsMakeAtTime(operand, time_stamp))
    {}

    const process_expression& operand() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const data::data_expression& time_stamp() const
    {
      return atermpp::aterm_cast<const data::data_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a at expression
/// \param x A term
/// \return True if \a x is a at expression
inline
bool is_at(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsAtTime(x);
}

/// \brief swap overload
inline void swap(at& t1, at& t2)
{
  t1.swap(t2);
}


/// \brief The sequential composition
class seq: public process_expression
{
  public:
    /// \brief Default constructor.
    seq()
      : process_expression(core::detail::constructSeq())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit seq(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Seq(*this));
    }

    /// \brief Constructor.
    seq(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeSeq(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a seq expression
/// \param x A term
/// \return True if \a x is a seq expression
inline
bool is_seq(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsSeq(x);
}

/// \brief swap overload
inline void swap(seq& t1, seq& t2)
{
  t1.swap(t2);
}


/// \brief The if-then operator
class if_then: public process_expression
{
  public:
    /// \brief Default constructor.
    if_then()
      : process_expression(core::detail::constructIfThen())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit if_then(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_IfThen(*this));
    }

    /// \brief Constructor.
    if_then(const data::data_expression& condition, const process_expression& then_case)
      : process_expression(core::detail::gsMakeIfThen(condition, then_case))
    {}

    const data::data_expression& condition() const
    {
      return atermpp::aterm_cast<const data::data_expression>(atermpp::arg1(*this));
    }

    const process_expression& then_case() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a if_then expression
/// \param x A term
/// \return True if \a x is a if_then expression
inline
bool is_if_then(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsIfThen(x);
}

/// \brief swap overload
inline void swap(if_then& t1, if_then& t2)
{
  t1.swap(t2);
}


/// \brief The if-then-else operator
class if_then_else: public process_expression
{
  public:
    /// \brief Default constructor.
    if_then_else()
      : process_expression(core::detail::constructIfThenElse())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit if_then_else(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_IfThenElse(*this));
    }

    /// \brief Constructor.
    if_then_else(const data::data_expression& condition, const process_expression& then_case, const process_expression& else_case)
      : process_expression(core::detail::gsMakeIfThenElse(condition, then_case, else_case))
    {}

    const data::data_expression& condition() const
    {
      return atermpp::aterm_cast<const data::data_expression>(atermpp::arg1(*this));
    }

    const process_expression& then_case() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }

    const process_expression& else_case() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg3(*this));
    }
};

/// \brief Test for a if_then_else expression
/// \param x A term
/// \return True if \a x is a if_then_else expression
inline
bool is_if_then_else(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsIfThenElse(x);
}

/// \brief swap overload
inline void swap(if_then_else& t1, if_then_else& t2)
{
  t1.swap(t2);
}


/// \brief The bounded initialization
class bounded_init: public process_expression
{
  public:
    /// \brief Default constructor.
    bounded_init()
      : process_expression(core::detail::constructBInit())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit bounded_init(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_BInit(*this));
    }

    /// \brief Constructor.
    bounded_init(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeBInit(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a bounded_init expression
/// \param x A term
/// \return True if \a x is a bounded_init expression
inline
bool is_bounded_init(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsBInit(x);
}

/// \brief swap overload
inline void swap(bounded_init& t1, bounded_init& t2)
{
  t1.swap(t2);
}


/// \brief The merge operator
class merge: public process_expression
{
  public:
    /// \brief Default constructor.
    merge()
      : process_expression(core::detail::constructMerge())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit merge(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Merge(*this));
    }

    /// \brief Constructor.
    merge(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeMerge(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a merge expression
/// \param x A term
/// \return True if \a x is a merge expression
inline
bool is_merge(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsMerge(x);
}

/// \brief swap overload
inline void swap(merge& t1, merge& t2)
{
  t1.swap(t2);
}


/// \brief The left merge operator
class left_merge: public process_expression
{
  public:
    /// \brief Default constructor.
    left_merge()
      : process_expression(core::detail::constructLMerge())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit left_merge(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_LMerge(*this));
    }

    /// \brief Constructor.
    left_merge(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeLMerge(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a left_merge expression
/// \param x A term
/// \return True if \a x is a left_merge expression
inline
bool is_left_merge(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsLMerge(x);
}

/// \brief swap overload
inline void swap(left_merge& t1, left_merge& t2)
{
  t1.swap(t2);
}


/// \brief The choice operator
class choice: public process_expression
{
  public:
    /// \brief Default constructor.
    choice()
      : process_expression(core::detail::constructChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit choice(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Choice(*this));
    }

    /// \brief Constructor.
    choice(const process_expression& left, const process_expression& right)
      : process_expression(core::detail::gsMakeChoice(left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg1(*this));
    }

    const process_expression& right() const
    {
      return atermpp::aterm_cast<const process_expression>(atermpp::arg2(*this));
    }
};

/// \brief Test for a choice expression
/// \param x A term
/// \return True if \a x is a choice expression
inline
bool is_choice(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsChoice(x);
}

/// \brief swap overload
inline void swap(choice& t1, choice& t2)
{
  t1.swap(t2);
}


/// \brief An untyped parameter identifier
class untyped_parameter_identifier: public process_expression
{
  public:
    /// \brief Default constructor.
    untyped_parameter_identifier()
      : process_expression(core::detail::constructUntypedParamId())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_parameter_identifier(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_UntypedParamId(*this));
    }

    /// \brief Constructor.
    untyped_parameter_identifier(const core::identifier_string& name, const data::data_expression_list& arguments)
      : process_expression(core::detail::gsMakeUntypedParamId(name, arguments))
    {}

    /// \brief Constructor.
    untyped_parameter_identifier(const std::string& name, const data::data_expression_list& arguments)
      : process_expression(core::detail::gsMakeUntypedParamId(core::identifier_string(name), arguments))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::data_expression_list& arguments() const
    {
      return atermpp::aterm_cast<const data::data_expression_list>(atermpp::list_arg2(*this));
    }
};

/// \brief Test for a untyped_parameter_identifier expression
/// \param x A term
/// \return True if \a x is a untyped_parameter_identifier expression
inline
bool is_untyped_parameter_identifier(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsUntypedParamId(x);
}

/// \brief swap overload
inline void swap(untyped_parameter_identifier& t1, untyped_parameter_identifier& t2)
{
  t1.swap(t2);
}


/// \brief An untyped process assginment
class untyped_process_assignment: public process_expression
{
  public:
    /// \brief Default constructor.
    untyped_process_assignment()
      : process_expression(core::detail::constructUntypedProcessAssignment())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_process_assignment(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_UntypedProcessAssignment(*this));
    }

    /// \brief Constructor.
    untyped_process_assignment(const core::identifier_string& name, const data::untyped_identifier_assignment_list& assignments)
      : process_expression(core::detail::gsMakeUntypedProcessAssignment(name, assignments))
    {}

    /// \brief Constructor.
    untyped_process_assignment(const std::string& name, const data::untyped_identifier_assignment_list& assignments)
      : process_expression(core::detail::gsMakeUntypedProcessAssignment(core::identifier_string(name), assignments))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data::untyped_identifier_assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const data::untyped_identifier_assignment_list>(atermpp::list_arg2(*this));
    }
};

/// \brief Test for a untyped_process_assignment expression
/// \param x A term
/// \return True if \a x is a untyped_process_assignment expression
inline
bool is_untyped_process_assignment(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsUntypedProcessAssignment(x);
}

/// \brief swap overload
inline void swap(untyped_process_assignment& t1, untyped_process_assignment& t2)
{
  t1.swap(t2);
}

//--- end generated classes ---//

// From the documentation:
// The descending order of precedence of the operators is: "|", "@", ".", { "<<", ">>" }, "->", { "||", "||_" }, "sum", "+".

/// \brief Defines a precedence relation on process expressions
inline
int precedence(const process_expression& x)
{
  if (is_choice(x))
  {
    return 1;
  }
  else if (is_sum(x))
  {
    return 2;
  }
  else if (is_merge(x))
  {
    return 3;
  }
  else if (is_left_merge(x))
  {
    return 4;
  }
  else if (is_if_then(x) || is_if_then_else(x))
  {
    return 5;
  }
  else if (is_bounded_init(x))
  {
    return 6;
  }
  else if (is_seq(x))
  {
    return 7;
  }
  else if (is_at(x))
  {
    return 8;
  }
  else if (is_sync(x))
  {
    return 9;
  }
  return max_precedence;
}

inline int precedence(const choice& x)       { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const sum& x)          { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const merge& x)        { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const left_merge& x)   { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const if_then& x)      { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const if_then_else& x) { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const bounded_init& x) { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const seq& x)          { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const at& x)           { return precedence(static_cast<const process_expression&>(x)); }
inline int precedence(const sync& x)         { return precedence(static_cast<const process_expression&>(x)); }

inline const process_expression& unary_operand(const sum& x)         { return x.operand(); }
inline const process_expression& unary_operand(const block& x)       { return x.operand(); }
inline const process_expression& unary_operand(const hide& x)        { return x.operand(); }
inline const process_expression& unary_operand(const rename& x)      { return x.operand(); }
inline const process_expression& unary_operand(const comm& x)        { return x.operand(); }
inline const process_expression& unary_operand(const allow& x)       { return x.operand(); }
inline const process_expression& unary_operand(const at& x)          { return x.operand(); }

inline const process_expression& binary_left(const sync& x)          { return x.left(); }
inline const process_expression& binary_right(const sync& x)         { return x.right(); }
inline const process_expression& binary_left(const seq& x)           { return x.left(); }
inline const process_expression& binary_right(const seq& x)          { return x.right(); }
inline const process_expression& binary_left(const bounded_init& x)  { return x.left(); }
inline const process_expression& binary_right(const bounded_init& x) { return x.right(); }
inline const process_expression& binary_left(const choice& x)        { return x.left(); }
inline const process_expression& binary_right(const choice& x)       { return x.right(); }
inline const process_expression& binary_left(const merge& x)         { return x.left(); }
inline const process_expression& binary_right(const merge& x)        { return x.right(); }
inline const process_expression& binary_left(const left_merge& x)    { return x.left(); }
inline const process_expression& binary_right(const left_merge& x)   { return x.right(); }

// template function overloads
std::string pp(const process_expression& x);
std::string pp(const process_expression_list& x);
std::string pp(const process_expression_vector& x);
std::string pp(const process_instance& x);
std::string pp(const process_instance_assignment& x);
std::string pp(const delta& x);
std::string pp(const tau& x);
std::string pp(const sum& x);
std::string pp(const block& x);
std::string pp(const hide& x);
std::string pp(const rename& x);
std::string pp(const comm& x);
std::string pp(const allow& x);
std::string pp(const sync& x);
std::string pp(const at& x);
std::string pp(const seq& x);
std::string pp(const if_then& x);
std::string pp(const if_then_else& x);
std::string pp(const bounded_init& x);
std::string pp(const merge& x);
std::string pp(const left_merge& x);
std::string pp(const choice& x);
std::string pp(const process::untyped_process_assignment& x);
std::string pp(const process::untyped_parameter_identifier& x);
std::set<data::sort_expression> find_sort_expressions(const process::process_expression& x);

// TODO: These should be removed when the aterm code has been replaced.
std::string pp(const atermpp::aterm& x);
std::string pp(const atermpp::aterm_appl& x);

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_H
