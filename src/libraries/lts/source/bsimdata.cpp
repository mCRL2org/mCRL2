/*
   SVC tools -- the SVC (Systems Validation Centre) tool set

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 $Id: bsimdata.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <algorithm>
#include <string>
#include "libprint_c.h"
#include "libprint.h"
#include "libstruct.h"
#include "lts/detail/bsim.h"

#define ATisAppl(t) (ATgetType(t) == AT_APPL)

using namespace std;
using namespace mcrl2::lts;

/* Data definition */

int second_lts_states_offset;

unsigned int Pi_pt = 0, n_partitions = 0;
       
int nstate=0, nlabel=0, npar=0;
ATbool *mark;
int *blockref;
int *s; /* In this resides the partition */
ATermList *lab, *par; /* [[key, sources], ... ] */
INTERVAL *Pi;
BLOK *blok;
BLOCKS blocks;

ATermTable *lab_src_tgt, *lab_tgt_src, graph, graph_i;
ATerm *label_name, *par_name; 
/* End data definition */

static int n_transitions = 0, n_states = 0;
static ATbool omitTauLoops;

int label_tau = -1;

static ATerm *MakeArrayOfATerms(int n)
     {
     ATerm *result = (ATerm *) calloc(n, sizeof(ATerm));
     if (!result) ATerror("Cannot allocate array with ATerms of size %d",
          n);
     ATprotectArray(result, n);
     return result;
     }
          
static void AllocData(void)
     {
     int i;
     if (!(mark = (ATbool*) malloc(nstate * sizeof(ATbool))))
     ATerror("Cannot allocate boolean array of size %d\n", nstate);
     if (!(blockref = (int*) malloc(nstate * sizeof(int))))
     ATerror("Cannot allocate array with block references of size %d\n", nstate);
     if (!(blocks.b = (int*) malloc(nstate * sizeof(int))))
     ATerror("Cannot allocate array with block references of size %d\n", nstate);
     if (!(s = (int*) malloc(nstate * sizeof(int))))
     ATerror("Cannot allocate array with state numbers of size %d\n", nstate);
     if (!(lab = (ATermList*) calloc(nstate,  sizeof(ATermList))))
     ATerror("Cannot allocate array with [labels] of size %d\n", nstate);
     ATprotectArray((ATerm*) lab, nstate); 
     if (!(Pi = (INTERVAL*) calloc(2*nstate, sizeof(INTERVAL))))
        ATerror("Indexed array Pi is not allocated (%d)\n",2*nstate);
     if (!(lab_src_tgt = (ATermTable*) malloc(nlabel*sizeof(ATermTable))))
           ATerror("Array of tables is not allocated (%d)\n",nlabel); 
     if (!(lab_tgt_src = (ATermTable*) malloc(nlabel*sizeof(ATermTable))))
           ATerror("Array of tables is not allocated (%d)\n",nlabel);
     if (!(blok = (BLOK*) malloc(2*nstate * sizeof(BLOK))))
        ATerror("BLOK is not allocated (%d)\n",nstate); 
     for (i=0;i<nlabel;i++) {
          if (!(lab_src_tgt[i] =  ATtableCreate(INITTAB, MAX_LOAD_PCT)))
               ATerror("Not possible to create table (%d)",i);
          if (!(lab_tgt_src[i] =  ATtableCreate(INITTAB, MAX_LOAD_PCT)))
               ATerror("Not possible to create table (%d)",i);
          } 
     label_name = MakeArrayOfATerms(nlabel);
     for (i=0;i<nstate;i++)
          { 
          lab[i] = ATempty;
          }
     if (classes) {
          if (!(par = (ATermList*) calloc(nstate,  sizeof(ATermList))))
          ATerror("Cannot allocate array with [parameters] of size %d\n", nstate);
          ATprotectArray((ATerm*) par, nstate);
          par_name = MakeArrayOfATerms(npar); 
          for (i=0;i<nstate;i++) { 
              par[i] = ATempty;
              }
          }
     blocks.pt = 0; 
     /* StartSplitting(); */         
     }

