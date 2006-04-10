/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "aterm2.h"
#include "make.h"
#include "deprecated.h"
#include "util.h"

#include <assert.h>

/*}}}  */
/*{{{  defines */

#define PATTERN_CACHE_SIZE	4099	/* nextprime(4096) */
#define NR_INLINE_TERMS 6

/*}}}  */
/*{{{  types */

typedef struct
{
  char *pat;
  ATerm term;
} at_entry;

/*}}}  */
/*{{{  globals */

char make_id[] = "$Id: make.c,v 1.30 2003/06/24 18:57:10 jurgenv Exp $";

static Symbol symbol_int;
static Symbol symbol_str;
static Symbol symbol_real;
static Symbol symbol_appl;
static Symbol symbol_list;
static Symbol symbol_blob;
static Symbol symbol_placeholder;
static Symbol symbol_term;
static at_entry pattern_table[PATTERN_CACHE_SIZE];

static va_list theargs;
static va_list *args = (va_list *) &theargs;

/*}}}  */
/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif
static ATerm makePlaceholder(ATermPlaceholder pat);
static ATermAppl makeArguments(ATermAppl appl, Symbol name);
static ATerm AT_vmakeTerm(ATerm pat);

static ATbool AT_vmatchTerm(ATerm t, ATerm pat);
static ATbool matchPlaceholder(ATerm t, ATermPlaceholder pat);
static ATbool matchArguments(ATermAppl appl, ATermAppl applpat);

ATerm ATvmake(const char *pat);
ATbool ATvmatchTerm(ATerm t, ATerm pat);

/*}}}  */
/*{{{  void AT_vmakeSetArgs(va_list args) */

void AT_vmakeSetArgs(va_list *newargs)
{
  args = newargs;
}

/*}}}  */
/*{{{  ATerm AT_getPattern(const char *pat) */

/**
 * Retrieve a pattern using hash techniques.
 */

ATerm AT_getPattern(const char *pat)
{
  unsigned int hash_val;
  char        *walk = (char *) pat;
  at_entry    *bucket;

  for(hash_val = 0; *walk; walk++)
    hash_val = 251 * hash_val + *walk;
  hash_val %= PATTERN_CACHE_SIZE;

  bucket = &(pattern_table[hash_val]);
  if (bucket->pat) {
    if (streq(bucket->pat, pat)) {
      return bucket->term;
    }
    else {
      free(bucket->pat);
    }
  }
  else {
    ATprotect(&(bucket->term));
  }

  bucket->pat = strdup(pat);
  if (!bucket->pat) {
    ATerror("ATvmake: no room for pattern.\n");
  }

  bucket->term = ATreadFromString(pat);
  return bucket->term;
}

/*}}}  */

/*{{{  void AT_initMake(int argc, char *argv[]) */
void AT_initMake(int argc, char *argv[])
{
  int	lcv;
  for (lcv=0; lcv < PATTERN_CACHE_SIZE; lcv++)
  {
    pattern_table[lcv].pat  = NULL;
    pattern_table[lcv].term = NULL;
  }
  symbol_int  = ATmakeSymbol("int",  0, ATfalse);
  ATprotectSymbol(symbol_int);
  symbol_str  = ATmakeSymbol("str",  0, ATfalse);
  ATprotectSymbol(symbol_str);
  symbol_real = ATmakeSymbol("real", 0, ATfalse);
  ATprotectSymbol(symbol_real);
  symbol_appl = ATmakeSymbol("appl", 0, ATfalse);
  ATprotectSymbol(symbol_appl);
  symbol_list = ATmakeSymbol("list", 0, ATfalse);
  ATprotectSymbol(symbol_list);
  symbol_blob = ATmakeSymbol("blob", 0, ATfalse);
  ATprotectSymbol(symbol_blob);
  symbol_term = ATmakeSymbol("term", 0, ATfalse);
  ATprotectSymbol(symbol_term);
  symbol_placeholder = ATmakeSymbol("placeholder", 0, ATfalse);
  ATprotectSymbol(symbol_placeholder);
}
/*}}}  */
/*{{{  ATerm ATmake(const char *pat, ...) */

ATerm ATmake(const char *pat, ...)
{
  ATerm t;

  va_start(*args, pat);
  t = ATvmake(pat);
  va_end(*args);

  return t;
}

