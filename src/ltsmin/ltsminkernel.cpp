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

 $Id: bsimkernel.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include "ltsmin.h"

static ATbool RefineBranching(int action, int splitter);

static ATbool branching = ATfalse;

static int offset = -1;

static int IndexOf(ATermList ls, int k, int i) {
     for (;!ATisEmpty(ls);ls=ATgetNext(ls),i++) 
     if (ATgetInt((ATermInt) ATgetFirst(ls))==k) return i;
     return -1;
}
     
static ATermList GetLabels(INTERVAL *p)
   {
   SVCstateIndex left = p->left, right = p->right;
   ATermList result = branching?ATmakeList1((ATerm) ATmakeInt(label_tau)):
        ATempty;
   /* Tau must be the last evaluated action */
   int i;
   for (i=left;i<right;i++) {
       ATermList labels = lab[s[i]];
       for (;!ATisEmpty(labels);labels=ATgetNext(labels))
          {
          if (IndexOf(result, ATgetInt((ATermInt) ATgetFirst(labels)),0)<0)
               result = ATinsert (result, ATgetFirst(labels));
          }
       } 
       return result;
   }

/* static ATbool AllTausAreExitting(SVCstateIndex p) {
     ATermList 
     targets = (ATermList) ATtableGet(lab_src_tgt[label_tau],(ATerm) ATmakeInt(p));
     int b = blockref[p];
     if (!targets) return ATtrue;
     for (;!ATisEmpty(targets);targets=ATgetNext(targets))
          {ATerm target = ATgetFirst(targets);
          
          if (blockref[ATgetInt((ATermInt) target)] != b) return ATfalse;
          }
    return ATtrue;
} */

static void AddBlock(int b) {
   int i = 0;
   for (;i<blocks.pt;i++) {
        if (blocks.b[i]==b) return;
   }
   blocks.b[blocks.pt++] = b;
}

static void Unstable(int a, INTERVAL *p)
     {
     ATermTable act = lab_tgt_src[a];
/* Returns a set of blocks which are candidates for splitting */     
     SVCstateIndex left = p->left, right = p->right, i;
     for (i=left;i<right;i++)
        { 
        SVCstateIndex target = s[i];
        if (IndexOf(lab[target],a,0)<0) continue;
        {
        ATermList sources = 
             (ATermList) ATtableGet(act, (ATerm) ATmakeInt(target));
        /* if (!sources) sources = ATmakeList0(); */
        for (;!ATisEmpty(sources);sources = ATgetNext(sources))
             {
             SVCstateIndex source = ATgetInt((ATermInt) ATgetFirst(sources));
             int b = blockref[source];
             mark[source] = ATtrue;
             AddBlock(b);
             }
        }
        }
     }
     
static int partition(SVCstateIndex left, SVCstateIndex right)
/* returns the left boundary of the second interval */
     {
     while (left < right)
          {
          for (;!mark[s[left]];left++) if (left == right) return left+1;
          for (;mark[s[right]];right--)
               {
               /* mark[s[right]] = ATfalse; */ 
               if (left == right) return right;
               } 
               {
               SVCstateIndex swap = s[left];
               s[left] = s[right];
               s[right] = swap;
               /* mark[swap] = ATfalse; */
               right--; left++;   
               }
          }
      if (left == right) 
           {
           SVCstateIndex swap = s[left];
           if (mark[swap])
                {
                /* mark[swap] = ATfalse; */
                return left;
                }
           left++;
           }
      return left;
      }
          
static ATbool split(INTERVAL *p, INTERVAL *p1, INTERVAL *p2)
     {
     SVCstateIndex left = p->left, right = p->right, left_bound_p2;
     left_bound_p2 = partition(left, right - 1);
     if (left_bound_p2 == left) {
          int i;
          for (i=left;i<right;i++) mark[s[i]] = ATfalse;
          return ATfalse;
          }
     if (left_bound_p2 == right) return ATfalse;
     p1->left = left; p1->right= p2->left = left_bound_p2;
     p2->right = right;
     p1->mode = p2->mode = UNSTABLE;
     return ATtrue;
     }
     
static ATbool isParentOf(int parent, int b) {
     for (;b>0&&b!=parent;b=blok[b].parent);
     return (b==parent)?ATtrue:ATfalse;
}
          
static void UpdateBlock(int b, INTERVAL *p)
     {
     SVCstateIndex i = p->left, right = p->right;
     for (;i<right;i++)
         blockref[s[i]] = b;
     }
     
