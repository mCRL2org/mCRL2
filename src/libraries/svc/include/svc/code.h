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

   $Id: code.h,v 1.2 2005/04/05 13:23:01 bertl Exp $ */

#ifndef __CODE_H
#define __CODE_H

#ifdef __cplusplus
extern "C" {
#endif 

#define NO_INT INT_MAX
#define NO_TERM NULL

typedef unsigned char Bit;
typedef unsigned char Byte;
typedef struct {
   Byte buffer;
   unsigned short numBits;
   FILE           *fp;
   long last;
} BitStream;


BitStream *BSinit(FILE *);
void BSfree(BitStream *);
void BSwriteBit(BitStream *, Bit );
void BSwriteByte(BitStream *, Byte);
void BSwriteChar(BitStream *, char);
void BSwriteString(BitStream *, char *);
void BSwriteInt(BitStream *, long);
int BSreadBit(BitStream *, Bit *);
int BSreadChar(BitStream *, char *);
int BSreadByte(BitStream *, Byte *);
int BSreadString(BitStream *, char *);
int BSreadInt(BitStream *, long *);
void BSflush(BitStream *);
int  BSseek(BitStream *, long, int);
long BStell(BitStream *);

#ifdef __cplusplus
}
#endif 

#endif
