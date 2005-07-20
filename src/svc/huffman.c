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

   $Id: huffman.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <limits.h>
#include "huffman.h"

static ATerm ESCAPE_SEQUENCE;
static ATerm NO_ATERM;

static struct HFnode *HFadd(HFtree *, ATerm);
static void          HFwriteCode(BitStream *, struct HFnode *);
static void          HFfreeLoop(struct HFnode *);

       void HFdump(struct HFnode *, int);
       void HFstats(struct HFnode *, int, long *);

void HFdumpCode(FILE *, struct HFnode *);



/* Initialise 'tree' by adding one separator code */

int HFinit(HFtree *tree, HTable *terms){

   /* Protect and assign constants */

   ESCAPE_SEQUENCE=ATmake("<str(<appl>)>","ESC","NEW");
   NO_ATERM       =ATmake("<str(<appl>)>","ESC","NIL");
   ATprotect(&ESCAPE_SEQUENCE);
   ATprotect(&NO_ATERM);

   /* Init LZ buffer */

   LZinit(&tree->buffer);

   /* Assign terms table */

   tree->terms=terms;

   /* Create the root node */

   tree->codes=(struct HFnode*)malloc(sizeof(struct HFnode));
   tree->codes->high=NULL;
   tree->codes->parent=NULL;
   tree->codes->frequency=0L;
   tree->codes->term=NULL;

   /* Create the leaf for the escape code */

   tree->codes->low=(struct HFnode*)malloc(sizeof(struct HFnode));
   tree->codes->low->high=NULL;
   tree->codes->low->low=NULL;
   tree->codes->low->parent=tree->codes;
   tree->codes->low->frequency=0L; 
   tree->codes->low->term=ESCAPE_SEQUENCE;

   /* Store the escape sequence term */

   tree->top=tree->codes->low;

   /* Initialise the block list */

   BLinit(&tree->blockList);
   BLinsert(&tree->blockList, tree->codes->low);
   BLinsert(&tree->blockList, tree->codes);

   return 1;
}



/* Free resources occupied by 'tree' */

void HFfree(HFtree *tree){
/*
fprintf(stderr,"Code table dump:\n");
HFdumpCodes(tree->codes);
*/
   BLfree(&tree->blockList);

   HFfreeLoop(tree->codes);

}

void HFfreeLoop(struct HFnode *node){

   if(node!=NULL){
      HFfreeLoop(node->low);
      HFfreeLoop(node->high);
      free(node);
   }
}


void HFstats(struct HFnode *tree, int level, long *sum){

   if(tree!=NULL){
      if (tree->low==NULL && tree->high==NULL){
         *sum+=tree->frequency*level;
      } else {
         HFstats(tree->low, level+1, sum);
         HFstats(tree->high, level+1, sum);
      }
      if (tree->parent==NULL){
          fprintf(stderr, "Average code length is %ld bits\n", *sum/tree->frequency);
      } 
   }
}

/* Write 'tree' to stderr */

void HFdump(struct HFnode *tree, int d){
   int i;

   if(tree!=NULL){
      if (tree->low==NULL && tree->high==NULL){
         if (tree->term==NULL)
            ATfprintf(stderr," (%d) Term NULL\n", tree->frequency);
         else
            ATfprintf(stderr," (%d) Term %t\n", tree->frequency, tree->term);
      } else {
         fprintf(stderr," (%ld)\n", tree->frequency);
         for(i=0;i<d;i++)fprintf(stderr," ");
         fprintf(stderr,"0");
         HFdump(tree->low,d+1);
         for(i=0;i<d;i++)fprintf(stderr," ");
         fprintf(stderr,"1");
         HFdump(tree->high,d+1);
      }
   }

}

void HFdumpCodes(struct HFnode *tree){

   if(tree!=NULL){

      if (tree->low==NULL && tree->high==NULL){

         ATfprintf(stderr, "%6d %t: ", tree->frequency, tree->term? tree->term:ATmake("nil"));
         HFdumpCode(stderr, tree);
         fprintf(stderr, "\n");

      } else {
         HFdumpCodes(tree->low);
         HFdumpCodes(tree->high);
      }
   }
}


/* Return the first node of 'current' in 'tree' in the left-to-right and bottom
   to top ordering  */

static struct HFnode *HFsuccessor(HFtree *tree, struct HFnode *current){
   tBlock *currentBlock;
   HFcursor last, prelast;

