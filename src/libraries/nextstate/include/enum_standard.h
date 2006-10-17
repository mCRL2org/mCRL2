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

class EnumeratorSolutionsStandard;

typedef struct {
		Rewriter *rewr_obj;

		ATermTable constructors;
		ATerm rewr_true, rewr_false;

		int *max_vars;

		ATerm opidAnd,eqstr;
		AFun tupAFun;
		
		bool (EnumeratorSolutionsStandard::*FindEquality)(ATerm,ATermList,ATerm*,ATerm*);
		ATerm (EnumeratorSolutionsStandard::*build_solution_aux)(ATerm,ATermList);
} enumstd_info;

class EnumeratorStandard : public Enumerator
{
	public:
		EnumeratorStandard(ATermAppl spec, Rewriter *r, bool clean_up_rewriter = false);
		~EnumeratorStandard();

		ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f = NULL);

		EnumeratorSolutions *findSolutions(ATermList vars, ATerm expr, bool true_only, EnumeratorSolutions *old = NULL);
		EnumeratorSolutions *findSolutions(ATermList vars, ATerm expr, EnumeratorSolutions *old = NULL);

		Rewriter *getRewriter();
		
	private:
		bool clean_up_rewr_obj;

		ATermAppl current_spec;

		enumstd_info info;

		int max_vars;
};

class EnumeratorSolutionsStandard : public EnumeratorSolutions
{
	public:
		EnumeratorSolutionsStandard(ATermList Vars, ATerm Expr, bool true_only, enumstd_info &Info);
		~EnumeratorSolutionsStandard();

		bool next(ATermList *solution);
		bool errorOccurred();

		void reset(ATermList Vars, ATerm Expr, bool true_only);

		bool FindInner3Equality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
		bool FindInnerCEquality(ATerm t, ATermList vars, ATerm *v, ATerm *e);
		ATerm build_solution_aux_innerc(ATerm t, ATermList substs);
		ATerm build_solution_aux_inner3(ATerm t, ATermList substs);
	private:
		enumstd_info info;
		bool check_true;
		
		ATermList enum_vars;
		ATerm enum_expr;

		bool error;

		int used_vars;

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

		void EliminateVars(fs_expr *e);
		bool IsInner3Eq(ATerm a);
		bool IsInnerCEq(ATermAppl a);
		ATerm build_solution_single(ATerm t, ATermList substs);
		ATermList build_solution2(ATermList vars, ATermList substs);
		ATermList build_solution(ATermList vars, ATermList substs);
};

#endif
