#ifndef _LIBENUM_H
#define _LIBENUM_H

#include <aterm2.h>
#include "librewrite.h"

enum EnumerateStrategy { ENUM_STANDARD };

typedef void (*FindSolutionsCallBack)(ATermList);

class Enumerator
{
	public:
		virtual ~Enumerator();

		virtual ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f = NULL) = 0;

		virtual void initialise(ATermList Vars, ATerm Expr) = 0;
		virtual bool next(ATermList *solution) = 0;
		virtual bool errorOccurred() = 0;
};

Enumerator *createEnumerator(ATermAppl spec, Rewriter &r, EnumerateStrategy strategy = ENUM_STANDARD);
Enumerator *createEnumerator(ATermAppl spec, Rewriter *r, EnumerateStrategy strategy = ENUM_STANDARD, bool clean_up_rewriter = false);

#endif