   currentBlock=current->block;
   last=Blast(currentBlock);
   prelast=Bprevious(last);
/*
ATfprintf(stderr,"Succ'or of %t\n", current->term?current->term:ATmake("nil"));
Bdump(stderr,current->block);
HFdump(tree->codes,0);
*/

   if(last==current){
      return NULL;
   } else {

      if(current==prelast){
         if(current->parent==last || last->parent==current){
            return NULL;
         } else {
            return last;
         }
      } else {
         if(current->parent==prelast || (prelast &&prelast->parent==current)){
            if(current->parent==last || last->parent==current){
               return NULL;
            } else {
               return last;
            }
         } else {
            return prelast;
         }
      }
   }

}




/* Swap in 'tree' the nodes 'node1' and 'node2' */

static void HFswap(struct HFnode **root, struct HFnode *node1, struct HFnode *node2){
   struct HFnode **parent1Ptr, **parent2Ptr, *tmp;
   
/*
   ATfprintf(stderr, "%t(%d) <-> %t(%d)\n", node1->term? node1->term:ATmake("nil"),
                                    node1->frequency,
                                    node2->term? node2->term:ATmake("nil"),
                                    node2->frequency);
*/
   
   if(*root==node1){
      parent1Ptr=root;
   } else {
      if (node1==node1->parent->low){
         parent1Ptr=&node1->parent->low;
      } else {
         parent1Ptr=&node1->parent->high;
      }
   }

   if(*root==node2){
      parent2Ptr=root;
   } else {
      if (node2==node2->parent->low){
         parent2Ptr=&node2->parent->low;
      } else {
         parent2Ptr=&node2->parent->high;
      }
   }

   *parent1Ptr=node2;
   *parent2Ptr=node1;
   tmp=node2->parent;
   node2->parent=node1->parent;
   node1->parent=tmp;


}



/* Update 'tree' starting from 'current' */

static void HFupdate(HFtree *tree, struct HFnode *current ){
   struct HFnode *successor=NULL;

   for (  ;current!=NULL; current= current->parent){

      successor=HFsuccessor(tree, current);

      if (successor==NULL){

         BLswap(&tree->blockList, current, NULL);

      } else {

         BLswap(&tree->blockList, current, successor);
         HFswap(&tree->codes, current, successor);

      }

   }
/*
   HFdump(tree->codes, 0);
   fprintf(stderr,"\n----------------------\n");
*/

}



int HFdecodeATerm(BitStream *fp, HFtree *tree, ATerm *term){
   Bit bit;
   struct HFnode *current;


   current=tree->codes;
   while(current!=NULL){
      if(current->high==NULL && current->low==NULL){
         *term=current->term;
         if(*term==ESCAPE_SEQUENCE){
/* OEPS
            HFupdate(tree,current);
*/
            if (LZreadATerm(fp, &tree->buffer, term)){
               current=HFadd(tree,*term);
               HFupdate(tree,current);
            } else {
               fprintf(stderr, "Cannot read string\n");
               return 0;
            }
         } else {
            HFupdate(tree,current);
         }

         if(*term==NO_ATERM){
            *term=NULL;
            return 0;
         } else {
            return 1;
         }
      } else {
         if(BSreadBit(fp, &bit)==1){
            if(bit==0){
              current = current->low;
            } else {
              current = current->high;
            }
         } else {
            return 0;
         }
      }
   }

   return 1;
}



int HFdecodeIndex(BitStream *fp, HFtree *tree, long *index){
   Bit bit;
   struct HFnode *current;
   ATerm term;


   current=tree->codes;
   while(current!=NULL){

      if(current->high==NULL && current->low==NULL){
         term=current->term;
         if(term==ESCAPE_SEQUENCE){
/* OEPS
            HFupdate(tree,current);
*/
            if(LZreadInt(fp,&tree->buffer,index)){
               term=(ATerm)ATmakeInt(*index);
               current=HFadd(tree,term);
               HFupdate(tree,current);
               return *index!=NO_INT;
            } else {
               return 0;
            }
         } else {
            HFupdate(tree,current);
            *index=ATgetInt((ATermInt)term);
            return *index!=NO_INT;
         }
      } else {
         if(BSreadBit(fp, &bit)==1){
            if(bit==0){
              current = current->low;
            } else {
              current = current->high;
            }
         } else {
            return 0;
         }
      }
   }

   return 1;
}

