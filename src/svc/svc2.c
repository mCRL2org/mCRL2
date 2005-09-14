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

   $Id: svc2.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#include <string.h>
#include <time.h>
#include "svcerrno.h"
#include "svc.h"

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

extern int svcErrno;
int SVCerrno;


   /* SVCnewLabel assigns a new index to a label
      of a transition, given as an ATerm. The last
      variable indicates whether the addition is
      actually a new one (1=new). SVCnewLabel returns 0 if an 
      error occurred, for instance because there
      is no room left to store the new term */

SVClabelIndex SVCnewLabel(SVCfile *file, ATerm term, SVCbool *_new){
   SVClabelIndex index;


   if(HTmember(&file->file.labelTable, term, &index)){
      *_new=SVCfalse;
      return index;
   } else{
      *_new=SVCtrue;
      if(file->file.fileMode==SVCwrite){
         file->header.numLabels++;
      }
      return HTinsert(&file->file.labelTable, term, NULL);
   }

}

   /* SVCaterm2Label gives the label index belonging
      to an ATerm. If no such label exists, the 
      value -1 is returned */

SVClabelIndex SVCaterm2Label(SVCfile *file, ATerm term){
   SVClabelIndex index;


   if (HTmember(&file->file.labelTable, term, &index)){
      return index;
   } else {
      return -1L;
   }

}

   /* SVClabel2ATerm provides the ATerm that belongs
      to a label index. In case of an error NULL is
      returned */

ATerm SVClabel2ATerm(SVCfile *file, SVClabelIndex index){

   return HTgetTerm(&file->file.labelTable, index);

}


SVCstateIndex SVCnewState(SVCfile *file, ATerm term, SVCbool *_new){
   SVCstateIndex index;


   if(HTmember(&file->file.stateTable, term, &index)){
      *_new=SVCfalse;
   } else{
      *_new=SVCtrue;
      if(file->file.fileMode==SVCwrite){
         file->header.numStates++;
      }
      index=HTinsert(&file->file.stateTable, term, NULL);
   }

   return index;

}


SVCstateIndex SVCaterm2State(SVCfile *file, ATerm term){
   SVCstateIndex index;


   if (HTmember(&file->file.stateTable, term, &index)){
      return index;
   } else {
      return -1L;
   }

}

ATerm SVCstate2ATerm(SVCfile *file, SVCstateIndex index){

   return HTgetTerm(&file->file.stateTable, index);

}


SVCparameterIndex SVCnewParameter(SVCfile *file, ATerm term, SVCbool *_new){
   SVCparameterIndex index;


   if(HTmember(&file->file.parameterTable, term, &index)){
      *_new=SVCfalse;
      return index;
   } else{
      *_new=SVCtrue;
      if(file->file.fileMode==SVCwrite){
         file->header.numParameters++;
      }
      return HTinsert(&file->file.parameterTable, term,NULL);
   }

}



SVCparameterIndex SVCaterm2Parameter(SVCfile *file, ATerm term){
   SVCparameterIndex index;


   if (HTmember(&file->file.parameterTable, term, &index)){
      return index;
   } else {
      return -1L;
   }

}



ATerm SVCparameter2ATerm(SVCfile *file, SVCparameterIndex index){

   return HTgetTerm(&file->file.parameterTable, index);
}


int SVCputTransition(SVCfile *file, 
         SVCstateIndex fromStateIndex, 
         SVClabelIndex labelIndex, 
         SVCstateIndex toStateIndex, 
         SVCparameterIndex paramIndex){
   struct ltsTransition transition;
   ATerm fromStateTerm,
         toStateTerm,
         labelTerm,
         paramTerm;
   int   ret;

   fromStateTerm=HTgetTerm(&file->file.stateTable, fromStateIndex);
   toStateTerm=HTgetTerm(&file->file.stateTable, toStateIndex);
   labelTerm=HTgetTerm(&file->file.labelTable, labelIndex);
   paramTerm=HTgetTerm(&file->file.parameterTable, paramIndex);

   if(fromStateTerm != NULL && toStateTerm !=NULL && 
      labelTerm != NULL     && paramTerm   != NULL) {
      transition.fromState=fromStateTerm;
      transition.toState=toStateTerm;
      transition.label=labelTerm;
      transition.parameters=paramTerm;

      ret=svcWriteTransition(&file->file, &transition);
      file->header.numTransitions++;

      return ret;

   } else {

      return 0;

   }
}


int SVCgetNextTransition(SVCfile *file, 
        SVCstateIndex *fromStateIndex, SVClabelIndex *labelIndex, 
        SVCstateIndex *toStateIndex, SVCparameterIndex *paramIndex){
   struct ltsTransition transition;


   if(svcReadNextTransition(&file->file, &transition)==0){

      HTmember(&file->file.stateTable,transition.fromState,fromStateIndex);
      HTmember(&file->file.stateTable,transition.toState,toStateIndex);
      HTmember(&file->file.parameterTable,transition.parameters,paramIndex);
      HTmember(&file->file.labelTable,transition.label,labelIndex);
      return 1;
   } else {
      return 0;
   }
}