/*}}}  */
/*{{{  ATerm ATmakeTerm(ATerm pat, ...) */
ATerm ATmakeTerm(ATerm pat, ...)
{
  ATerm t;

  va_start(*args, pat);
  t = AT_vmakeTerm(pat);
  va_end(*args);

  return t;
}
/*}}}  */
/*{{{  ATerm ATvmake(const char *pat) */

/**
 * Make a new term using a string pattern and a list of arguments.
 */

ATerm ATvmake(const char *pat)
{
  return AT_vmakeTerm(AT_getPattern(pat));
}

/*}}}  */
/*{{{  ATerm ATvmakeTerm(ATerm pat) */

/**
 * Match a term pattern against a list of arguments.
 */

ATerm ATvmakeTerm(ATerm pat)
{
  return AT_vmakeTerm(pat);
}

/*}}}  */
/*{{{  ATerm AT_vmakeTerm(ATerm pat) */

static ATerm
AT_vmakeTerm(ATerm pat)
{
  int nr_args;
  ATermAppl appl;
  ATermList list = NULL;
  ATermList arglist = NULL;
  ATerm term;
  ATerm type;
  Symbol sym;
  ATerm annos;

  switch (ATgetType(pat))
  {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
      return pat;

    case AT_APPL:
      annos = ATgetAnnotations(pat);
      appl = (ATermAppl) pat;
      sym = ATgetSymbol(appl);

      if (annos) {
	return ATsetAnnotations((ATerm) makeArguments(appl, sym), annos);
      }
      else {
	return (ATerm) makeArguments(appl, sym);
      }

    case AT_LIST:
      /*{{{  Handle list */

      annos = ATgetAnnotations(pat);
      list = (ATermList) pat;

      if(ATisEmpty(list))
	return pat;

      nr_args = ATgetLength(list);
      arglist = ATmakeList0();
      while (--nr_args > 0)
      {
	term = ATgetFirst(list);
	arglist = ATinsert(arglist, AT_vmakeTerm(term));
	list = ATgetNext(list);
      }
      term = ATgetFirst(list);
      if (ATgetType(term) == AT_PLACEHOLDER)
      {
	type = ATgetPlaceholder((ATermPlaceholder) term);
	if (ATgetType(type) == AT_APPL &&
	    ATgetSymbol((ATermAppl)type) == symbol_list)
	{
	  list = va_arg(*args, ATermList);
	}
	else
	  list = ATmakeList1(AT_vmakeTerm(term));
      } else
	list = ATmakeList1(AT_vmakeTerm(term));

      while (!ATisEmpty(arglist))
      {
	list = ATinsert(list, ATgetFirst(arglist));
	arglist = ATgetNext(arglist);
      }

      if (annos) {
	return ATsetAnnotations((ATerm) list, annos);
      }
      else {
	return (ATerm) list;
      }

      /*}}}  */

    case AT_PLACEHOLDER:
      return makePlaceholder((ATermPlaceholder)pat);

    default:
      ATerror("AT_vmakeTerm: illegal type %d.\n", ATgetType(pat));
      return (ATerm) NULL;
  }
}

/*}}}  */
/*{{{  static ATermAppl makeArguments(ATermAppl appl, name) */

