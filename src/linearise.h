
/* This file contains the basic definitions for the timed mCRL 
   static semantics checker 

   Work on this file started on 12 juli 1997, based on the CWI
   technical report "The Syntax and Semantics of Timed mCRL",
   by J.F. Groote.

   Everybody is free to use this software, provided it is not
   changed.

   In case problems are encountered when using this software,
   please report them to J.F. Groote, CWI, Amsterdam, jfg@cwi.nl

   This software comes as it is. I.e. the author assumes no
   responsibility for the use of this software. 

   THIS TEKST MUST STILL BE IMPROVED */


#ifndef __TMCRL_H__
#define __TMCRL_H__

/* #include <TB.h> */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aterm2.h"
#include "gsfunc.h"

#define MAXLEN 200  /* maximum length of the name of a variable */
/* ERRORLENGTH gives the maximum length of an errormessage.
   PRINTEDTERMLENGTH gives the length of a ATerm in an errormessage.
   it should be substantially less than half ERRORLENGTH */
#define ERRORLENGTH 250
#define PRINTEDTERMLENGTH 80

#define LINEBREAKLIMIT 70
#define VERSION "0.0.0"

/* #define number long */

typedef struct specificationbasictype {
            ATermList sorts;     /* storage place for sorts */
            ATermList funcs;     /* storage place for functions */
            ATermList maps;      /* storage place for constructors */
            ATermList eqns;      /* storage place for equations */          
            ATermList acts;      /* storage place for actions */
            ATermList procs;     /* storage place for processes,
                                         uses alt, seq, par, lmer, cond,sum,
                                         com, bound, at, name, delta,
                                         tau, hide, rename, encap */
            ATermAppl init;      /* storage place for initial process */       
} specificationbasictype;

  
typedef struct string {
  char s[MAXLEN];  
  struct string *next; } string;  
  
/* void initializefmts(void);
   void finalizefmts(void); */

#endif /* __TMCRL_H__ */

