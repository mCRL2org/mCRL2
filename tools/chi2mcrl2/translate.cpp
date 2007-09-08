#include <iostream>
#include <cstdio>
#include <string.h>
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "translate.h"
#include <vector> 

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
  gsDebugMsg("input of manipulateProcess: %T\n", input);
  // INPUT: ProcDef( "ID", ProcDecl( ... ), ProcSpec( ... ))
  // First element process name
  // Used to set the variable prefix and to increase scope level
  // TODO: The process name is taken as prefix. This may causes problems when 
  // having duplicate instantiations.
  vector<RVT> DeclaredProcessDefinition;
  vector<RPV> ProcessSpecification;
  RPV tmpRPV;
  vector<RVT>::iterator itRVT;
  vector<RPV>::iterator itRPV;
  map<std::string, RPV>::iterator itMap;
  string result;

  variable_prefix = ATgetName(ATgetAFun(ATgetArgument(input , 0)));
  std::string processName =  ATgetName(ATgetAFun(ATgetArgument(input , 0)));
  scope_level++;
 
  // 
  // WARNING: if ExplicedTemplates are added shift  ATgetArgument(input, +1)
  // manipulateExplicitTemplates(ATgetArgument(input, 1))

  //Manipulate the process variables declared in the definition
  DeclaredProcessDefinition = manipulateDeclaredProcessDefinition((ATermAppl) ATgetArgument(input, 1 )); 
    for( itRVT = DeclaredProcessDefinition.begin(); itRVT != DeclaredProcessDefinition.end(); itRVT++ ) 
      { tmpRPV.Name = itRVT->Name; 
        tmpRPV.Type = itRVT->Type;
        //passthrough function needed
        tmpRPV.InitValue = "";
        ProcessVariableMap[tmpRPV.Name] = tmpRPV;
      }
  //Manipulate the procees variables declared and the statements in the specification
  ProcessSpecification = manipulateProcessSpecification((ATermAppl) ATgetArgument(input, 2 )); 
    for( itRPV = ProcessSpecification.begin(); itRPV != ProcessSpecification.end(); itRPV++ ) 
      { 
        ProcessVariableMap[itRPV->Name] = *itRPV;
      }

  //Write the output for a process 
  result = processName;
  result.append("(");
  for( itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
    {
      /*if (itMap != ProcessVariableMap.begin())
        {*/
          result.append(", ");
      /*  }*/
      result.append(itMap->first);
      result.append(": ");
      result.append((itMap->second).Type);
    }
  result.append("_state_: nat");
  result.append(")=");
  cout << result << endl;
  return ""; 
}

