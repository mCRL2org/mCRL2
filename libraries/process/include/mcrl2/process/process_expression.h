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
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/precedence.h"
#include "mcrl2/data/untyped_data_parameter.h"
#include "mcrl2/process/action_label.h"
#include "mcrl2/process/process_identifier.h"
#include "mcrl2/process/rename_expression.h"
#include "mcrl2/process/communication_expression.h"

namespace mcrl2
{

namespace process
{

// Needed for argument dependent lookup (?)
using namespace core::detail::precedences;

//--- start generated classes ---//
/// \brief A process expression
class process_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    process_expression()
      : atermpp::aterm_appl(core::detail::default_values::ProcExpr)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit process_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_ProcExpr(*this));
    }

    /// \brief Constructor.
    process_expression(const data::untyped_data_parameter& x)
      : atermpp::aterm_appl(x)
    {}
};

/// \brief list of process_expressions
typedef atermpp::term_list<process_expression> process_expression_list;

/// \brief vector of process_expressions
typedef std::vector<process_expression>    process_expression_vector;

// prototypes
inline bool is_action(const atermpp::aterm_appl& x);
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
inline bool is_stochastic_operator(const atermpp::aterm_appl& x);
inline bool is_untyped_process_assignment(const atermpp::aterm_appl& x);

/// \brief Test for a process_expression expression
/// \param x A term
/// \return True if \a x is a process_expression expression
inline
bool is_process_expression(const atermpp::aterm_appl& x)
{
  return process::is_action(x) ||
         process::is_process_instance(x) ||
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
         process::is_stochastic_operator(x) ||
         process::is_untyped_process_assignment(x) ||
         data::is_untyped_data_parameter(x);
}

// prototype declaration
std::string pp(const process_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const process_expression& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(process_expression& t1, process_expression& t2)
{
  t1.swap(t2);
}


/// \brief An action
class action: public process_expression
{
  public:
    /// \brief Default constructor.
    action()
      : process_expression(core::detail::default_values::Action)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit action(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Action(*this));
    }

    /// \brief Constructor.
    action(const action_label& label, const data::data_expression_list& arguments)
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Action(), label, arguments))
    {}

    const action_label& label() const
    {
      return atermpp::down_cast<action_label>((*this)[0]);
    }

    const data::data_expression_list& arguments() const
    {
      return atermpp::down_cast<data::data_expression_list>((*this)[1]);
    }
};

/// \brief list of actions
typedef atermpp::term_list<action> action_list;

/// \brief vector of actions
typedef std::vector<action>    action_vector;

/// \brief Test for a action expression
/// \param x A term
/// \return True if \a x is a action expression
inline
bool is_action(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Action;
}

// prototype declaration
std::string pp(const action& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(action& t1, action& t2)
{
  t1.swap(t2);
}


/// \brief A process
class process_instance: public process_expression
{
  public:
    /// \brief Default constructor.
    process_instance()
      : process_expression(core::detail::default_values::Process)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Process(), identifier, actual_parameters))
    {}

    const process_identifier& identifier() const
    {
      return atermpp::down_cast<process_identifier>((*this)[0]);
    }

    const data::data_expression_list& actual_parameters() const
    {
      return atermpp::down_cast<data::data_expression_list>((*this)[1]);
    }
};

/// \brief Test for a process_instance expression
/// \param x A term
/// \return True if \a x is a process_instance expression
inline
bool is_process_instance(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Process;
}

// prototype declaration
std::string pp(const process_instance& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const process_instance& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::ProcessAssignment)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_ProcessAssignment(), identifier, assignments))
    {}

    const process_identifier& identifier() const
    {
      return atermpp::down_cast<process_identifier>((*this)[0]);
    }

    const data::assignment_list& assignments() const
    {
      return atermpp::down_cast<data::assignment_list>((*this)[1]);
    }
};

/// \brief Test for a process_instance_assignment expression
/// \param x A term
/// \return True if \a x is a process_instance_assignment expression
inline
bool is_process_instance_assignment(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::ProcessAssignment;
}

// prototype declaration
std::string pp(const process_instance_assignment& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const process_instance_assignment& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Delta)
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
  return x.function() == core::detail::function_symbols::Delta;
}

