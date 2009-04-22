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

// inherits from rewriter only for data implementation/reconstruction
// 
// To minimize changes to the existing implementation, data
// implementation/reconstruction is performed manually.
struct legacy_rewriter : protected mcrl2::new_data::rewriter
{
  typedef atermpp::aterm_appl term_type;

  legacy_rewriter(mcrl2::new_data::rewriter const& other) :
                                 mcrl2::new_data::rewriter(other) {
  }

  legacy_rewriter() {
    throw mcrl2::logic_error("Invalid rewriter construction!");
  }

  ATerm translate(ATermAppl t)
  {
    return m_rewriter->toRewriteFormat(this->implement(static_cast< mcrl2::new_data::data_expression >(t)));
  }

  ATermAppl translate(ATerm t)
  {
    return this->reconstruct(atermpp::aterm_appl(m_rewriter->fromRewriteFormat(t)));
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
  atermpp::aterm_appl operator()(atermpp::aterm const& e, Substitution const& s) const {
    mcrl2::new_data::detail::Rewriter& local_rewriter(*m_rewriter);

    for (typename Substitution::const_iterator i(s.begin()); i != s.end(); ++i) {
      local_rewriter.setSubstitutionInternal(static_cast< ATermAppl >(i->first),
          static_cast< ATerm >(i->second));
    }

    ATermAppl result = local_rewriter.rewriteInternal(static_cast< ATerm >(e));

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
struct legacy_enumerator_factory : public mcrl2::new_data::enumerator_factory< Enumerator >
{
  typedef mcrl2::new_data::enumerator_factory< mcrl2::new_data::classic_enumerator< > > standard_factory;

  struct extractor : public standard_factory
  {
    extractor(standard_factory const& other) : standard_factory(other)
    {
    }

    standard_factory::shared_context_type const& get_context()
    {
      return this->m_enumeration_context;
    }

    standard_factory::evaluator_type const& get_evaluator()
    {
      return this->m_evaluator;
    }
  };

  mcrl2::new_data::enumerator_factory< Enumerator > m_factory;

  legacy_enumerator_factory(standard_factory const& other) :
        mcrl2::new_data::enumerator_factory< Enumerator >(extractor(other).get_context(), extractor(other).get_evaluator())
  {
  }

  Enumerator make(ATermList v, atermpp::aterm c)
  {
    return m_factory.make(mcrl2::new_data::convert< std::set< atermpp::aterm_appl > >(v), c);
  }


  legacy_rewriter& get_evaluator()
  {
    return this->m_evaluator;
  }
};

// Uses static context so code is not reentrant
struct legacy_selector
{
  static atermpp::aterm& term()
  {
    static atermpp::aterm term = mcrl2::new_data::sort_bool_::false_();

    return term;
  }

  /// \brief returns true if and only if the argument is equal to true
  template < typename ExpressionType >
  static bool test(ExpressionType const& e) {
    return e != term();
  }
};

namespace mcrl2 {
  namespace new_data {

    // Assumes that all terms are in internal rewrite format.
    template <>
    struct expression_traits< atermpp::aterm >
    {
      static legacy_rewriter& get_rewriter()
      {
        static legacy_rewriter local_rewriter = legacy_rewriter(mcrl2::new_data::rewriter());

        return local_rewriter;
      }

      static atermpp::aterm false_()
      {
        return get_rewriter().translate(mcrl2::new_data::sort_bool_::false_());
      }

      static atermpp::aterm true_()
      {
        return get_rewriter().translate(mcrl2::new_data::sort_bool_::true_());
      }
    };
  }
}

struct ns_info
{
	NextStateStandard *parent;

        // Uses terms in internal format... *Sigh*
        typedef mcrl2::new_data::classic_enumerator<
            mcrl2::new_data::mutable_substitution< atermpp::aterm_appl, atermpp::aterm >,
            legacy_rewriter, legacy_selector > enumerator_type;

        legacy_enumerator_factory< enumerator_type > m_enumerator_factory;
        legacy_rewriter&                             m_rewriter; // only for translation to/from rewrite format

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
//          return m_enumerator_factory.make(variables, mcrl2::new_data::data_expression(c));
          return m_enumerator_factory.make(v, c);
        }

        ATermAppl export_term(ATerm term) {
          return m_rewriter.translate(term);
        }

        ATerm import_term(ATermAppl term) {
          return m_rewriter.translate(term);
        }

        ns_info(mcrl2::new_data::enumerator_factory< mcrl2::new_data::classic_enumerator< > > const& factory) :
           m_enumerator_factory(factory),
           m_rewriter(m_enumerator_factory.get_evaluator()) {

           // Configure selector to compare with term that represents false
           legacy_selector::term() = m_rewriter.translate(mcrl2::new_data::sort_bool_::false_());
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
