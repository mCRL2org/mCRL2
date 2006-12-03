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

   $Id: huffman.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#ifndef __HUFFMANHEADER
#define __HUFFMANHEADER
 
#ifdef __cplusplus
extern "C" {
#endif 

#include "hashtable.h"
#include "blocklist.h"
#include "lz.h"


struct HFnode {
   struct HFnode *high, *low, *parent, *next, *previous;
   tBlock  *block;
   unsigned long frequency;
   ATerm        term;
};

typedef struct {
   struct HFnode *codes, *top;
   HTable *terms;
   BList blockList;  
   LZbuffer buffer;
} HFtree;
   


int HFinit(HFtree *, HTable *);
void HFfree(HFtree *);

int HFencodeATerm(BitStream *, HFtree *, ATerm);
int HFencodeIndex(BitStream *, HFtree *, long);
int HFdecodeATerm(BitStream *, HFtree *, ATerm *);
int HFdecodeIndex(BitStream *, HFtree *, long *);

#ifdef __cplusplus
}
#endif 

#endif