static ATermAppl
makeArguments(ATermAppl appl, Symbol name)
{
  Symbol sym = ATgetSymbol(appl);
  int nr_args = ATgetArity(sym);
  int cur = -1;
  ATerm terms[NR_INLINE_TERMS];
  ATerm term = NULL;
  ATerm type = NULL;
  ATermList list = NULL;
  ATermList arglist = NULL;

  if(nr_args == 0) {
    if(ATgetArity(name) == 0)
      sym = name;
    else
      sym = ATmakeSymbol(ATgetName(name), 0, ATisQuoted(name));
    return ATmakeAppl0(sym);
  } else if (nr_args-- <= NR_INLINE_TERMS) {
    for (cur = 0; cur < nr_args; cur++)
      terms[cur] = AT_vmakeTerm(ATgetArgument(appl, cur));
    terms[nr_args] = ATgetArgument(appl, nr_args);
    if (ATgetType(terms[nr_args]) == AT_PLACEHOLDER) {
      type = ATgetPlaceholder((ATermPlaceholder)terms[nr_args]);
      if (ATgetType(type) == AT_APPL &&
	  ATgetSymbol((ATermAppl)type) == symbol_list) {
	list = va_arg(*args, ATermList);
	for (--cur; cur >= 0; cur--)
	  list = ATinsert(list, terms[cur]);
	if(ATgetArity(name) == ATgetLength(list))
	  sym = name;
	else
	  sym = ATmakeSymbol(ATgetName(name), ATgetLength(list), 
			     ATisQuoted(name));
	return ATmakeApplList(sym, list);
      }
    }
    terms[nr_args] = AT_vmakeTerm(terms[nr_args]);
    if(ATgetArity(name) == ATgetArity(sym))
      sym = name;
    else
      sym = ATmakeSymbol(ATgetName(name), ATgetArity(sym), ATisQuoted(name));
    return ATmakeApplArray(sym, terms);
  }

  arglist = ATmakeList0();
  for (cur = 0; cur < nr_args; cur++)
    arglist = ATinsert(arglist, AT_vmakeTerm(ATgetArgument(appl,cur)));

  term = ATgetArgument(appl, nr_args);
  if (ATgetType(term) == AT_PLACEHOLDER)
  {
    type = ATgetPlaceholder((ATermPlaceholder)term);
    if (ATgetType(type) == AT_APPL &&
	ATgetSymbol((ATermAppl)type) == symbol_list)
    {
      list = va_arg(*args, ATermList);
    }
  }
  if (list == NULL)
    list = ATmakeList1(AT_vmakeTerm(term));


  while (!ATisEmpty(arglist))
  {
    list = ATinsert(list, ATgetFirst(arglist));
    arglist = ATgetNext(arglist);
  }

  if(ATgetArity(name) == ATgetLength(list))
    sym = name;
  else
    sym = ATmakeSymbol(ATgetName(name), ATgetLength(list), ATisQuoted(name));
  return ATmakeApplList(sym, list);
}

/*}}}  */
/*{{{  static ATerm makePlaceholder(ATermPlaceholder pat) */

static ATerm
makePlaceholder(ATermPlaceholder pat)
{
  ATerm type = ATgetPlaceholder(pat);

  if (ATgetType(type) == AT_APPL) {
    ATermAppl appl = (ATermAppl) type;
    Symbol sym = ATgetSymbol(appl);
    if (sym == symbol_int && ATgetArity(sym) == 0)
      return (ATerm) ATmakeInt(va_arg(*args, int));
    else if (sym == symbol_real && ATgetArity(sym) == 0)
      return (ATerm) ATmakeReal(va_arg(*args, double));
    else if (sym == symbol_blob) {
      int len = va_arg(*args, int);
      void *data = va_arg(*args, void *);
      return (ATerm) ATmakeBlob(len, data);
    } else if(sym == symbol_placeholder)
      return (ATerm) ATmakePlaceholder(va_arg(*args, ATerm));
    else if(sym == symbol_term)
      return va_arg(*args, ATerm);
    else {
      ATbool makeAppl = ATfalse, quoted = ATfalse;
      char *name = ATgetName(sym);

      if(streq(name, "appl") || streq(name, "id"))
	makeAppl = ATtrue;
      else if(streq(name, "str")) {
	makeAppl = ATtrue;
	quoted = ATtrue;
      }
      if(makeAppl) {
	sym = ATmakeSymbol(va_arg(*args, char *), 0, quoted);
	return (ATerm) makeArguments(appl, sym);
      }
    }
  }
  ATerror("makePlaceholder: illegal pattern %t\n", pat);
  return (ATerm) NULL;
}

/*}}}  */

/*{{{  ATbool ATvmatch(ATerm t, const char *pat) */

/**
 * Match a string pattern against a term using a list of arguments.
 */

ATbool ATvmatch(ATerm t, const char *pat)
{
  return AT_vmatchTerm(t, AT_getPattern(pat));
}

/*}}}  */
/*{{{  ATbool ATmatch(ATerm t, const char *pat, ...) */

/**
 * Match a term against a string pattern.
 */

