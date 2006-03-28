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

#include "libprint_c.h"
#include "libstruct.h"
#include "ltsmin.h"

/* Data definition */

unsigned int Pi_pt = 0, n_partitions = 0;
       
SVCint nstate=0, nlabel=0, npar=0;
ATbool *mark;
SVCint *blockref;
SVCstateIndex *s; /* In this resides the partition */
ATermList *lab, *par; /* [[key, sources], ... ] */
INTERVAL *Pi;
BLOK *blok;
BLOCKS blocks;

SVCfile inFile[MAX_INFILES], outFile[1];
SVCbool readIndex[MAX_INFILES];
ATermTable *lab_src_tgt, *lab_tgt_src, graph, graph_i;
ATerm *label_name, *par_name; 
/* End data definition */

static int n_transitions = 0, n_states = 0;
static ATbool omitTauLoops;

int label_tau = -1;

static void Info(SVCfile *inFile)
     {
      /* Get file header info */
     fprintf(stderr, "Svc version %s %s\n", SVCgetFormatVersion(inFile),
          SVCgetIndexFlag(inFile)?"(indexed)":"");
     fprintf(stderr, " filename      %s\n", SVCgetFilename(inFile));
     fprintf(stderr, " date          %s\n", SVCgetDate(inFile));
     fprintf(stderr, " version       %s\n", SVCgetVersion(inFile));
     fprintf(stderr, " type          %s\n", SVCgetType(inFile));
     fprintf(stderr, " creator       %s\n",  SVCgetCreator(inFile));
     fprintf(stderr, " states        %ld\n", SVCnumStates(inFile));
     fprintf(stderr, " transitions   %ld\n", SVCnumTransitions(inFile));
     fprintf(stderr, " labels        %ld\n", SVCnumLabels(inFile));
     fprintf(stderr, " parameters    %ld\n", SVCnumParameters(inFile));
     fprintf(stderr, " initial state %ld\n", SVCgetInitialState(inFile));
     fprintf(stderr, " comments      %s\n", SVCgetComments(inFile));
     }
     
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
     if (!(blockref = (SVCint*) malloc(nstate * sizeof(SVCint))))
     ATerror("Cannot allocate array with block references of size %d\n", nstate);
     if (!(blocks.b = (int*) malloc(nstate * sizeof(int))))
     ATerror("Cannot allocate array with block references of size %d\n", nstate);
     if (!(s = (SVCstateIndex*) malloc(nstate * sizeof(SVCstateIndex))))
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

static void UpdateParArray(int state, int parameter) {
     ATermList newval = par[state];
     ATerm parno = (ATerm) ATmakeInt(parameter);
     if (!newval) newval = ATmakeList1(parno);
     else
     if (ATindexOf(newval, parno,0)<0) 
          newval = ATinsert(newval, parno);
     par[state] = newval;
}

static char **tau_actions = NULL;
static int num_tau_actions = 0;
static int size_tau_actions = 0;
void add_tau_action(char *action)
{
	if ( num_tau_actions == size_tau_actions )
	{
		size_tau_actions += 8;
		tau_actions = (char **) realloc(tau_actions,size_tau_actions*sizeof(char *));
	}
	tau_actions[num_tau_actions] = action;
	num_tau_actions++;
	gsVerboseMsg("marked action '%s' as a tau action\n",action);
}

bool is_tau_action(char *action)
{
	for (int i=0; i<num_tau_actions; i++)
	{
		if ( !strcmp(action,tau_actions[i]) )
		{
			return true;
		}
	}
	return false;
}

bool is_tau_mact(ATermAppl mact)
{
	ATermList l = ATLgetArgument(mact,0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl actid = ATAgetArgument(ATAgetFirst(l),0);
		if ( !is_tau_action(ATgetName(ATgetAFun(ATAgetArgument(actid,0)))) )
		{
			return false;
		}
	}
	return true;
}

