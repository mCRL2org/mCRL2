#define NAME "mcrl2i"

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include <assert.h>

#include "libstruct.h"
#include "librewrite.h"
#include "libenum.h"
#include "libprint_c.h"
#include "mcrl2/lps/specification.h"
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "libparse.h"
#include "typecheck.h"
#include "dataimpl.h"

using namespace std;
using namespace lps;
using namespace mcrl2::utilities;

char help_gsMessage[] = "At the prompt any mCRL2 data expression can be given. This term will be\n"
                      "rewritten to normal form and printed. Also, one can assign values to declared\n"
                      "variables by writing x := v, for variable x and value v. These variables can\n"
                      "then be used in expressions. Besides this, the prompt accepts the following\n"
                      "commands:\n"
                      "  :h[elp]                         print this help gsMessage\n"
                      "  :q[uit]                         quit\n"
                      "  :t[ype] EXPRESSION              print type of EXPRESSION\n"
                      "  :v[ar] VARLIST                  declare variables in VARLIST\n"
                      "  :r[ewriter] STRATEGY            use STRATEGY for rewriting\n"
                      "  :s[solve] <VARLIST> EXPRESSION  give all valuations of the variables in\n"
                      "                                  VARLIST that satisfy EXPRESSION\n"
                      "  where VARLIST is of the form x,y,...: S; v,w,...: T\n";

static Rewriter *rewr;
static Enumerator *e;
static ATermTable variables;
static ATermTable assignments;
            
void clear_rewr_substs(ATermList vars)
{
  if ( vars == NULL )
  {
    vars = ATtableKeys(assignments);
  }
  for (; !ATisEmpty(vars); vars=ATgetNext(vars))
  {
    rewr->clearSubstitution(ATAgetFirst(vars));
  }
}

void reset_rewr_substs()
{
  ATermList vars = ATtableKeys(assignments);
  for (; !ATisEmpty(vars); vars=ATgetNext(vars))
  {
    ATermAppl var = ATAgetFirst(vars);
    rewr->setSubstitution(var,ATtableGet(assignments,(ATerm) var));
  }
}

string trim_spaces(const string &str)
{
  // function from http://www.codeproject.com/vcpp/stl/stdstringtrim.asp
  string s(str);
  string::size_type pos = s.find_last_not_of(' ');
  if(pos != string::npos)
  {
    s.erase(pos + 1);
    pos = s.find_first_not_of(' ');
    if (pos != string::npos)
      s.erase(0, pos);
  } else
    s.erase(s.begin(), s.end());

  return s;
}

bool parse_var_decl(string decl, ATermList *varlist, specification &spec)
{
    string::size_type semi_pos = decl.find(':');
    if ( semi_pos == string::npos )
    {
      gsErrorMsg("invalid type declaration '%s'\n",decl.c_str());
      return false;
    }

    stringstream ss(decl.substr(semi_pos+1));
    
    ATermAppl sort = parse_sort_expr(ss);
    if ( sort == NULL )
      return false;

    gsDebugMsg("parsed: %T\n",sort);

    sort = type_check_sort_expr(sort,spec);
    if ( sort == NULL )
      return false;

    gsDebugMsg("type checked: %T\n",sort);

    sort = implement_data_sort_expr(sort,spec);
    if ( sort == NULL )
      return false;

    gsDebugMsg("data implemented: %T\n",sort);

    string names = decl.substr(0,semi_pos) + ',';
    string::size_type i;
    while ( ( i = names.find(',') ) != string::npos )
    {
      *varlist = ATinsert(*varlist,(ATerm) gsMakeDataVarId(gsString2ATermAppl(trim_spaces(names.substr(0,i)).c_str()),sort));
      names = names.substr(i+1);
    }

    return true;
}

bool parse_var_decl_list(string decl_list, ATermList *varlist, specification &spec)
{
  decl_list += ';';
  string::size_type pos;
  while ( (pos = decl_list.find(';')) != string::npos )
  {
    string decl(decl_list.substr(0,pos));
    decl_list = decl_list.substr(pos+1);

    if ( !parse_var_decl(decl,varlist,spec) )
      return false;
  }

  return true;
}

ATermList parse_varlist_from_string(string &s, specification &spec)
{
  string::size_type start = s.find('<');
  if ( start == string::npos )
  {
    gsErrorMsg("opening bracket for variable list not found\n");
    return NULL;
  }
  string::size_type end = s.find('>',start);
  if ( end == string::npos )
  {
    gsErrorMsg("closing bracket for variable list not found\n");
    return NULL;
  }
  string varlist = s.substr(start+1,end-start-1);
  s = s.substr(end+1);

  ATermList result = ATmakeList0();
  if ( !parse_var_decl_list(varlist,&result,spec) )
    return NULL;
    
  gsDebugMsg("variable list: %T\n",result);

  return result;
}