void StartSplitting(void) {
     int i, nstate2 = 2* nstate;
     Pi_pt = 0; 
     n_partitions = 0;     
     for (i=0;i<nstate;i++)
          { 
          mark[i] = ATfalse;
          s[i] = i;
          blockref[i] = Pi_pt;
          }
     for (i=0;i<nstate2;i++)
          {
          Pi[i].mode = EMPTY;
          }
     blok[Pi_pt].action = -1;
     blok[Pi_pt].parent = 0;
     blok[Pi_pt].splitter = 0;     
     Pi_pt = Push(STABLE,0,nstate);
     n_partitions++;
     } 
/*
--------------- Strongly Connected Components -------------------------
*/
static int *dfsn2state, *visited, dfsn;

static void ExtraNode(void) {
     /* Add extra node which is connected to each point */
     int i;
     ATermList states = ATempty;
     for (i=0;i<nstate;i++) states = ATinsert(states, (ATerm) ATmakeInt(i));
     ATtablePut(graph, (ATerm) ATmakeInt(nstate), (ATerm) ATreverse(states));
} 

static void RemoveExtraNode(void) {
     /* Remove extra node which is connected to each point */
     ATtableRemove(graph, (ATerm) ATmakeInt(nstate));
     dfsn--; 
} 

void DfsNumbering(ATerm t) {
     int d = ATgetInt((ATermInt) t);
     if (visited[d]>=0) return;
     visited[d] = 0;
     {
     ATermList targets = (ATermList) ATtableGet(graph, t);
     if (targets) {
          for (;!ATisEmpty(targets);targets=ATgetNext(targets)) {
               ATerm target = ATgetFirst(targets);
               DfsNumbering(target);
               }
     }
     if (dfsn>nstate || dfsn <0) ATerror("Wrong3: %d\n",dfsn);
     if (d>nstate || d <0) ATerror("Wrong4: %d\n",d);
     visited[d] = dfsn;
     dfsn2state[dfsn] = d;
     dfsn++;
     }
} 

int TakeComponent(ATerm t) {
     static int s_pt = 0;
     int d = ATgetInt((ATermInt) t);
     /* ATwarning("Help d = %d visited[d] = %d dfsn = %d\n",d, visited[d], dfsn); */
     if (visited[d]<0) return s_pt;
     {
     ATermList sources = (ATermList) ATtableGet(graph_i, t);
     if (visited[d] == dfsn) {
          dfsn--; /* Removal of deepest node */
          while (dfsn>=0 && visited[dfsn2state[dfsn]]== -1) dfsn--;
          }
     if (d>=nstate || d <0) ATerror("Wrong: %d\n",d);
     visited[d] = -1;
     if (sources) {
          for (;!ATisEmpty(sources);sources=ATgetNext(sources)) {
               ATerm source = ATgetFirst(sources);
               TakeComponent(source);
               }
     }
     if (s_pt>=nstate || s_pt <0) ATerror("Wrong2: %d\n",s_pt);
     s[s_pt] = d; s_pt++;
     blockref[d] = Pi_pt;
     return s_pt;
     }    
} 

static void MakeUnitPartition(void) {
     int i;
     /* ATwarning("Make Unit Partition\n"); */
     for (i=0;i<nstate;i++) {
          blockref[i] = i;
          s[i]=i;
          Pi[i].left = i; Pi[i].right = i+1;
          Pi[i].mode = STABLE;
     }
     n_partitions = i;
     Pi_pt = i;
}

void SCC(void) {
     int i, left = 0;
     if (label_tau<0) {MakeUnitPartition(); return;}
     graph = lab_src_tgt[label_tau];
     graph_i = lab_tgt_src[label_tau];
     ExtraNode(); 
     if (!(visited = (int*) calloc(nstate+1, sizeof(int))))
        ATerror("Visited is not allocated (%d)\n",nstate);
     for (i=0;i<=nstate;i++) visited[i] = -1;
     if (!(dfsn2state = (int*) calloc(nstate+1, sizeof(int))))
        ATerror("Dfsn2state is not allocated (%d)\n",nstate);
     DfsNumbering((ATerm) ATmakeInt(nstate));
     RemoveExtraNode();
     dfsn--;
     while (dfsn>=0) { 
          Pi[Pi_pt].left = left;
          left = Pi[Pi_pt].right = TakeComponent(
               (ATerm) ATmakeInt(dfsn2state[dfsn]));
          Pi[Pi_pt].mode = STABLE;
          Pi_pt++;n_partitions++;
          }
     /* for (i=0;i<Pi_pt;i++) ATwarning("(%d,%d)\n",Pi[i].left, Pi[i].right); */
     free(visited);
     free(dfsn2state);                 
}
/*
--------------- End Strongly Connected Components -------------------------
*/     
               