static void PushUnstableMark(int action, int splitter, int parent, INTERVAL *p)
     {
     /* Push the new interval on the unstable stack,
     and assign to the belonging states a new block number,
     which is the (relative) address of the block on stack */
     Pi_pt = n_partitions; 
     /* while (!isEmpty(Pi + Pi_pt)) Pi_pt++; */ 
     /* ATwarning("QQQ: PushUnstableMark %d\n", Pi_pt); */  
     Pi[Pi_pt] = *p;
     blok[Pi_pt].action = action;
     blok[Pi_pt].splitter = splitter;
     blok[Pi_pt].parent = parent; 
     UpdateBlock(Pi_pt, p);
     if (action>=0) {
          int i = p->left, right = p->right;
          for (;i<right;i++) mark[s[i]] = ATfalse;
     }
     Pi_pt++; 
     if (n_partitions < Pi_pt) n_partitions = Pi_pt;
     }
     
static void ClearMarks(int k) {    
     for (;k<blocks.pt;k++) {
          INTERVAL *p = Pi + blocks.b[k];
          int i = p->left;
          for (;i<p->right;i++) mark[s[i]] = ATfalse;
          }
     blocks.pt = 0;
     }
                                          
static ATbool Refine(int action, int splitter)
     {
     
/* Splits the blocks in "blocks" into two blocks.
One contains the marked states, the other contains the unmarked states.
Both block become unstable blocks. They will later used as occasion to
split blocks.  */

     INTERVAL *p = Pi+splitter;
     ATbool result = ATfalse;
     int i = 0;
     for (;i<blocks.pt && !isRemoved(p);i++)
          {
          INTERVAL p1[1], p2[1];
          int b = blocks.b[i];
          /* fprintf(stderr, "b = %d: ",b); */
          
          if (!split(Pi+b, p1, p2)) continue; 
          /* Removals */
          PushUnstableMark(-1, splitter, b, p1);
          PushUnstableMark(action, splitter, b, p2);
          /* {int i;for (i=0;i<p2->right;i++) if (mark[s[i]]) 
               ATerror("Mark is not cleared %d\n",i);} */
          RemoveInterval(Pi+b); 
	  if (!result) result = ATtrue;        
          }
     ClearMarks(i);
     return result;
     }
                 
static ATbool CheckUnstableBlock(int splitter)
/* for all labels while p not splitted */
     {
     INTERVAL *p = Pi+splitter;
     ATermList labels = GetLabels(p);
     ATbool result = ATfalse;
     for (;!ATisEmpty(labels) && !isRemoved(p);
          labels = ATgetNext(labels))
          {
          int a = ATgetInt((ATermInt) ATgetFirst(labels));
          Unstable(a, p);
          if (branching)
               {if (RefineBranching(a, splitter)) result = ATtrue;}
          else 
	       {if (Refine(a, splitter)) result = ATtrue;}   
          }
     return result;
     }
    
static int NumberOfPartitions(void) {
     unsigned int i, cnt = 0;
     for (i=0;i<n_partitions;i++) {
         INTERVAL *p = Pi + i;
	 if (p->mode == STABLE) {
	     p->mode = UNSTABLE;
	     cnt++;
	     }
     }
     return cnt; 
} 
   
static ATbool ReduceKernel(void)
/* 
For all unstable blocks refine partition until there are no unstable
blocks */
     {
     ATbool result = ATfalse; 
     Pi_pt = n_partitions;
     while (Pi_pt > 0)
       {
       INTERVAL *p = Pi+ (--Pi_pt);
       /* ATwarning("QQ: %d\n",p->removed); */
       if (p->mode == UNSTABLE)
            {
            p->mode = STABLE;
            if (CheckUnstableBlock(Pi_pt)) result = ATtrue; 
            }
       }
     return result;
     }
          
void Reduce(void)
/* 
For all unstable blocks refine partition until there are no unstable
blocks */
     {
     int cnt = 0, nBlocks = 0, last_nBlocks = 0;
     StartSplitting();
     do {
       nBlocks = NumberOfPartitions(); 
       if (traceLevel && cnt>=2 && last_nBlocks != nBlocks) 
     ATwarning("Extra cycle needed. Number of blocks is increased from %d to %d\n", 
           last_nBlocks, nBlocks); 
       cnt++; last_nBlocks = nBlocks; 
     } while (ReduceKernel() && branching); 
     /*
    if (traceLevel && branching) ATwarning("Cycle %d finished. Number of partitions: %d\n",
       cnt, nBlocks); 
     */     
     }

