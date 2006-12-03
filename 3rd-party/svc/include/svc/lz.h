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

   $Id: lz.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#ifndef __LZ_H
#define __LZ_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "aterm1.h"
#include "code.h"

#define SEARCH_BITS       15
#define LOOKAHEAD_BITS    8
#define SEARCHBUF_SIZE    (256*128)
#define LOOKAHEADBUF_SIZE 255

typedef unsigned char LZtoken[3]; /* CHANGED FROM 2 */
typedef struct  {
   unsigned char search[SEARCHBUF_SIZE];
   int last;
   int read, written;
} LZbuffer;



void LZinit(LZbuffer *buffer);
int LZwriteATerm(BitStream *fp, LZbuffer *buffer, ATerm term);
int LZreadATerm(BitStream *fp, LZbuffer *buffer, ATerm *term);
int LZwriteString(BitStream *fp, LZbuffer *buffer, char *str);
int LZreadString(BitStream *fp, LZbuffer *buffer, char **str);
int LZwriteInt(BitStream *fp, LZbuffer *buffer, long n);
int LZreadInt(BitStream *fp, LZbuffer *buffer, long *n);

#ifdef __cplusplus
}
#endif 

#endif
