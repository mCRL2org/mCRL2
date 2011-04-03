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

#include "aterm2.h"
#include "mcrl2/lps/nextstate.h"

class NextState;

/// \cond INTERNAL_DOCS
// inherits from rewriter only for data implementation/reconstruction
//
// To minimize changes to the existing implementation, data
// implementation/reconstruction is performed manually.
struct legacy_rewriter : public mcrl2::data::rewriter
{
  typedef atermpp::aterm_appl term_type;

  template < typename ExpressionOrEquation >
  ATermAppl implement(ExpressionOrEquation const& expression) const
  {
    return expression;
  }

  template < typename EquationSelector >
  legacy_rewriter(mcrl2::data::data_specification const& d, EquationSelector const& selector, strategy s = jitty) :
    mcrl2::data::rewriter(d, selector, s)
  { }

  legacy_rewriter(mcrl2::data::rewriter const& other) :
    mcrl2::data::rewriter(other)
  { 
  }

  legacy_rewriter()
  {
    assert(false);
  }

  ATerm translate(ATermAppl t) const
  {
    return m_rewriter->toRewriteFormat(mcrl2::data::rewriter::implement(mcrl2::data::data_expression(t)));
  }

  ATerm translate(atermpp::aterm_appl t) const
  {
    return m_rewriter->toRewriteFormat(mcrl2::data::rewriter::implement(mcrl2::data::data_expression(t)));
  }

  ATermAppl translate(ATerm t) const
  {
    return this->reconstruct(atermpp::aterm_appl(m_rewriter->fromRewriteFormat(t)));
  }

  ATerm operator()(atermpp::aterm const& t) const
  {
    return m_rewriter->rewriteInternal(t);
  }

  ATerm operator()(ATerm const& t) const
  {
    return m_rewriter->rewriteInternal(t);
  }

  ATermList operator()(ATermList const& t) const
  {
    return m_rewriter->rewriteInternalList(t);
  }

  /// \brief Evaluate a substitution applied to an expression
  ///
  /// \param[in] e expression to evaluate
  /// \param[in] s substitution to apply to expression
  /// \return an expression equivalent to m_rewriter(s(e))
  template < typename Substitution >
  atermpp::aterm operator()(atermpp::aterm const& e, Substitution const& s) const
  {
    mcrl2::data::detail::Rewriter& local_rewriter(*m_rewriter);

    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i)
    {
      local_rewriter.setSubstitutionInternal(static_cast< ATermAppl >(i->first),
                                             static_cast< ATerm >(i->second));
    }

    ATerm result = local_rewriter.rewriteInternal(static_cast< ATerm >(e));

    // Subtle removal as NextStateGenerator requires substitutions for other variables
    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i)
    {
      local_rewriter.clearSubstitution(static_cast< ATermAppl >(i->first));
    }

    return result;
  }

  ATerm internally_associated_value(ATermAppl t) const
  {
    return m_rewriter->getSubstitutionInternal(t);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void set_internally_associated_value(ATermAppl t, ATerm e) const
  {
    m_rewriter->setSubstitutionInternal(t, e);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void set_internally_associated_value(ATermAppl t, ATermAppl e) const
  {
    m_rewriter->setSubstitution(t, e);
  }

  /// Note the internal state of the rewriter changes so constness is violated
  void clear_internally_associated_value(ATermAppl t) const
  {
    m_rewriter->clearSubstitution(t);
  }

  mcrl2::data::detail::Rewriter& get_rewriter() const
  {
    return *const_cast< mcrl2::data::detail::Rewriter* >(m_rewriter.get());
  }
};

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

// The following specialisations are here as a bridge to use the
// classic_enumerator on terms in rewrite format. Actually classic
// enumerator was built to function at the level of data expressions, but
// since there is an isomorphism between terms in both formats it is
// possible to replace only the methods below (since these are the only
// onse to make any specific assumptions about the format).
namespace detail
{

// Specialisation of classic_enumerator_impl to circumvent data reconstruction trick
template < >
inline
bool classic_enumerator_impl< mcrl2::data::mutable_map_substitution< atermpp::map< atermpp::aterm_appl, atermpp::aterm > >,
     legacy_rewriter, legacy_selector >::increment()
{
  ATermList assignment_list;

  while (m_generator.next(&assignment_list))
  {

    for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
         i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i)
    {
      m_substitution[static_cast< variable_type >((*i)(0))] = (*i)(1);
    }

    if (legacy_selector::test(m_evaluator(m_condition, m_substitution)))
    {
      return true;
    }
  }

  return false;
}

// Specialisation of classic_enumerator_impl to circumvent data implementation trick
template < >
template < typename Container >
bool classic_enumerator_impl< mcrl2::data::mutable_map_substitution< atermpp::map< atermpp::aterm_appl, atermpp::aterm > >,
     legacy_rewriter, legacy_selector >::initialise(Container const& v, typename atermpp::detail::enable_if_container< Container, variable >::type*)
{
  m_shared_context->m_enumerator.findSolutions(atermpp::convert< atermpp::term_list< variable_type > >(v), m_condition, true, &m_generator); // Changed one but last argument to true to check that enumerated conditions always reduce to true or false 7/12/2009 JFG

  return increment();
}
}

// Assumes that all terms are in internal rewrite format.
template <>
struct expression_traits< atermpp::aterm >
{
  static legacy_rewriter& get_rewriter()
  {
    static legacy_rewriter local_rewriter = legacy_rewriter(mcrl2::data::rewriter());

    return local_rewriter;
  }

  static atermpp::aterm false_()
  {
    return get_rewriter().translate(mcrl2::data::sort_bool::false_());
  }

  static atermpp::aterm true_()
  {
    return get_rewriter().translate(mcrl2::data::sort_bool::true_());
  }
};
}
}


struct ns_info
{
  NextState* parent;

  // Uses terms in internal format... *Sigh*
  typedef mcrl2::data::classic_enumerator<
              mcrl2::data::mutable_map_substitution< atermpp::map< atermpp::aterm_appl, atermpp::aterm > >,
              legacy_rewriter, legacy_selector > enumerator_type;


  mcrl2::data::data_specification const&                            m_specification;
  // Storing the legacy rewriter below by reference can lead to problems.
  legacy_rewriter const                                            m_rewriter; // only for translation to/from rewrite format

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

  enumerator_type get_sols(ATermList v, ATerm c)
  {
    const enumerator_type m(m_specification,mcrl2::data::variable_list(v),m_rewriter,c);

    return m;
  }

  ATermAppl export_term(ATerm term) const
  {
    return m_rewriter.translate(term);
  }

  ATerm import_term(ATermAppl term) const
  {
    return m_rewriter.translate(term);
  }

  ns_info(const mcrl2::data::data_specification & specification,
          const legacy_rewriter & rewriter) :
    m_specification(specification),
    m_rewriter(rewriter)
  {

    // Configure selector to compare with term that represents false
    legacy_selector::term() = m_rewriter.translate(mcrl2::data::sort_bool::true_());
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

    ns_info::enumerator_type valuations;

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
    NextState(mcrl2::lps::specification const& spec, bool allow_free_vars, int state_format, const legacy_rewriter& e);
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
      return const_cast< legacy_rewriter& >(info.m_rewriter);
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