static void UpdateTable(ATermTable db, int key, int val) {
     ATerm newkey = (ATerm) ATmakeInt(key);
     ATermList newval = (ATermList) ATtableGet(db , newkey); 
     if (!newval) 
          newval = ATmakeList1((ATerm) ATmakeInt(val));
     else
          newval = ATinsert(newval, (ATerm) ATmakeInt(val));
     ATtablePut(db, newkey, (ATerm) newval);
}

static void UpdateLabArray(int state, int label) {
     ATermList newval = lab[state];
     ATerm labno = (ATerm) ATmakeInt(label);
     if (!newval) newval = ATmakeList1(labno);
     else
     if (ATindexOf(newval, labno,0)<0) 
          newval = ATinsert(newval, labno);
     lab[state] = newval;
}

static vector<string> *tau_actions = NULL;
void set_tau_actions(vector<string> *actions)
{
	tau_actions = actions;
}

static ATerm apply_hiding(ATerm act)
{
  if ( (tau_actions != NULL) && ATisAppl(act) )
  {
    if ( gsIsMultAct((ATermAppl) act) )
    {
      ATermList l = ATLgetArgument((ATermAppl) act,0);
      ATermList m = ATmakeList0();
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        if ( find(tau_actions->begin(),tau_actions->end(),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(ATAgetFirst(l),0),0)))) == tau_actions->end() )
        {
          m = ATinsert(m,ATgetFirst(l));
        }
      }
      act = (ATerm) gsMakeMultAct(m);
    } else if ( ATgetArity(ATgetAFun((ATermAppl) act)) == 0 )
    {
      string s(ATgetName(ATgetAFun((ATermAppl) act)));
      string::size_type pos = s.find("(");
      if ( pos != string::npos )
      {
        s = s.substr(0,pos);
      }
      if ( find(tau_actions->begin(),tau_actions->end(),s) != tau_actions->end() )
      {
        act = (ATerm) ATmakeAppl0(ATmakeAFun("tau",0,ATtrue));
      }
    }
  }

  return act;
}

static int get_label_index(lts &l, unsigned int idx, int tau_idx, int offset = 0)
{
  ATerm label_term = l.label_value(idx);
  if ( ATisAppl(label_term) && is_timed_pair((ATermAppl) label_term) )
  {
    label_term = ATgetArgument((ATermAppl) label_term, 0);
  }
  int label = idx+offset;
  if ( l.is_tau(idx) )
  {
    label = tau_idx;
  }
  if (!label_name[label]) {
    label_term = apply_hiding(label_term);
    if ( ATisAppl(label_term) && gsIsMultAct((ATermAppl) label_term) )
    {
      label_term = (ATerm) gsSortMultAct((ATermAppl) label_term);
    }
    for (int i=0; i<nlabel; i++)
    {
      if ( ATisEqual(label_name[i],label_term) )
      {
        label = i;
        break;
      }
    }
    label_name[label] = label_term;
  }
  return label;
}

int ReadData(lts &l) 
   {
   nstate = l.num_states(); 
   nlabel = l.num_labels() + 1; // +1 for tau label
   AllocData();
   label_tau = nlabel-1;
   if ( l.get_type() == lts_mcrl2 )
   {
     label_name[label_tau] = (ATerm) gsMakeMultAct(ATmakeList0());
#ifdef MCRL2_BCG
   } else if ( l.get_type() == lts_bcg )
   {
     label_name[label_tau] = (ATerm) ATmakeAppl0(ATmakeAFun("i",0,ATtrue));
#endif
   } else {
     label_name[label_tau] = (ATerm) ATmakeAppl0(ATmakeAFun("tau",0,ATtrue));
   }
   transition_iterator i(&l);
   for (; i.more(); ++i) 
      {
      int label = get_label_index(l,i.label(),label_tau);
      UpdateLabArray(i.to(), label);
      UpdateTable(lab_src_tgt[label], i.from(), i.to());
      UpdateTable(lab_tgt_src[label], i.to(), i.from());
      }   
   return l.initial_state(); 
   }
   