ATermAppl parse_term(string &term_string, specification &spec, ATermList vars = NULL)
{
  stringstream ss(term_string);

  ATermAppl term = parse_data_expr(ss);
  if ( term == NULL )
    return NULL;

  gsDebugMsg("parsed: %T\n",term);

  ATermList save_assignments = ATmakeList0();
  if ( vars != NULL )
  {
    for (ATermList l=vars; !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermAppl var = ATAgetFirst(l);
      if ( ATtableGet(variables,ATgetArgument(var,0)) != NULL )
      {
        ATerm old_val = rewr->getSubstitution(var);
        rewr->clearSubstitution(var);
        save_assignments = ATinsert(save_assignments,(ATerm) gsMakeSubst((ATerm) var,old_val));
      }
      ATtablePut(variables,ATgetArgument(var,0),ATgetArgument(var,1));
    }
  }
  term = type_check_data_expr(term,NULL,spec,false,variables);
  if ( vars != NULL )
  {
    for (ATermList l=vars; !ATisEmpty(l); l=ATgetNext(l))
    {
      ATtableRemove(variables,ATgetArgument(ATAgetFirst(l),0));
    }
    for (; !ATisEmpty(save_assignments); save_assignments=ATgetNext(save_assignments))
    {
      ATermAppl subst = ATAgetFirst(save_assignments);
      ATermAppl var = ATAgetArgument(subst,0);
      rewr->setSubstitution(var,ATgetArgument(subst,1));
      ATtablePut(variables,ATgetArgument(var,0),ATgetArgument(var,1));
    }
  }
  if ( term == NULL )
    return NULL;

  gsDebugMsg("type checked: %T\n",term);

  term = implement_data_data_expr(term,spec);
  if ( term == NULL )
    return NULL;

  gsDebugMsg("data implemented: %T\n",term);

  return term;
}

void declare_variables(string &vars, specification &spec)
{
  ATermList varlist = ATmakeList0();
  if ( ! parse_var_decl_list(vars,&varlist,spec) )
    return;

  for (; !ATisEmpty(varlist); varlist=ATgetNext(varlist))
  {
    ATermAppl var = ATAgetFirst(varlist);
    ATtablePut(variables,ATgetArgument(var,0),ATgetArgument(var,1));
  }
}

void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... INFILE\n"
    "Evaluate expressions using the data specification of the LPS in INFILE via a\n"
    "text-based interface.\n"
    "\n"
    "%s"
    "\n"
    "The following command line options are available.\n"
    "  -h, --help               display this help gsMessage\n"
    "  -q, --quiet              do not display any unrequested information\n"
    "  -v, --verbose            display consise intermediate gsMessages\n"
    "  -d, --debug              display detailed intermediate gsMessages\n"
    "  -y, --dummy              replace free variables in the LPS with dummy values\n"
    "  -RNAME, --rewriter=NAME  use rewriter NAME (default 'inner')\n",
    Name,
    help_gsMessage
  );
}

