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

   $Id: svc1.c,v 1.2 2005/01/25 14:53:00 uid523 Exp $ */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <svcerrno.h>
#include <svc1.h>

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

int svcErrno;

/* Initialise `file' with file `filename' */

int svcInit(ltsFile *file, char *filename, SVCfileMode fileMode, SVCbool *indexed){
   FILE *fp;
   Bit indexFlag;


   file->fileMode=fileMode;

   if (fileMode==SVCread) {
      fp=fopen(filename,"r");
      if (fp==NULL) {
         svcErrno=EACCESS;
         return -1;
#ifdef _MSC_VER
     _setmode(_fileno(fp),_O_BINARY);
#endif
      } else {

         HTinit(&file->stateTable);
         HTinit(&file->labelTable);
         HTinit(&file->parameterTable);

         /* Init bit stream and read index bit */

         file->bs=BSinit(fp);
         BSreadBit(file->bs,&indexFlag);
         *indexed=indexFlag;
         file->indexFlag=indexFlag;

         /* Init compressed streams */

         file->csStates=CSinit(&file->stateTable, file->bs, *indexed);
         file->csLabels=CSinit(&file->labelTable, file->bs, 0);
         file->csParameters=CSinit(&file->parameterTable, file->bs, 0);

         /* Read file index */

         if (CSureadInt(file->csStates, &file->headerPosition) &&
             CSureadInt(file->csStates, &file->bodyPosition) &&
             CSureadInt(file->csStates, &file->trailerPosition) &&
             CSureadInt(file->csStates, &file->versionPosition)){
            file->firstTransition=1;
            return 0;
         } else {
            svcErrno=EINDEX;
            return -1;
         }
      }
   } else {

      fp=fopen(filename,"w");
      if (fp==NULL) {
         svcErrno=EACCESS;
         return -1;
      } else {

         HTinit(&file->stateTable);
         HTinit(&file->labelTable);
         HTinit(&file->parameterTable);

         /* Init compressed streams */

         file->bs=BSinit(fp);

         file->csStates=CSinit(&file->stateTable, file->bs, *indexed);
         file->csLabels=CSinit(&file->labelTable, file->bs, *indexed);
         file->csParameters=CSinit(&file->parameterTable, file->bs, *indexed);

         /* Reserve space for file index */

         ATfprintf(fp, "                             \n");
         file->headerPosition=0L;
         file->bodyPosition=0L;
         file->trailerPosition=0L;
         file->versionPosition=0L;
         file->firstTransition=1;

         return 0;

      }
   }

}



/* Read the format version from `fp' into `version' */

int svcReadVersion(ltsFile *file, char **version){
   char *str;


   CSflush(file->csStates); 
   CSseek(file->csStates, file->versionPosition, SEEK_SET);

   if (CSureadString(file->csStates, &str)){
      *version=strdup(str);
      return 0;
   }

   svcErrno=EVERSION;
   return -1;

} /* svcReadVersion */



/* Read the header from `bs' into `header' */

int svcReadHeader(ltsFile *file, struct ltsHeader *header){
   char *str;


   CSflush(file->csStates);
   CSseek(file->csStates, file->headerPosition, SEEK_SET);
/*
   if (!CSureadATerm(file->csStates, &tmp) || tmp!= SVC_HEADERLABEL){
      svcErrno=ENOHEADER;
      return -1;
   }
*/
   if (!CSureadString(file->csStates,&str)){
      svcErrno=EFILENAME;
      return -1;
   }
   header->filename=strdup(str);

   if (!CSureadString(file->csStates,&str)){
      svcErrno=EDATE;
      return -1;
   }
   header->date=strdup(str);

   if (!CSureadString(file->csStates,&str)){
      svcErrno=EVERSION;
      return -1;
   }
   header->version=strdup(str);

   if (!CSureadString(file->csStates,&str)){
      svcErrno=EFILETYPE;
      return -1;
   }
   header->type=strdup(str);

   if (!CSureadString(file->csStates, &str)){
      svcErrno=ECREATOR;
      return -1;
   }
   header->creator=strdup(str);


   if (!CSureadInt(file->csStates, &header->numStates)){
      svcErrno=ENUMSTATES;
      return -1;
   }

   if (!CSureadInt(file->csStates, &header->numTransitions)){
      svcErrno=ENUMTRANSITIONS;
      return -1;
   }

   if (!CSureadInt(file->csStates, &header->numLabels)){
      svcErrno=ENUMLABELS;
      return -1;
   }

   if (!CSureadInt(file->csStates, &header->numParameters)){
      svcErrno=ENUMPARAMETERS;
      return -1;
   }

   if (!CSureadString(file->csStates,&str)){
      svcErrno=ECOMMENTS;
      return -1;
   }
   header->initialState=strdup(str);

   if (!CSureadString(file->csStates, &str)){
      svcErrno=ECOMMENTS;
      return -1;
   }
   header->comments=strdup(str);


   return 0;

} /* svcReadHeader */



