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
#include "mcrl2/fdr/detail/syntax_checks.h"

namespace mcrl2 {

namespace fdr {

//--- start generated classes ---//
/// \brief A common expression
class common
{
  public:
    /// \brief Default constructor.
    common()
      : atermpp::aterm_appl(fdr::detail::constructCommon())
    {}

    /// \brief Constructor.
    /// \param term A term
    common(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Common(m_term));
    }

    /// \brief Constructor.
    common(const common_expression& operand)
      : atermpp::aterm_appl(fdr::detail::gsMakeCommon(operand))
    {}

    common_expression operand() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of commons
    typedef atermpp::term_list<common> common_list;

    /// \brief vector of commons
    typedef atermpp::vector<common>    common_vector;


/// \brief A stop
class stop
{
  public:
    /// \brief Default constructor.
    stop()
      : atermpp::aterm_appl(fdr::detail::constructSTOP())
    {}

    /// \brief Constructor.
    /// \param term A term
    stop(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_STOP(m_term));
    }
};/// \brief list of stops
    typedef atermpp::term_list<stop> stop_list;

    /// \brief vector of stops
    typedef atermpp::vector<stop>    stop_vector;


/// \brief A skip
class skip
{
  public:
    /// \brief Default constructor.
    skip()
      : atermpp::aterm_appl(fdr::detail::constructSKIP())
    {}

    /// \brief Constructor.
    /// \param term A term
    skip(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SKIP(m_term));
    }
};/// \brief list of skips
    typedef atermpp::term_list<skip> skip_list;

    /// \brief vector of skips
    typedef atermpp::vector<skip>    skip_vector;


/// \brief A chaos
class chaos
{
  public:
    /// \brief Default constructor.
    chaos()
      : atermpp::aterm_appl(fdr::detail::constructCHAOS())
    {}

    /// \brief Constructor.
    /// \param term A term
    chaos(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_CHAOS(m_term));
    }

    /// \brief Constructor.
    chaos(const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeCHAOS(set))
    {}

    set_expression set() const
    {
      return atermpp::arg1(*this);
    }
};/// \brief list of chaoss
    typedef atermpp::term_list<chaos> chaos_list;

    /// \brief vector of chaoss
    typedef atermpp::vector<chaos>    chaos_vector;


/// \brief A prefix
class prefix
{
  public:
    /// \brief Default constructor.
    prefix()
      : atermpp::aterm_appl(fdr::detail::constructPrefix())
    {}

    /// \brief Constructor.
    /// \param term A term
    prefix(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Prefix(m_term));
    }

    /// \brief Constructor.
    prefix(const dotted_expression& dotted, const field_list& fields, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakePrefix(dotted, fields, process))
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
};/// \brief list of prefixs
    typedef atermpp::term_list<prefix> prefix_list;

    /// \brief vector of prefixs
    typedef atermpp::vector<prefix>    prefix_vector;


/// \brief An external choice
class externalchoice
{
  public:
    /// \brief Default constructor.
    externalchoice()
      : atermpp::aterm_appl(fdr::detail::constructExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    externalchoice(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_ExternalChoice(m_term));
    }

