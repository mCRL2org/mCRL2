#ifndef _ATERM_H
#define _ATERM_H

#include "aterm2.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

typedef struct ProtEntry
{
	struct ProtEntry *next;
	ATerm *start;
	int    size;
} ProtEntry;

#define STRING_MARK 0xFF /* marker for binary strings */
#define LENSPEC 12

extern ATbool silent;
extern ATbool low_memory;

extern ProtEntry **at_prot_table;
extern int at_prot_table_size;
extern ProtEntry *at_prot_memory;

void AT_markTerm(ATerm t);
void AT_markTerm_young(ATerm t);

void AT_unmarkTerm(ATerm t);
void AT_unmarkIfAllMarked(ATerm t);
void AT_unmarkAll();
int  AT_calcTextSize(ATerm t);
int  AT_calcCoreSize(ATerm t);
int  AT_calcSubterms(ATerm t);
int  AT_calcUniqueSubterms(ATerm t);
int  AT_calcUniqueSymbols(ATerm t);
int  AT_calcTermDepth(ATerm t);
void AT_assertUnmarked(ATerm t);
void AT_assertMarked(ATerm t);
int AT_calcAllocatedSize();

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
