// Author(s): Muck van Weerdenburg, Bert Lisser
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsminkernel.cpp

#include <iostream>
#include "mcrl2/utilities/logger.h"
#include "ltsmin.h"

using namespace aterm;

static bool RefineBranching(int action, int splitter);

static bool branching = false;

static int offset = -1;

static int IndexOf(ATermList ls, int k, int i)
{
  for (; !ATisEmpty(ls); ls=ATgetNext(ls),i++)
    if (ATgetInt((ATermInt) ATgetFirst(ls))==k)
    {
      return i;
    }
  return -1;
}

static ATermList GetLabels(INTERVAL* p)
{
  SVCstateIndex left = p->left, right = p->right;
  ATermList result = branching?ATmakeList1((ATerm) ATmakeInt(label_tau)):
                     ATempty;
  /* Tau must be the last evaluated action */
  int i;
  for (i=left; i<right; i++)
  {
    ATermList labels = lab[s[i]];
    for (; !ATisEmpty(labels); labels=ATgetNext(labels))
    {
      if (IndexOf(result, ATgetInt((ATermInt) ATgetFirst(labels)),0)<0)
      {
        result = ATinsert(result, ATgetFirst(labels));
      }
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

static void AddBlock(int b)
{
  int i = 0;
  for (; i<blocks.pt; i++)
  {
    if (blocks.b[i]==b)
    {
      return;
    }
  }
  blocks.b[blocks.pt++] = b;
}

static void Unstable(int a, INTERVAL* p)
{
  ATermTable act = lab_tgt_src[a];
  /* Returns a set of blocks which are candidates for splitting */
  SVCstateIndex left = p->left, right = p->right, i;
  for (i=left; i<right; i++)
  {
    SVCstateIndex target = s[i];
    if (IndexOf(lab[target],a,0)<0)
    {
      continue;
    }
    {
      ATermList sources =
        (ATermList) ATtableGet(act, (ATerm) ATmakeInt(target));
      /* if (!sources) sources = ATmakeList0(); */
      for (; !ATisEmpty(sources); sources = ATgetNext(sources))
      {
        SVCstateIndex source = ATgetInt((ATermInt) ATgetFirst(sources));
        int b = blockref[source];
        mark[source] = true;
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
    for (; !mark[s[left]]; left++) if (left == right)
      {
        return left+1;
      }
    for (; mark[s[right]]; right--)
    {
      /* mark[s[right]] = ATfalse; */
      if (left == right)
      {
        return right;
      }
    }
    {
      SVCstateIndex swap = s[left];
      s[left] = s[right];
      s[right] = swap;
      /* mark[swap] = ATfalse; */
      right--;
      left++;
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

static bool split(INTERVAL* p, INTERVAL* p1, INTERVAL* p2)
{
  SVCstateIndex left = p->left, right = p->right, left_bound_p2;
  left_bound_p2 = partition(left, right - 1);
  if (left_bound_p2 == left)
  {
    int i;
    for (i=left; i<right; i++)
    {
      mark[s[i]] = false;
    }
    return false;
  }
  if (left_bound_p2 == right)
  {
    return false;
  }
  p1->left = left;
  p1->right= p2->left = left_bound_p2;
  p2->right = right;
  p1->mode = p2->mode = UNSTABLE;
  return true;
}

static bool isParentOf(int parent, int b)
{
  for (; b>0&&b!=parent; b=blok[b].parent) {};
  return (b==parent)?true:false;
}

static void UpdateBlock(int b, INTERVAL* p)
{
  SVCstateIndex i = p->left, right = p->right;
  for (; i<right; i++)
  {
    blockref[s[i]] = b;
  }
}

static void PushUnstableMark(int action, int splitter, int parent, INTERVAL* p)
{
  /* Push the new interval on the unstable stack,
  and assign to the belonging states a new block number,
  which is the (relative) address of the block on stack */
  Pi_pt = n_partitions;
  Pi[Pi_pt] = *p;
  blok[Pi_pt].action = action;
  blok[Pi_pt].splitter = splitter;
  blok[Pi_pt].parent = parent;
  UpdateBlock(Pi_pt, p);
  if (action>=0)
  {
    int i = p->left, right = p->right;
    for (; i<right; i++)
    {
      mark[s[i]] = false;
    }
  }
  Pi_pt++;
  if (n_partitions < Pi_pt)
  {
    n_partitions = Pi_pt;
  }
}

static void ClearMarks(int k)
{
  for (; k<blocks.pt; k++)
  {
    INTERVAL* p = Pi + blocks.b[k];
    int i = p->left;
    for (; i<p->right; i++)
    {
      mark[s[i]] = false;
    }
  }
  blocks.pt = 0;
}

static bool Refine(int action, int splitter)
{

  /* Splits the blocks in "blocks" into two blocks.
  One contains the marked states, the other contains the unmarked states.
  Both block become unstable blocks. They will later used as occasion to
  split blocks.  */

  INTERVAL* p = Pi+splitter;
  bool result = false;
  int i = 0;
  for (; i<blocks.pt && !isRemoved(p); i++)
  {
    INTERVAL p1[1], p2[1];
    int b = blocks.b[i];
    /* fprintf(stderr, "b = %d: ",b); */

    if (!split(Pi+b, p1, p2))
    {
      continue;
    }
    /* Removals */
    PushUnstableMark(-1, splitter, b, p1);
    PushUnstableMark(action, splitter, b, p2);
    RemoveInterval(Pi+b);
    if (!result)
    {
      result = true;
    }
  }
  ClearMarks(i);
  return result;
}

static bool CheckUnstableBlock(int splitter)
/* for all labels while p not splitted */
{
  INTERVAL* p = Pi+splitter;
  ATermList labels = GetLabels(p);
  bool result = false;
  for (; !ATisEmpty(labels) && !isRemoved(p);
       labels = ATgetNext(labels))
  {
    int a = ATgetInt((ATermInt) ATgetFirst(labels));
    Unstable(a, p);
    if (branching)
    {
      if (RefineBranching(a, splitter))
      {
        result = true;
      }
    }
    else
    {
      if (Refine(a, splitter))
      {
        result = true;
      }
    }
  }
  return result;
}

static int NumberOfPartitions(void)
{
  unsigned int i, cnt = 0;
  for (i=0; i<n_partitions; i++)
  {
    INTERVAL* p = Pi + i;
    if (p->mode == STABLE)
    {
      p->mode = UNSTABLE;
      cnt++;
    }
  }
  return cnt;
}

static bool ReduceKernel(void)
/*
For all unstable blocks refine partition until there are no unstable
blocks */
{
  bool result = false;
  Pi_pt = n_partitions;
  while (Pi_pt > 0)
  {
    INTERVAL* p = Pi+ (--Pi_pt);
    if (p->mode == UNSTABLE)
    {
      p->mode = STABLE;
      if (CheckUnstableBlock(Pi_pt))
      {
        result = true;
      }
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
  do
  {
    nBlocks = NumberOfPartitions();
    if (traceLevel && cnt>=2 && last_nBlocks != nBlocks)
    {
      mCRL2log(mcrl2::log::verbose) << "Extra cycle needed. Number of blocks is increased from " << last_nBlocks << " to " << nBlocks << std::endl;
    }
    cnt++;
    last_nBlocks = nBlocks;
  }
  while (ReduceKernel() && branching);
}

void ReduceBranching(void)
{
  if (label_tau >= 0)
  {
    branching = true;
  }
  Reduce();
}
/* ------------------- Branching bisimulation ----------------------------*/

static bool MarkTau(int b,  INTERVAL* p2)
{
  int left = p2->left, right = p2->right, i;
  bool result = false;
  for (i=left; i<right; i++)
  {
    ATermList sources = (ATermList)
                        ATtableGet(lab_tgt_src[label_tau],(ATerm) ATmakeInt(s[i]));

    if (!sources)
    {
      continue;
    }
    for (; !ATisEmpty(sources); sources=ATgetNext(sources))
    {
      ATerm source = ATgetFirst(sources);
      int d = ATgetInt((ATermInt) source);
      if (blockref[d] == b && !mark[d])
      {
        result = mark[d] = true;
      }
    }
  }
  return result;
}

static bool RefineBranching(int action, int splitter)
{
  INTERVAL* p = Pi+splitter;
  bool result = false;
  int i = 0;
  for (; i<blocks.pt && !isRemoved(p); i++)
  {
    bool stop = false;
    int b = blocks.b[i];
    INTERVAL p1[1], p2[1], q[1], *r = Pi + b;
    if (action == label_tau && b == splitter)
    {
      int i;
      for (i=p->left; i<p->right; i++)
      {
        mark[s[i]] = false;
      }
      continue;
    }
    q->right = r->right;
    if (!split(r, p1, p2))
    {
      continue;
    }
    q->left = p2->left;
    q->mode = UNSTABLE;
    while (MarkTau(b, p2))
    {
      r=p1;
      if (!split(r, p1, p2) /* No nonmarked states */
          /* && p1->left == p1->right */)
      {
        int i;
        for (i = p2->left; i < q->right; i++)
        {
          mark[s[i]]= false;
        }
        stop = true;
        break;
      }
      q->left = p2->left;
    }
    if (stop)
    {
      continue;
    }
    if (!result)
    {
      result = true;
    }
    PushUnstableMark(-1, splitter, b, p1);
    PushUnstableMark(action, splitter, b, q);
    RemoveInterval(Pi+b);
  }
  ClearMarks(i);
  return result;
}

static void Parent(int* b1, int* b2)
{
  while (blok[*b1].parent != blok[*b2].parent)
  {
    if (blok[*b1].parent > blok[*b2].parent)
    {
      *b1 = blok[*b1].parent;
    }
    if (blok[*b2].parent > blok[*b1].parent)
    {
      *b2 = blok[*b2].parent;
    }
  }
}

static int InsideSplitter(ATermList tgt, int splitter)
{
  int b = -1, d = -1;
  for (; !ATisEmpty(tgt); tgt = ATgetNext(tgt))
  {
    d = ATgetInt((ATermInt) ATgetFirst(tgt));
    b = blockref[d];
    if (isParentOf(splitter, b))
    {
      break;
    }
  }
  return d;
}

static void PrintTransition(ATerm p, bool tp, int action, ATerm q, bool tq)
{
  static char buf[80];
  size_t pt = 0;
  strncpy(buf, ATwriteToString(p).c_str(),24);
  strcat(buf, (tp?"!":" "));
  pt = strlen(buf);
  for (; pt<25; pt++)
  {
    buf[pt] = ' ';
  }
  buf[pt] = '\0';
  strcat(buf,"V ");
  strncat(buf, ATwriteToString(label_name[action]).c_str(), 21);
  pt = strlen(buf);
  for (; pt<50; pt++)
  {
    buf[pt] = ' ';
  }
  buf[pt]='\0';
  strcat(buf,"V   ");
  strncat(buf, ATwriteToString(q).c_str(),24);
  strcat(buf, (tq?"'!":"'"));
  mCRL2log(mcrl2::log::verbose) << buf << std::endl;
}

static bool PrintNonBisimilarStates(int* p, int* q)
{
  int b1 = blockref[*p], b2 = blockref[*q], splitter = -1, action = -1;
  ATermList tgt1 = NULL, tgt2 = NULL;
  ATerm
  p_lab = (ATerm) ATmakeInt(*p),
  q_lab = (ATerm) ATmakeInt(*q-offset);
  Parent(&b1, &b2);
  splitter = blok[b1].splitter;
  action = blok[b1].action == -1? blok[b2].action: blok[b1].action;
  tgt1 = (ATermList) ATtableGet(lab_src_tgt[action],(ATerm) ATmakeInt(*p));
  tgt2 = (ATermList) ATtableGet(lab_src_tgt[action],(ATerm) ATmakeInt(*q));
  if (blok[b1].action == -1)
  {
    if (!tgt1)
    {
      PrintTransition(p_lab, true, action, q_lab, false);
      return false;
    }
    else
    {
      PrintTransition(p_lab, false, action, q_lab, false);
      *p = ATgetInt((ATermInt) ATgetFirst(tgt1));
      *q = InsideSplitter(tgt2, splitter);
      return true;
    }
  }
  if (blok[b2].action == -1)
  {
    if (!tgt2)
    {
      PrintTransition(p_lab, false, action, q_lab, true);
      return false;
    }
    else
    {
      PrintTransition(p_lab, false, action, q_lab, false);
      *q = ATgetInt((ATermInt) ATgetFirst(tgt2));
      *p = InsideSplitter(tgt1, splitter);
      return true;
    }
  }
  return false;
}

static bool CompareCheckUnstableBlock(int splitter, SVCstateIndex init1,
                                        SVCstateIndex init2, bool* different)
/* for all labels while p is not splitted */
{
  INTERVAL* p = Pi+ splitter;
  ATermList labels = GetLabels(p);
  bool result = false;
  offset = SVCnumStates(inFile);
  for (; !ATisEmpty(labels) && !isRemoved(p);
       labels = ATgetNext(labels))
  {
    int a = ATgetInt((ATermInt) ATgetFirst(labels));
    Unstable(a, p);
    if (branching)
    {
      if (RefineBranching(a, splitter))
      {
        result = true;
      }
    }
    else
    {
      if (Refine(a, splitter))
      {
        result = true;
      }
    }
    if (blockref[init1] !=  blockref[init2])
    {
      if (traceLevel)
      {
        mCRL2log(mcrl2::log::verbose) << "Not " << (branching?"branching":"strongly") << "bisimilar. Generation of witness trace." << std::endl;
        while (PrintNonBisimilarStates((int*) &init1, (int*) &init2)) {};
      }
      *different = true;
      return result;
    }
  }
  *different = false;
  return result;
}

static bool CompareKernel(SVCstateIndex init1,SVCstateIndex init2,
                            bool* different)
{
  bool result = false;
  Pi_pt = n_partitions;
  while (Pi_pt-- > 0)
  {
    INTERVAL* p = Pi+ Pi_pt;
    if (p->mode == UNSTABLE)
    {
      p->mode = STABLE;
      if (CompareCheckUnstableBlock(Pi_pt, init1, init2, different))
      {
        result = true;
        if (*different)
        {
          return false;
        }
      }
    }
  }
  return result;
}

int Compare(SVCstateIndex init1, SVCstateIndex init2)
{
  int cnt = 0, nBlocks = 0, last_nBlocks = 0;
  bool different = true;
  StartSplitting();
  do
  {
    nBlocks = NumberOfPartitions();
    if (traceLevel && cnt>=2 && last_nBlocks != nBlocks)
    {
      mCRL2log(mcrl2::log::verbose) << "Extra cycle needed. Number of blocks is increased from " << last_nBlocks << " to " << nBlocks << std::endl;
    }
    cnt++;
    last_nBlocks = nBlocks;
  }
  while (CompareKernel(init1, init2, &different) && branching);
  return different?EXIT_NOTOK:EXIT_OK;
}

int CompareBranching(SVCstateIndex init1,SVCstateIndex init2)
{
  if (label_tau>=0)
  {
    branching = true;
  }
  return Compare(init1, init2);
}
