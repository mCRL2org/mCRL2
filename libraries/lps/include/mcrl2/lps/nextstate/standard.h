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
#include "mcrl2/data/enumerator_factory.h"

class NextStateStandard;

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

  legacy_rewriter(mcrl2::data::rewriter const& other) :
                                 mcrl2::data::rewriter(other) {
  }

  legacy_rewriter() {
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
  atermpp::aterm operator()(atermpp::aterm const& e, Substitution const& s) const {
    mcrl2::data::detail::Rewriter& local_rewriter(*m_rewriter);

    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i) {
      local_rewriter.setSubstitutionInternal(static_cast< ATermAppl >(i->first),
          static_cast< ATerm >(i->second));
    }

    ATerm result = local_rewriter.rewriteInternal(static_cast< ATerm >(e));

    // Subtle removal as NextStateGenerator requires substitutions for other variables
    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i) {
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
    static atermpp::aterm term = mcrl2::data::sort_bool::false_();

    return term;
  }

  /// \brief returns true if and only if the argument is equal to true
  template < typename ExpressionType >
  static bool test(ExpressionType const& e) {
    return e != term();
  }
};

// serves to extract the rewriter object, which used to be an implementation
// detail of the enumerator that was exposed through its interface
template < typename Enumerator >
class legacy_enumerator_factory : public mcrl2::data::enumerator_factory< Enumerator >
{
    typedef mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > > standard_factory;

    struct extractor : public standard_factory
    {
      extractor(standard_factory& other) : standard_factory(other)
      {
      }

      standard_factory::evaluator_type evaluator()
      {
        return *(standard_factory::m_evaluator);
      }

      boost::shared_ptr< typename mcrl2::data::enumerator_factory< Enumerator >::shared_context_type > context()
      {
        return boost::shared_ptr< typename mcrl2::data::enumerator_factory< Enumerator >::shared_context_type >(
            new typename mcrl2::data::enumerator_factory< Enumerator >::shared_context_type(*standard_factory::m_enumeration_context, *standard_factory::m_evaluator));
      }
    };

    boost::shared_ptr< const legacy_rewriter > m_local_evaluator;

  public:

    // \attention not exception safe
    legacy_enumerator_factory(standard_factory& other) :
      mcrl2::data::enumerator_factory< Enumerator >(extractor(other).context(), *(new legacy_rewriter(extractor(other).evaluator())))
    {
      m_local_evaluator.reset(this->m_evaluator);
    }

    legacy_enumerator_factory(legacy_enumerator_factory const& other) :
        mcrl2::data::enumerator_factory< Enumerator >(other),
        m_local_evaluator(other.m_local_evaluator)
    {
    }

    Enumerator make(ATermList v, ATerm c)
    {
      return mcrl2::data::enumerator_factory< Enumerator >::make(boost::make_iterator_range(atermpp::aterm_list(v)), atermpp::aterm(c));
    }


    legacy_rewriter const& get_evaluator()
    {
      return *m_local_evaluator;
    }
};

namespace mcrl2 {
  namespace data {

    // The following specialisations are here as a bridge to use the
    // classic_enumerator on terms in rewrite format. Actually classic
    // enumerator was built to function at the level of data expressions, but
    // since there is an isomorphism between terms in both formats it is
    // possible to replace only the methods below (since these are the only
    // onse to make any specific assumptions about the format).
    namespace detail {

      // Specialisation of classic_enumerator_impl to circumvent data reconstruction trick
      template < >
      bool classic_enumerator_impl< mcrl2::data::mutable_map_substitution< atermpp::aterm_appl, atermpp::aterm >,
                  legacy_rewriter, legacy_selector >::increment() {

        ATermList assignment_list;

        while (m_generator->next(&assignment_list)) {
          if (m_generator->errorOccurred()) {
            throw mcrl2::runtime_error(std::string("Failed enumeration of condition ") + pp(m_condition) + "; cause unknown");
          }

          for (atermpp::term_list_iterator< atermpp::aterm_appl > i(assignment_list);
                             i != atermpp::term_list_iterator< atermpp::aterm_appl >(); ++i) {
            m_substitution[static_cast< variable_type >((*i)(0))] = (*i)(1);
          }

          if (legacy_selector::test(m_evaluator(m_condition, m_substitution))) {
            return true;
          }
        }

        return false;
      }