SVCstateIndex ReadData(void) 
   {
   SVCstateIndex fromState, toState, initState;
   SVClabelIndex label;
   SVCparameterIndex parameter;
//   ATerm term_tau = (ATerm) gsMakeMultAct(ATmakeList0());
   if (traceLevel>0) Info(inFile); 
   nstate = SVCnumStates(inFile);
   nlabel = SVCnumLabels(inFile) + 1; // +1 for tau label
   if (classes) npar = SVCnumParameters(inFile);
   AllocData();
   label_tau = nlabel-1;
   label_name[label_tau] = (ATerm) gsMakeMultAct(ATmakeList0());
   while (SVCgetNextTransition(inFile, &fromState, &label, &toState, &parameter)) 
      {
      ATermAppl label_term = (ATermAppl) SVClabel2ATerm(inFile,label);
      if ( !gsIsMultAct(label_term) )
      {
	      label_term = (ATermAppl) ATgetArgument((ATermAppl) label_term, 0);
      }
      if ( is_tau_mact(label_term) )
      {
	      label = label_tau;
      }
      ATerm name = label_name[label];
      if (!name) {
	  name = (ATerm) gsSortMultAct(label_term);
	  for (int i=0; i<nlabel; i++)
	  {
             if ( ATisEqual(label_name[i],name) )
	     {
                     label = i;
		     break;
	     }
	  }
          label_name[label] = name;
      }
      if (classes) {
          ATerm parname = par_name[parameter];
          if (!parname) parname = par_name[parameter] = 
                SVCparameter2ATerm(inFile,parameter);
      } 
      UpdateLabArray(toState, label);
      if (classes) UpdateParArray(fromState, parameter);
      UpdateTable(lab_src_tgt[label], fromState, toState);
      UpdateTable(lab_tgt_src[label], toState, fromState);
      }   
   initState =  SVCgetInitialState(inFile); 
   if (classes == 0 && readIndex[0] && SVCclose(inFile)<0)
      {
      fprintf(stderr, "File trailer corrupt...\n");
      }
   return initState;
   }
   
static int Recode(int label) {
     int n = SVCnumLabels(inFile+1), m = SVCnumLabels(inFile), i;
     static int *newlabel = NULL, pt = 0;
     ATerm name = NULL;
     if (!newlabel) { 
          if (!(newlabel = (int *) calloc(n, sizeof(int))))
               ATerror("No allocation of newlabel (%d)",n);
          for (i=0;i<n;i++) newlabel[i] = -1;
          pt = m;
          } 
     if (label == -1) {free(newlabel);return -1;}
     if (newlabel[label]>=0) return newlabel[label];
     name = SVClabel2ATerm(inFile+1,label);
     if (ATisEqual(label_name[label] , name)) return label;
     /* ATwarning("Collision %t <-> %t\n", label_name[label], name); */ 
     for (i=0;i<m;i++) {
        if (ATisEqual(label_name[i],name)) break;
        }
     if (i< m) {newlabel[label]=i; return i;}
     /* ATwarning("New label\n"); */
     newlabel[label] = pt; return pt++;
     } 
   
