/*
   SVC -- the SVC (Systems Validation Centre) file format library

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

   $Id: blocklist.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#ifndef __BLOCKLIST_H
#define __BLOCKLIST_H

#ifdef __cplusplus
extern "C" {
#endif 


typedef unsigned short Bool;
typedef struct HFnode *HFcursor;

typedef struct {
   struct HFnode *first, *last;
   unsigned long frequency;
} tBlock;

typedef tBlock *BLcursor;

typedef tBlock *BList;






void BLinit(BList *);

#define BLfirst(list) (list->first)
#define BLnext(list) (list->last->next?list->last->next->block:NULL)
#define BLprevious(list) (list->first->previous?list->first->previous->block:NULL)
#define BLfrequency(cursor) (cursor->frequency)

void BLinsert(BList *, struct HFnode *);
void BLdeleteNode(BList *, struct HFnode *);
void BLdelete(BList *, BLcursor);
void BLfree(BList *);

void Binit(tBlock *);
#define Bfirst(list) (list->first)
#define Blast(list) (list->last)
#define Bnext(list) (list->next)
#define Bprevious(list) (list->previous)

void BLswap(BList *, struct HFnode *, struct HFnode *);
void Binsert(tBlock *, struct HFnode *);
void Bdelete(tBlock *, HFcursor);
unsigned long Bfrequency(tBlock *);
void Bfree(tBlock *);

#ifdef __cplusplus
}
#endif 

#endif