void ReduceBranching(void) {
     if (label_tau >= 0) branching = ATtrue;
     Reduce();
}     
/* ------------------- Branching bisimulation ----------------------------*/

static ATbool MarkTau(int b,  INTERVAL *p2) {
     int left = p2->left, right = p2->right, i;
     ATbool result = ATfalse;
     /* ATwarning("Entry MarkTau b = %d left = %d right = %d\n", b, left, right); */
     for (i=left;i<right;i++) {
          ATermList sources = (ATermList) 
               ATtableGet(lab_tgt_src[label_tau],(ATerm) ATmakeInt(s[i]));
               
          if (!sources) continue;
          /* ATwarning("Sources = %t\n",sources); */
          for (;!ATisEmpty(sources);sources=ATgetNext(sources))
               {ATerm source = ATgetFirst(sources);
               int d = ATgetInt((ATermInt) source);
               if (blockref[d] == b && !mark[d]) result = mark[d] = ATtrue;
               }      
     }
     return result;
}
     
static ATbool RefineBranching(int action, int splitter)
     {
     INTERVAL *p = Pi+splitter;
     ATbool result = ATfalse;
     int i = 0; 
     for (;i<blocks.pt && !isRemoved(p); i++)
          {
          ATbool stop = ATfalse;
          int b = blocks.b[i];
          INTERVAL p1[1], p2[1], q[1], *r = Pi + b;
          if (action == label_tau && b == splitter) {
               int i;
               for (i=p->left;i<p->right;i++) mark[s[i]] = ATfalse;
               continue;
               }
          q->right = r->right;
          if (!split(r, p1, p2)) continue;
          q->left = p2->left;
          q->mode = UNSTABLE;
          while (MarkTau(b, p2)) {
          /* ATwarning("Inside Mark Tau: %d %d\n",p1->left, p1->right); */
               r=p1;
               if (!split(r, p1, p2) /* No nonmarked states */ 
               /* && p1->left == p1->right */ ) {
                    int i;
                    for (i = p2->left;i < q->right;i++) mark[s[i]]= ATfalse;
                    stop = ATtrue; break;
               }
               q->left = p2->left;
          }
          if (stop) continue;
          /* ATwarning("%d < %d== %d < %d\n",p1->left, p1->right, q->left,
          q->right); */
	  if (!result) result = ATtrue;
          PushUnstableMark(-1, splitter, b, p1);
          PushUnstableMark(action, splitter, b, q);
          RemoveInterval(Pi+b);         
          }
     ClearMarks(i);
     return result;
     }
     
static void Parent(int *b1, int *b2) {
     while (blok[*b1].parent != blok[*b2].parent) {
          if (blok[*b1].parent > blok[*b2].parent) *b1 = blok[*b1].parent;
          if (blok[*b2].parent > blok[*b1].parent) *b2 = blok[*b2].parent;
          }
     }
     
static int InsideSplitter(ATermList tgt, int splitter) {
       int b = -1, d = -1;
       for (;!ATisEmpty(tgt);tgt = ATgetNext(tgt)) {
           d = ATgetInt((ATermInt) ATgetFirst(tgt));
           b = blockref[d];
	   if (isParentOf(splitter, b)) break;
	   }
       return d;
       }
       
static void PrintTransition(ATerm p, ATbool tp, int action, ATerm q, ATbool tq)  {
     static char buf[80];
     int pt = 0;
     strncpy(buf, ATwriteToString(p),24);
     strcat(buf, (tp?"!":" "));
     pt = strlen(buf);
     for (;pt<25;pt++) buf[pt] = ' '; buf[pt] = '\0';
     strcat(buf,"V ");
     strncat(buf, ATwriteToString(label_name[action]), 21);
     pt = strlen(buf);
     for (;pt<50;pt++) buf[pt] = ' ';buf[pt]='\0';
     strcat(buf,"V   ");
     strncat(buf, ATwriteToString(q),24);
     strcat(buf, (tq?"'!":"'"));
     ATfprintf(stderr,"%s\n",buf);
}
       
