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

   $Id: hashtable.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */
/* 
 * This module implements a hash table for storing mCRL states,
 * represented as term-number pairs. All the states stored in a
 * table are protected by being inserted in a protected ATerm
 * list constructed with "ins" and "emt". The hashing function
 * assumes that each ATerm address is unique. The operations
 * provided are insertion and membership.
 */

/* Changes per 1/1/00 by Izak van Langevelde:
   - numbers used are unsigned long instead of long, 0 means absent
   - indices returned are in the range 1..N, but internally in the
     term table the entries are in the range 0..N-1
   - hash table is parameter instead of global variable
   - errors are flagged by return value instead of error message 
   Changes per 1/5/00 by Izak van Langevelde:
   - use the new ATerm lib with indexed sets*/ 

#include "hashtable.h"




/* ======= Initialize the hash table ======= */
int HTinit (HTable *table)
{ 

  table->terms=ATindexedSetCreate(PT_INITIALSIZE, 75);
  PTinit(&table->pointers);

  return 0;
}

void HTfree(HTable *table){

  ATindexedSetDestroy(table->terms);
  PTfree(&table->pointers);
}

long HTinsert (HTable *table, ATerm a, void *ptr) {
  ATbool _new;
  long ret;

  ret= ATindexedSetPut(table->terms, a, &_new);
  PTput(&table->pointers,ret,ptr);

  return ret;
}


int HTmember (HTable *table, ATerm a, long *pn)
{ long index;


  index=ATindexedSetGetIndex(table->terms,a);

  if(index>=0){
     *pn=index;
     return 1;
  } else {
     return 0;
  }

}

/*----------------------------------------------------------*/
/* Get the term belonging to an index */

ATerm HTgetTerm(HTable *table, long n) { 

  return ATindexedSetGetElem(table->terms,n);

}


/*----------------------------------------------------------*/
/* Get the stored pointer belonging to an index */

void *HTgetPtr(HTable *table, long n) { 

   return PTget(&table->pointers,n);
}


void HTsetPtr(HTable *table, long n, void *ptr) { 

   PTput(&table->pointers, n,ptr);
}
