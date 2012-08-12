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

#include "mcrl2/data/detail/rewriter_wrapper.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/nextstate/nextstate_options.h"

namespace mcrl2
{
namespace lps
{

class NextState;

struct ns_info
{
  NextState* parent;

  // Uses terms in internal format... *Sigh*
  typedef mcrl2::data::classic_enumerator< mcrl2::data::detail::legacy_rewriter > enumerator_type;
  typedef enumerator_type::substitution_type substitution_type;
  typedef enumerator_type::internal_substitution_type internal_substitution_type;

  const mcrl2::data::data_specification &m_specification;
  // Storing the legacy rewriter below by reference can lead to problems.
  const mcrl2::data::detail::legacy_rewriter m_rewriter; // only for translation to/from rewrite format
  enumerator_type m_enumerator;

  // size_t num_summands;
  std::vector<atermpp::aterm_appl> summands;
  size_t num_prioritised;
  atermpp::aterm_list procvars;
  int stateformat;
  atermpp::aterm_appl nil;
  atermpp::function_symbol pairAFun;
  size_t statelen;
  atermpp::function_symbol stateAFun;
  size_t* current_id;

  enumerator_type::iterator_internal get_sols(const atermpp::aterm_list &v, 
                                              const atermpp::aterm &c,
                                              internal_substitution_type &sigma)
  {
    using namespace atermpp;
    return m_enumerator.begin_internal(aterm_cast<mcrl2::data::variable_list>(v),aterm_cast<aterm_appl>(c), sigma); // Laatste expressie is intern.
  }

  ns_info(const mcrl2::data::data_specification & specification,
          const mcrl2::data::detail::legacy_rewriter & rewriter) :
    m_specification(specification),
    m_rewriter(rewriter),
    m_enumerator(specification,rewriter)
  {
  }

};
/// \endcond

class NextStateGenerator
{
  public:
    typedef ns_info::substitution_type substitution_type;
    typedef ns_info::internal_substitution_type internal_substitution_type;

    NextStateGenerator(const atermpp::aterm &State, ns_info& Info, size_t identifier, bool SingleSummand = false, size_t SingleSummandIndex = 0);
    ~NextStateGenerator();

    bool next(mcrl2::lps::multi_action &Transition, atermpp::aterm* State, bool* prioritised = NULL);

    void reset(const atermpp::aterm &State, size_t SummandIndex = 0);

    atermpp::aterm get_state() const;

  private:
    ns_info info;
    size_t id;

    bool single_summand;

    size_t sum_idx;

    atermpp::aterm cur_state;
    internal_substitution_type current_substitution;
    atermpp::aterm_appl cur_act;
    atermpp::aterm_list cur_nextstate;

    std::vector <atermpp::aterm> stateargs;

    mcrl2::data::variable_list enumerated_variables;
    ns_info::enumerator_type::iterator_internal valuations;

    void set_substitutions();

    void SetTreeStateVars(const atermpp::aterm &tree, atermpp::aterm_list* vars);
    atermpp::aterm_appl rewrActionArgs(const atermpp::aterm_appl &act);
    atermpp::aterm makeNewState(const atermpp::aterm &old, const atermpp::aterm_list &assigns);
    atermpp::aterm_list ListFromFormat(const atermpp::aterm_list &l);
};

class NextState
{
    friend class NextStateGenerator;
  public:
    typedef NextStateGenerator::substitution_type substitution_type;
    typedef NextStateGenerator::internal_substitution_type internal_substitution_type;

    NextState(mcrl2::lps::specification const& spec, bool allow_free_vars, int state_format, const mcrl2::data::detail::legacy_rewriter& e);
    ~NextState();

    void prioritise(const char* action);

    atermpp::aterm getInitialState();
    NextStateGenerator* getNextStates(const atermpp::aterm &state, NextStateGenerator* old = NULL);
    NextStateGenerator* getNextStates(const atermpp::aterm &state, size_t group, NextStateGenerator* old = NULL);

    void gatherGroupInformation();

    size_t getGroupCount() const;
    size_t getStateLength();
    atermpp::aterm_appl getStateArgument(const atermpp::aterm &state, size_t index);
    atermpp::aterm_appl makeStateVector(const atermpp::aterm &state);
    mcrl2::lps::state make_new_state_vector(const atermpp::aterm &s);
    // atermpp::aterm parseStateVector(const atermpp::aterm_appl &state, const atermpp::aterm &match = NULL);
    atermpp::aterm parseStateVector(const atermpp::aterm_appl &state, const atermpp::aterm &match);
    atermpp::aterm parse_state_vector_new(const mcrl2::lps::state &s, const mcrl2::lps::state &match=mcrl2::lps::state(), bool check_match=false);
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

    atermpp::function_symbol smndAFun;
    bool* tree_init;
    std::vector <atermpp::aterm> stateargs;

    atermpp::aterm_list pars;
    atermpp::aterm initial_state;

    atermpp::aterm buildTree(std::vector<atermpp::aterm> &args);
    atermpp::aterm getTreeElement(const atermpp::aterm &tree, size_t index);

    atermpp::aterm SetVars(const atermpp::aterm &a, const atermpp::aterm_list &free_vars);
    atermpp::aterm_list ListToFormat(const atermpp::aterm_list &l, const atermpp::aterm_list &free_vars);
    atermpp::aterm_list ListFromFormat(const atermpp::aterm_list &l);
    atermpp::aterm_appl ActionToRewriteFormat(const atermpp::aterm_appl &act, const atermpp::aterm_list &free_vars);
    atermpp::aterm_list AssignsToRewriteFormat(const atermpp::aterm_list &assigns, const atermpp::aterm_list &free_vars);
};


NextState* createNextState(
     const mcrl2::lps::specification &spec,
     const mcrl2::data::rewriter &rewriter,
     bool allow_free_vars,
     int state_format = GS_STATE_VECTOR,
     NextStateStrategy strategy = nsStandard
   );


} // namespace lps
} // namespace mcrl2

#endif