ATbool ATmatch(ATerm t, const char *pat, ...)
{
  ATbool result;

  va_start(*args, pat);
  result = ATvmatch(t, pat);
  va_end(*args);

  return result;
}

/*}}}  */
/*{{{  ATbool ATmatchTerm(ATerm t, ATerm pat, ...) */

/**
 * Match a term against a pattern using a list of arguments.
 */

ATbool ATmatchTerm(ATerm t, ATerm pat, ...)
{
  ATbool result;

  va_start(*args, pat);
  result = ATvmatchTerm(t, pat);
  va_end(*args);

  return result;
}

/*}}}  */
/*{{{  ATbool ATvmatchTerm(ATerm t, ATerm pat) */

/**
 * Match a term against a string pattern using a list of arguments.
 */

ATbool ATvmatchTerm(ATerm t, ATerm pat)
{
  return AT_vmatchTerm(t, pat);
}

/*}}}  */
/*{{{  ATbool AT_vmatchTerm(ATerm t, ATerm pat) */

/**
 * Match a term against a term pattern using a list of arguments.
 */

ATbool AT_vmatchTerm(ATerm t, ATerm pat)
{
  ATermList listpat, list;
  Symbol sym, psym;
  int len;

  if(ATgetType(pat) == AT_PLACEHOLDER)
    return matchPlaceholder(t, (ATermPlaceholder)pat);

  if(ATgetType(pat) != ATgetType(t))
    return ATfalse;

  switch(ATgetType(pat)) {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
      return ATisEqual(t, pat);

    case AT_APPL:
      sym = ATgetSymbol((ATermAppl)t);
      psym = ATgetSymbol((ATermAppl)pat);
      if(!streq(ATgetName(sym), ATgetName(psym)) ||
	 ATisQuoted(sym) != ATisQuoted(psym))
	return ATfalse;
      return matchArguments((ATermAppl)t, (ATermAppl)pat);

    case AT_LIST:
      /*{{{  Match a list */

      listpat = (ATermList)pat;
      list = (ATermList)t;
      len = ATgetLength(listpat);

      if(len == 0)
	return ATisEmpty(list);

      while(--len > 0) {
	if(ATisEmpty(list))
	  return ATfalse;

	pat     = ATgetFirst(listpat);
	t       = ATgetFirst(list);
	listpat = ATgetNext(listpat);
	list    = ATgetNext(list);

	if(!AT_vmatchTerm(t, pat))
	  return ATfalse;
      }

      /* We now reached the last element of the pattern list */
      pat = ATgetFirst(listpat);
      if(ATgetType(pat) == AT_PLACEHOLDER) {
	ATerm type = ATgetPlaceholder((ATermPlaceholder)pat);
	if(ATgetType(type) == AT_APPL && 
	   ATgetSymbol((ATermAppl)type) == symbol_list) {
	  ATermList *listarg = va_arg(*args, ATermList *);
	  if(listarg)
	    *listarg = list;
	  return ATtrue;
	}
      }
      /* Last element was not a <list> pattern */
      if(ATgetLength(list) != 1)
	return ATfalse;

      return AT_vmatchTerm(ATgetFirst(list), pat);

      /*}}}  */

    default:
      ATerror("AT_vmatchTerm: illegal type %d\n", ATgetType(pat));
      return ATfalse;
  }
}

/*}}}  */
/*{{{  static ATbool matchArguments(ATermAppl appl, applpat) */

/**
 * Match the arguments of a function application against a term pattern.
 */

static ATbool matchArguments(ATermAppl appl, ATermAppl applpat)
{
  Symbol sym = ATgetSymbol(appl);
  Symbol psym = ATgetSymbol(applpat);
  int i, arity = ATgetArity(sym);
  int parity = ATgetArity(psym)-1; /* -1, because last arg can be <list> */
  ATerm pat;

  if(parity == -1)
    return arity == 0 ? ATtrue : ATfalse;

  for(i=0; i<parity; i++) {
    if(i >= arity)
      return ATfalse;
    if(!AT_vmatchTerm(ATgetArgument(appl,i), ATgetArgument(applpat,i)))
      return ATfalse;
  }

  /* We now reached the last argument */
  pat = ATgetArgument(applpat, parity);
  if(ATgetType(pat) == AT_PLACEHOLDER) {
    ATerm type = ATgetPlaceholder((ATermPlaceholder)pat);
    if(ATgetType(type) == AT_APPL &&
       ATgetSymbol((ATermAppl)type) == symbol_list)
    {
      ATermList *listarg = va_arg(*args, ATermList *);
      if(listarg) {
	*listarg = ATmakeList0();
	for(i=arity-1; i>=parity; i--) {
	  *listarg = ATinsert(*listarg, ATgetArgument(appl, i));
	}
      }
      return ATtrue;
    }
  }

  /* Last pattern was not '<list>' */
  if((arity-1) != parity)
    return ATfalse;

  /* Match the last argument */
  return AT_vmatchTerm(ATgetArgument(appl, parity),
		       ATgetArgument(applpat, parity));
}