/* Read the next transition from `file' into `transition' */

int svcReadNextTransition(ltsFile *file, struct ltsTransition *transition){ 

   if (file->firstTransition) {
      CSflush(file->csStates);
      CSseek(file->csStates, file->bodyPosition, SEEK_SET);
      file->firstTransition=0;
   }

   if (file->indexFlag){

      if (!CSreadIndex(file->csStates, &transition->fromState)){ 
         return -1;
      }

      if (!CSreadATerm(file->csLabels, &transition->label)){
         svcErrno=ELABEL;
         return -1;
      }

      if (!CSreadIndex(file->csStates, &transition->toState)){
         svcErrno=ESTATE;
         return -1;
      }

      if (!CSreadATerm(file->csParameters, &transition->parameters)){
         svcErrno=EPARAMETERS;
         return -1;
      }

   } else {

      if (!CSreadATerm(file->csStates, &transition->fromState)){ 
         return -1;
      }

      if (!CSreadATerm(file->csLabels, &transition->label)){
         svcErrno=ELABEL;
         return -1;
      }

      if (!CSreadATerm(file->csStates, &transition->toState)){
         svcErrno=ESTATE;
         return -1;
      }

      if (!CSreadATerm(file->csParameters, &transition->parameters)){
         svcErrno=EPARAMETERS;
         return -1;
      }
   }

   return 0;

} /* svcReadNextTransition */



/* Read the trailer from `file' and check the crc */

int svcReadTrailer(ltsFile *file){
   long crc32;


   CSseek(file->csStates, file->trailerPosition, SEEK_SET);
/*
   if (!CSreadATerm(file->csStates, &tmp) || tmp!=SVC_TRAILERLABEL){
      svcErrno=ENOTRAILER;
      return -1;
   }
*/
   if (!CSreadInt(file->csStates, &crc32)){
      svcErrno=ECRC;
      return -1;
   }

   return 0;

} /* svcReadTrailer */



/* Write the version of the file format into `bs' */

int svcWriteVersion(ltsFile *file){

   CSflush(file->csStates);
   file->versionPosition=CStell(file->csStates);
/*
fprintf(stderr,"WriteVersion %d at %ld\n",  indexFlag, file->versionPosition);
   CSuwriteInt(file->csStates, indexFlag);
*/
   CSuwriteString(file->csStates, SVC_VERSION);

   return 0;

} /* svcWriteVersion */



/* Write the file header from `header' into `fp' */

int svcWriteHeader(ltsFile *file,  struct ltsHeader *header){

   if(file->indexFlag){
      CSwriteIndex(file->csStates, NULL);
   } else {
      CSwriteATerm(file->csStates, NULL);
   }
   
   CSflush(file->csStates);
   file->headerPosition=CStell(file->csStates);

/*
   CSuwriteATerm(file->csStates, SVC_HEADERLABEL);
*/
   CSuwriteString(file->csStates, header->filename);
   CSuwriteString(file->csStates, header->date);
   CSuwriteString(file->csStates, header->version);
   CSuwriteString(file->csStates, header->type);
   CSuwriteString(file->csStates, header->creator);
   CSuwriteInt(file->csStates, header->numStates);
   CSuwriteInt(file->csStates, header->numTransitions);
   CSuwriteInt(file->csStates, header->numLabels);
   CSuwriteInt(file->csStates, header->numParameters);
   CSuwriteString(file->csStates, header->initialState);
   CSuwriteString(file->csStates, header->comments);

   return 0;

} /* svcWriteHeader */



