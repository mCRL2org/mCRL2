#ifndef _ENUM_STANDARD_H
#define _ENUM_STANDARD_H

#include <aterm2.h>
#include "librewrite.h"
#include "libenum.h"

typedef struct {
	ATermList vars;
	ATermList vals;
	ATerm expr;
} fs_expr;

class EnumeratorStandard : public Enumerator
{
	public:
		EnumeratorStandard(ATermAppl spec, Rewriter &r);
		EnumeratorStandard(ATermAppl spec, Rewriter *r, bool clean_up_rewriter = false);
		~EnumeratorStandard();

		ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f = NULL);

		void initialise(ATermList vars, ATerm expr);
		bool next(ATermList *solution);
		bool errorOccurred();

	private:
		Rewriter &rewr_obj;
		Rewriter *prewr_obj;
		bool clean_up_rewr_obj;

		bool error;

		ATermAppl current_spec;
		ATermTable constructors;
		ATerm rewr_true, rewr_false;

		ATermList enum_vars;
		ATerm enum_expr;

		int used_vars;

		int max_vars;

		bool (EnumeratorStandard::*FindEquality)(ATerm,ATermList,ATerm*,ATerm*);
		ATerm (EnumeratorStandard::*build_solution_aux)(ATerm,ATermList);
		ATerm opidAnd,eqstr;
		AFun tupAFun;

		fs_expr *fs_stack;
		int fs_stack_size;
		int fs_stack_pos;

		ATermList *ss_stack;
		int ss_stack_size;
		int ss_stack_pos;

		void fs_reset();
		void fs_push(ATermList vars, ATermList vals, ATerm expr);
		void fs_pop(fs_expr *e = NULL);

		void ss_reset();
		void ss_push(ATermList s);
		ATermList ss_pop();

		void initialiseEnumerator(ATermAppl spec);
		bool IsInner3Eq(ATerm a);
		bool IsInnerCEq(ATermAppl a);
		bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
		bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
		void EliminateVars(fs_expr *e);
		ATerm build_solution_single(ATerm t, ATermList substs);
		ATerm build_solution_aux_innerc(ATerm t, ATermList substs);
		ATerm build_solution_aux_inner3(ATerm t, ATermList substs);
		ATermList build_solution2(ATermList vars, ATermList substs);
		ATermList build_solution(ATermList vars, ATermList substs);
};

#endif