/*}}}  */
/*{{{  static ATbool matchPlaceholder(ATerm t, pat) */

/**
 * Match a term against a placeholder term.
 */

static ATbool matchPlaceholder(ATerm t, ATermPlaceholder pat)
{
  ATerm type = ATgetPlaceholder(pat);
  ATbool matchAppl = ATfalse, matchStr = ATfalse, matchId = ATfalse;

  if(ATgetType(type) == AT_APPL) {
    ATermAppl pappl = (ATermAppl)type;
    Symbol psym = ATgetSymbol(pappl);
    if(psym == symbol_int && ATgetArity(psym) == 0) {	  
      /*{{{  handle pattern <int> */

      if(ATgetType(t) == AT_INT) {
	int *iarg = va_arg(*args, int *);
	if(iarg)
	  *iarg = ATgetInt((ATermInt)t);
	return ATtrue;
      }
      return ATfalse;

      /*}}}  */
    } else if(psym == symbol_real && ATgetArity(psym) == 0) {
      /*{{{  handle pattern <real> */

      if(ATgetType(t) == AT_REAL) {
	double *darg = va_arg(*args, double *);
	if(darg)
	  *darg = ATgetReal((ATermReal)t);
	return ATtrue;
      }
      return ATfalse;

      /*}}}  */
    } else if(psym == symbol_blob) {
      /*{{{  handle pattern <blob> */

      if(ATgetType(t) == AT_BLOB) {
	int *size  = va_arg(*args, int *);
	void **data = va_arg(*args, void **);
	if(size)
	  *size = ATgetBlobSize((ATermBlob)t);
	if(data)
	  *data = ATgetBlobData((ATermBlob)t);
	return ATtrue;
      }
      return ATfalse;

      /*}}}  */
    } else if(psym == symbol_placeholder) {
      /*{{{  handle pattern <placeholder> */

      if(ATgetType(t) == AT_PLACEHOLDER) {
	ATerm *type = va_arg(*args, ATerm *);
	if(type)
	  *type = ATgetPlaceholder((ATermPlaceholder)t);
	return ATtrue;
      }
      return ATfalse;

      /*}}}  */
    } else if(psym == symbol_term) {
      ATerm *term = va_arg(*args, ATerm *);
      if(term)
	*term = t;
      return ATtrue;
    } else if(streq(ATgetName(psym), "appl")) {
      matchAppl = ATtrue;
    } else if(streq(ATgetName(psym), "str")) {
      matchStr = ATtrue;
      matchAppl = ATtrue;
    } else if(streq(ATgetName(psym), "id")) {
      matchId = ATtrue;
      matchAppl = ATtrue;
    }

    if(matchAppl) {
      /*{{{  handle patterns <appl> and <str> */
      Symbol sym;
      ATermAppl appl;
      int arity, parity;
      char **name;

      if(ATgetType(t) != AT_APPL)
	return ATfalse;

      appl = (ATermAppl)t;
      sym = ATgetSymbol(appl);
      arity = ATgetArity(sym);
      parity = ATgetArity(psym)-1;

      if(ATisQuoted(sym)) {
	if(matchId)
	  return ATfalse;
      } else if(matchStr) {
	return ATfalse;
      }

      name = va_arg(*args, char **);
      if(name)
	*name = ATgetName(sym);

      return matchArguments(appl, pappl);

      /*}}}  */
    }
  }
  ATerror("matchPlaceholder: illegal pattern %t\n", pat);
  return ATfalse;
}

/*}}}  */