static int recode_num_lab2 = 0; // set by ReadCompareData
static void Recode(int lts_label, int label) {
     static int *newlabel = NULL;
     int i;

     if (!newlabel) { 
          if (!(newlabel = (int *) calloc(recode_num_lab2, sizeof(int))))
               ATerror("No allocation of newlabel (%d)",recode_num_lab2);
          for (i=0;i<recode_num_lab2;i++) newlabel[i] = -1;
          } 
     if (lts_label == -1) {free(newlabel);}

     if (newlabel[lts_label]>=0) return;

        newlabel[lts_label] = label;
     } 

static void pp_lts(lts &l)
{
  for (unsigned int i=0; i<l.num_labels(); i++)
  {
    l.set_label(i,(ATerm) ATmakeAppl0(ATmakeAFun(PrintPart_CXX(l.label_value(i),ppDefault).c_str(),0,ATtrue)),l.is_tau(i));
  }
}

void ReadCompareData(lts &l1, int *init1, lts &l2, int *init2) 
   {
     if ( l1.get_type() != l2.get_type() )
     {
       if ( l1.get_type() == lts_mcrl2 )
       {
         pp_lts(l1);
       } else if ( l2.get_type() == lts_mcrl2 )
       {
         pp_lts(l2);
       }
     }

   recode_num_lab2 = l2.num_labels();

   second_lts_states_offset = l1.num_states();
   int offset = second_lts_states_offset;
   
   nstate = l1.num_states()+l1.num_states(); 
   nlabel = l1.num_labels()+l2.num_labels()+1; 
   AllocData();
   label_tau = nlabel-1;
   if ( l1.get_type() == lts_mcrl2 )
   {
     label_name[label_tau] = (ATerm) gsMakeMultAct(ATmakeList0());
#ifdef MCRL2_BCG
   } else if ( l1.get_type() == lts_bcg )
   {
     label_name[label_tau] = (ATerm) ATmakeAppl0(ATmakeAFun("i",0,ATtrue));
#endif
   } else {
     label_name[label_tau] = (ATerm) ATmakeAppl0(ATmakeAFun("tau",0,ATtrue));
   }

   for (transition_iterator i(&l1); i.more(); ++i)
      {
      int label = get_label_index(l1,i.label(),label_tau);
      UpdateLabArray(i.to(), label);
      UpdateTable(lab_src_tgt[label], i.from(), i.to());
      UpdateTable(lab_tgt_src[label], i.to(), i.from());
      }
   *init1 = l1.initial_state(); 

   /* Second file */
   for (transition_iterator i(&l2); i.more(); ++i)
      {
      int label = get_label_index(l2,i.label(),label_tau,l1.num_labels());
      Recode(i.label(),label); 
      UpdateLabArray(i.to() + offset, label);
      UpdateTable(lab_src_tgt[label], i.from() +offset, i.to() + offset);
      UpdateTable(lab_tgt_src[label], i.to() + offset, i.from() +offset);
      }
   Recode(-1,-1);
   *init2 = l2.initial_state() + offset;
   } 
     
static ATermList Union(ATermList t1s, ATermList t2s)
     {
     ATermList result = t2s;
     /* ATwarning("Arguments union %t %t",t1s,t2s); */ 
     for (;!ATisEmpty(t1s);t1s=ATgetNext(t1s))
          {ATerm t1 = ATgetFirst(t1s);
          if (ATindexOf(t2s, t1,0)<0) result = ATinsert(result, t1);
          }
     return result;
     }

static ATerm BlockCode(int b) {
     static ATermIndexedSet indeks = NULL;
     int d = -1;
     ATbool nnew;
     if (b< 0) {
     /* ATwarning("ATindexedSetDestroy\n"); */
     ATindexedSetDestroy(indeks);indeks = NULL;return NULL;}
     if (!indeks) {
          /* ATwarning("ATindexedSetCreate\n"); */
          indeks = ATindexedSetCreate(INITSIZE, MAX_LOAD_PCT);
          }
    /*  ATwarning("ATindexedSetPut %d %d\n",indeks, b); */
     d = ATindexedSetPut(indeks, (ATerm) ATmakeInt(b), &nnew);   
     return (ATerm) ATmakeInt(d);
}

static ATermList  BlockNumbers(ATermList sources)
     {
     ATermList result = ATempty;
     for (;!ATisEmpty(sources);sources=ATgetNext(sources))
          {
          int source = ATgetInt((ATermInt) ATgetFirst(sources));
          ATerm block = BlockCode(blockref[source]);
          if (ATindexOf(result, block,0)<0) result = ATinsert(result, block); 
          }
     return result;
     }
      
