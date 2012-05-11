/*
   SVC -- the SVC (Systems Validation Centre) file format library

   Copyright (C) 2000  Stichting Mathematisch Centrum, Amsterdam,
                       The  Netherlands

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   $Id: compress.h,v 1.2 2008/09/30 08:22:51 bertl Exp $ */

#ifndef __COMPRESS_H
#define __COMPRESS_H

#include "huffman.h"


  typedef struct
  {
    BitStream* bs;
    HFtree tree;
    HTable* indices, deltas;
    long last, preLast;
  } CompressedStream;



  CompressedStream* CSinit(HTable*, BitStream*, int);
  void CSfree(CompressedStream*);
  int   CSreadATerm(CompressedStream*, aterm_deprecated::ATerm*);
  int   CSreadIndex(CompressedStream*, aterm_deprecated::ATerm*);
  int   CSureadATerm(CompressedStream*, aterm_deprecated::ATerm*);
  int   CSreadString(CompressedStream*, char**);
  int   CSureadString(CompressedStream*, char**);
  int CSreadInt(CompressedStream*, long*);
  int CSureadInt(CompressedStream*, long*);
  int   CSwriteATerm(CompressedStream*, aterm_deprecated::ATerm);
  int   CSwriteIndex(CompressedStream*, aterm_deprecated::ATerm);
  int   CSuwriteATerm(CompressedStream*, aterm_deprecated::ATerm);
  int   CSwriteString(CompressedStream*, const char*);
  int   CSuwriteString(CompressedStream*, const char*);
  int   CSwriteInt(CompressedStream*, long);
  int   CSuwriteInt(CompressedStream*, long);
  void  CSflush(CompressedStream*);
  int  CSseek(CompressedStream*, long, int);
  long CStell(CompressedStream*);

#endif