static ATbool PrintNonBisimilarStates(int *p, int *q) { 
     int b1 = blockref[*p], b2 = blockref[*q], splitter = -1, action = -1;
     ATermList tgt1 = NULL, tgt2 = NULL;
     ATerm 
     p_lab = (ATerm) ATmakeInt(*p),
     q_lab = (ATerm) ATmakeInt(*q-offset);  
     Parent(&b1, &b2);
     /* 
     ATfprintf(stderr,"QQ2: b1 = %d b2 = %d %t %t\n",b1, b2, p_lab, q_lab);
     */
     splitter = blok[b1].splitter;
     action = blok[b1].action == -1? blok[b2].action: blok[b1].action;
     tgt1 = (ATermList) ATtableGet(lab_src_tgt[action],(ATerm) ATmakeInt(*p));
     tgt2 = (ATermList) ATtableGet(lab_src_tgt[action],(ATerm) ATmakeInt(*q));
     if (blok[b1].action == -1) {
            if (!tgt1) {
                 PrintTransition(p_lab, ATtrue, action, q_lab, ATfalse);
		 return ATfalse;
		 }
            else {
                 PrintTransition(p_lab, ATfalse, action, q_lab, ATfalse);
		 *p = ATgetInt((ATermInt) ATgetFirst(tgt1));
		 *q = InsideSplitter(tgt2, splitter);
		 return ATtrue;
		 }
	    }
      if (blok[b2].action == -1) {
            if (!tgt2) {
                 PrintTransition(p_lab, ATfalse, action, q_lab, ATtrue);
		 return ATfalse;
		 }
            else {
	         PrintTransition(p_lab, ATfalse, action, q_lab, ATfalse);
		  *q = ATgetInt((ATermInt) ATgetFirst(tgt2));
		  *p = InsideSplitter(tgt1, splitter);
		  return ATtrue;
		  }
	    }	        
       return ATfalse;
     }
                                         
static ATbool CompareCheckUnstableBlock(int splitter, SVCstateIndex init1, 
                              SVCstateIndex init2, ATbool *different)
/* for all labels while p is not splitted */
     {
     INTERVAL *p = Pi+ splitter;
     ATermList labels = GetLabels(p);
     ATbool result = ATfalse;
     offset = SVCnumStates(inFile);
     for (;!ATisEmpty(labels) && !isRemoved(p);
          labels = ATgetNext(labels))
          {
          int a = ATgetInt((ATermInt) ATgetFirst(labels));
          Unstable(a, p);
          if (branching)
             {if (RefineBranching(a, splitter)) result = ATtrue;}
          else
             {if (Refine(a, splitter)) result = ATtrue;}
          if (blockref[init1] !=  blockref[init2])
               {
	       if (traceLevel) {
	       ATwarning("Not %s bisimilar. Generation of witness trace.\n",
               branching?"branching":"strongly");
	       while (PrintNonBisimilarStates((int*) &init1, (int*) &init2));
	       }
               *different = ATtrue;
               return result; 
               }
          } 
     *different = ATfalse;
     return result;
     }
     
static ATbool CompareKernel(SVCstateIndex init1,SVCstateIndex init2, 
     ATbool *different)
     {
     ATbool result = ATfalse;
     Pi_pt = n_partitions;   
     while (Pi_pt-- > 0)
       {
       INTERVAL *p = Pi+ Pi_pt;
       if (p->mode == UNSTABLE)
            {
            p->mode = STABLE;
            if (CompareCheckUnstableBlock(Pi_pt, init1, init2, different)) { 
                result = ATtrue;
                if (*different) return ATfalse;
                }
            }
       }
     return result;
     }
     
int Compare(SVCstateIndex init1, SVCstateIndex init2)
     {
     int cnt = 0, nBlocks = 0, last_nBlocks = 0;
     ATbool different = ATtrue;
     StartSplitting();
     do {
       nBlocks = NumberOfPartitions();
       if (traceLevel && cnt>=2 && last_nBlocks != nBlocks) 
     ATwarning("Extra cycle needed. Number of blocks is increased from %d to %d\n", 
           last_nBlocks, nBlocks);   
       cnt++; last_nBlocks = nBlocks;
       } while (CompareKernel(init1, init2, &different) && branching);
     /*
     if (traceLevel && branching) ATwarning("Cycle %d finished. Number of partitions: %d\n",
       cnt, nBlocks);
     */         
     return different?EXIT_NOTOK:EXIT_OK;
     }
          
int CompareBranching(SVCstateIndex init1,SVCstateIndex init2)
     {
     if (label_tau>=0) branching = ATtrue;
     return Compare(init1, init2);
     }