/* Write the transition from `transition' into `fp' */

int svcWriteTransition(ltsFile *file,  struct ltsTransition *transition){

   if (file->bodyPosition==0L){
      CSflush(file->csStates);
      file->bodyPosition=CStell(file->csStates);
   }
/*
   CSwriteATerm(file->csStates, transition->fromState);
   CSwriteATerm(file->csLabels, transition->label);
   CSwriteATerm(file->csStates, transition->toState);
   CSwriteATerm(file->csParameters, transition->parameters);
*/
   if(file->indexFlag){
      CSwriteIndex(file->csStates, transition->fromState);
      CSwriteATerm(file->csLabels, transition->label);
      CSwriteIndex(file->csStates, transition->toState);
      CSwriteATerm(file->csParameters, transition->parameters);
   } else {
      CSwriteATerm(file->csStates, transition->fromState);
      CSwriteATerm(file->csLabels, transition->label);
      CSwriteATerm(file->csStates, transition->toState);
      CSwriteATerm(file->csParameters, transition->parameters);
   }

   return 0;

} /* svcWriteTransition */



/* Write the trailer with computed CRC into `fp' */

int svcWriteTrailer(ltsFile *file){

   CSflush(file->csStates);
   file->trailerPosition=CStell(file->csStates);

/*
   ATfprintf(file->fp, "%t %d\n", SVC_TRAILERLABEL, 666);
*/
/*
   CSwriteATerm(file->csStates, SVC_TRAILERLABEL);
*/
   CSwriteInt(file->csStates, 666);

   return 0;

} /* svcWriteTrailer */



int svcFree(ltsFile *file){

   if(file->fileMode==SVCwrite){
      /* Write index */

      CSflush(file->csStates);
      CSseek(file->csStates, 0L, SEEK_SET);

      BSwriteBit(file->bs,file->indexFlag);
      CSuwriteInt(file->csStates, file->headerPosition);
      CSuwriteInt(file->csStates, file->bodyPosition);
      CSuwriteInt(file->csStates, file->trailerPosition);
      CSuwriteInt(file->csStates, file->versionPosition);
      CSflush(file->csStates);

   }

   /* Clean up memory */

   HTfree(&file->stateTable);
   HTfree(&file->labelTable);
   HTfree(&file->parameterTable);

   CSfree(file->csStates);
   CSfree(file->csLabels);
   CSfree(file->csParameters);
   BSfree(file->bs);

   free(file->formatVersion);
   
   return 0;
}



/* Return an error message string for the svc error number in `errnum' */

char *svcError(int errnum) {

   switch(errnum){
      case EINDEX: return "index section not found";
      case ENOHEADER: return "header section not found";
      case EFILENAME: return "filename in header not found";
      case EDATE    : return "date in header not found";
      case EVERSION : return "version in header not found";
      case EFILETYPE: return "filetypein header not found";
      case ECREATOR : return "creator in header not found";
      case ENUMSTATES: return "number of states in header not found";
      case ENUMTRANSITIONS: return "number of transitions in header not found";
      case ENUMLABELS: return "number of labels in header not found";
      case ENUMPARAMETERS: return "number of parameters in header not found";
      case ECOMMENTS: return "comments in header not found";
      case EACCESS: return strerror(errno);
      case ENOINFO: return "information section not found";
      case EFORMATVERSION: return "version in information not found";
      case ELABEL: return "label in transition not found";
      case ESTATE: return "state in transition not found";
      case EPARAMETERS: return "parameter in transition not found";
      case ENOTRAILER: return "trailer section not found";
      case ECRC: return "crc in trailer not found";
      default: return "Errno undefined";
   }

} /* svcError */

