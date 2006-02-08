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


   $Id: blocklist.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "blocklist.h"
#include "huffman.h"

void BLdump(FILE *, BList *);



void BLinit(BList *list){

   *list=NULL;
}


   
void BLinsert(BList *blockList, struct HFnode *node){

   node->frequency=0L;

   if(*blockList==NULL){
     node->previous=NULL;
     node->next=NULL;
     *blockList=(tBlock*)malloc(sizeof(tBlock));
     Binit(*blockList);
   } else {
     node->previous=NULL;
     node->next    =(*blockList)->first;
     (*blockList)->first->previous=node;
     (*blockList)->first=node;
   }

   Binsert(*blockList,node);

}


void BLdelete(BList * blockList, BLcursor cursor){
   tBlock *tmp;

   tmp=cursor;
/*

   if(tmp->previous==NULL){
      blockList->first=tmp->next;
   } else {
      tmp->previous->next=tmp->next;
   }

   if(tmp->next!=NULL){
      tmp->next->previous=tmp->previous;
   }
*/
   free(tmp);
      
}



void BLfree(BList *blockList){
   tBlock *tmp;

   while(*blockList!=NULL){
      tmp=(*blockList)->last->next?(*blockList)->last->next->block:NULL;
      free(*blockList);
      *blockList=tmp;
   }
}



void Binit(tBlock *list){

   list->first=NULL;
   list->last=NULL;
   list->frequency=0L;

}



void BLswap(BList *blockList, struct HFnode *node1, struct HFnode *node2){
   tBlock *newBlock;

/*
if(node1){
ATfprintf(stderr, "%t(%p) <-> %t(%p)\n", node1->term?node1->term:ATmake("nil"),
node1,
node2&&node2->term?node2->term:ATmake("nil"),
node2);
Bdump(stderr, node1->block);
}
oldBlock=node1->block;
fprintf(stderr,"IN %p %p\n", oldBlock->last, oldBlock->last->previous);
Bdump(stderr, node1->block);
*/
   if(node2==NULL){

      /* Remove node1 from its block */
      if(node1->block->first==node1 && node1->block->last==node1){
         if(*blockList==node1->block){
            *blockList=NULL;
         }
         Bfree(node1->block);
      } else {
         if(node1->block->last==node1){
            node1->block->last=node1->previous;
         } else {
            if(node1->block->first==node1){
               node1->block->first=node1->next;
            }
            /* Shift node1 over this block */
            if(node1->previous!=NULL){
               node1->previous->next=node1->next;
            }
            if(node1->next!=NULL){
               node1->next->previous=node1->previous;
            }
            node1->previous=node1->block->last;
            node1->next    =node1->block->last->next;
         }

      }

   } else {

      if(node1->block->first==node1){
         node1->block->first=node2;
      } 

      if(node1->next!=node2){
         if(node1->block->last==node2){
            node1->block->last=node2->previous;
         }
         node2->previous->next=node2->next;
         if(node2->next!=NULL){
            node2->next->previous=node2->previous;
         }
         node2->next=node1->next;
      }

      node2->previous=node1->previous;

      if(node2->previous!=NULL){
         node2->previous->next=node2;
      }
      if(node2->next!=NULL){
         node2->next->previous=node2;
      }

      node1->previous=node1->block->last;
      node1->next    =node1->block->last->next;

   }

   node1->frequency++;

   if(node1->next!=NULL && node1->next->block->frequency==node1->frequency){
      Binsert(node1->next->block, node1);
   } else {
      newBlock=(tBlock*)malloc(sizeof(tBlock));
      Binit(newBlock);
      Binsert(newBlock,node1);
   }

   if(*blockList==NULL){
      *blockList=node1->block;
   }
 
   if(node1->previous!=NULL){
      node1->previous->next=node1;
   }
   if(node1->next!=NULL){
      node1->next->previous=node1;
   }

}   


   
void Binsert(tBlock *nodeList, struct HFnode *node){

   if(nodeList->last==NULL){
      nodeList->last=node;
   } 
   nodeList->first=node;
   nodeList->frequency=node->frequency;
   node->block=nodeList;

}

void Bdelete(tBlock *nodeList, HFcursor cursor){

   if(cursor->previous!=NULL){
      cursor->previous->next=cursor->next;
   }

   if(cursor->next!=NULL){
      cursor->next->previous=cursor->previous;
   }

   if(nodeList->last==cursor && nodeList->first==cursor){
     nodeList->last=NULL;
     nodeList->first=NULL;
   } else {
     if(nodeList->last==cursor){
       nodeList->last=cursor->previous;
     } 
     if(nodeList->first==cursor){
       nodeList->first=cursor->next;
     } 
   }

}



unsigned long Bfrequency(tBlock *nodeList){

   return nodeList->frequency;

}

void Bfree(tBlock *nodeList){

   free(nodeList);

}



void BLdeleteNode(BList *blockList, struct HFnode *node){

   Bdelete(node->block, node);
   if(node->block->first==NULL){
      free(node->block);
      BLdelete(blockList, node->block);
   }

}


void Bdump(FILE *fp, tBlock *block){
   struct HFnode *tmp;
   
   fprintf(fp, "Block %ld:\n", block->frequency);
   for(tmp=block->first; tmp!=NULL; tmp=tmp->next){
      ATfprintf(stderr, "    %t %6d(%p<%p<%p)\n", tmp->term?tmp->term:ATmake("nil"), tmp->frequency, tmp->previous, tmp, tmp->next);
      if(tmp==block->last)
        break;
   }

}



void BLdump(FILE *fp, BList *blockList){
   tBlock *tmp;

   fprintf(fp, "Blocklist:\n");
   for(tmp=*blockList;tmp!=NULL;tmp=BLnext(tmp)){
      Bdump(fp, tmp);
   }

}
