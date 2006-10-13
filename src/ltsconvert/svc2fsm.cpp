/*
 * This tool is based on the version (taken from the muCRL toolset) of Huub van
 * de Wetering for the FSMView tool of Frank van Ham.
 */

#include <cstring>
#include <getopt.h>
#include "aterm2.h"
#include "svc/svc.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"

#define NAME "svc2fsm"
#define VERSION "0.1m2"

char** names=NULL; // array of parameter names
char** types=NULL; // array of parameter types
int num_names = 0;
static bool is_mcrl2;

static void print_version(FILE *f)
{
  fprintf(f, NAME " " VERSION " (revision %i)\n", REVISION);
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE [OUTFILE]\n"
    "Convert a state space in the SVC format in INFILE to the FSM format and save the\n"
    "result to OUTFILE. If OUTFILE is not supplied, stdout is used.\n"
    "\n"
    "Note that tools that use the FSM format may depend on state information and\n"
    "correct parameter names. The former requires that the state information is in\n"
    "the SVC file and the latter that the -l option is used (supplying the\n"
    "corresponding LPE).\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message\n"
    "      --version         display version information\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         display extra information about the state space\n"
    "                        generation\n"
    "  -l, --lpe=FILE        supply LPE file from which the input SVC was generated\n"
    "                        (needed to store the correct parameter names of states)\n"
    ,Name
    );
}

