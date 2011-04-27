// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/nextstate/standard.h

#ifndef _NEXTSTATE_STANDARD_H
#define _NEXTSTATE_STANDARD_H

#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/lps/specification.h"

class NextState;

// Uses static context so code is not reentrant
struct legacy_selector
{
  static atermpp::aterm& term()
  {
    static atermpp::aterm term = mcrl2::data::sort_bool::true_();

    return term;
  }

  /// \brief returns true if and only if the argument is equal to true
  template < typename ExpressionType >
  static bool test(ExpressionType const& e)
  {
    return e == term();
  }
};

namespace mcrl2
{
namespace data
{

// Assumes that all terms are in internal rewrite format.
/* template <>
struct expression_traits< atermpp::aterm >
{
  static mcrl2::data::detail::legacy_rewriter& get_rewriter()
  {
    static mcrl2::data::detail::legacy_rewriter local_rewriter = mcrl2::data::detail::legacy_rewriter(mcrl2::data::rewriter());

    return local_rewriter;
  }

  static atermpp::aterm false_()
  {
    return get_rewriter().convert_to(mcrl2::data::data_expression(mcrl2::data::sort_bool::false_()));
  }

  static atermpp::aterm true_()
  {
    return get_rewriter().convert_to(mcrl2::data::data_expression(mcrl2::data::sort_bool::true_()));
  }
}; */
}
} 


struct ns_info
{
  NextState* parent;

  // Uses terms in internal format... *Sigh*
  typedef mcrl2::data::classic_enumerator< mcrl2::data::detail::legacy_rewriter > enumerator_type;


  const mcrl2::data::data_specification &m_specification;
  // Storing the legacy rewriter below by reference can lead to problems.
  const mcrl2::data::detail::legacy_rewriter m_rewriter; // only for translation to/from rewrite format
  enumerator_type m_enumerator;

  size_t num_summands;
  ATermAppl* summands;
  size_t num_prioritised;
  ATermList procvars;
  int stateformat;
  ATermAppl nil;
  AFun pairAFun;
  size_t statelen;
  AFun stateAFun;
  size_t* current_id;

  enumerator_type::iterator_internal get_sols(ATermList v, const ATerm c)
  {
    // const enumerator_type m(m_specification,mcrl2::data::variable_list(v),m_rewriter,c);

    return m_enumerator.begin_internal(mcrl2::data::variable_list(v),(ATermAppl)c); // Laatste expressie is intern.
  }

  ATermAppl export_term(ATerm term) const
  {
    return m_rewriter.convert_from(term);
  }

  ATerm import_term(ATermAppl term) const
  {
    return m_rewriter.convert_to(mcrl2::data::data_expression(term));
  }

  ns_info(const mcrl2::data::data_specification & specification,
          const mcrl2::data::detail::legacy_rewriter & rewriter) :
    m_specification(specification),
    m_rewriter(rewriter),
    m_enumerator(specification,rewriter)
  {
    // Configure selector to compare with term that represents false
    legacy_selector::term() = m_rewriter.convert_to(mcrl2::data::data_expression(mcrl2::data::sort_bool::true_()));
  }

};
/// \endcond

class NextStateGenerator // : public NextStateGenerator
{
  public:
    NextStateGenerator(ATerm State, ns_info& Info, size_t identifier, bool SingleSummand = false, size_t SingleSummandIndex = 0);
    ~NextStateGenerator();

    bool next(ATermAppl* Transition, ATerm* State, bool* prioritised = NULL);

    // bool errorOccurred();

    void reset(ATerm State, size_t SummandIndex = 0);

    ATerm get_state() const;

  private:
    ns_info info;
    size_t id;

    bool single_summand;

    size_t sum_idx;

    ATerm cur_state;
    ATerm cur_act;
    ATermList cur_nextstate;

    ATerm* stateargs;

    ns_info::enumerator_type::iterator_internal valuations;

    void set_substitutions();

    void SetTreeStateVars(ATerm tree, ATermList* vars);
    ATermAppl rewrActionArgs(ATermAppl act);
    ATerm makeNewState(ATerm old, ATermList assigns);
    ATermList ListFromFormat(ATermList l);
};

class NextState // : public NextState
{
    friend class NextStateGenerator;
  public:
    NextState(mcrl2::lps::specification const& spec, bool allow_free_vars, int state_format, const mcrl2::data::detail::legacy_rewriter& e);
    ~NextState();

    void prioritise(const char* action);

    ATerm getInitialState();
    NextStateGenerator* getNextStates(ATerm state, NextStateGenerator* old = NULL);
    NextStateGenerator* getNextStates(ATerm state, size_t group, NextStateGenerator* old = NULL);

    void gatherGroupInformation();

    size_t getGroupCount() const;
    size_t getStateLength();
    ATermAppl getStateArgument(ATerm state, size_t index);
    ATermAppl makeStateVector(ATerm state);
    ATerm parseStateVector(ATermAppl state, ATerm match = NULL);
    mcrl2::data::rewriter& getRewriter()   // Deprecated. Do not use.
    {
      return const_cast< mcrl2::data::detail::legacy_rewriter& >(info.m_rewriter);
    }

  private:
    ns_info info;
    size_t next_id;
    size_t current_id;

    bool stateAFun_made;

    bool usedummies;

    AFun smndAFun;
    bool* tree_init;
    ATerm* stateargs;

    ATermList pars;
    ATerm initial_state;

    ATerm buildTree(ATerm* args);
    ATerm getTreeElement(ATerm tree, size_t index);

    ATerm SetVars(ATerm a, ATermList free_vars);
    ATermList ListToFormat(ATermList l,ATermList free_vars);
    ATermList ListFromFormat(ATermList l);
    ATermAppl ActionToRewriteFormat(ATermAppl act, ATermList free_vars);
    ATermList AssignsToRewriteFormat(ATermList assigns, ATermList free_vars);
};


NextState* createNextState(
     mcrl2::lps::specification const& spec,
     const mcrl2::data::rewriter &rewriter,
     bool allow_free_vars,
     int state_format = GS_STATE_VECTOR,
     NextStateStrategy strategy = nsStandard
   ); 


#endif