int main(int argc, char **argv)
{
  FILE *SpecStream;
  ATerm stackbot;
  ATermAppl Spec;
  #define sopts "hqvdyR:"
  struct option lopts[] = {
    { "help",  no_argument,  NULL,  'h' },
    { "quiet",  no_argument,  NULL,  'q' },
    { "verbose",    no_argument,  NULL,  'v' },
    { "debug",  no_argument,  NULL,  'd' },
    { "dummy",  no_argument,  NULL,  'y' },
    { "rewriter",  no_argument,  NULL,  'R' },
    { 0, 0, 0, 0 }
  };

  ATinit(argc,argv,&stackbot);

  bool quiet = false;
  bool verbose = false;
  bool debug = false;
  bool usedummy = false;
  RewriteStrategy strat = GS_REWR_INNER;
  int opt;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 'q':
        quiet = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'd':
        debug = true;
        break;
      case 'y':
        usedummy = true;
        break;
      case 'R':
        strat = RewriteStrategyFromString(optarg);
        if ( strat == GS_REWR_INVALID )
        {
          gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
          return 1;
        }
        break;
      default:
        break;
    }
  }

  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
    return false;
  }
  if ( quiet && debug )
  {
    gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
    return false;
  }
  if ( quiet )
  {
    gsSetQuietMsg();
  }
  if ( verbose )
  {
    gsSetVerboseMsg();
  }
  if ( debug )
  {
    gsSetDebugMsg();
  }

  if ( argc-optind < 1 )
  {
    print_help(stderr, argv[0]);
    return 1;
  }

  char *SpecFileName = argv[optind];
  if ( (SpecStream = fopen(SpecFileName, "rb")) == NULL )
  {
                gsErrorMsg("could not open input file '%s' for reading: ",
                  argv[optind]);
    perror(NULL);
    return 1;
  }
  Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if ( Spec == NULL )
  {
                gsErrorMsg("could not read LPS from '%s'\n", SpecFileName);
                fclose(SpecStream);
    return 1;
  }
  assert(Spec != NULL);
  gsEnableConstructorFunctions();
  if (!gsIsSpecV1(Spec)) {
    gsErrorMsg("'%s' does not contain an LPS\n", SpecFileName);
    fclose(SpecStream);
    return false;
  }
  assert(gsIsSpecV1(Spec));
  specification spec(Spec);

  gsMessage("mCRL2 interpreter (type :h for help)\n");

  rewr = createRewriter(spec.data(),strat);
  e = createEnumerator(spec,rewr);
  variables = ATtableCreate(50,50);
  assignments = ATtableCreate(50,50);

  bool notdone = true;
  while ( notdone )
  {
    while ( true )
    {
      string s;

      (cout << "? ").flush();
      getline(cin, s);
      
      if ( cin.eof() )
      {
        cout << endl;
        notdone = false;
        break;
      }

      if ( s.substr(0,1) == ":" )
      {
        s = s.substr(1);
        if ( (s == "q") || (s == "quit") )
        {
          if ( cin.eof() )
            cout << endl;  
          notdone = false;
          break;
        } else if ( (s == "h") || (s == "help") )
        {
          cout << help_gsMessage;
        } else if ( (s.substr(0,2) == "r ") || (s.substr(0,9) == "rewriter ") ) 
        {
          if ( s.substr(0,2) == "r " )
            s = s.substr(2);
          else
            s = s.substr(9);
          strat = RewriteStrategyFromString(s.c_str());
          if ( strat == GS_REWR_INVALID )
          {
            gsErrorMsg("invalid rewrite strategy '%s'\n",s.c_str());
            return 1;
          } else {
            delete e;
            delete rewr;
            rewr = createRewriter(spec.data(),strat);
            e = createEnumerator(spec,rewr);
          }
        } else if ( (s.substr(0,2) == "t ") || (s.substr(0,5) == "type ") )
        {
          if ( s.substr(0,2) == "t " )
            s = s.substr(2);
          else
            s = s.substr(5);
          ATermAppl term = parse_term(s,spec);
          if ( term != NULL )
          {
            gsprintf("%P\n",gsGetSort(term));
          }
        } else if ( (s.substr(0,2) == "v ") || (s.substr(0,4) == "var ") )
        {
          if ( s.substr(0,2) == "v " )
            s = s.substr(2);
          else
            s = s.substr(4);
          declare_variables(s,spec);
        } else if ( (s.substr(0,2) == "s ") || (s.substr(0,6) == "solve ") )
        {
          if ( s.substr(0,2) == "s " )
            s = s.substr(2);
          else
            s = s.substr(6);
          ATermList vars = parse_varlist_from_string(s,spec);
          if ( vars == NULL )
            break;
          ATermAppl term = parse_term(s,spec,vars);
          if ( term != NULL )
          {
            if ( gsGetSort(term) != gsMakeSortExprBool() )
            {
              gsErrorMsg("expression is not of sort Bool\n");
              break;
            }
            static EnumeratorSolutions *sols = NULL;
            clear_rewr_substs(vars);
            sols = e->findSolutions(vars,rewr->toRewriteFormat(term),false,sols);
            ATermList sol;
            while ( sols->next(&sol) )
            {
              gsprintf("[");
              for (; !ATisEmpty(sol); sol=ATgetNext(sol))
              {
                ATermAppl subst = ATAgetFirst(sol);
                rewr->setSubstitution(ATAgetArgument(subst,0),ATgetArgument(subst,1));
                gsprintf("%P := %P",ATAgetArgument(subst,0),rewr->fromRewriteFormat(ATgetArgument(subst,1)));
                if ( !ATisEmpty(ATgetNext(sol)) )
                {
                  gsprintf(", ");
                }
              }
              gsprintf("] gives %P\n",rewr->rewrite(term));
            }
            for (; !ATisEmpty(vars); vars=ATgetNext(vars))
            {
              rewr->clearSubstitution(ATAgetFirst(vars));
            }
            reset_rewr_substs();
          }
        } else {
          cout << "unknown command (try ':h' for help)" << endl;
        }
      } else {
        string::size_type assign_pos = s.find(":=");
        ATermAppl var = NULL;
        if ( assign_pos != string::npos )
        {
          ATermAppl var_name = gsString2ATermAppl(trim_spaces(s.substr(0,assign_pos)).c_str());
          ATermAppl var_sort;
          if ( (var_sort = (ATermAppl) ATtableGet(variables,(ATerm) var_name)) == NULL )
          {
            gsErrorMsg("variable '%T' is not declared\n",var_name);
            break;
          }
          var = gsMakeDataVarId(var_name,var_sort);
          s = s.substr(assign_pos+2);
        }
        ATermAppl term = parse_term(s,spec);
        if ( term != NULL )
        {
          term = rewr->rewrite(term);
          if ( var == NULL )
          {
            gsprintf("%P\n",term);
          } else {
            rewr->setSubstitution(var,rewr->toRewriteFormat(term));
            ATtablePut(assignments,(ATerm) var,rewr->toRewriteFormat(term));
          }
        }
      }
    }
  }

  delete rewr;
}
