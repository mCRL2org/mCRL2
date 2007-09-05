#include <iostream>
#include <cstdio>
#include <string.h>
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "translate.h"

using namespace ::mcrl2::utilities;
using namespace std;

bool CAsttransform::translator(ATermAppl ast)
{
  if( StrcmpIsFun( "ProcDef", ast ) )
  {
    manipulateProcess( ast );
  }
  return true;
}

bool CAsttransform::StrcmpIsFun( const char* str, ATermAppl aterm) 
{
  return ATgetAFun( aterm ) == 
	ATmakeAFun( str, ATgetArity( ATgetAFun( aterm ) ), ATisQuoted( ATgetAFun( aterm ) ) ); 
}

std::string CAsttransform::manipulateProcess(ATermAppl input)
{

  // First element process name
  // Used to set the variable prefix and to increase scope level
  // TODO: The process name is taken as prefix. This may causes problems when 
  // having duplicate instantiations.
  variable_prefix = ATgetName(ATgetAFun(ATgetArgument(input , 0)));
  scope_level++;
 
  // 
  // WARNING: if ExplicedTemplates are added shift  ATgetArgument(input, +1)
  // manipulateExplicitTemplates(ATgetArgument(input, 1))

  manipulateDeclaredProcessDefinition((ATermAppl) ATgetArgument(input, 1 )); 
 
  return ""; 
}

std::string CAsttransform::manipulateDeclaredProcessDefinition(ATermAppl input)
{
  gsDebugMsg("input of manipulateDeclaredProcessDefinition: %T\n", input);
  // INPUT: ProcDecl( ... )
  // Arity is 1 because VarDecl the argument is of the form list*
  std::string result = "";
	
  if ATisEmpty(input)
  {
	gsDebugMsg("No variables/channels are declare in the process definition");
	return "";
  } else {
    //Get first argument
    ATermList to_process = (ATermList) ATgetArgument(input, 0);
    while (!ATisEmpty(to_process)){
	  if ( StrcmpIsFun( "VarDecl", (ATermAppl) ATgetFirst(to_process) ) )
	  {
        result = manipulateDeclaredProcessVariables((ATermList) ATgetFirst(to_process));
      };
	/* TODO: Channel declaration not yet implemented
	  if ( StrcmpisFun( "ChanDecl", ATgetFirst(to_process) ) )
	  {
        manipulateDeclaredProcessChannels(ATgetFirst(to_process))
      }
	*/
	  to_process = ATgetNext(to_process);
	}
  }
  return ""; 
}

std::string CAsttransform::manipulateDeclaredProcessVariables(ATermList input)
{
  gsDebugMsg("input of manipulateDeclaredProcessVariables: %T\n", input);
  // INPUT: VarDecl( ... ),VarDecl( ... ),... 
 
  std::string result = "";
  ATermList to_process = (ATermList) ATgetFirst(input);
  //input cannot be empty
  assert(!ATisEmpty(to_process)); 
  while (!ATisEmpty(to_process)){
    if ( StrcmpIsFun( "DataVarID", (ATermAppl) ATgetFirst(to_process) ) )
	{
        result.append(manipulateDeclaredProcessVariable((ATermAppl) ATgetFirst(to_process)));
    };
	to_process = ATgetNext(to_process);
  }
  return result;
}

std::string CAsttransform::manipulateDeclaredProcessVariable(ATermAppl input)
{
  gsDebugMsg("input of manipulateDeclaredProcessVariable: %T\n", input);
  // INPUT: VarDecl( ... ),VarDecl( ... ),...
  getVariablesNamesFromList( (ATermList) ATgetArgument(input, 0 ) );
  return "";
}

std::string CAsttransform::getVariablesNamesFromList(ATermList input) 
{
  gsDebugMsg("input of getVariablesNamesFromList: %T\n", input);
  // INPUT: [a,b,....]
 
  string result = ""; 
  ATermList to_process = input; 
  while (!ATisEmpty(to_process)){
	result.append( ATgetName( ATgetAFun( ATgetFirst( to_process) ) ) );
	to_process = ATgetNext(to_process);
    if (!ATisEmpty(to_process)) 
	{
	  result.append( ", " );
	} 
  }
  cout << result << endl;
  return result;
} 
