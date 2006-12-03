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

   $Id: compress.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <limits.h>
#include <compress.h>

static char buffer[102400];
static void calcDelta(CompressedStream *, long *);
static void uncalcDelta(CompressedStream *, long *);



CompressedStream *CSinit(HTable *table, BitStream *bs, int indexed){
   CompressedStream *ret;

   ret=(CompressedStream *)malloc(sizeof(CompressedStream));
   if(indexed){
      ret->indices=table;   
      HTinit(&ret->deltas);
      HFinit(&ret->tree, &ret->deltas);
   } else {
      HTinit(&ret->deltas);
      HFinit(&ret->tree, table);
   }

   ret->bs=bs;
   ret->last=0;
   ret->preLast=0;

   return ret;
}


void CSfree(CompressedStream *cs){

   HFfree(&cs->tree);
   HTfree(&cs->deltas);
   free(cs);

}

int CSreadIndex(CompressedStream *cs, ATerm *term){
   long index;
   if(HFdecodeIndex(cs->bs, &cs->tree, &index)){
      uncalcDelta(cs,&index);
      *term=(ATerm)ATmakeInt(index); 
         HTinsert(cs->indices,*term,NULL); /* IZAK */
      return 1;
   } else {
      return 0;
   }
}


int CSreadATerm(CompressedStream *cs, ATerm *term){

   if(HFdecodeATerm(cs->bs, &cs->tree, term)){
/*
ATfprintf(stderr, "Read %t\n", *term?*term:ATmake("nil"));
*/

      if(*term==NULL){
         return 0;
      } else {
         return 1;
      }
   } else {
      return 0;
   }

}

int CSureadATerm(CompressedStream *cs, ATerm *term){

   if (BSreadString(cs->bs,buffer)){
/*
      ATfprintf(stderr, "Read ATerm %t\n", ATmake("<appl>", str));
*/
      *term=ATreadFromString(buffer);
      return 1;
   } else {
      return 0;
   }
}

int CSreadString(CompressedStream *cs, char **str){
   ATerm term;

   if(HFdecodeATerm(cs->bs, &cs->tree, &term) && ATmatch(term, "<str>", str)){
      return 1;
   } else {
      return 0;
   }

}

int CSureadString(CompressedStream *cs, char **str){

   if(BSreadString(cs->bs, buffer)){
/*
ATfprintf(stderr, "Uread %s\n", buffer);
*/
      *str=buffer;
      return 1;
   } else {
      return 0;
   }
}


int CSreadInt(CompressedStream *cs, long *n){
   ATerm term;

   if(HFdecodeATerm(cs->bs, &cs->tree, &term) && ATmatch(term, "<int>", &n)){
      return 1;
   } else {
      return 0;
   }

}

int CSureadInt(CompressedStream *cs, long *n){

   if (BSreadInt(cs->bs,n)){
/*
fprintf(stderr, "Read int %ld\n", *n);
*/
      return 1;
   } else {
      return 0;
   }

}



int CSwriteIndex(CompressedStream *cs, ATerm term){
   long index;


   if(term==NULL){
      return HFencodeIndex(cs->bs, &cs->tree, NO_INT); 
   } else {
      if(!HTmember(cs->indices,term,&index)){
         index=HTinsert(cs->indices,term,NULL);
      }
      calcDelta(cs, &index);

      return HFencodeIndex(cs->bs, &cs->tree, index); 
   }
}


int CSwriteATerm(CompressedStream *cs, ATerm term){

   return HFencodeATerm(cs->bs, &cs->tree, term); 
}

int CSuwriteATerm(CompressedStream *cs, ATerm term){

   BSwriteString(cs->bs,ATwriteToString(term));
   return 1;
}
int CSwriteString(CompressedStream *cs, char *str){

   return HFencodeATerm(cs->bs, &cs->tree, (ATerm)ATmakeAppl(ATmakeAFun(str,0,ATtrue)));
}

int CSuwriteString(CompressedStream *cs, char *str){
   BSwriteString(cs->bs,str);
   return 1;
}
int CSwriteInt(CompressedStream *cs, long n){

/*
ATfprintf(stderr,"Write int %d\n", n);
*/

   return HFencodeATerm(cs->bs, &cs->tree, (ATerm)ATmakeInt(n));
}
int CSuwriteInt(CompressedStream *cs, long n){
   BSwriteInt(cs->bs,n);
   return 1;
}

void  CSflush(CompressedStream *cs){

   BSflush(cs->bs);

}

int  CSseek(CompressedStream *cs, long offset, int whence){

   return BSseek(cs->bs, offset, whence);
}

long CStell(CompressedStream *cs){

   return BStell(cs->bs);
}



static void calcDelta(CompressedStream *bs, long *n){
   long tmp;

   tmp=*n;
   *n=*n-bs->preLast;
   bs->preLast=bs->last;
   bs->last=tmp;
}

static void uncalcDelta(CompressedStream *bs, long *n){
   long tmp;

   tmp=*n;
   *n=*n+bs->preLast;
   bs->preLast=bs->last;
   bs->last=*n;
}
