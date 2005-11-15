/* $Id: svc2fsm.c,v 1.3 2005/04/05 13:23:01 bertl Exp $ */

/* This tool is provisory and not comform the MCRL standards */

#include <string.h>
#include "aterm2.h"
#include "svc.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"

char** names=NULL; // array of parameter names
char** types=NULL; // array of parameter types
static bool is_mcrl2;

static void version(void)
    {
    fprintf(stderr, "svc2fsm: version %s\n", "0.1m2");
    }

#define P(msg)  fprintf(stderr,"%s\n",msg)

static void help(void) {
P("This tool is a translator from svc files to fsm files. The fsm format serves");
P("as input for the visualisation tool made by Frank van Ham.");
P("This tool is available for MS-windows and Linux and can be obtained");
P("at www.win.tue.nl/~fvham/fsm/. Explanations of the visual tool can");
P("be found in F. van Ham, H. van de Wetering and J.J. van Wijk. Visualisation");
P("state transition graphs. IEEE conf. Information Visualisation '01,");
P("pp. 59-66, 2001, and in Interactive Visualization of State Transition Systems.");
P("F. van Ham, H. van de Wetering, and J. van Wijk.");
P("IEEE transactions on visualization and computer graphics,");
P("volume 8, number 4, pp319-329.");
P("Applications can be found in J.F. Groote and F. van Ham.");
P("State space visualisation. Technical Report. Eindhoven");
P("University of Technology, 2002. The tool svc2fsm has been made");
P("by Huub van de Wetering.");
P("===================================================================");
P("");
P("Usage: svc2fsm file.svc [file.ext]");
P("");
P("If the .svc file is made using the muCRL instantiator with the flag");
P("-svc-term or using the mCRL2 lpe2lts without the flag --no-info, then");
P("the visualisation tool can visualize the values");
P("of process parameters. When using the optional second argument (a .tbf or .lpe,");
P("depending on the tool used to generate the state space)");
P("the tool will also indicate the proper names of these variables.");
P("");
P("The following options can be used:");
P("-help:       yields this message");
}
#undef P

static void print_states(SVCfile file, int* in, int* out) {

    // collect state information //////////////////////////////////////////
    SVCint nos=SVCnumStates(&file), nov, i, j, k;     // number of states
    ATermIndexedSet *set;    // storage for the different values 
                            // of the parameters
    int *a;
    ATermList* state = (ATermList*) malloc(nos*sizeof(ATermList));
    fprintf(stderr,"collect state data ...\n");

    // create array of states
    if (state==NULL) {
	fprintf(stderr,"malloc failed");exit(2);
    }

    // gather state information
    if ( is_mcrl2 )
    {
       for(i = 0 ; i < nos; i++) {
            state[i]=ATgetArguments((ATermAppl)SVCstate2ATerm(&file,i)); 
            if (state[i]==NULL) fprintf(stderr,"\nstate[%d]==null\n",i);
       }
    } else {
       for(i = 0 ; i < nos; i++) {
            state[i]=(ATermList)SVCstate2ATerm(&file,i); 
            if (state[i]==NULL) fprintf(stderr,"\nstate[%d]==null\n",i);
       }
    }
    fprintf(stderr,"\n");


    // collect state variable values ///////////////////////////////////////
    if ( ATgetType((ATerm) state[0]) == AT_LIST )
        nov=ATgetLength(state[0]);   // number of state variables
    else                                // assuming that all states have the
        nov=0;                          // same number of variables
    
    set = (ATermIndexedSet*) malloc(nov*sizeof(ATermIndexedSet));
    if (set==NULL) {
	fprintf(stderr,"malloc set failed %d", nov);exit(2);
    }   
    fprintf(stderr,"collect state variable values ...");
    a = (int*) malloc(nov*sizeof(int));
    if (a==NULL) {
	fprintf(stderr,"malloc a failed %d", nov);exit(2);
    }                // number of different values per variable
    for(j = 0 ; j < nov; j++)
    {
      set[j]=ATindexedSetCreate(30,50);  // size=100, fill
                                         // percentage=50%
      a[j]=0;

      //for each state
      for(i = 0 ; i < nos; i++)
      {
         // get value of state variable j in state i
         ATerm vj=ATgetFirst(state[i]);  // state variable j
         ATbool newValue;                // set to true if value did not exist in set
         // store value in set
         ATindexedSetPut(set[j],vj,&newValue);
         // count number of different values
         if (newValue) a[j]=a[j]+1;   
         // take next element of list
         state[i]=ATgetNext(state[i]);
      }
    }
    fprintf(stderr,"\n");

    // restore state data
    fprintf(stderr,"restore state data ...");
    if ( is_mcrl2 )
    {
       for(i = 0 ; i < nos; i++) {
            state[i]=ATgetArguments((ATermAppl)SVCstate2ATerm(&file,i)); 
            if (state[i]==NULL) fprintf(stderr,"\nstate[%d]==null\n",i);
       }
    } else {
       for(i = 0 ; i < nos; i++) {
            state[i]=(ATermList)SVCstate2ATerm(&file,i); 
            if (state[i]==NULL) fprintf(stderr,"\nstate[%d]==null\n",i);
       }
    }
    fprintf(stderr,"\n");

   // print the variables with name and type from names and types /////////
   // if these are not available use "si" and "unknown"
   fprintf(stderr,"print variable table ...");
   for(j = 0 ; j < nov; j++) {
     if (names==NULL)
        fprintf(stdout,"s%1d(%d) %s ",j,a[j], "unknown");
     else
        fprintf(stdout,"%s(%d) %s ",names[j],a[j], types[j]);
     if ( is_mcrl2 )
     {
       for(k=0;k<a[j];k++) 
         gsfprintf(stdout," \"%P\"",ATindexedSetGetElem(set[j],k));
     } else {
       for(k=0;k<a[j];k++) 
         gsfprintf(stdout," \"%T\"",ATindexedSetGetElem(set[j],k));
     }
     fprintf(stdout,"\r\n");
   }
   if (in!=NULL) fprintf(stdout,"fan_in(0)\r\n");
   if (in!=NULL) fprintf(stdout,"fan_out(0)\r\n");
   fprintf(stdout,"node_nr(0)\r\n");
   fprintf(stderr,"\n");

   fprintf(stdout,"---\r\n");

   // for each state
    fprintf(stderr,"print states ...");
    for(i = 0 ; i < nos; i++)
    {
      //for each state variable
      for(j = 0 ; j < nov; j++)
      {
         // get value of state variable j in state i
         ATerm vj=ATgetFirst(state[i]);  // state variable j
         ATbool newValue;                // set to true if value did not exist in set
         // get index in set
         long value=(short)ATindexedSetPut(set[j],vj,&newValue);
         fprintf(stdout,"%d ",value);
         // take next element of list
         state[i]=ATgetNext(state[i]);
      }
      if (in !=NULL) fprintf(stdout,"%d ",in[i] );
      if (out!=NULL) fprintf(stdout,"%d ",out[i]);
      fprintf(stdout,"%d\r\n",i+1);
    }
   fprintf(stderr,"\n");
   free(state);free(a);free(set);
}