void ReadCompareData(SVCstateIndex *init1, SVCstateIndex *init2) 
   {
   SVCstateIndex fromState, toState, offset = SVCnumStates(inFile);
   SVClabelIndex label;
   SVCparameterIndex parameter;
   ATerm term_tau = (ATerm) gsMakeMultAct(ATmakeList0());
   if (traceLevel>0) Info(inFile);
   nstate = SVCnumStates(inFile)+SVCnumStates(inFile+1);
   nlabel = SVCnumLabels(inFile)+SVCnumLabels(inFile+1); 
   AllocData(); 
   while (SVCgetNextTransition(inFile, &fromState, &label, &toState, &parameter))
      {
      ATerm name = label_name[label];
      if (!name) name = label_name[label] = SVClabel2ATerm(inFile,label);
      if (label_tau < 0 && ATisEqual(name, term_tau)) label_tau =  label;
      UpdateLabArray(toState, label);
      UpdateTable(lab_src_tgt[label], fromState, toState);
      UpdateTable(lab_tgt_src[label], toState, fromState);
      }
   *init1 =  SVCgetInitialState(inFile); 
   if (readIndex[0]) 
   if (SVCclose(inFile)<0)
       {
       fprintf(stderr, "File trailer corrupt...\n");
       } 
   /* Second file */
   if (traceLevel>0) Info(inFile+1);  
   while (SVCgetNextTransition(inFile+1, &fromState, &label, &toState, &parameter)) 
      {
      int label0 = label;
      label = Recode(label0); 
      {
      ATerm name = label_name[label];
      if (!name) name = label_name[label] = SVClabel2ATerm(inFile+1,label0);
      if (label_tau < 0 && ATisEqual(name, term_tau)) 
        label_tau =  label;
      UpdateLabArray(toState + offset, label);
      UpdateTable(lab_src_tgt[label], fromState +offset, toState + offset);
      UpdateTable(lab_tgt_src[label], toState + offset, fromState +offset);
      }
      }
   Recode(-1);
   *init2 =  SVCgetInitialState(inFile+1) + offset;
   if (readIndex[1]) 
   if (SVCclose(inFile+1)<0)
      {
      fprintf(stderr, "File trailer corrupt...\n");
      } 
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
      
void GetBlockBoundaries(SVCint b, SVCstateIndex *left, SVCstateIndex *right)
     {
     *left = Pi[b].left; *right = Pi[b].right;
     }
      
static void print_state(FILE *f,ATerm state)
{
	int arity = ATgetArity(ATgetAFun((ATermAppl) state));
	fprintf(f,"[");
	for (int i=0; i<arity; i++)
	{
		if ( i == 0 )
		{
			gsfprintf(f,"%P",ATgetArgument((ATermAppl) state,i));
		} else {
			gsfprintf(f,",%P",ATgetArgument((ATermAppl) state,i));
		}
	}
	fprintf(f,"]");
}

static void TransitionsGoingToBlock(SVCint b, ATermList *newlab) {
   SVCstateIndex left, right, i;
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
        if (classes) {
            print_state(stdout,SVCstate2ATerm(inFile,s[i]));
            gsfprintf(stdout, "\n"); 
            }
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
       
static SVCstateIndex MakeEquivalenceClasses(SVCstateIndex initState, 
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
     for (i=0;i<nstate;i++) {
          mark[i] = ATfalse;
          s[i] = i;
          blockref[i] = 0;
          }
     return (SVCstateIndex) result;
     }
     
static int WriteTransitions(void) {
   int b, n_tau_transitions = 0;
   SVCbool nnew; 
   for (b=0;b<nstate;b++) {
        ATerm bname = (ATerm) ATmakeInt(b);
        ATermList labels = lab[b];
        SVCstateIndex toState=SVCnewState(outFile, bname, &nnew);
        for (;!ATisEmpty(labels);labels = ATgetNext(labels)) {
             int label = ATgetInt((ATermInt) ATgetFirst(labels));
             ATerm action = label_name[label];
             ATermList sources = 
             (ATermList) ATtableGet(lab_tgt_src[label], bname);
	     if (!ATisEmpty(sources))
	     {
             SVClabelIndex labelno = SVCnewLabel(outFile, action ,&nnew);
             for (;!ATisEmpty(sources);sources=ATgetNext(sources)) {
                 SVCstateIndex fromState=SVCnewState(outFile, ATgetFirst(sources), 
                 &nnew);
                 SVCparameterIndex parameter=
                 SVCnewParameter(outFile, (ATerm)ATmakeList0(), &nnew);
                 SVCputTransition(outFile, fromState, labelno, toState, parameter);
                 n_transitions++;
                 if (label == label_tau) n_tau_transitions++;
                 }    
             }
             }
        }
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

SVCstateIndex ReturnEquivalenceClasses(SVCstateIndex initState, ATbool
     deleteTauLoops) {
     ATermList blocks = StableBlockNumbers();
     omitTauLoops = deleteTauLoops;
     /* ATwarning("Blocks = %t\n",blocks); */
     {
     SVCstateIndex result =  MakeEquivalenceClasses(initState, blocks);
     /* ATwarning("Block number of initial state: %d\n", result); */
     BlockCode(-1);
     return result;
     }   
}
       
int WriteData(SVCstateIndex initState, int omit_tauloops)
    {
    static char buf[1024];
    SVCbool nnew;
    int n_tau_transitions;
    SVCstateIndex newState = SVCnewState(outFile, 
    (ATerm) ATmakeInt(ReturnEquivalenceClasses(initState, omit_tauloops?ATtrue:ATfalse)), &nnew);
    SVCsetInitialState(outFile, newState); 
    SVCsetType(outFile, "mCRL2");
    SVCsetCreator(outFile, "ltsmin");
    n_tau_transitions = WriteTransitions();
    if (omit_tauloops == DELETE_TAULOOPS) {
         sprintf(buf, 
"branching bisimulation equivalence classes with %d nonsilent tau transitions",
         n_tau_transitions);
    } else
         sprintf(buf, "strong bisimulation equivalence classes");
    /* if (traceLevel)
         {
         ATwarning("Number of states: %d\n",n_states);
         ATwarning("Number of transitions: %d\n",n_transitions);
         if (label_tau>=0) 
         ATwarning("Number of tau steps: %d\n",n_tau_transitions);
         } */
    SVCsetComments(outFile, buf); 
    if (traceLevel) ATwarning("Info output file: \n");
    if (traceLevel) Info(outFile); 
    if (SVCclose(outFile)<0)
       {
       fprintf(stderr, "File trailer corrupt...\n");
       }         
    return EXIT_OK;     
    }


int state_arity = -1;
ATerm *state_args;
AFun state_afun;
SVCstateIndex get_new_state(SVCfile *in, SVCstateIndex s)
{
	ATermAppl state = (ATermAppl) SVCstate2ATerm(in,s);

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

	SVCbool nnew;
	return SVCnewState(outFile,(ATerm) state, &nnew);
}

SVClabelIndex get_new_label(SVCfile *in, SVClabelIndex l)
{
	SVCbool nnew;
	return SVCnewLabel(outFile,SVClabel2ATerm(in,l), &nnew);
}

SVCparameterIndex get_new_param()
{
  SVCbool nnew;
  return SVCnewParameter(outFile, (ATerm)ATmakeList0(), &nnew);
}

int WriteDataAddParam(SVCfile *in, SVCstateIndex initState, int is_branching)
{
//  SVCbool nnew;
  SVCstateIndex from,to;
  SVClabelIndex lab;
  SVCparameterIndex param;

  SVCsetType(outFile, "mCRL2+info");
  SVCsetCreator(outFile, "ltsmin2");

  SVCstateIndex init = SVCgetInitialState(in);
  ReturnEquivalenceClasses(initState,is_branching?ATtrue:ATfalse);

  SVCsetInitialState(outFile, get_new_state(in,init)); 

//  fprintf(stderr,"%i: %i\n",init,blockref[init]);
//  fprintf(stderr,"%i: %i\n",init,ATgetInt((ATermInt) BlockCode(init)));
  SVCparameterIndex new_param = get_new_param();
  while ( SVCgetNextTransition(in,&from,&lab,&to,&param) )
  {
//  	fprintf(stderr,"%i(%i) - %i -> %i(%i)\n",from,blockref[from],lab,to,blockref[to]);
//	gsfprintf(stderr,"%i(%i) = ",from,blockref[from]);print_state(stderr,SVCstate2ATerm(in,from));fprintf(stderr,"\n");
//  	fprintf(stderr,"%i(%i) - %i -> %i(%i)\n",from,ATgetInt((ATermInt) BlockCode(from)),lab,to,ATgetInt((ATermInt) BlockCode(to)));
  	SVCstateIndex new_from = get_new_state(in,from); 
  	SVCstateIndex new_to = get_new_state(in,to); 
	SVClabelIndex new_lab = get_new_label(in,lab);
	SVCputTransition(outFile,new_from,new_lab,new_to,new_param);
  }

  SVCclose(outFile);

  return EXIT_OK;
}
