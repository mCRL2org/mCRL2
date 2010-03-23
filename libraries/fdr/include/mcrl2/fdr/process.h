// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process.h
/// \brief add your file description here.

#ifndef MCRL2_FDR_PROCESS_H
#define MCRL2_FDR_PROCESS_H

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/fdr/detail/term_functions.h"
#include "mcrl2/fdr/detail/constructors.h"

namespace mcrl2 {

namespace fdr {

  /// \brief Process
  class process: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      process()
        : atermpp::aterm_appl(fdr::detail::constructProcess())
      {}

      /// \brief Constructor.
      /// \param term A term
      process(atermpp::aterm_appl term)
        : atermpp::aterm_appl(term)
      {
        assert(fdr::detail::check_rule_Process(m_term));
      }
  };

//--- start generated classes ---//
/// \brief A common expression
class common: public process
{
  public:
    /// \brief Default constructor.
    common()
      : process(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : process(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A stop
class stop: public process
{
  public:
    /// \brief Default constructor.
    stop()
      : process(fdr::detail::constructSTOP())
    {}

    /// \brief Constructor.
    /// \param term A term
    stop(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_STOP(m_term));
    }
};

/// \brief A skip
class skip: public process
{
  public:
    /// \brief Default constructor.
    skip()
      : process(fdr::detail::constructSKIP())
    {}

    /// \brief Constructor.
    /// \param term A term
    skip(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_SKIP(m_term));
    }
};

/// \brief A chaos
class chaos: public process
{
  public:
    /// \brief Default constructor.
    chaos()
      : process(fdr::detail::constructCHAOS())
    {}

    /// \brief Constructor.
    /// \param term A term
    chaos(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_CHAOS(m_term));
    }