      // Specialisation of classic_enumerator_impl to circumvent data implementation trick
      template < >
      template < typename ForwardIteratorRange >
      bool classic_enumerator_impl< mcrl2::data::mutable_map_substitution< atermpp::aterm_appl, atermpp::aterm >,
                  legacy_rewriter, legacy_selector >::initialise(boost::iterator_range< ForwardIteratorRange > const& v) {

        atermpp::term_list< data::variable > variables;

        // normalise variables
        for (ForwardIteratorRange i = v.begin(); i != v.end(); ++i)
        {
          variables = atermpp::push_front(variables, variable(variable(*i).name(), m_shared_context->m_specification.normalise(variable(*i).sort())));
        }

        m_generator.reset(static_cast< EnumeratorSolutionsStandard* >(
                    m_shared_context->m_enumerator.findSolutions(atermpp::reverse(variables), m_condition, false)));

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
  NextStateStandard *parent;

  // Uses terms in internal format... *Sigh*
  typedef mcrl2::data::classic_enumerator<
      mcrl2::data::mutable_map_substitution< atermpp::aterm_appl, atermpp::aterm >,
      legacy_rewriter, legacy_selector > enumerator_type;

  typedef legacy_enumerator_factory< enumerator_type > enumerator_factory_type;

  mcrl2::data::data_specification const&                            m_specification;
  boost::shared_ptr< legacy_enumerator_factory< enumerator_type > > m_enumerator_factory;
  legacy_rewriter const&                                            m_rewriter; // only for translation to/from rewrite format

  int num_summands;
  ATermAppl *summands;
  int num_prioritised;
  ATermList procvars;
  int stateformat;
  ATermAppl nil;
  AFun pairAFun;
  int statelen;
  AFun stateAFun;
  unsigned int *current_id;

  enumerator_type get_sols(ATermList v, ATerm c) {
    return m_enumerator_factory->make(v, c);
  }

  ATermAppl export_term(ATerm term) {
    return m_rewriter.translate(term);
  }

  ATerm import_term(ATermAppl term) {
    return m_rewriter.translate(term);
  }

  ns_info(mcrl2::data::data_specification const& specification,
          mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > >& factory) :
    m_specification(specification),
    m_enumerator_factory(new enumerator_factory_type(factory)),
    m_rewriter(m_enumerator_factory->get_evaluator()) {

    // Configure selector to compare with term that represents false
    legacy_selector::term() = m_rewriter.translate(mcrl2::data::sort_bool::false_());
  }
};
/// \endcond

class NextStateGeneratorStandard : public NextStateGenerator
{
  public:
    NextStateGeneratorStandard(ATerm State, ns_info &Info, unsigned int identifier, bool SingleSummand = false);
    ~NextStateGeneratorStandard();

    bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL);

    bool errorOccurred();

    void reset(ATerm State, size_t SummandIndex = 0);

    ATerm get_state() const;

  private:
    ns_info info;
    unsigned int id;

    bool error;
    bool single_summand;

    int sum_idx;

    ATerm cur_state;
    ATerm cur_act;
    ATermList cur_nextstate;

    ATerm *stateargs;

                ns_info::enumerator_type valuations;

    void set_substitutions();

    void SetTreeStateVars(ATerm tree, ATermList *vars);
    ATermAppl rewrActionArgs(ATermAppl act);
    ATerm makeNewState(ATerm old, ATermList assigns);
    ATermList ListFromFormat(ATermList l);
};

class NextStateStandard : public NextState
{
  friend class NextStateGeneratorStandard;
  public:
                typedef mcrl2::data::enumerator_factory< mcrl2::data::classic_enumerator< > > enumerator_factory_type;
    NextStateStandard(mcrl2::lps::specification const& spec, bool allow_free_vars, int state_format, enumerator_factory_type& e);
    ~NextStateStandard();

    void prioritise(const char *action);

    ATerm getInitialState();
    NextStateGenerator *getNextStates(ATerm state, NextStateGenerator *old = NULL);
    NextStateGenerator *getNextStates(ATerm state, int group, NextStateGenerator *old = NULL);

                void gatherGroupInformation();

    int getGroupCount() const;
    int getStateLength();
    ATermAppl getStateArgument(ATerm state, int index);
    ATermAppl makeStateVector(ATerm state);
    ATerm parseStateVector(ATermAppl state, ATerm match = NULL);
                mcrl2::data::rewriter& getRewriter() { // Deprecated. Do not use.
                  return const_cast< legacy_rewriter& >(info.m_rewriter);
                }

  private:
    ns_info info;
    unsigned int next_id;
    unsigned int current_id;

    bool stateAFun_made;

    bool usedummies;

    AFun smndAFun;
    bool *tree_init;
    ATerm *stateargs;

    ATermList pars;
    ATerm initial_state;

    ATerm buildTree(ATerm *args);
    ATerm getTreeElement(ATerm tree, int index);

    ATerm SetVars(ATerm a, ATermList free_vars);
    ATermList ListToFormat(ATermList l,ATermList free_vars);
    ATermList ListFromFormat(ATermList l);
    ATermAppl ActionToRewriteFormat(ATermAppl act, ATermList free_vars);
    ATermList AssignsToRewriteFormat(ATermList assigns, ATermList free_vars);
};

#endif