// prototype declaration
std::string pp(const delta& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const delta& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Tau)
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
  return x.function() == core::detail::function_symbols::Tau;
}

// prototype declaration
std::string pp(const tau& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const tau& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Sum)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit sum(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_Sum(*this));
    }

    /// \brief Constructor.
    sum(const data::variable_list& variables, const process_expression& operand)
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Sum(), variables, operand))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a sum expression
/// \param x A term
/// \return True if \a x is a sum expression
inline
bool is_sum(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Sum;
}

// prototype declaration
std::string pp(const sum& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const sum& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Block)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Block(), block_set, operand))
    {}

    const core::identifier_string_list& block_set() const
    {
      return atermpp::down_cast<core::identifier_string_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a block expression
/// \param x A term
/// \return True if \a x is a block expression
inline
bool is_block(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Block;
}

// prototype declaration
std::string pp(const block& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const block& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Hide)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Hide(), hide_set, operand))
    {}

    const core::identifier_string_list& hide_set() const
    {
      return atermpp::down_cast<core::identifier_string_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a hide expression
/// \param x A term
/// \return True if \a x is a hide expression
inline
bool is_hide(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Hide;
}

// prototype declaration
std::string pp(const hide& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const hide& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Rename)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Rename(), rename_set, operand))
    {}

    const rename_expression_list& rename_set() const
    {
      return atermpp::down_cast<rename_expression_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a rename expression
/// \param x A term
/// \return True if \a x is a rename expression
inline
bool is_rename(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Rename;
}

// prototype declaration
std::string pp(const rename& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const rename& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Comm)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Comm(), comm_set, operand))
    {}

    const communication_expression_list& comm_set() const
    {
      return atermpp::down_cast<communication_expression_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a comm expression
/// \param x A term
/// \return True if \a x is a comm expression
inline
bool is_comm(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Comm;
}

// prototype declaration
std::string pp(const comm& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const comm& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Allow)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Allow(), allow_set, operand))
    {}

    const action_name_multiset_list& allow_set() const
    {
      return atermpp::down_cast<action_name_multiset_list>((*this)[0]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a allow expression
/// \param x A term
/// \return True if \a x is a allow expression
inline
bool is_allow(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Allow;
}

// prototype declaration
std::string pp(const allow& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const allow& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Sync)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Sync(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a sync expression
/// \param x A term
/// \return True if \a x is a sync expression
inline
bool is_sync(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Sync;
}

// prototype declaration
std::string pp(const sync& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const sync& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::AtTime)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_AtTime(), operand, time_stamp))
    {}

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const data::data_expression& time_stamp() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[1]);
    }
};

/// \brief Test for a at expression
/// \param x A term
/// \return True if \a x is a at expression
inline
bool is_at(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::AtTime;
}

// prototype declaration
std::string pp(const at& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const at& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Seq)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Seq(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a seq expression
/// \param x A term
/// \return True if \a x is a seq expression
inline
bool is_seq(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Seq;
}

// prototype declaration
std::string pp(const seq& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const seq& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::IfThen)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_IfThen(), condition, then_case))
    {}

    const data::data_expression& condition() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }

    const process_expression& then_case() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a if_then expression
/// \param x A term
/// \return True if \a x is a if_then expression
inline
bool is_if_then(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::IfThen;
}

// prototype declaration
std::string pp(const if_then& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const if_then& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::IfThenElse)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_IfThenElse(), condition, then_case, else_case))
    {}

    const data::data_expression& condition() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[0]);
    }

    const process_expression& then_case() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }

    const process_expression& else_case() const
    {
      return atermpp::down_cast<process_expression>((*this)[2]);
    }
};

/// \brief Test for a if_then_else expression
/// \param x A term
/// \return True if \a x is a if_then_else expression
inline
bool is_if_then_else(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::IfThenElse;
}