static void print_states(FILE *f, SVCfile file, int* in, int* out) {

    // collect state information //////////////////////////////////////////
    SVCint nos=SVCnumStates(&file), nov, i, j, k;     // number of states
    ATermIndexedSet *set;    // storage for the different values 
                            // of the parameters
    int *a;
    ATermList* state = (ATermList*) malloc(nos*sizeof(ATermList));
    for (i=0; i<nos; i++)
	    state[i] = NULL;
    ATprotectArray((ATerm *) state,nos);
    gsVerboseMsg("collecting state data...\n");

    // create array of states
    if (state==NULL)
    {
      gsErrorMsg("malloc failed");
      exit(2);
    }

    // gather state information
    if ( is_mcrl2 )
    {
       for(i = 0 ; i < nos; i++) {
            state[i]=ATgetArguments((ATermAppl)SVCstate2ATerm(&file,i)); 
            if (state[i]==NULL) gsErrorMsg("\nstate[%d]==null\n",i);
       }
    } else {
       for(i = 0 ; i < nos; i++) {
            state[i]=(ATermList)SVCstate2ATerm(&file,i); 
            if (state[i]==NULL) gsErrorMsg("\nstate[%d]==null\n",i);
       }
    }


    // collect state variable values ///////////////////////////////////////
    if ( ATgetType((ATerm) state[0]) == AT_LIST )
        nov=ATgetLength(state[0]);   // number of state variables
    else                                // assuming that all states have the
        nov=0;                          // same number of variables
    
    set = (ATermIndexedSet*) malloc(nov*sizeof(ATermIndexedSet));
    if (set==NULL)
    {
      gsErrorMsg("malloc set failed %d", nov);
      exit(2);
    }   
    gsVerboseMsg("collecting state parameter values...\n");
    a = (int*) malloc(nov*sizeof(int));
    if (a==NULL)
    {
      gsErrorMsg("malloc a failed %d", nov);
      exit(2);
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

    // restore state data
    gsVerboseMsg("restoring state data...\n");
    if ( is_mcrl2 )
    {
       for(i = 0 ; i < nos; i++) {
            state[i]=ATgetArguments((ATermAppl)SVCstate2ATerm(&file,i)); 
            if (state[i]==NULL) gsErrorMsg("\nstate[%d]==null\n",i);
       }
    } else {
       for(i = 0 ; i < nos; i++) {
            state[i]=(ATermList)SVCstate2ATerm(&file,i); 
            if (state[i]==NULL) gsErrorMsg("\nstate[%d]==null\n",i);
       }
    }

    // print the variables with name and type from names and types /////////
    // if these are not available use "si" and "unknown"
    gsVerboseMsg("writing parameter table...\n");
    for(j = 0 ; j < nov; j++) {
      if ( j < num_names )
         fprintf(f,"%s(%d) %s ",names[j],a[j], types[j]);
      else
         fprintf(f,"unknown%1d(%d) %s ",(int) j,a[j], "unknown");
      if ( is_mcrl2 )
      {
        for(k=0;k<a[j];k++) 
          gsfprintf(f," \"%P\"",ATindexedSetGetElem(set[j],k));
      } else {
        for(k=0;k<a[j];k++) 
          gsfprintf(f," \"%T\"",ATindexedSetGetElem(set[j],k));
      }
      fprintf(f,"\n");
    }
    if (in!=NULL)
    {
      int max = 0;
      for (int i=0; i<nos; i++)
      {
        if ( in[i] > max )
        {
          max = in[i];
        }
      }
      fprintf(f,"fan_in(%i) Nat ",max+1);
      for (int i=0; i<=max; i++)
      {
        fprintf(f," \"%i\"",i);
      }
      fprintf(f,"\n");
    }
    if (out!=NULL)
    {
      int max = 0;
      for (int i=0; i<nos; i++)
      {
        if ( out[i] > max )
        {
          max = out[i];
        }
      }
      fprintf(f,"fan_out(%i) Nat ",max+1);
      for (int i=0; i<=max; i++)
      {
        fprintf(f," \"%i\"",i);
      }
      fprintf(f,"\n");
    }
    fprintf(f,"node_nr(0)\n");
 
    fprintf(f,"---\n");
 
    // for each state
    gsVerboseMsg("writing states...\n");
    for(i = 0 ; i < nos; i++)
    {
      //for each state variable
      for(j = 0 ; j < nov; j++)
      {
         // get value of state variable j in state i
         ATerm vj=ATgetFirst(state[i]);  // state variable j
         ATbool newValue;                // set to true if value did not exist in set
         // get index in set
         short value = ATindexedSetPut(set[j],vj,&newValue);
         fprintf(f,"%hd ",value);
         // take next element of list
         state[i]=ATgetNext(state[i]);
      }
      if (in !=NULL) fprintf(f,"%d ",in[i] );
      if (out!=NULL) fprintf(f,"%d ",out[i]);
      fprintf(f,"%ld\n",i+1);
    }
    ATunprotectArray((ATerm *) state);
    free(state);free(a);free(set);
}

static ATermList global_param=NULL;

static void readParameterNames(char* filename, char*** names, char*** types)
{
  FILE* file=fopen(filename,"rb");
  if (file==NULL)
  {
    gsErrorMsg("unable to open TBF-file %s.\n",filename);
    *names = *types = NULL;
    return;
  }

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
  num_names = n;
  *names=(char**) malloc(n*sizeof(char*));
  *types=(char**) malloc(n*sizeof(char*));
  while (!ATisEmpty(param))
  {
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
}

static void readParameterNames2(char* filename, char*** names, char*** types)
{
  FILE* file = fopen(filename,"rb");
  if ( file == NULL )
  {
    gsErrorMsg("unable to open LPE-file %s.\n",filename);
    *names = *types = NULL;
    return;
  }
  
  int n, i = 0;
  ATerm t =ATreadFromFile(file);
  
  if ( (t == NULL) || (ATgetType(t) != AT_APPL) || !gsIsSpecV1((ATermAppl) t) )
  {
    gsErrorMsg("invalid LPE-file %s.\n",filename);
    *names = *types = NULL;
    return;
  }
  
  ATermList param = ATLgetArgument(ATAgetArgument((ATermAppl) t,5),1);
  //gsfprintf(stderr,"parameters=%T\n",param);
  
  // peel of parameters one by one
  n=ATgetLength(param);
  num_names = n;
  *names=(char**) malloc(n*sizeof(char*));
  *types=(char**) malloc(n*sizeof(char*));
  while (!ATisEmpty(param))
  {
    ATerm at = ATgetFirst(param);

    (*names)[i  ] = strdup(PrintPart_CXX(at,ppDefault).c_str());
    (*types)[i++] = strdup(PrintPart_CXX(ATgetArgument((ATermAppl) at,1),ppDefault).c_str());

    param = ATgetNext(param);
  }
}

// compute number of ingoing and outgoing edges
static void compute_in_out(SVCfile file,int** in, int** out) 
{
    int nos=SVCnumStates(&file), i, notr;

    // create arrays
    *in =(int*)malloc(nos*sizeof(int));
    *out=(int*)malloc(nos*sizeof(int));
     if(in==NULL || out==NULL)
     {
        gsErrorMsg("malloc failed"); 
        exit(2);
     }

    // initialize arrays
    for(i=0;i<nos;i++) { (*in)[i]=(*out)[i]=0; }

    notr=SVCnumTransitions(&file);

    gsVerboseMsg("reading transitions...");
    // compute fan in, fan out, and edges
    for(i = 0 ; i < notr; i++)
    {
      SVCparameterIndex pi;
      SVClabelIndex     li;
      SVCstateIndex source,dest;
      SVCgetNextTransition(&file, &source, &li, &dest, &pi) ;
      if ((i+1)%100000==0 || i+1==notr)  {
        if ((i+1)%1000000==0 || i+1==notr) 
  	     gsVerboseMsg(" (%d) ",i+1);
        else gsVerboseMsg(".");
      }
      (*out)[source]++;
      (*in )[dest]++;
    }
    gsVerboseMsg("\n");
}

// 
// print the edges : source destination label
//
static void print_edges(FILE *f, SVCfile file)
{
  int notr=SVCnumTransitions(&file), i;
  gsVerboseMsg("writing edges...");
  fprintf(f,"---\n");
  for(i = 0 ; i < notr; i++)
  {
    SVCparameterIndex pi;
    SVClabelIndex     li;
    SVCstateIndex source,dest;
    SVCgetNextTransition(&file, &source, &li, &dest, &pi) ;
    if ( is_mcrl2 )
    {
       ATerm lab = SVClabel2ATerm(&file,li);
       if ( !gsIsMultAct((ATermAppl) lab) ) // check for backwards compatibility for untimed svc versions
       {
	       lab = ATgetArgument((ATermAppl) lab,0);
       }
       gsfprintf(f,"%1d %1d \"%P\"\n",source+1,dest+1,lab);
    } else {
       gsfprintf(f,"%1d %1d %T\n",source+1,dest+1,SVClabel2ATerm(&file,li));
    }
    if ((i+1)%100000==0 || i+1==notr)
    {
      if ((i+1)%1000000==0 || i+1==notr) 
             gsVerboseMsg(" (%d) ",i+1);
      else gsVerboseMsg(".");
    }
  }
  gsVerboseMsg("\n");
}



int main(int argc,char** argv) 
{
  fprintf(stderr,"warning: this tool is deprecated; use ltscp instead\n");

  ATerm bottom;
  ATinit(argc, argv, &bottom);

  #define sopts "hqvl:"
  struct option lopts[] = {
    { "help",        no_argument,         NULL, 'h' },
    { "version",     no_argument,         NULL, 0   },
    { "quiet",       no_argument,         NULL, 'q' },
    { "verbose",     no_argument,         NULL, 'v' },
    { "lpe",         required_argument,   NULL, 'l' },
    { 0, 0, 0, 0 }
  };

  bool quiet = false;
  bool verbose = false;
  char *lpefile = NULL;
  int opt;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 0:
        print_version(stderr);
        return 0;
      case 'q':
        quiet = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'l':
        lpefile = strdup(optarg);
        break;
      default:
	break;
    }
  }
  
  if ( (argc-optind < 1) || (argc-optind > 2) )
  {
    fprintf(stderr,"Try '%s --help' for more information.\n",argv[0]);
    return 1;
  }

  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return 1;
  }
  if ( quiet )
  {
    gsSetDebugMsg();
  }
  if ( verbose )
  {
    gsSetVerboseMsg();
  }

  if ( (argc-optind == 2) && !strcmp(argv[optind],argv[optind+1]) )
  {
    gsErrorMsg("input file and output file are not allowed to be the same\n");
    return 1;
  }

  SVCbool tmpBool=SVCfalse;
  SVCfile file;
  // 1st argument is svc file
  // Open the file and start reading all objects.
  if( SVCopen(&file, argv[optind], SVCread, &tmpBool) != 0 )
  {
    gsErrorMsg("unable to open SVC file '%s' for reading.\n",argv[optind]);
    return 1;
  }
  FILE *outf = stdout;
  if ( (argc-optind == 2) && ((outf = fopen(argv[optind+1],"w")) == NULL) )
  {
    gsErrorMsg("unable to open FSM file '%s' for writing.\n",argv[optind+1]);
    return 1;
  }

  int* in;  // array of ingoing edges
  int* out; // array of outgoing edges
  // do something with file
  // ...
  gsVerboseMsg("number of parameters : %d\n", SVCnumParameters(&file));
  gsVerboseMsg("number of states     : %d\n", SVCnumStates(&file));
  gsVerboseMsg("number of action     : %d\n", SVCnumLabels(&file));
  gsVerboseMsg("number of transitions: %d\n", SVCnumTransitions(&file));

  is_mcrl2 = !strcmp("mCRL2+info",SVCgetType(&file));
  if ( !strcmp("mCRL2",SVCgetType(&file)) )
  {
     gsWarningMsg("state space does not contain state information\n");
     is_mcrl2 = true;
  }
  if ( is_mcrl2 )
  {
     gsEnableConstructorFunctions();
  }

  if ( lpefile != NULL )
  {
     if ( is_mcrl2 )
        readParameterNames2(lpefile,&names,&types);
     else 
        readParameterNames(lpefile,&names,&types);
     free(lpefile);
  } else { 
     gsWarningMsg("parameter names are unknown (use -l/--lpe option)\n");
     names=types=NULL;
     num_names = 0;
  }

  //
  // compute number of incoming and outgoing edges
  //
  
  compute_in_out(file,&in,&out);

  //
  // write states
  //
  print_states(outf,file,in,out);

  SVCclose(&file);    // close

  //
  // write edges
  //

  SVCopen(&file, argv[optind], SVCread, &tmpBool);  // reopen

  print_edges(outf,file);

  SVCclose(&file);                             // close

  if ( outf != stdout )
  {
    fclose(outf);
  }

  return 0;
}                                                                             //
