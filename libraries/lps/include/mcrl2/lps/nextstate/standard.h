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
#include "mcrl2/new_data/enumerator_factory.h"

class NextStateStandard;

//
// inherits from rewriter only for data implementation/reconstruction
struct legacy_rewriter : public mcrl2::new_data::rewriter
{
  legacy_rewriter(mcrl2::new_data::rewriter const& other) :
                                 mcrl2::new_data::rewriter(other) {
  }

  ATerm translate(ATermAppl t)
  {
    return m_rewriter->toRewriteFormat(implement(static_cast< mcrl2::new_data::data_expression >(t)));
  }

  ATermAppl translate(ATerm t)
  {
    return reconstruct(atermpp::aterm_appl(m_rewriter->fromRewriteFormat(t)));
  }

  ATerm operator()(ATerm const& t) const
  {
    return m_rewriter->rewriteInternal(t);
  }

  ATermList operator()(ATermList const& t) const
  {
    return m_rewriter->rewriteInternalList(t);
  }

  ATerm internally_associated_value(ATermAppl t) const
  {
    return m_rewriter->getSubstitutionInternal(t);
  }

  void set_internally_associated_value(ATermAppl t, ATerm e)
  {
    m_rewriter->setSubstitutionInternal(t, e);
  }

  void set_internally_associated_value(ATermAppl t, ATermAppl e)
  {
    m_rewriter->setSubstitution(t, e);
  }

  void clear_internally_associated_value(ATermAppl t)
  {
    m_rewriter->clearSubstitution(t);
  }
};

// serves to extract the rewriter object, which used to be an implementation
// detail of the enumerator that was exposed through its interface
template < typename Enumerator >
struct legacy_enumerator_factory : public mcrl2::new_data::enumerator_factory< Enumerator > {

  legacy_enumerator_factory(mcrl2::new_data::enumerator_factory< Enumerator > const& other) :
                                         mcrl2::new_data::enumerator_factory< Enumerator >(other)
  {
  }

  mcrl2::new_data::rewriter const& get_evaluator() const
  {
    return this->m_evaluator;
  }
};

struct ns_info {
	NextStateStandard *parent;

        typedef mcrl2::new_data::classic_enumerator< mcrl2::new_data::mutable_substitution< >,
                 mcrl2::new_data::rewriter, mcrl2::new_data::selectors::select_not< false > > enumerator_type;

        legacy_enumerator_factory< enumerator_type > m_enumerator_factory;
        legacy_rewriter                              m_rewriter;

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

        mcrl2::new_data::mutable_substitution< > m_substitution;

        enumerator_type get_sols(ATermList v, ATerm c) {
//          return m_enumerator_factory.make(variables, mcrl2::new_data::data_expression(c));
          return m_enumerator_factory.make(
                        mcrl2::new_data::convert< std::set< mcrl2::new_data::variable > >(v), mcrl2::new_data::data_expression(c));
        }

        ATermAppl export_term(ATerm term) {
          return m_rewriter.get_rewriter().fromRewriteFormat(term);
        }

        ATerm import_term(ATermAppl term) {
          return m_rewriter.get_rewriter().toRewriteFormat(term);
        }

        ns_info(mcrl2::new_data::enumerator_factory< mcrl2::new_data::classic_enumerator< > > const& factory) :
           m_enumerator_factory(factory),
           m_rewriter(legacy_enumerator_factory< mcrl2::new_data::classic_enumerator< > >(factory).get_evaluator()) {
        }
};

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
		NextStateStandard(mcrl2::lps::specification const& spec, bool allow_free_vars, int state_format, mcrl2::new_data::enumerator_factory< mcrl2::new_data::classic_enumerator<> > const& e);
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
                mcrl2::new_data::detail::Rewriter *getRewriter(); // Deprecated. Do not use.

	private:
		ns_info info;
		unsigned int next_id;
		unsigned int current_id;

		bool stateAFun_made;

		bool usedummies;

		AFun smndAFun;
		bool *tree_init;
		ATerm *stateargs;

		ATermAppl current_spec;
		ATermList pars;
		ATerm initial_state;

		ATerm buildTree(ATerm *args);
		ATerm getTreeElement(ATerm tree, int index);

		ATermAppl FindDummy(ATermAppl sort, ATermList no_dummy = ATempty);
		ATerm SetVars(ATerm a, ATermList free_vars);
		ATermList ListToFormat(ATermList l,ATermList free_vars);
		ATermList ListFromFormat(ATermList l);
		ATermAppl ActionToRewriteFormat(ATermAppl act, ATermList free_vars);
		ATermList AssignsToRewriteFormat(ATermList assigns, ATermList free_vars);
};

#endif
