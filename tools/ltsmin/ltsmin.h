// Author(s): Muck van Weerdenburg, Bert Lisser
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsmin.h

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cerrno>
#include "aterm1.h"
#include <cassert>
extern "C" {
  #include "svc/svcerrno.h"
  #include "svc/svc.h"
}	
#define EXIT_OK 0
#define EXIT_NOTOK 1
#define EXIT_ERR_ARGS -1
#define EXIT_ERR_FILE -2
#define EXIT_ERR_INPUT -3

#define ERR_ARGS 1
#define ERR_FILE 2
#define CMD_HELP 3
#define CMD_VERSION 4
#define CMD_REDUCE 5
#define CMD_COMPARE 6
#define CMD_BRANCH_REDUCE 7
#define CMD_BRANCH_COMPARE 8
#define SVC_EXT  ".svc"
#define OUTFILE_EXT  ".red"

#define WITH_TAULOOPS ATfalse
#define DELETE_TAULOOPS ATtrue

/* extern declarations data structures */
#define INITSIZE 50
#define INITTAB 10
#define MAX_LOAD_PCT 70
#define MAX_INFILES 2

#define EMPTY 0
#define REMOVED 1
#define UNSTABLE 2
#define STABLE 3

/* Pi_u is stack */
typedef struct
  {
  SVCstateIndex left, right;
  int mode;
  } INTERVAL;

typedef struct
  {
  int action, parent, splitter;
  } BLOK;

typedef struct {
   int pt;
   int *b;
} BLOCKS;
/* List of candidate blocks for being splitted */

extern BLOK *blok;
extern BLOCKS blocks;
extern unsigned int Pi_pt, n_partitions;
extern SVCint nstate, nlabel; /* number of states, number of labels */
extern ATbool *mark;
extern SVCint *blockref;
extern SVCstateIndex *s; /* In this resides the partition */
extern ATerm *label_name;
extern ATermList  *lab; /* [[key, sources], ... ] */
extern INTERVAL *Pi;
/* end extern declarations and data structures */

extern int label_tau;
extern SVCfile inFile[], outFile[];
extern SVCbool readIndex[];
extern int traceLevel, optimal, classes;
extern ATermTable *lab_src_tgt, *lab_tgt_src;

/* MACRO definitions */
#define RemoveInterval(p) ((p)->mode=REMOVED)
#define isEmpty(p) ((p)->mode==EMPTY)
#define isRemoved(p) ((p)->mode==REMOVED)
#define Push(kind, lft, rgh) ((Pi[Pi_pt].mode=kind),(Pi[Pi_pt].left=lft), \
(Pi[Pi_pt].right=rgh),Pi_pt+1)

//int  parseArgs(int argc, char *argv[], int *traceLevel, int *optimal, int *classes);
//void doHelp(char *);
//void doVersion();
//int  doReduce(void);
//int  doBranchReduce(void);
//int  doCompare(void);
//int  doBranchCompare(void);
SVCstateIndex ReadData(void);
//ATerm Term(Symbol s, int d);
void ReadCompareData(SVCstateIndex *init1, SVCstateIndex *init2);
int WriteData(SVCstateIndex initState, int tau_toops);
int WriteDataAddParam(SVCfile *f, SVCstateIndex initState, int tau_toops);
void add_tau_action(std::string const& s);
//void GetBlockBoundaries(SVCint b, SVCstateIndex *left, SVCstateIndex *right);
//void Check(void);
void StartSplitting(void);
void Reduce(void);
void ReduceBranching(void);
void SCC(void);
int Compare(SVCstateIndex init1,SVCstateIndex init2);
int CompareBranching(SVCstateIndex init1,SVCstateIndex init2);
//SVCstateIndex ReturnEquivalenceClasses(SVCstateIndex initState, ATbool tauloops);