void GetBlockBoundaries(int b, int *left, int *right)
     {
     *left = Pi[b].left; *right = Pi[b].right;
     }
      
static void TransitionsGoingToBlock(int b, ATermList *newlab) {
   int left, right, i;
   int newb = ATgetInt((ATermInt) BlockCode(b));
   ATerm bb = (ATerm) ATmakeInt(newb);
   ATermList newlabels = ATempty;
   GetBlockBoundaries(b, &left, &right);
   if (classes) fprintf(stdout,
   "--------------------------- block %d --------------------------\n", (int) b);
   /* ATwarning("TransitionGoingTo b = %d newb = %d\n",b, newb); */
   for (i = left; i < right;i++) {
        ATermList labels = lab[s[i]], pars = ATempty; 
        ATerm ss = (ATerm) ATmakeInt(s[i]);
        for (;!ATisEmpty(labels);labels = ATgetNext(labels)) {
             int label = ATgetInt((ATermInt) ATgetFirst(labels));            
             ATermList val = (ATermList) ATtableGet(lab_src_tgt[label], bb);
             ATermList sources = (ATermList) ATtableGet(lab_tgt_src[label], ss);
             ATermList newsources = BlockNumbers(sources);
             if (val) 
                    newsources = Union(val, newsources);
             if (omitTauLoops && label_tau == label) 
                  newsources = ATremoveElement(newsources, bb);
             if (!val || !ATisEqual(val, newsources))
                  ATtablePut(lab_src_tgt[label], bb, (ATerm) newsources); 
             if (ATindexOf(newlabels, ATgetFirst(labels),0)<0)
                  newlabels = ATinsert(newlabels, ATgetFirst(labels));                  
             }
        if (classes && npar > 1) {
             pars = par[s[i]];
             ATfprintf(stdout, "Summands:\t");
             for (;!ATisEmpty(pars);pars=ATgetNext(pars))
                 {
                  ATfprintf(stdout, "%t ",
                        par_name[ATgetInt((ATermInt) ATgetFirst(pars))]);
                  }
             ATfprintf(stdout, "\n");
             }        
        }
     newlab[newb] = newlabels;    
}    

static void SwapClearTables(void) {
     int i; 
     for (i=0;i<nlabel;i++) {
          ATermTable swap = lab_tgt_src[i];
          ATtableReset(swap);
          lab_tgt_src[i] = lab_src_tgt[i];
          lab_src_tgt[i] = swap;
          }
     }
       
static int MakeEquivalenceClasses(int initState, 
     ATermList blocks) {
     int i;
     ATermList *newlab = NULL;
     int result = -1;
     n_states = ATgetLength(blocks);
     if (!(newlab = (ATermList*) calloc(n_states,  sizeof(ATermList))))
     ATerror("Cannot allocate array with [labels] of size %d\n", n_states);
     ATprotectArray((ATerm*) newlab, n_states);
     for (i=0;i<nlabel;i++) ATtableReset(lab_src_tgt[i]);
     for (;!ATisEmpty(blocks);blocks=ATgetNext(blocks))
         {int b = ATgetInt((ATermInt) ATgetFirst(blocks));
         TransitionsGoingToBlock(b, newlab);
         }
     SwapClearTables();
     ATunprotectArray((ATerm*) lab);
     free(lab);
     lab = newlab;
     for (i=0;i<n_states;i++)
         {
         ATerm bb = (ATerm) ATmakeInt(i);
         ATermList labels = lab[i];
         
         for (;!ATisEmpty(labels);labels=ATgetNext(labels))
           {int label = ATgetInt((ATermInt) ATgetFirst(labels));
           ATermList sources = (ATermList) ATtableGet(lab_tgt_src[label], bb);
           for (;!ATisEmpty(sources);sources=ATgetNext(sources)) {
           ATermList tgts = (ATermList) 
                ATtableGet(lab_src_tgt[label],ATgetFirst(sources));
           if (!tgts)
                ATtablePut(lab_src_tgt[label], ATgetFirst(sources),
                     (ATerm) ATmakeList1(bb));
           else {
                if (ATindexOf(tgts, ATgetFirst(sources), 0) < 0) tgts =
                     ATinsert(tgts, ATgetFirst(sources));
                ATtablePut(lab_src_tgt[label], ATgetFirst(sources),(ATerm) tgts);
                }
           }
           }
         }
     result = ATgetInt((ATermInt) BlockCode(blockref[initState]));     
     nstate = n_states;
     /* Pi_pt = 0;
     n_partitions = 0; */
     for (i=0;i<nstate;i++) { // XXX What is the purpose of this loop?
          mark[i] = ATfalse;
          s[i] = i;
          //blockref[i] = 0;
          }
     return (int) result;
     }
     
