// Author(s): Muck van Weerdenburg, Bert Lisser
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bsim.h

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "aterm1.h"
#include <cassert>
#include "mcrl2/lts/lts.h"

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
  int left, right;
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
extern int nstate, nlabel; /* number of states, number of labels */
extern ATbool *mark;
extern int *blockref;
extern int *s; /* In this resides the partition */
extern ATerm *label_name;
extern ATermList  *lab; /* [[key, sources], ... ] */
extern INTERVAL *Pi;
extern int second_lts_states_offset;
/* end extern declarations and data structures */

extern int label_tau;
extern int traceLevel, optimal, classes;
extern ATermTable *lab_src_tgt, *lab_tgt_src;

/* MACRO definitions */
#define RemoveInterval(p) ((p)->mode=REMOVED)
#define isEmpty(p) ((p)->mode==EMPTY)
#define isRemoved(p) ((p)->mode==REMOVED)
#define Push(kind, lft, rgh) ((Pi[Pi_pt].mode=kind),(Pi[Pi_pt].left=lft), \
(Pi[Pi_pt].right=rgh),Pi_pt+1)
        
int  doReduce(void);
int  doBranchReduce(void);
int  doCompare(void);
int  doBranchCompare(void);
void set_tau_actions(std::vector<std::string> const*actions);
int ReadData(mcrl2::lts::lts &l);
ATerm Term(Symbol s, int d);
void ReadCompareData(mcrl2::lts::lts &l, int *init1, mcrl2::lts::lts &l2, int *init2);
int WriteData(mcrl2::lts::lts &l, int initState, bool is_branching);
int WriteDataAddParam(mcrl2::lts::lts &l, bool is_branching);
void GetBlockBoundaries(int b, int *left, int *right);
void Check(void);
void StartSplitting(void);
void Reduce(void);
void ReduceBranching(void);
void SCC(void);
bool Compare(int init1, int init2); 
bool CompareBranching(int init1, int init2);
int ReturnEquivalenceClasses(int initState, ATbool tauloops);

void bsim_reset_kernel_data();
