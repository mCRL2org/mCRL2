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

   $Id: ptable.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif 


#define PT_INITIALSIZE 1024


typedef struct{
   long size;
   void **nodes;
} PTable;


void PTinit(PTable *);
void PTput(PTable *, long index, void *ptr);
void *PTget(PTable *, long index);
void PTfree(PTable *);

#ifdef __cplusplus
}
#endif 