    /// \brief Constructor.
    externalchoice(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeExternalChoice(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of externalchoices
    typedef atermpp::term_list<externalchoice> externalchoice_list;

    /// \brief vector of externalchoices
    typedef atermpp::vector<externalchoice>    externalchoice_vector;


/// \brief An internal choice
class internalchoice
{
  public:
    /// \brief Default constructor.
    internalchoice()
      : atermpp::aterm_appl(fdr::detail::constructInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    internalchoice(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_InternalChoice(m_term));
    }

    /// \brief Constructor.
    internalchoice(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeInternalChoice(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of internalchoices
    typedef atermpp::term_list<internalchoice> internalchoice_list;

    /// \brief vector of internalchoices
    typedef atermpp::vector<internalchoice>    internalchoice_vector;


/// \brief A sequential composition
class sequentialcomposition
{
  public:
    /// \brief Default constructor.
    sequentialcomposition()
      : atermpp::aterm_appl(fdr::detail::constructSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    sequentialcomposition(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_SequentialComposition(m_term));
    }

    /// \brief Constructor.
    sequentialcomposition(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeSequentialComposition(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of sequentialcompositions
    typedef atermpp::term_list<sequentialcomposition> sequentialcomposition_list;

    /// \brief vector of sequentialcompositions
    typedef atermpp::vector<sequentialcomposition>    sequentialcomposition_vector;


/// \brief An interrupt
class interrupt
{
  public:
    /// \brief Default constructor.
    interrupt()
      : atermpp::aterm_appl(fdr::detail::constructInterrupt())
    {}

    /// \brief Constructor.
    /// \param term A term
    interrupt(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Interrupt(m_term));
    }

    /// \brief Constructor.
    interrupt(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeInterrupt(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of interrupts
    typedef atermpp::term_list<interrupt> interrupt_list;

    /// \brief vector of interrupts
    typedef atermpp::vector<interrupt>    interrupt_vector;


/// \brief An hiding
class hiding
{
  public:
    /// \brief Default constructor.
    hiding()
      : atermpp::aterm_appl(fdr::detail::constructHiding())
    {}

    /// \brief Constructor.
    /// \param term A term
    hiding(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Hiding(m_term));
    }

    /// \brief Constructor.
    hiding(const process& process, const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeHiding(process, set))
    {}

    process process() const
    {
      return atermpp::arg1(*this);
    }

    set_expression set() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of hidings
    typedef atermpp::term_list<hiding> hiding_list;

    /// \brief vector of hidings
    typedef atermpp::vector<hiding>    hiding_vector;


/// \brief A renaming
class rename
{
  public:
    /// \brief Default constructor.
    rename()
      : atermpp::aterm_appl(fdr::detail::constructRename())
    {}

    /// \brief Constructor.
    /// \param term A term
    rename(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Rename(m_term));
    }

    /// \brief Constructor.
    rename(const process& process, const renaming& renaming)
      : atermpp::aterm_appl(fdr::detail::gsMakeRename(process, renaming))
    {}

    process process() const
    {
      return atermpp::arg1(*this);
    }

    renaming renaming() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of renames
    typedef atermpp::term_list<rename> rename_list;

    /// \brief vector of renames
    typedef atermpp::vector<rename>    rename_vector;


/// \brief An interleave
class interleave
{
  public:
    /// \brief Default constructor.
    interleave()
      : atermpp::aterm_appl(fdr::detail::constructInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    interleave(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Interleave(m_term));
    }

    /// \brief Constructor.
    interleave(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeInterleave(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of interleaves
    typedef atermpp::term_list<interleave> interleave_list;

    /// \brief vector of interleaves
    typedef atermpp::vector<interleave>    interleave_vector;


/// \brief A sharing
class sharing
{
  public:
    /// \brief Default constructor.
    sharing()
      : atermpp::aterm_appl(fdr::detail::constructSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    sharing(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_Sharing(m_term));
    }

    /// \brief Constructor.
    sharing(const process& left, const process& right, const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeSharing(left, right, set))
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
};/// \brief list of sharings
    typedef atermpp::term_list<sharing> sharing_list;

    /// \brief vector of sharings
    typedef atermpp::vector<sharing>    sharing_vector;


/// \brief An alpha parallel
class alphaparallel
{
  public:
    /// \brief Default constructor.
    alphaparallel()
      : atermpp::aterm_appl(fdr::detail::constructAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    alphaparallel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_AlphaParallel(m_term));
    }

    /// \brief Constructor.
    alphaparallel(const process& left, const process& right, const set_expression& left_set, const set_expression& right_set)
      : atermpp::aterm_appl(fdr::detail::gsMakeAlphaParallel(left, right, left_set, right_set))
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
};/// \brief list of alphaparallels
    typedef atermpp::term_list<alphaparallel> alphaparallel_list;

    /// \brief vector of alphaparallels
    typedef atermpp::vector<alphaparallel>    alphaparallel_vector;


/// \brief A replicated external choice
class repexternalchoice
{
  public:
    /// \brief Default constructor.
    repexternalchoice()
      : atermpp::aterm_appl(fdr::detail::constructRepExternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repexternalchoice(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepExternalChoice(m_term));
    }

    /// \brief Constructor.
    repexternalchoice(const generator_list& gens, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepExternalChoice(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of repexternalchoices
    typedef atermpp::term_list<repexternalchoice> repexternalchoice_list;

    /// \brief vector of repexternalchoices
    typedef atermpp::vector<repexternalchoice>    repexternalchoice_vector;


/// \brief A replicated internal choice
class repinternalchoice
{
  public:
    /// \brief Default constructor.
    repinternalchoice()
      : atermpp::aterm_appl(fdr::detail::constructRepInternalChoice())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinternalchoice(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepInternalChoice(m_term));
    }

    /// \brief Constructor.
    repinternalchoice(const generator_list& gens, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepInternalChoice(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of repinternalchoices
    typedef atermpp::term_list<repinternalchoice> repinternalchoice_list;

    /// \brief vector of repinternalchoices
    typedef atermpp::vector<repinternalchoice>    repinternalchoice_vector;


/// \brief A replicated sequential composition
class repsequentialcomposition
{
  public:
    /// \brief Default constructor.
    repsequentialcomposition()
      : atermpp::aterm_appl(fdr::detail::constructRepSequentialComposition())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsequentialcomposition(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepSequentialComposition(m_term));
    }

    /// \brief Constructor.
    repsequentialcomposition(const generator_list& gens, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepSequentialComposition(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of repsequentialcompositions
    typedef atermpp::term_list<repsequentialcomposition> repsequentialcomposition_list;

    /// \brief vector of repsequentialcompositions
    typedef atermpp::vector<repsequentialcomposition>    repsequentialcomposition_vector;


/// \brief A replicated interleave
class repinterleave
{
  public:
    /// \brief Default constructor.
    repinterleave()
      : atermpp::aterm_appl(fdr::detail::constructRepInterleave())
    {}

    /// \brief Constructor.
    /// \param term A term
    repinterleave(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepInterleave(m_term));
    }

    /// \brief Constructor.
    repinterleave(const generator_list& gens, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepInterleave(gens, process))
    {}

    generator_list gens() const
    {
      return atermpp::list_arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of repinterleaves
    typedef atermpp::term_list<repinterleave> repinterleave_list;

    /// \brief vector of repinterleaves
    typedef atermpp::vector<repinterleave>    repinterleave_vector;


/// \brief A replicated sharing
class repsharing
{
  public:
    /// \brief Default constructor.
    repsharing()
      : atermpp::aterm_appl(fdr::detail::constructRepSharing())
    {}

    /// \brief Constructor.
    /// \param term A term
    repsharing(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepSharing(m_term));
    }

    /// \brief Constructor.
    repsharing(const generator_list& gens, const process& process, const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepSharing(gens, process, set))
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
};/// \brief list of repsharings
    typedef atermpp::term_list<repsharing> repsharing_list;

    /// \brief vector of repsharings
    typedef atermpp::vector<repsharing>    repsharing_vector;


/// \brief A replicated alpha parallel
class repalphaparallel
{
  public:
    /// \brief Default constructor.
    repalphaparallel()
      : atermpp::aterm_appl(fdr::detail::constructRepAlphaParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    repalphaparallel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepAlphaParallel(m_term));
    }

    /// \brief Constructor.
    repalphaparallel(const generator_list& gens, const process& process, const set_expression& set)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepAlphaParallel(gens, process, set))
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
};/// \brief list of repalphaparallels
    typedef atermpp::term_list<repalphaparallel> repalphaparallel_list;

    /// \brief vector of repalphaparallels
    typedef atermpp::vector<repalphaparallel>    repalphaparallel_vector;


/// \brief An untimed time-out
class untimedtimeout
{
  public:
    /// \brief Default constructor.
    untimedtimeout()
      : atermpp::aterm_appl(fdr::detail::constructUntimedTimeOut())
    {}

    /// \brief Constructor.
    /// \param term A term
    untimedtimeout(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_UntimedTimeOut(m_term));
    }

    /// \brief Constructor.
    untimedtimeout(const process& left, const process& right)
      : atermpp::aterm_appl(fdr::detail::gsMakeUntimedTimeOut(left, right))
    {}

    process left() const
    {
      return atermpp::arg1(*this);
    }

    process right() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of untimedtimeouts
    typedef atermpp::term_list<untimedtimeout> untimedtimeout_list;

    /// \brief vector of untimedtimeouts
    typedef atermpp::vector<untimedtimeout>    untimedtimeout_vector;


/// \brief A boolean guard
class boolguard
{
  public:
    /// \brief Default constructor.
    boolguard()
      : atermpp::aterm_appl(fdr::detail::constructBoolGuard())
    {}

    /// \brief Constructor.
    /// \param term A term
    boolguard(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_BoolGuard(m_term));
    }

    /// \brief Constructor.
    boolguard(const boolean_expression& guard, const process& process)
      : atermpp::aterm_appl(fdr::detail::gsMakeBoolGuard(guard, process))
    {}

    boolean_expression guard() const
    {
      return atermpp::arg1(*this);
    }

    process process() const
    {
      return atermpp::arg2(*this);
    }
};/// \brief list of boolguards
    typedef atermpp::term_list<boolguard> boolguard_list;

    /// \brief vector of boolguards
    typedef atermpp::vector<boolguard>    boolguard_vector;


/// \brief A linked parallel
class linkedparallel
{
  public:
    /// \brief Default constructor.
    linkedparallel()
      : atermpp::aterm_appl(fdr::detail::constructLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    linkedparallel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_LinkedParallel(m_term));
    }

    /// \brief Constructor.
    linkedparallel(const process& left, const process& right, const linkpar& linkpar)
      : atermpp::aterm_appl(fdr::detail::gsMakeLinkedParallel(left, right, linkpar))
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
};/// \brief list of linkedparallels
    typedef atermpp::term_list<linkedparallel> linkedparallel_list;

    /// \brief vector of linkedparallels
    typedef atermpp::vector<linkedparallel>    linkedparallel_vector;


/// \brief A replicated linked parallel
class replinkedparallel
{
  public:
    /// \brief Default constructor.
    replinkedparallel()
      : atermpp::aterm_appl(fdr::detail::constructRepLinkedParallel())
    {}

    /// \brief Constructor.
    /// \param term A term
    replinkedparallel(atermpp::aterm_appl term)
      : atermpp::aterm_appl(term)
    {
      assert(fdr::detail::check_term_RepLinkedParallel(m_term));
    }

    /// \brief Constructor.
    replinkedparallel(const generator_list& gens, const process& process, const linkpar& linkpar)
      : atermpp::aterm_appl(fdr::detail::gsMakeRepLinkedParallel(gens, process, linkpar))
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
};/// \brief list of replinkedparallels
    typedef atermpp::term_list<replinkedparallel> replinkedparallel_list;

    /// \brief vector of replinkedparallels
    typedef atermpp::vector<replinkedparallel>    replinkedparallel_vector;

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
