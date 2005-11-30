#include "librewrite.h"
#include "libenum.h"
#include "enum_standard.h"

Enumerator::~Enumerator()
{
}

Enumerator *createEnumerator(ATermAppl spec, Rewriter &r, EnumerateStrategy strategy)
{
	switch ( strategy )
	{
		case ENUM_STANDARD:
			return new EnumeratorStandard(spec, r);
		default:
			return NULL;
	}
}

Enumerator *createEnumerator(ATermAppl spec, Rewriter *r, EnumerateStrategy strategy, bool clean_up_rewriter)
{
	switch ( strategy )
	{
		case ENUM_STANDARD:
			return new EnumeratorStandard(spec, r,clean_up_rewriter);
		default:
			return NULL;
	}
}