    /// \brief Constructor.
    chaos(const set_expression& set)
      : process(fdr::detail::gsMakeCHAOS(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};

/// \brief A prefix
class prefix: public process
{
  public:
    /// \brief Default constructor.
    prefix()
      : process(fdr::detail::constructPrefix())
    {}

    /// \brief Constructor.
    /// \param term A term
    prefix(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Prefix(m_term));
    }

    /// \brief Constructor.
    prefix(const dotted_expression& dotted, const field_list& fields, const process& process)
      : process(fdr::detail::gsMakePrefix(dotted, fields, process))
    {}

    dotted_expression dotted() const
    {
      return atermpp::arg1(*this);
    }

    field_list fields() const
    {
      return atermpp::list_arg2(*this);
    }

    process process() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief An external choice
class externalchoice: public process
{
  public:
    /// \brief Default constructor.
    externalchoice()
      : process(fdr::detail::constructExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    externalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_ExternalChoice(m_term));
    }

    /// \brief Constructor.
    externalchoice(const process& left, const process& right)
      : process(fdr::detail::gsMakeExternalChoice(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An internal choice
class internalchoice: public process
{
  public:
    /// \brief Default constructor.
    internalchoice()
      : process(fdr::detail::constructInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    internalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_InternalChoice(m_term));
    }

    /// \brief Constructor.
    internalchoice(const process& left, const process& right)
      : process(fdr::detail::gsMakeInternalChoice(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A sequential composition
class sequentialcomposition: public process
{
  public:
    /// \brief Default constructor.
    sequentialcomposition()
      : process(fdr::detail::constructSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    sequentialcomposition(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_SequentialComposition(m_term));
    }

    /// \brief Constructor.
    sequentialcomposition(const process& left, const process& right)
      : process(fdr::detail::gsMakeSequentialComposition(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An interrupt
class interrupt: public process
{
  public:
    /// \brief Default constructor.
    interrupt()
      : process(fdr::detail::constructInterrupt())
    {}

    /// \brief Constructor.
    /// \param term A term
    interrupt(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Interrupt(m_term));
    }

    /// \brief Constructor.
    interrupt(const process& left, const process& right)
      : process(fdr::detail::gsMakeInterrupt(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An hiding
class hiding: public process
{
  public:
    /// \brief Default constructor.
    hiding()
      : process(fdr::detail::constructHiding())
    {}

    /// \brief Constructor.
    /// \param term A term
    hiding(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Hiding(m_term));
    }

    /// \brief Constructor.
    hiding(const process& process, const set_expression& set)
      : process(fdr::detail::gsMakeHiding(process, set))
    {}

    process process() const
    {
      return atermpp::arg1(*this);
    }

    set_expression set() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A renaming
class rename: public process
{
  public:
    /// \brief Default constructor.
    rename()
      : process(fdr::detail::constructRename())
    {}

    /// \brief Constructor.
    /// \param term A term
    rename(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Rename(m_term));
    }

    /// \brief Constructor.
    rename(const process& process, const renaming& renaming)
      : process(fdr::detail::gsMakeRename(process, renaming))
    {}

    process process() const
    {
      return atermpp::arg1(*this);
    }

    renaming renaming() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief An interleave
class interleave: public process
{
  public:
    /// \brief Default constructor.
    interleave()
      : process(fdr::detail::constructInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    interleave(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Interleave(m_term));
    }

    /// \brief Constructor.
    interleave(const process& left, const process& right)
      : process(fdr::detail::gsMakeInterleave(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A sharing
class sharing: public process
{
  public:
    /// \brief Default constructor.
    sharing()
      : process(fdr::detail::constructSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    sharing(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_Sharing(m_term));
    }

    /// \brief Constructor.
    sharing(const process& left, const process& right, const set_expression& set)
      : process(fdr::detail::gsMakeSharing(left, right, set))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }

    set_expression set() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief An alpha parallel
class alphaparallel: public process
{
  public:
    /// \brief Default constructor.
    alphaparallel()
      : process(fdr::detail::constructAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    alphaparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_AlphaParallel(m_term));
    }

    /// \brief Constructor.
    alphaparallel(const process& left, const process& right, const set_expression& left_set, const set_expression& right_set)
      : process(fdr::detail::gsMakeAlphaParallel(left, right, left_set, right_set))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }

    set_expression left_set() const
    {
      return atermpp::arg3(*this);
    }

    set_expression right_set() const
    {
      return atermpp::arg4(*this);
    }
};

/// \brief A replicated external choice
class repexternalchoice: public process
{
  public:
    /// \brief Default constructor.
    repexternalchoice()
      : process(fdr::detail::constructRepExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repexternalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepExternalChoice(m_term));
    }

    /// \brief Constructor.
    repexternalchoice(const generator_list& gens, const process& process)
      : process(fdr::detail::gsMakeRepExternalChoice(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A replicated internal choice
class repinternalchoice: public process
{
  public:
    /// \brief Default constructor.
    repinternalchoice()
      : process(fdr::detail::constructRepInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinternalchoice(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepInternalChoice(m_term));
    }

    /// \brief Constructor.
    repinternalchoice(const generator_list& gens, const process& process)
      : process(fdr::detail::gsMakeRepInternalChoice(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A replicated sequential composition
class repsequentialcomposition: public process
{
  public:
    /// \brief Default constructor.
    repsequentialcomposition()
      : process(fdr::detail::constructRepSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsequentialcomposition(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepSequentialComposition(m_term));
    }

    /// \brief Constructor.
    repsequentialcomposition(const generator_list& gens, const process& process)
      : process(fdr::detail::gsMakeRepSequentialComposition(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A replicated interleave
class repinterleave: public process
{
  public:
    /// \brief Default constructor.
    repinterleave()
      : process(fdr::detail::constructRepInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinterleave(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepInterleave(m_term));
    }

    /// \brief Constructor.
    repinterleave(const generator_list& gens, const process& process)
      : process(fdr::detail::gsMakeRepInterleave(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A replicated sharing
class repsharing: public process
{
  public:
    /// \brief Default constructor.
    repsharing()
      : process(fdr::detail::constructRepSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsharing(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepSharing(m_term));
    }

    /// \brief Constructor.
    repsharing(const generator_list& gens, const process& process, const set_expression& set)
      : process(fdr::detail::gsMakeRepSharing(gens, process, set))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }

    set_expression set() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief A replicated alpha parallel
class repalphaparallel: public process
{
  public:
    /// \brief Default constructor.
    repalphaparallel()
      : process(fdr::detail::constructRepAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    repalphaparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepAlphaParallel(m_term));
    }

    /// \brief Constructor.
    repalphaparallel(const generator_list& gens, const process& process, const set_expression& set)
      : process(fdr::detail::gsMakeRepAlphaParallel(gens, process, set))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }

    set_expression set() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief An untimed time-out
class untimedtimeout: public process
{
  public:
    /// \brief Default constructor.
    untimedtimeout()
      : process(fdr::detail::constructUntimedTimeOut())
    {}

    /// \brief Constructor.
    /// \param term A term
    untimedtimeout(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_UntimedTimeOut(m_term));
    }

    /// \brief Constructor.
    untimedtimeout(const process& left, const process& right)
      : process(fdr::detail::gsMakeUntimedTimeOut(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A boolean guard
class boolguard: public process
{
  public:
    /// \brief Default constructor.
    boolguard()
      : process(fdr::detail::constructBoolGuard())
    {}

    /// \brief Constructor.
    /// \param term A term
    boolguard(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_BoolGuard(m_term));
    }

    /// \brief Constructor.
    boolguard(const boolean_expression& guard, const process& process)
      : process(fdr::detail::gsMakeBoolGuard(guard, process))
    {}

    boolean_expression guard() const
    {
      return atermpp::arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};

/// \brief A linked parallel
class linkedparallel: public process
{
  public:
    /// \brief Default constructor.
    linkedparallel()
      : process(fdr::detail::constructLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    linkedparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_LinkedParallel(m_term));
    }

    /// \brief Constructor.
    linkedparallel(const process& left, const process& right, const linkpar& linkpar)
      : process(fdr::detail::gsMakeLinkedParallel(left, right, linkpar))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }

    linkpar linkpar() const
    {
      return atermpp::arg3(*this);
    }
};

/// \brief A replicated linked parallel
class replinkedparallel: public process
{
  public:
    /// \brief Default constructor.
    replinkedparallel()
      : process(fdr::detail::constructRepLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    replinkedparallel(atermpp::aterm_appl term)
      : process(term)
    {
      assert(fdr::detail::check_term_RepLinkedParallel(m_term));
    }

    /// \brief Constructor.
    replinkedparallel(const generator_list& gens, const process& process, const linkpar& linkpar)
      : process(fdr::detail::gsMakeRepLinkedParallel(gens, process, linkpar))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }

    linkpar linkpar() const
    {
      return atermpp::arg3(*this);
    }
};
//--- end generated classes ---//

//--- start generated is-functions ---//

    /// \brief Test for a common expression
    /// \param t A term
    /// \return True if it is a common expression
    inline
    bool is_common(const process& t)
    {
      return fdr::detail::gsIsCommon(t);
    }

    /// \brief Test for a stop expression
    /// \param t A term
    /// \return True if it is a stop expression
    inline
    bool is_stop(const process& t)
    {
      return fdr::detail::gsIsSTOP(t);
    }

    /// \brief Test for a skip expression
    /// \param t A term
    /// \return True if it is a skip expression
    inline
    bool is_skip(const process& t)
    {
      return fdr::detail::gsIsSKIP(t);
    }

    /// \brief Test for a chaos expression
    /// \param t A term
    /// \return True if it is a chaos expression
    inline
    bool is_chaos(const process& t)
    {
      return fdr::detail::gsIsCHAOS(t);
    }

    /// \brief Test for a prefix expression
    /// \param t A term
    /// \return True if it is a prefix expression
    inline
    bool is_prefix(const process& t)
    {
      return fdr::detail::gsIsPrefix(t);
    }

    /// \brief Test for a externalchoice expression
    /// \param t A term
    /// \return True if it is a externalchoice expression
    inline
    bool is_externalchoice(const process& t)
    {
      return fdr::detail::gsIsExternalChoice(t);
    }

    /// \brief Test for a internalchoice expression
    /// \param t A term
    /// \return True if it is a internalchoice expression
    inline
    bool is_internalchoice(const process& t)
    {
      return fdr::detail::gsIsInternalChoice(t);
    }

    /// \brief Test for a sequentialcomposition expression
    /// \param t A term
    /// \return True if it is a sequentialcomposition expression
    inline
    bool is_sequentialcomposition(const process& t)
    {
      return fdr::detail::gsIsSequentialComposition(t);
    }

    /// \brief Test for a interrupt expression
    /// \param t A term
    /// \return True if it is a interrupt expression
    inline
    bool is_interrupt(const process& t)
    {
      return fdr::detail::gsIsInterrupt(t);
    }

    /// \brief Test for a hiding expression
    /// \param t A term
    /// \return True if it is a hiding expression
    inline
    bool is_hiding(const process& t)
    {
      return fdr::detail::gsIsHiding(t);
    }

    /// \brief Test for a rename expression
    /// \param t A term
    /// \return True if it is a rename expression
    inline
    bool is_rename(const process& t)
    {
      return fdr::detail::gsIsRename(t);
    }

    /// \brief Test for a interleave expression
    /// \param t A term
    /// \return True if it is a interleave expression
    inline
    bool is_interleave(const process& t)
    {
      return fdr::detail::gsIsInterleave(t);
    }

    /// \brief Test for a sharing expression
    /// \param t A term
    /// \return True if it is a sharing expression
    inline
    bool is_sharing(const process& t)
    {
      return fdr::detail::gsIsSharing(t);
    }

    /// \brief Test for a alphaparallel expression
    /// \param t A term
    /// \return True if it is a alphaparallel expression
    inline
    bool is_alphaparallel(const process& t)
    {
      return fdr::detail::gsIsAlphaParallel(t);
    }

    /// \brief Test for a repexternalchoice expression
    /// \param t A term
    /// \return True if it is a repexternalchoice expression
    inline
    bool is_repexternalchoice(const process& t)
    {
      return fdr::detail::gsIsRepExternalChoice(t);
    }

    /// \brief Test for a repinternalchoice expression
    /// \param t A term
    /// \return True if it is a repinternalchoice expression
    inline
    bool is_repinternalchoice(const process& t)
    {
      return fdr::detail::gsIsRepInternalChoice(t);
    }

    /// \brief Test for a repsequentialcomposition expression
    /// \param t A term
    /// \return True if it is a repsequentialcomposition expression
    inline
    bool is_repsequentialcomposition(const process& t)
    {
      return fdr::detail::gsIsRepSequentialComposition(t);
    }

    /// \brief Test for a repinterleave expression
    /// \param t A term
    /// \return True if it is a repinterleave expression
    inline
    bool is_repinterleave(const process& t)
    {
      return fdr::detail::gsIsRepInterleave(t);
    }

    /// \brief Test for a repsharing expression
    /// \param t A term
    /// \return True if it is a repsharing expression
    inline
    bool is_repsharing(const process& t)
    {
      return fdr::detail::gsIsRepSharing(t);
    }

    /// \brief Test for a repalphaparallel expression
    /// \param t A term
    /// \return True if it is a repalphaparallel expression
    inline
    bool is_repalphaparallel(const process& t)
    {
      return fdr::detail::gsIsRepAlphaParallel(t);
    }

    /// \brief Test for a untimedtimeout expression
    /// \param t A term
    /// \return True if it is a untimedtimeout expression
    inline
    bool is_untimedtimeout(const process& t)
    {
      return fdr::detail::gsIsUntimedTimeOut(t);
    }

    /// \brief Test for a boolguard expression
    /// \param t A term
    /// \return True if it is a boolguard expression
    inline
    bool is_boolguard(const process& t)
    {
      return fdr::detail::gsIsBoolGuard(t);
    }

    /// \brief Test for a linkedparallel expression
    /// \param t A term
    /// \return True if it is a linkedparallel expression
    inline
    bool is_linkedparallel(const process& t)
    {
      return fdr::detail::gsIsLinkedParallel(t);
    }

    /// \brief Test for a replinkedparallel expression
    /// \param t A term
    /// \return True if it is a replinkedparallel expression
    inline
    bool is_replinkedparallel(const process& t)
    {
      return fdr::detail::gsIsRepLinkedParallel(t);
    }
//--- end generated is-functions ---//

} // namespace fdr

} // namespace mcrl2

#endif // MCRL2_FDR_PROCESS_H