/* Insert 'term' into tree */

int HFencodeATerm(BitStream *bs, HFtree *tree, ATerm term){
   struct HFnode *tmp;
   long index;

   if(term==NULL){
      term=NO_ATERM;
   }

   if(HTmember(tree->terms,term,&index)&&HTgetPtr(tree->terms,index)){
      tmp=(struct HFnode *)HTgetPtr(tree->terms,index);
      HFwriteCode(bs, tmp);
      HFupdate(tree,tmp);
      return 1;
   } else {

      tmp=tree->top;

      HFwriteCode(bs,tmp); 
/* OEPS
      HFupdate(tree,tmp); 
*/

      LZwriteATerm(bs, &tree->buffer, term);

      tmp=HFadd(tree,term);
      HFupdate(tree,tmp);

      return 0;

   }

}



int HFencodeIndex(BitStream *bs, HFtree *tree, long index){
   struct HFnode *tmp;
   ATerm term;
   long n;


   term=(ATerm)ATmakeInt(index);

   if(HTmember(tree->terms,term,&n)&&HTgetPtr(tree->terms,n)){
      tmp=(struct HFnode *)HTgetPtr(tree->terms,n);
      HFwriteCode(bs, tmp);
      HFupdate(tree,tmp);
      return 1;
   } else {

      tmp=tree->top;
      HFwriteCode(bs,tmp); 
/* OEPS
      HFupdate(tree,tmp); 
*/

      LZwriteInt(bs,  &tree->buffer, index);

      tmp=HFadd(tree,term);
      HFupdate(tree,tmp);

      return 0;

   }

}


void HFdumpCode(FILE *fp, struct HFnode *node){

   if(node->parent!=NULL){
      HFdumpCode(fp,node->parent);
      if(node==node->parent->high){
        fprintf(fp, "1");
      } else {
        fprintf(fp, "0");
      }
   } 

}



void HFwriteCode(BitStream *fp, struct HFnode *node){

   if(node->parent!=NULL){
      HFwriteCode(fp,node->parent);
      if(node==node->parent->high){
        BSwriteBit(fp,1);
      } else {
        BSwriteBit(fp,0);
      }
   } 

}


/* Add 'term' to 'tree' */

static struct HFnode *HFadd(HFtree *tree, ATerm term){
   struct HFnode *newNode, *tmp;
   long index;


   /* Find the node with the escape sequence */
   tmp=tree->top;

   /* This is the only child of its parent? */
   if(tmp->parent->high==NULL){

      /* Create a new sibling */

      newNode=(struct HFnode*)malloc(sizeof(struct HFnode));
      newNode->high=NULL;
      newNode->low=NULL;
      newNode->parent=tmp->parent;
      newNode->frequency=0L;
      newNode->term=term;
      tmp->parent->high=newNode;

      BLinsert(&tree->blockList, newNode);

      if(HTmember(tree->terms, term, &index)){
         HTsetPtr(tree->terms,index, newNode);
      } else {
         HTinsert(tree->terms, term, newNode);
      }

      return newNode;

  } else {

      /* Create new interior node */

      newNode=(struct HFnode*)malloc(sizeof(struct HFnode));
      newNode->parent=tmp->parent;
      newNode->frequency=tmp->frequency;
      newNode->term=NULL;
      if (tmp->parent->low==tmp){
         tmp->parent->low=newNode;
      } else {
         tmp->parent->high=newNode;
      }

      /* Old leaf becomes low child of new node */

      newNode->low=tmp;
      tmp->parent=newNode;

      /* Create new leaf that is high child of new interior node */

      newNode->high=(struct HFnode*)malloc(sizeof(struct HFnode));
      newNode->high->high=NULL;
      newNode->high->low=NULL;
      newNode->high->parent=newNode;
      newNode->high->frequency=0L;
      newNode->high->term=term;

      BLinsert(&tree->blockList, newNode);
      BLinsert(&tree->blockList, newNode->high);
/*
      HTinsert(tree->terms, term, newNode->high);
*/
if(HTmember(tree->terms, term, &index)){
      HTsetPtr(tree->terms,index, newNode->high);
} else {
      HTinsert(tree->terms, term, newNode->high);
}
      return newNode->high;


   }

}