std::vector<RVT> CAsttransform::manipulateDeclaredProcessDefinition(ATermAppl input)
{
  vector<RVT> result; 
  gsDebugMsg("input of manipulateDeclaredProcessDefinition: %T\n", input);
  // INPUT: ProcDecl( ... )
  // Arity is 1 because VarDecl the argument is of the form list*
	
  if ATisEmpty(input)
  {
	gsDebugMsg("No variables/channels are declare in the process definition");
	return result;
  } else {
    //Get first argument
    ATermList to_process = (ATermList) ATgetArgument(input, 0);
    while (!ATisEmpty(to_process)){
	  if ( StrcmpIsFun( "VarDecl", (ATermAppl) ATgetFirst(to_process) ) )
	  {
	   result =	manipulateDeclaredProcessVariables((ATermList) ATgetFirst(to_process));
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
  return result; 
}

std::vector<RVT> CAsttransform::manipulateDeclaredProcessVariables(ATermList input)
{
  std::vector<RVT>::iterator it;
  std::vector<RVT> result;
  std::vector<RVT> tmpRVT;
  gsDebugMsg("input of manipulateDeclaredProcessVariables: %T\n", input);
  // INPUT: VarDecl( ... ),VarDecl( ... ),... 
 
  ATermList to_process = (ATermList) ATgetFirst(input);
  //input cannot be empty
  assert(!ATisEmpty(to_process)); 
  while (!ATisEmpty(to_process)){
    if ( StrcmpIsFun( "DataVarID", (ATermAppl) ATgetFirst(to_process) ) )
	{
      //Merge vector with previous vector
      tmpRVT = manipulateDeclaredProcessVariable((ATermAppl) ATgetFirst(to_process));
	  for( it = tmpRVT.begin(); it != tmpRVT.end(); it++ ) 
	    { result.push_back(*it); 
		}
    };
	to_process = ATgetNext(to_process);
  }
  return result;
}

std::vector<RVT> CAsttransform::manipulateDeclaredProcessVariable(ATermAppl input)
{
  std::vector<RVT> Result;
  std::string Type;
  vector<std::string> ProcessVariableVectorNames;
  RVT tmpRVT;
  vector<std::string>::iterator it;

  gsDebugMsg("input of manipulateDeclaredProcessVariable: %T\n", input);
  // INPUT:  [a,b,.... ], Type( ... ) )

  //get variable name 
  ProcessVariableVectorNames = getVariablesNamesFromList( (ATermList) ATgetArgument(input, 0 ) ) ;

  //Get Types
  if ( StrcmpIsFun( "Type", (ATermAppl) ATgetArgument(input, 1 ) ) ){
    Type = (ATgetName( ATgetAFun( ATgetArgument( ATgetArgument(input, 1 ) , 0 ) ) ) );
	for( it = ProcessVariableVectorNames.begin(); 
		 it != ProcessVariableVectorNames.end(); it++ ) 
		 { 
			tmpRVT.Name = *it;
            tmpRVT.Type = Type;
			/* TODO: construct passthrough */
			Result.push_back(tmpRVT);
		 }  
  	} else {
 	gsErrorMsg("Incorrect AST-format: Type Expected");
    exit(1);
  }
  return Result;
}

std::vector<std::string> CAsttransform::getVariablesNamesFromList(ATermList input) 
{
  gsDebugMsg("input of getVariablesNamesFromList: %T\n", input);
  // INPUT: [a,b,....]
 
  std::vector<std::string> result; 
  ATermList to_process = input; 
  while (!ATisEmpty(to_process)){
	result.push_back( ATgetName( ATgetAFun( ATgetFirst( to_process) ) ) );
	to_process = ATgetNext(to_process);
  }
  return result;
} 

std::vector<RPV> CAsttransform::manipulateProcessSpecification(ATermAppl input)  
{
  gsDebugMsg("input of manipulateProcessSpecification: %T\n", input);
  //INPUT: ProcSpec( [...] , SepStat ( [...] ))
  std::vector<RPV> result;
  std::vector<RPV> tmpRPV;
  std::vector<RPV>::iterator it;

	
  if ATisEmpty(input)
  {
	gsDebugMsg("No variables/channels are declare in the process definition");
	return result;
  } else {
    //Get first argument
    ATermList to_process = (ATermList) ATgetArgument(input, 0);
    while (!ATisEmpty(to_process)){
	  if ( StrcmpIsFun( "VarSpec", (ATermAppl) ATgetFirst(to_process) ) )
	  {
      //Merge vector with previous vector
        tmpRPV = manipulateProcessVariableDeclarations((ATermList) ATgetFirst(to_process)); 
	  for( it = tmpRPV.begin(); it != tmpRPV.end(); it++ ) 
	    { result.push_back(*it); 
		}

		//result.append(manipulateDeclaredProcessSpecification((ATermList) ATgetFirst(to_process)));
      };
	/* TODO: Channel declaration not yet implemented
	  if ( StrcmpisFun( "ChanDecl", ATgetFirst(to_process) ) )
	  {
        manipulateDeclaredProcessChannels(ATgetFirst(to_process))
      }
	*/
	  to_process = ATgetNext(to_process);
	}
    
    //Process Statements
    manipulateStatements((ATermAppl) ATgetArgument(input, 1));
  }
  return result; 
}

std::vector<RPV> CAsttransform::manipulateProcessVariableDeclarations(ATermList input)
{
  vector<RVT>::iterator it;
  RPV tmpRPV;
  vector<RVT> tmpRVT;
  vector<RPV> result;
  string InitValue;
  gsDebugMsg("input of manipulateProcessVariableDeclarations: %T\n", input);
  // INPUT: ProcDecl( ... )
  // Arity is 1 because VarDecl the argument is of the form list*
	
  if ATisEmpty(input)
  {
	gsDebugMsg("No variables/channels are declare in the process definition");
	return result;
  } else {
    //Get first argument
    ATermList to_process = (ATermList) ATgetArgument(input, 0);
    while (!ATisEmpty(to_process)){
      if ( StrcmpIsFun( "DataVarID", (ATermAppl) ATgetFirst(to_process) ) )
	  {
        tmpRVT = manipulateDeclaredProcessVariable((ATermAppl) ATgetFirst(to_process));
		for( it =  tmpRVT.begin(); 
		     it != tmpRVT.end(); it++ ) 
		 	{ 
			  tmpRPV.Name = it->Name;
              tmpRPV.Type = it->Type;
			  /* Set Initial Value -- See Future NOTE */
			  if (tmpRPV.Type == "bool" )
			    {
			  	  tmpRPV.InitValue = "false"; 
				};
			  if (tmpRPV.Type == "nat" )
			    {
			      tmpRPV.InitValue = "0"; 
				};
			   /*
			    *  Future NOTE: Use mapping when using self defined data types
			    */ 
			 result.push_back(tmpRPV);
		 	}	  
      };
      if ( StrcmpIsFun( "DataVarExprID", (ATermAppl) ATgetFirst(to_process) ) )
	  {
		//First Argument of DataVarExprID is a DataVarID 
        tmpRVT = manipulateDeclaredProcessVariable( (ATermAppl) ATgetArgument(ATgetFirst(to_process), 0 ));
		//Second Argument of DataVarExprID is an expression
        InitValue = manipulateExpression( (ATermAppl) ATgetArgument(ATgetFirst(to_process), 1 ));
		for( it =  tmpRVT.begin(); 
		     it != tmpRVT.end(); it++ ) 
		 	{ 
			  tmpRPV.Name = it->Name;
              tmpRPV.Type = it->Type;
			  tmpRPV.InitValue = InitValue;
			  result.push_back(tmpRPV);
		 	}	  
	  };
	  to_process = ATgetNext(to_process);
	}
  }
  return result; 
  
} 

std::string CAsttransform::manipulateExpression(ATermAppl input)
{
  gsDebugMsg("input of manipulateExpression: %T\n", input);
  // INPUT: Expression(...)
  if (ATgetArity(ATgetAFun(input)) == 2 )
  {
    return ATgetName(ATgetAFun( ATgetArgument(input,0) ) );
  }
  gsErrorMsg("Long expressions are not supported");
  exit(1);
  return "";
} 

void CAsttransform::manipulateStatements(ATermAppl input)
{
  gsDebugMsg("input of manipulateStatements: %T\n", input);

  int statementLevel = 1;
  int numberOfStatements = ATgetArity( ATgetAFun( (ATerm) input ) );
 
  cout << numberOfStatements << endl; 
  exit(1);
  return;
}