int SVCopen(SVCfile *file, char *filename, SVCfileMode mode, SVCbool *indexed){
   char *version;
   char buffy[256];
   time_t now;
   SVCbool _new;


   switch(mode){
      case SVCwrite:

         if(svcInit(&file->file, filename, SVCwrite, indexed)>=0){

            (void) time(&now);
            strftime(buffy,256,"%x", localtime(&now) );

            file->header.comments=strdup("");
            file->header.type=strdup("generic");
            file->header.version=strdup("0.0");
            file->header.filename=strdup(filename);
            file->header.date=strdup(buffy);
            file->header.creator=strdup("svclib");
            file->header.initialState=strdup("0");
            file->header.numStates=0L;
            file->header.numTransitions=0L;
            file->header.numLabels=0L;
            file->header.numParameters=0L;
            file->file.indexFlag=*indexed;
            file->file.formatVersion=strdup(SVC_VERSION);

            return 0;

         } else {
            SVCerrno=svcErrno;
            return -1;
         }

      case SVCread:

         if(svcInit(&file->file, filename, SVCread, indexed)>=0){
            if(svcReadVersion(&file->file, &version)==0 &&
               svcReadHeader(&file->file, &file->header)==0) {
              file->file.formatVersion=version;
              if(SVCnewState(file, ATmake(file->header.initialState), &_new)<0){
                 SVCerrno=ENEWSTATE;
                 return -1;
              }

              return 0;
            } else {
              SVCerrno=svcErrno;
              return -1;
            }
         } else {
            SVCerrno=svcErrno;
            return -1;
         }

      default:
         SVCerrno=EFILEMODE;
         return -1;
   }
   
}



int SVCclose(SVCfile *file){


   if (file->file.fileMode==SVCwrite){

      /* Write file header and trailer */

      svcWriteHeader(&file->file,&file->header);
      svcWriteVersion(&file->file);
      svcWriteTrailer(&file->file);
   }

   /* Clean up memory */

   svcFree(&file->file);

   free(file->header.comments);
   free(file->header.type);
   free(file->header.version);
   free(file->header.filename);
   free(file->header.date);
   free(file->header.creator);
   free(file->header.initialState);

   /* Close file */

   return 0;
}

char *SVCgetFormatVersion(SVCfile *file){

   return file->file.formatVersion;

}

SVCbool SVCgetIndexFlag(SVCfile *file){

   return file->file.indexFlag;

}


SVCstateIndex SVCgetInitialState(SVCfile *file){

   return SVCaterm2State(file, ATmake(file->header.initialState));
}

int SVCsetInitialState(SVCfile *file, SVCstateIndex index){
   char str[16];

   free(file->header.initialState);
   if(file->file.indexFlag){
      sprintf(str,"%ld",index);
      file->header.initialState=strdup(str); 
   } else {
      file->header.initialState=strdup(ATwriteToString(SVCstate2ATerm(file,index))); 
   }
   return 0;
}

/* Functions to put and get header information */
char *SVCgetComments(SVCfile *file){

   return file->header.comments;

}
int   SVCsetComments(SVCfile *file, char *comments){

   free(file->header.comments);
   file->header.comments=strdup(comments);
   return 0;
}
char *SVCgetType(SVCfile *file){

   return file->header.type;

}
int   SVCsetType(SVCfile *file, char *type){

   free(file->header.type);
   file->header.type=strdup(type);
   return 0;
}
char *SVCgetVersion(SVCfile *file){

   return file->header.version;

}
int   SVCsetVersion(SVCfile *file, char *version){

   free(file->header.version);
   file->header.version=strdup(version);
   return 0;

}
char *SVCgetCreator(SVCfile *file){

   return file->header.creator;

}
int   SVCsetCreator(SVCfile *file, char *creator){

   free(file->header.creator);
   file->header.creator=strdup(creator);
   return 0;
}
char *SVCgetDate(SVCfile *file){

   return file->header.date;

}
char *SVCgetFilename(SVCfile *file){

   return file->header.filename;
   return 0;

}
SVCint SVCnumStates(SVCfile *file){

   return file->header.numStates;

}
SVCint SVCnumLabels(SVCfile *file){

   return file->header.numLabels;
}
SVCint SVCnumParameters(SVCfile *file){

   return file->header.numParameters;

}
SVCint SVCnumTransitions(SVCfile *file){

   return file->header.numTransitions;
}


char *SVCerror(int errnum){

   switch(errnum){
      default: return svcError(errnum);
   }

}