// prototype declaration
std::string pp(const if_then_else& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const if_then_else& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::BInit)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_BInit(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a bounded_init expression
/// \param x A term
/// \return True if \a x is a bounded_init expression
inline
bool is_bounded_init(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::BInit;
}

// prototype declaration
std::string pp(const bounded_init& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const bounded_init& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Merge)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Merge(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a merge expression
/// \param x A term
/// \return True if \a x is a merge expression
inline
bool is_merge(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Merge;
}

// prototype declaration
std::string pp(const merge& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const merge& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::LMerge)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_LMerge(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a left_merge expression
/// \param x A term
/// \return True if \a x is a left_merge expression
inline
bool is_left_merge(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::LMerge;
}

// prototype declaration
std::string pp(const left_merge& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const left_merge& x)
{
  return out << process::pp(x);
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
      : process_expression(core::detail::default_values::Choice)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_Choice(), left, right))
    {}

    const process_expression& left() const
    {
      return atermpp::down_cast<process_expression>((*this)[0]);
    }

    const process_expression& right() const
    {
      return atermpp::down_cast<process_expression>((*this)[1]);
    }
};

/// \brief Test for a choice expression
/// \param x A term
/// \return True if \a x is a choice expression
inline
bool is_choice(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::Choice;
}

// prototype declaration
std::string pp(const choice& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const choice& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(choice& t1, choice& t2)
{
  t1.swap(t2);
}


/// \brief The distribution operator
class stochastic_operator: public process_expression
{
  public:
    /// \brief Default constructor.
    stochastic_operator()
      : process_expression(core::detail::default_values::StochasticOperator)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit stochastic_operator(const atermpp::aterm& term)
      : process_expression(term)
    {
      assert(core::detail::check_term_StochasticOperator(*this));
    }

    /// \brief Constructor.
    stochastic_operator(const data::variable_list& variables, const data::data_expression& distribution, const process_expression& operand)
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_StochasticOperator(), variables, distribution, operand))
    {}

    const data::variable_list& variables() const
    {
      return atermpp::down_cast<data::variable_list>((*this)[0]);
    }

    const data::data_expression& distribution() const
    {
      return atermpp::down_cast<data::data_expression>((*this)[1]);
    }

    const process_expression& operand() const
    {
      return atermpp::down_cast<process_expression>((*this)[2]);
    }
};

/// \brief Test for a stochastic_operator expression
/// \param x A term
/// \return True if \a x is a stochastic_operator expression
inline
bool is_stochastic_operator(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::StochasticOperator;
}

// prototype declaration
std::string pp(const stochastic_operator& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const stochastic_operator& x)
{
  return out << process::pp(x);
}

/// \brief swap overload
inline void swap(stochastic_operator& t1, stochastic_operator& t2)
{
  t1.swap(t2);
}


/// \brief An untyped process assginment
class untyped_process_assignment: public process_expression
{
  public:
    /// \brief Default constructor.
    untyped_process_assignment()
      : process_expression(core::detail::default_values::UntypedProcessAssignment)
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
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedProcessAssignment(), name, assignments))
    {}

    /// \brief Constructor.
    untyped_process_assignment(const std::string& name, const data::untyped_identifier_assignment_list& assignments)
      : process_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedProcessAssignment(), core::identifier_string(name), assignments))
    {}

    const core::identifier_string& name() const
    {
      return atermpp::down_cast<core::identifier_string>((*this)[0]);
    }

    const data::untyped_identifier_assignment_list& assignments() const
    {
      return atermpp::down_cast<data::untyped_identifier_assignment_list>((*this)[1]);
    }
};

/// \brief Test for a untyped_process_assignment expression
/// \param x A term
/// \return True if \a x is a untyped_process_assignment expression
inline
bool is_untyped_process_assignment(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedProcessAssignment;
}

