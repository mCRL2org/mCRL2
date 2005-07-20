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

   $Id: svc1.h,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

/* svc1.h: Header for svc level 1 interface */

#ifndef _SVC1_H
#define _SVC1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include "aterm2.h"

#include "compress.h"

#define SVC_VERSION     "1.2 beta"

typedef long             SVCint;
typedef enum {SVCfalse=0, 
              SVCtrue}    SVCbool;
typedef enum {SVCwrite, 
              SVCread}    SVCfileMode;


typedef struct {
   CompressedStream *csStates, 
                    *csLabels, 
                    *csParameters;
   BitStream *bs;
   SVCbool firstTransition, 
           indexFlag;
   char *formatVersion;
   SVCfileMode fileMode;
   long headerPosition,
        bodyPosition,
        trailerPosition,
        versionPosition;
   HTable stateTable,
          labelTable,
          parameterTable;
} ltsFile;


struct ltsHeader {
   char *filename,     /* filename */
        *date,         /* file creation date */
        *version,      /* file version */
        *type,         /* file subtype */
        *creator,      /* name of person or software */
        *initialState, /* label of the initial state */
        *comments;     /* comments on the file */
   long numStates,     /* number of states in the system */
        numTransitions,/* number of transitions in the system */
        numLabels,
        numParameters;
};



struct ltsTransition {
   ATerm fromState,    /* label of source state */
         toState,      /* label of destination state */
         label,        /* label of transition */
         parameters;   /* parameters of the transition */
};


int svcInit(ltsFile *, char *, SVCfileMode, SVCbool *);
int svcReadVersion(ltsFile *, char **version);
int svcReadHeader(ltsFile *, struct ltsHeader *);
int svcReadNextTransition(ltsFile *, struct ltsTransition *);
int svcReadTrailer(ltsFile *);
int svcWriteVersion(ltsFile *);
int svcWriteHeader(ltsFile *,  struct ltsHeader *);
int svcWriteTransition(ltsFile *,  struct ltsTransition *);
int svcWriteTrailer(ltsFile *);
int svcFree(ltsFile *);
char *svcError(int errnum);

#ifdef __cplusplus
}
#endif

#endif