static int WriteTransitions(lts &l) {
   int b, n_tau_transitions = 0;
   DECL_A(label2new,int,nlabel);
   for (b=0;b<nstate;b++) {
     l.add_state();
   }
   for (b=0;b<nstate;b++) {
        ATermList labels = lab[b];
        for (;!ATisEmpty(labels);labels = ATgetNext(labels)) {
             int label = ATgetInt((ATermInt) ATgetFirst(labels));
             if ( label_name[label] != NULL )
             {
               label2new[label] = l.add_label(label_name[label],label==label_tau);
               label_name[label] = NULL;
             }
             ATermList sources = 
             (ATermList) ATtableGet(lab_tgt_src[label], (ATerm) ATmakeInt(b));
	     if (!ATisEmpty(sources))
	     {
             for (;!ATisEmpty(sources);sources=ATgetNext(sources)) {
                 int fromState=ATgetInt((ATermInt) ATgetFirst(sources));
                 l.add_transition(fromState,label2new[label],b);
                 n_transitions++;
                 if (label == label_tau) n_tau_transitions++;
                 }    
             }
             }
        }
   FREE_A(label2new);
   return n_tau_transitions;
   }
   
/*static void TestTransitions(void) { 
int label;
for (label=0;label<nlabel;label++) {
ATwarning("Test: %d: %t\n",label, ATtableKeys(lab_tgt_src[label]));
}
}*/
  
static ATermList  StableBlockNumbers(void)
/* returns a list of the block numbers of all stable blocks */
     {
     ATermList result = ATempty;
     int i, cnt = 0;
     /* if (traceLevel) ATwarning("Highest possible block number %d\n",n_partitions-1); */
     for (i = n_partitions;i>=0; --i)
     if (Pi[i].mode==STABLE)
          result = (cnt++, ATinsert(result, (ATerm) ATmakeInt(i)));
     /* if (traceLevel) ATwarning("Number of blocks: %d\n", cnt); */
     return result;
     }

int ReturnEquivalenceClasses(int initState, ATbool
     deleteTauLoops) {
     ATermList blocks = StableBlockNumbers();
     omitTauLoops = deleteTauLoops;
     /* ATwarning("Blocks = %t\n",blocks); */
     {
     int result =  MakeEquivalenceClasses(initState, blocks);
     /* ATwarning("Block number of initial state: %d\n", result); */
     BlockCode(-1);
     return result;
     }   
}
       
int WriteData(lts &l, int initState, bool omit_tauloops)
    {
    int newState = ReturnEquivalenceClasses(initState, omit_tauloops?ATtrue:ATfalse);
    l.reset(l.get_type(),false);
    WriteTransitions(l);
    l.set_initial_state(newState);
    return 1;     
    }


int state_arity = -1;
ATerm *state_args;
AFun state_afun;
void set_new_state(lts &l, unsigned int s)
{
	ATermAppl state = (ATermAppl) l.state_value(s);

	if ( state_arity < 0 )
	{
		state_arity = ATgetArity(ATgetAFun(state));
		state_args = (ATerm *) malloc((state_arity+1)*sizeof(ATerm));
		state_afun = ATmakeAFun(ATgetName(ATgetAFun(state)),state_arity+1,ATfalse);
	}

	for (int i=0; i<state_arity; i++)
	{
		state_args[i] = ATgetArgument(state,i);
	}
	state_args[state_arity] = (ATerm) gsMakeDataExprNat_int(blockref[s]);

	state = ATmakeApplArray(state_afun,state_args);

	l.set_state(s,(ATerm) state);
}

int WriteDataAddParam(lts &l, bool is_branching)
{
  ReturnEquivalenceClasses(l.initial_state(),is_branching?ATtrue:ATfalse);

  set_new_state(l,l.initial_state()); 
  for (unsigned int i=0; i<l.num_states(); i++)
  {
    set_new_state(l,i);
  }

  return 1;
}