static ATermList global_param=NULL;

static void readParameterNames(char* filename, char*** names, char*** types) {
	FILE* file=fopen(filename,"r");
        if (file==NULL) {
	     fprintf(stderr,"Unable to open TBF-file %s.\n",filename);
	     *names = *types = NULL;
             return;
	}
{
        int n, i = 0;
	ATerm t =ATreadFromFile(file);
	// file should have following form:
	// spec2gen(<term>,initprocspec(<term>,<term>,<term>))
	// where the third term is an ATermList of parameters.
	// where each parameter has the form  v(<str>,<str>)
	// where the strings are the name and type of the parameter,
	// respectively
	ATerm ht1, ht2, ht3;
	ATermList param;
	ATprotect((ATerm *)&global_param);
	if ( !ATmatch(t,
               "spec2gen(<term>,initprocspec(<term>,<term>,<term>))",
                         &ht1,               &ht2,  &param,&ht3)
           )
		ATerror("no match in %t",t);
	
	//We use the assignment below to protect the names of
	//parameters from being garbage collected.
	global_param=param;
	//gsfprintf(stderr,"parameters=%T\n",param);
	
	// peel of parameters one by one
	 n=ATgetLength(param);
	*names=(char**) malloc(n*sizeof(char*));
	*types=(char**) malloc(n*sizeof(char*));
	while (!ATisEmpty(param)) {
		char* name; char* type;

		ATerm at=ATgetFirst(param);
		if (!ATmatch(at,"v(<str>,<str>)",&name,&type))
                   ATerror("no match for parameter %t",at);
		
		// remove '#' from name
		{ int n=strlen(name); if (name[n-1]=='#') name[n-1]='\0'; }

		(*names)[i  ]=name;
		(*types)[i++] =type;
		param=ATgetNext(param);
	}
	fprintf(stderr,"\n");
}
}

static void readParameterNames2(char* filename, char*** names, char*** types)
{
	FILE* file = fopen(filename,"r");
        if ( file == NULL )
	{
	     fprintf(stderr,"Unable to open LPE-file %s.\n",filename);
	     *names = *types = NULL;
             return;
	}

        int n, i = 0;
	ATerm t =ATreadFromFile(file);
	
	if ( (t == NULL) || (ATgetType(t) != AT_APPL) || !gsIsSpecV1((ATermAppl) t) )
	{
	     fprintf(stderr,"Invalid LPE-file %s.\n",filename);
	     *names = *types = NULL;
             return;
	}
	
	ATermList param = ATLgetArgument(ATAgetArgument((ATermAppl) t,5),1);
	//gsfprintf(stderr,"parameters=%T\n",param);
	
	// peel of parameters one by one
	 n=ATgetLength(param);
	*names=(char**) malloc(n*sizeof(char*));
	*types=(char**) malloc(n*sizeof(char*));
	while (!ATisEmpty(param))
	{
		ATerm at = ATgetFirst(param);
		
		(*names)[i  ] = strdup(PrintPart_CXX(at,ppAdvanced).c_str());
		(*types)[i++] = strdup(PrintPart_CXX(ATgetArgument((ATermAppl) at,1),ppAdvanced).c_str());

		param = ATgetNext(param);
	}
	fprintf(stderr,"\n");
}