// prototype declaration
std::string pp(const untyped_process_assignment& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_process_assignment& x)
{
  return out << process::pp(x);
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
inline int left_precedence(const choice&)              { return 1; }
inline int left_precedence(const sum&)                 { return 2; }
inline int left_precedence(const stochastic_operator&) { return 2; }
inline int left_precedence(const merge&)               { return 3; }
inline int left_precedence(const left_merge&)          { return 4; }
inline int left_precedence(const if_then&)             { return 5; }
inline int left_precedence(const if_then_else&)        { return 5; }
inline int left_precedence(const bounded_init&)        { return 6; }
inline int left_precedence(const seq&)                 { return 7; }
inline int left_precedence(const at&)                  { return 8; }
inline int left_precedence(const sync&)                { return 9; }
inline int left_precedence(const process_expression& x)
{
       if (is_choice(x))              { return left_precedence(static_cast<const choice&>(x)); }
  else if (is_sum(x))                 { return left_precedence(static_cast<const sum&>(x)); }
  else if (is_stochastic_operator(x)) { return left_precedence(static_cast<const stochastic_operator&>(x)); }
  else if (is_merge(x))               { return left_precedence(static_cast<const merge&>(x)); }
  else if (is_left_merge(x))          { return left_precedence(static_cast<const left_merge>(x)); }
  else if (is_if_then(x))             { return left_precedence(static_cast<const if_then&>(x)); }
  else if (is_if_then_else(x))        { return left_precedence(static_cast<const if_then_else&>(x)); }
  else if (is_bounded_init(x))        { return left_precedence(static_cast<const bounded_init&>(x)); }
  else if (is_seq(x))                 { return left_precedence(static_cast<const seq&>(x)); }
  else if (is_at(x))                  { return left_precedence(static_cast<const at&>(x)); }
  else if (is_sync(x))                { return left_precedence(static_cast<const sync&>(x)); }
  return core::detail::precedences::max_precedence;
}

inline int right_precedence(const process_expression& x) { return left_precedence(x); }

inline const process_expression& unary_operand(const sum& x)                 { return x.operand(); }
inline const process_expression& unary_operand(const stochastic_operator& x) { return x.operand(); }
inline const process_expression& unary_operand(const block& x)               { return x.operand(); }
inline const process_expression& unary_operand(const hide& x)                { return x.operand(); }
inline const process_expression& unary_operand(const rename& x)              { return x.operand(); }
inline const process_expression& unary_operand(const comm& x)                { return x.operand(); }
inline const process_expression& unary_operand(const allow& x)               { return x.operand(); }
inline const process_expression& unary_operand(const at& x)                  { return x.operand(); }

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
std::string pp(const process_expression_list& x);
std::string pp(const process_expression_vector& x);
std::set<data::sort_expression> find_sort_expressions(const process::process_expression& x);
std::string pp(const action_list& x);
std::string pp(const action_vector& x);
// action normalize_sorts(const action& x, const data::sort_specification& sortspec);
action translate_user_notation(const action& x);
std::set<data::variable> find_all_variables(const action& x);
std::set<data::variable> find_free_variables(const action& x);

/// \brief Compares the signatures of two actions
/// \param a An action
/// \param b An action
/// \return Returns true if the actions a and b have the same label, and
/// the sorts of the arguments of a and b are equal.
inline
bool equal_signatures(const action& a, const action& b)
{
  if (a.label() != b.label())
  {
    return false;
  }

  const data::data_expression_list& a_args = a.arguments();
  const data::data_expression_list& b_args = b.arguments();

  if (a_args.size() != b_args.size())
  {
    return false;
  }

  return std::equal(a_args.begin(), a_args.end(), b_args.begin(), mcrl2::data::detail::equal_data_expression_sort());
}

/// \brief Represents the name of a multi action
typedef std::multiset<core::identifier_string> multi_action_name;

/// \brief Represents a set of multi action names
typedef std::set<multi_action_name> multi_action_name_set;

/// \brief Represents a set of action names
typedef std::set<core::identifier_string> action_name_set;

/// \brief Pretty print function for a multi action name
inline
std::string pp(const multi_action_name& x)
{
  std::ostringstream out;
  if (x.empty())
  {
    out << "tau";
  }
  else
  {
    for (auto i = x.begin(); i != x.end(); ++i)
    {
      if (i != x.begin())
      {
        out << " | ";
      }
      out << core::pp(*i);
    }
  }
  return out.str();
}

/// \brief Pretty print function for a set of multi action names
inline
std::string pp(const multi_action_name_set& A)
{
  std::ostringstream out;
  out << "{";
  for (auto i = A.begin(); i != A.end(); ++i)
  {
    if (i != A.begin())
    {
      out << ", ";
    }
    out << pp(*i);
  }
  out << "}";
  return out.str();
}

} // namespace process

} // namespace mcrl2

namespace std
{

/// \brief Standard has function for actions.
template <>
struct hash<mcrl2::process::action>
{
  std::size_t operator()(const mcrl2::process::action& t) const
  {
    return std::hash<atermpp::aterm>()(t);
  }

};


} // namespace std;

#endif // MCRL2_PROCESS_PROCESS_EXPRESSION_H
