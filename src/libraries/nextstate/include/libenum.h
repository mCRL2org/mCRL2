#ifndef _LIBENUM_H
#define _LIBENUM_H

#include <aterm2.h>
#include "librewrite.h"

enum EnumerateStrategy { ENUM_STANDARD };

typedef void (*FindSolutionsCallBack)(ATermList);

class EnumeratorSolutions
{
	public:
		virtual ~EnumeratorSolutions();

		virtual bool next(ATermList *solution) = 0;
		virtual bool errorOccurred() = 0;
};

class Enumerator
{
	public:
		virtual ~Enumerator();

		virtual ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f = NULL) = 0;

		virtual EnumeratorSolutions *findSolutions(ATermList Vars, ATerm Expr, bool true_only, EnumeratorSolutions *old = NULL) = 0;
		virtual EnumeratorSolutions *findSolutions(ATermList Vars, ATerm Expr, EnumeratorSolutions *old = NULL) = 0;
		
		virtual Rewriter *getRewriter() = 0;
};

Enumerator *createEnumerator(ATermAppl spec, Rewriter *r, bool clean_up_rewriter = false, EnumerateStrategy strategy = ENUM_STANDARD);

#endif