// compute number of ingoing and outgoing edges
static void compute_in_out(SVCfile file,int** in, int** out) 
{
    int nos=SVCnumStates(&file), i, notr;

    // create arrays
    *in =(int*)malloc(nos*sizeof(int));
    *out=(int*)malloc(nos*sizeof(int));
     if(in==NULL || out==NULL) {
         fprintf(stderr,"malloc failed"); exit(2);
     }

    // initialize arrays
    for(i=0;i<nos;i++) { (*in)[i]=(*out)[i]=0; }

    notr=SVCnumTransitions(&file);

    fprintf(stderr,"read transactions");
    // compute fan in, fan out, and edges
    for(i = 0 ; i < notr; i++)
    {
      SVCparameterIndex pi;
      SVClabelIndex     li;
      SVCstateIndex source,dest;
      SVCgetNextTransition(&file, &source, &li, &dest, &pi) ;
      if ((i+1)%100000==0 || i+1==notr)  {
        if ((i+1)%1000000==0 || i+1==notr) 
  	     fprintf(stderr,"(%d)",i+1);
        else fprintf(stderr,".");
      }
      (*out)[source]++;
      (*in )[dest]++;
    }
    fprintf(stderr,"\n");
}

// 
// print the edges : source destination label
//
static void print_edges(SVCfile file) {
    int notr=SVCnumTransitions(&file), i;
    fprintf(stderr,"print edges");
    fprintf(stdout,"---\r\n");
    for(i = 0 ; i < notr; i++) {
        SVCparameterIndex pi;
        SVClabelIndex     li;
        SVCstateIndex source,dest;
        SVCgetNextTransition(&file, &source, &li, &dest, &pi) ;
	if ( is_mcrl2 )
           gsfprintf(stdout,"%1d %1d %P\r\n",source+1,dest+1,SVClabel2ATerm(&file,li));
	else
           gsfprintf(stdout,"%1d %1d %T\r\n",source+1,dest+1,SVClabel2ATerm(&file,li));
        if ((i+1)%100000==0 || i+1==notr)  {
          if ((i+1)%1000000==0 || i+1==notr) 
  	       fprintf(stderr,"(%d)",i+1);
          else fprintf(stderr,".");
        }
    }
    fprintf(stderr,"\n\n");
}



int main(int argc,char** argv) 
{
  SVCbool   tmpBool=SVCfalse;
  SVCfile file;
  
  ATerm bottom;
  if (argc > 1)
  {
  if (!strcmp(argv[1],"-version")) {
	    version();
	    exit(0);
	    }
  if (!strcmp(argv[1],"-help")) {
	    help();
	    exit(0);
	    }
  }
  ATinit(argc, argv, &bottom);

  // 2nd argument is file for reading variable names & types
  // if none available, default values are used
  if (argc==1||argc>3) {
     fprintf(stderr,"Usage: %s svc-file [tbf-file]\n",argv[0]);
     exit(1);
  }
  // 1st argument is svc file
  // Open the file and start reading all objects.
  if(SVCopen(&file, argv[1], SVCread, &tmpBool) == 0)
  {
    int* in;  // array of ingoing edges
    int* out; // array of outgoing edges
    // do something with file
    // ...
    fprintf(stderr,"Number of parameters : %d\n", SVCnumParameters(&file));
    fprintf(stderr,"Number of states     : %d\n", SVCnumStates(&file));
    fprintf(stderr,"Number of labels     : %d\n", SVCnumLabels(&file));
    fprintf(stderr,"Number of transitions: %d\n", SVCnumTransitions(&file));

    is_mcrl2 = !strcmp("mCRL2+info",SVCgetType(&file));
    if ( !strcmp("mCRL2",SVCgetType(&file)) )
    {
       gsfprintf(stderr,"state space does not contain state information\n");
       is_mcrl2 = true;
    }
    if ( is_mcrl2 )
    {
       gsEnableConstructorFunctions();
    }

    if (argc==3)
    {
       if ( is_mcrl2 )
          readParameterNames2(argv[2],&names,&types);
       else 
          readParameterNames(argv[2],&names,&types);
    } else { 
       names=types=NULL;
    }

    //
    // compute number of incoming and outgoing edges
    //
    
    compute_in_out(file,&in,&out);

    //
    // write states
    //
    print_states(file,in,out);

    SVCclose(&file);    // close

    //
    // write edges
    //

    SVCopen(&file, argv[1], SVCread, &tmpBool);  // reopen

    print_edges(file);

    SVCclose(&file);                             // close
    return 0;
  }
  else
  {
	fprintf(stderr,"Unable to open SVC-file %s.\n",argv[1]);
	return 1;
  }
}                                                                             //
