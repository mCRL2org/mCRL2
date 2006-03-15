/* $Id: svc2dot.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $
   SVC tools -- the SVC (Systems Validation Centre) tool set

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

 $Id: svc2dot.c,v 1.1.1.1 2004/09/07 15:06:33 uid523 Exp $ */

#define NAME     "ltspp"
#define VERSION  "0.1"

#include <getopt.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "ltspp.h"

#include "mcrl2_revision.h"

bool print_state = true;

int main(int argc, char *argv[])
{ FILE *fpOut=NULL;
  int traceLevel;
  ATerm bottom;
  ATinit(argc,argv,&bottom);
  gsEnableConstructorFunctions();

   switch(parseArgs(argc, argv,&fpOut,&traceLevel)) {
      case ERR_ARGS:
         fprintf(stderr,NAME ": incorrect number of arguments supplied (see --help)\n");
    //     doHelp(argv[0]);
         exit(EXIT_ERR_ARGS);
         break;
      case ERR_FILE:
         exit(EXIT_ERR_FILE);
         break;
      case CMD_HELP:
         doHelp(argv[0]);
         exit(EXIT_OK);
         break;
      case CMD_VERSION:
         doVersion();
         exit(EXIT_OK);
      case CMD_CONVERT:
         exit(doConvert(fpOut, traceLevel));
         break;
      default:
         exit(EXIT_OK);
   }


} /* main */


int parseArgs(int argc, char *argv[], FILE **fpOut, int *traceLevel){
   int c,/* ret,*/ cautious;
   extern int optind;
//   char *inFilename=NULL, *outFilename=NULL;
   SVCbool indexed/*, allocatedFileName = SVCfalse*/;
   #define ShortOptions      "hqvn"
   #define VersionOption     0x1
   struct option LongOptions[] = { 
     {"help"      , no_argument,       NULL, 'h'},
     {"version"   , no_argument,       NULL, VersionOption},
     {"quiet"     , no_argument,       NULL, 'q'},
     {"verbose"   , no_argument,       NULL, 'v'},
     {"no-state"  , no_argument,       NULL, 'n'},
     {0, 0, 0, 0}
   };

   *traceLevel=0;
   cautious=0;
   while ((c = getopt_long(argc, argv, ShortOptions, LongOptions, NULL)) != -1) {
      switch(c){
	 case 'h':
		 return CMD_HELP;
	 case VersionOption:
		 return CMD_VERSION;
	 case 'q':
		 *traceLevel=0;
		 break;
	 case 'v':
		 *traceLevel=1;
		 break;
	 case 'n':
		 print_state = false;
		 break;
	 default:
		 return ERR_ARGS;
/*         case 'c': cautious=1;
                   break;
         case 'h': return CMD_HELP;
         case 'o': outFilename=optarg;
                   break;
         case 's': *traceLevel=0;
                   break;
         case 'v': return CMD_VERSION;
         case '?': return ERR_ARGS;*/
      }
   }

   if ( (optind >= argc) || (optind+2 < argc) ) {

      /* No filename is given as argument: this is an error */

      return ERR_ARGS;

   }
   if ( (argc-optind == 2) && !strcmp(argv[optind],argv[optind+1]) )
   {
      fprintf(stderr,NAME ": input file and output file are not allowed to be the same\n");
      return ERR_FILE;
   }



      /* Open the filename given as argument */

//      inFilename = (char*)malloc(sizeof(char)*(strlen(argv[optind])+strlen(INFILE_EXT)+2));
//      strcpy(inFilename, argv[optind]);

      if (SVCopen(&inFile, argv[optind], SVCread, &indexed) !=0) {
            fprintf(stderr, "%s: %s\n", argv[optind],strerror(errno));
            return ERR_FILE;
	      
      }
      if ( !strcmp(SVCgetType(&inFile),"mCRL2") )
      {
            print_state = false;
      } else if ( strcmp(SVCgetType(&inFile),"mCRL2+info") )
      {
	      fprintf(stderr, "error: input file '%s' is not a valid mCRL2 SVC file\n",argv[optind]);
	      return ERR_FILE;
      }
/*      } else {

         // Open the filename given as argument with extension

         if (SVCerrno==EACCESS){

            sprintf(inFilename,"%s%s", argv[optind],INFILE_EXT);

            if (SVCopen(&inFile, inFilename, SVCread, &indexed) >= 0) {
               ret= CMD_CONVERT;
            } else {
               fprintf(stderr, "%s: %s\n", argv[optind], SVCerror(SVCerrno));
               ret= ERR_FILE;
            }
         } else{
            fprintf(stderr, "%s: %s\n", argv[optind], SVCerror(SVCerrno));
            ret= ERR_FILE;
         }

      }*/

   if (optind+2 == argc) {
         if ((*fpOut=fopen(argv[optind+1],"wb"))==NULL) {
            fprintf(stderr, "%s: %s\n", argv[optind+1],strerror(errno));
            return ERR_FILE;
	 }
   } else {
	   *fpOut = stdout;
   }

/*   if (ret==CMD_CONVERT) {

      if (outFilename==NULL){

         // Remove the extension from the input filename

         if(strlen(inFilename)>strlen(INFILE_EXT) &&
            strcmp(inFilename+strlen(inFilename)-strlen(INFILE_EXT),INFILE_EXT)==0) {
            inFilename[strlen(inFilename)-strlen(INFILE_EXT)]='\0';
         } 

         // Compose output filename

         outFilename=(char*) malloc(sizeof(char)*(strlen(inFilename)+strlen(OUTFILE_EXT)+1));
         allocatedFileName = SVCtrue;
         sprintf(outFilename,"%s%s",inFilename,OUTFILE_EXT);

      }

      if (cautious && access(outFilename,F_OK)==0) {
         fprintf(stderr, "%s: file already exists\n", outFilename);
         ret=ERR_FILE;
      } else {
         if ((*fpOut=fopen(outFilename,"w"))!=NULL) {
            ret= CMD_CONVERT;
         } else {
            fprintf(stderr, "%s: %s\n", outFilename,strerror(errno));
            ret= ERR_FILE;
         }
      }

   }*/

//   free(inFilename);
//   if (allocatedFileName) free(outFilename);

   return CMD_CONVERT;

} /* parseArgs */



