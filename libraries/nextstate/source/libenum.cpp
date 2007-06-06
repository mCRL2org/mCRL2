#include "librewrite.h"
#include "libenum.h"
#include "enum_standard.h"

EnumeratorSolutions::~EnumeratorSolutions()
{
}

Enumerator::~Enumerator()
{
}

Enumerator *createEnumerator(ATermAppl spec, Rewriter *r, bool clean_up_rewriter, EnumerateStrategy strategy)
{
	switch ( strategy )
	{
		case ENUM_STANDARD:
			return new EnumeratorStandard(spec, r,clean_up_rewriter);
		default:
			return NULL;
	}
}