void doHelp(char *cmd) {
  fprintf(stdout, "Usage: %s [OPTION] INFILE [OUTFILE]\n"
    "Print the LTS in the SVC format from INFILE to OUTFILE in the DOT format. If\n"
    "OUTFILE is not present, stdout is used.\n"
    "\n"
    "Note that the input SVC file must have been generated by lpe2lts.\n"
    "\n"
    "Options:\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
		   "  -n, --no-state        do not save state information\n"
  //                 "-s  Silent: no logging is printed\n");
//                   "-c  Cautious mode: don't overwrite existing files\n");
//                   "-o  Output to `outfile'\n");
		   , cmd);
} /* doHelp */



void doVersion() {

   fprintf(stderr, NAME " " VERSION " (revision %i)\n", REVISION);

} /* doVersion */



int doConvert(FILE *fpOut, int traceLevel) {
   SVCstateIndex fromState, toState;
   SVClabelIndex label/*,taulabel*/;
   SVCparameterIndex parameter;
//   SVCbool tempbool=SVCfalse;
   char name[256], *first, *last;
   if (traceLevel>0){

      /* Get file header info */
      fprintf(stderr, "SVC version %s %s\n", SVCgetFormatVersion(&inFile),
                                       SVCgetIndexFlag(&inFile)?"(indexed)":"");
      fprintf(stderr, " filename      %s\n", SVCgetFilename(&inFile));
      fprintf(stderr, " date          %s\n", SVCgetDate(&inFile));
      fprintf(stderr, " version       %s\n", SVCgetVersion(&inFile));
      fprintf(stderr, " type          %s\n", SVCgetType(&inFile));
      fprintf(stderr, " creator       %s\n",  SVCgetCreator(&inFile));
      fprintf(stderr, " states        %ld\n", SVCnumStates(&inFile));
      fprintf(stderr, " transitions   %ld\n", SVCnumTransitions(&inFile));
      fprintf(stderr, " labels        %ld\n", SVCnumLabels(&inFile));
      fprintf(stderr, " parameters    %ld\n", SVCnumParameters(&inFile));
    ATfprintf(stderr, " initial state %t\n", SVCstate2ATerm(&inFile,SVCgetInitialState(&inFile)));
      fprintf(stderr, " comments      %s\n", SVCgetComments(&inFile));
   }
   
   /* base name of input file */
   strncpy(name, SVCgetFilename(&inFile), 256);
   first = strrchr(name,'/');
   if (!first) first = name;
   last = strrchr(first,'.');
   if (last && !strcmp(last,INFILE_EXT)) *last ='\0';
   
   /* fprintf(fpOut, "des(%ld,%ld,%ld)\n", SVCgetInitialState(&inFile),
                                      SVCnumTransitions(&inFile),
                                      SVCnumStates(&inFile));

   while (SVCgetNextTransition(&inFile, &fromState, &label, &toState, &parameter)) {
      ATfprintf(fpOut, "(%d, %t, %d)\n", fromState, SVClabel2ATerm(&inFile,label), toState);
   } */

  /* adaption to output dot files (by Jan Friso Groote) */
  /* taulabel=SVCnewLabel(&inFile,ATmake("\"tau\""),&tempbool);
  if (tempbool)
  { *//* the label tau did not exist yet, let's try i */
  /*  taulabel=SVCnewLabel(&inFile,ATmake("\"i\""),&tempbool);
    if (tempbool)
    { *//* the label i does not exist also, so forget about tau's */
  /*    taulabel=0;
    }
  }*/
  fprintf(fpOut,"digraph \"%s\" {\n", first);
  /* fprintf(fpOut,"size=\"7,10.5\";\n"); */
  fprintf(fpOut,"center=TRUE;\n");
  fprintf(fpOut,"mclimit=10.0;\n");
  fprintf(fpOut,"nodesep=0.05;\n");
  fprintf(fpOut,"node[width=0.25,height=0.25,label=\"\"];\n");
  fprintf(fpOut,"%d[peripheries=2];\n",SVCgetInitialState(&inFile));
  int last_state = -1;
  while (SVCgetNextTransition(&inFile, &fromState, &label, &toState, &parameter)) {

      int n = (fromState>toState)?fromState:toState;
      if ( print_state && (n > last_state) )
      {
        for (int i=last_state+1; i<=n; i++)
        {
          fprintf(fpOut, "%d[label=\"(",i);
          ATermList args = ATgetArguments((ATermAppl) SVCstate2ATerm(&inFile,fromState));
          for (; !ATisEmpty(args); args=ATgetNext(args))
          {
            gsfprintf(fpOut, "%P", ATgetFirst(args));
            if ( !ATisEmpty(ATgetNext(args)) )
              fprintf(fpOut, ",");
          }
          fprintf(fpOut, ")\"];\n");
        }
        last_state = n;
      }

      ATermAppl t = (ATermAppl) SVClabel2ATerm(&inFile,label);
      if ( !gsIsMultAct(t) ) // for backwards compatibility with untimed svc version
      {
        t = ATAgetArgument(t,0);
      }
/*      AFun s = ATgetAFun(t);
      if (ATisQuoted(s) && ATgetArity(s)==0) 
        ATfprintf(fpOut, "%d->%d[label=%t];\n",fromState, toState, t);
      else
        ATfprintf(fpOut, "%d->%d[label=\"%t\"];\n",fromState, toState, t);*/
      if ( ATisEmpty(ATLgetArgument(t,0)) )
      {
        gsfprintf(fpOut, "%d->%d[label=\"tau\"];\n",fromState, toState);
      } else {
        gsfprintf(fpOut, "%d->%d[label=\"%P\"];\n",fromState, toState, t);
      }
      /*
      if (label==taulabel)
       ATfprintf(fpOut,"color=\"grey\",fontsize=\"18\"]\n");
      else
       ATfprintf(fpOut,"fontsize=\"32\", weight=\"4\"]\n");
     */
  }

  fprintf(fpOut,"}\n");

   if (SVCclose(&inFile)<0){
      if (traceLevel>0) {
         fprintf(stderr, "File trailer corrupt...\n");
      }
      return -1;
   }


   fclose(fpOut);
   return EXIT_OK;

} /* doConvert */
