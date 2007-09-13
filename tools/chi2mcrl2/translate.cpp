#include <iostream>
#include <cstdio>
#include <string.h>
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "translate.h"
#include <vector> 
#include <sstream>
#include <math.h> 

using namespace ::mcrl2::utilities;
using namespace std;

bool CAsttransform::translator(ATermAppl ast)
{
  if( StrcmpIsFun( "ProcDef", ast ) )
  {
    mcrl2_result = manipulateProcess( ast );
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
  vector<RAT>::iterator itRAT;
  map<std::string, RPV>::iterator itMap;
  string result;

  int max_state = 0;
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
  result = "act Terminator; \n \n proc \n   ";
  result.append(processName);
  result.append("(");
  for( itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
    {
      if (itMap != ProcessVariableMap.begin())
        {
          result.append(", ");
        }
      result.append(itMap->first);
      result.append(": ");
      result.append((itMap->second).Type);
    }
  result.append(", state: Nat");
  result.append(")= \n");
    
  for( itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++ ) 
    { 
      if (itRAT != transitionSystem.begin()){
         result.append("\t+ ");
      } else {
         result.append("\t  ");
      };
      
      //append Guard;
      result.append("( ");
      if (!itRAT->guard.empty())
      {
        result.append(itRAT->guard);
        result.append(" && ");
      }

      //append current state
      result.append("state == ");
      result.append(to_string(itRAT->state));
      result.append(") -> ");
      result.append(itRAT->action);

      max_state = max(max_state, itRAT->nextstate);

      if(itRAT->nextstate != -1)
        {
          result.append(".");
          result.append(processName);
          result.append("(");
		  for( itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
			{
			  if( itRAT->vectorUpdate.find(itMap->first) != itRAT->vectorUpdate.end() ) 
              {
                result.append(itRAT->vectorUpdate[itMap->first]);
              } else {
                result.append(itMap->first);
              } 
			  result.append(", ");
			}
          //append vectorupdate
          // TODO: with assignments
          //append next state
          result.append(to_string(itRAT->nextstate));
          result.append(")\n");
        } else {
          result.append("\n");
        }
    }
          result.append("\t+ ( state == ");
          result.append( to_string(max_state) );
          result.append(") -> Terminator\n");

  result.append("\t;\n");


  //write initialisation
  result.append("\ninit ");
  //Should be provided by MODEL()=|[ ]|
  result.append(processName);
  result.append("(");
  for( itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
    { 
      if ( !(itMap->second).InitValue.empty() ) 
      { 
        result.append((itMap->second).InitValue);
      } else {
	 //TODO: remove if model is supported and replace code by assertion
	    if ((itMap->second).Type == "Bool" )
		  {
		    result.append("false"); 
		  };
	    if ((itMap->second).Type == "Nat" )
	  	  {
		    result.append("0"); 
		  };
      //end TODO
	  }
    result.append(", ");
    }
  result.append("0");
  result.append(");\n");

  return result; 
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

std::vector<std::string> CAsttransform::getExpressionsFromList(ATermList input) 
{
  gsDebugMsg("input of getVariablesNamesFromList: %T\n", input);
  // INPUT: [Expression(...), Expression(...)]
 
  std::vector<std::string> result; 
  ATermList to_process = input; 
  while (!ATisEmpty(to_process)){
	//result.push_back( ATgetName( ATgetAFun( ATgetArgument( ATgetFirst( to_process), 0 ) ) ) );
	result.push_back( manipulateExpression((ATermAppl) ATgetFirst( to_process ) ) );
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
    int lastelement = manipulateStatements((ATermAppl) ATgetArgument(input, 1),0, 1);
    //Process bypasses
    std::vector<int>::iterator bypassIt;
    for( bypassIt = bypass.begin(); bypassIt != bypass.end(); bypassIt++ ) 
    {
       transitionSystem.at(*bypassIt).nextstate = lastelement;
    }
    //Last element is the succesfull termination therefore all nextsate's == lastelement to -1
/*    std::vector<RAT>::iterator tsIt;

    for( tsIt = transitionSystem.begin(); tsIt != transitionSystem.end(); tsIt++ ) 
    {
       if ((*tsIt).nextstate == lastelement)
         {
           (*tsIt).nextstate = -1;
         }
    }
*/
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
			  if (tmpRPV.Type == "Bool" )
			    {
			  	  tmpRPV.InitValue = "false"; 
				};
			  if (tmpRPV.Type == "Nat" )
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
  std::string result;
  gsDebugMsg("input of manipulateExpression: %T\n", input);
  // INPUT: Expression(...)
  if ( StrcmpIsFun( "Expression", input ) ) 
  {
    return ATgetName(ATgetAFun( ATgetArgument(input,0) ) );
  }
  if ( StrcmpIsFun( "BinaryExpression", input ) ) 
  {
     result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
     result.append( ATgetName ( ATgetAFun( ATgetArgument(input , 0) ) ) );
     result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
     return result;
  }
  if ( StrcmpIsFun( "UnaryExpression", input ) ) 
  {
     //Special unary operators
     if(StrcmpIsFun( "()", (ATermAppl) ATgetArgument(input , 0) )  )
     {
       result.append("(");
     } else { 
       result.append( ATgetName ( ATgetAFun( ATgetArgument(input , 0) ) ) );
     }
      
     result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
     
     if(StrcmpIsFun( "()",(ATermAppl) ATgetArgument(input , 0) )  )
     {
       result.append(")");
     }  
     return result;
  }
  gsErrorMsg("Encounterd unknown expressions %T", input);
  exit(1);
  return "";
} 

int CAsttransform::manipulateStatements(ATermAppl input, int current, int next)
{
  gsDebugMsg("input of manipulateStatements: %T\n", input);
  RAT transition;
      

  if ( StrcmpIsFun( "DeltaStat", input ) )
    {
      if(!StrcmpIsFun( "Nil", (ATermAppl) ATgetArgument(input,0) ))
      {
        transition.guard =  manipulateExpression( (ATermAppl) ATgetArgument(input,0));
      }
      
      transition.state = current;
      transition.nextstate = next;
      transition.action = "delta";
      transitionSystem.push_back(transition);
      return next;
     }
  if ( StrcmpIsFun( "SkipStat", input ) )
    {
      if(!StrcmpIsFun( "Nil", (ATermAppl) ATgetArgument(input,0) ))
      {
        transition.guard =  manipulateExpression( (ATermAppl) ATgetArgument(input,0));
      }
      
      transition.state = current;
      transition.nextstate = next;
      transition.action = "tau";
      transitionSystem.push_back(transition);
      return next;
    }
  if ( StrcmpIsFun( "SepStat", input ) )
    {
      next = manipulateStatements( (ATermAppl) ATgetArgument(input,0), current, current + 1 );
      next = manipulateStatements( (ATermAppl) ATgetArgument(input,1), next, next+1  );
      return next;
    }
  if ( StrcmpIsFun( "AltStat", input ) )
    {
      next = manipulateStatements( (ATermAppl) ATgetArgument(input,0), current, next );
      bypass.push_back(transitionSystem.size()-1);

      next = manipulateStatements( (ATermAppl) ATgetArgument(input,1), current, next );
      return next;
    }

  if ( StrcmpIsFun( "ParenthesisedStat", input ) )
    {
      next = manipulateStatements( (ATermAppl) ATgetArgument(input,0), current, current + 1 );
      //Process bypasses
      std::vector<int>::iterator bypassIt;
      for( bypassIt = bypass.begin(); bypassIt != bypass.end(); bypassIt++ ) 
      {
         transitionSystem.at(*bypassIt).nextstate = next;
      }
      //empty bypass-vector;
      bypass.clear();
      return next;
    }
  if ( StrcmpIsFun( "AssignmentStat", input ) )
    {
      transition.state = current;
      transition.nextstate = next;
      transition.action = "tau";
      transition.vectorUpdate = manipulateAssignmentStat((ATermList) ATgetArgument(input, 2), (ATermList) ATgetArgument(input, 3) );
      transitionSystem.push_back(transition);
      return next;
    }
  gsErrorMsg("%T operator is not supported yet",  input ) ;
  exit(1);
  return current;
}

std::map<std::string, std::string> CAsttransform::manipulateAssignmentStat(ATermList input_id, ATermList input_exp) 
{
  std::map<std::string, std::string> result;
  vector<std::string> identifiers;
  vector<std::string> expressions;
  vector<std::string>::iterator it;
  vector<std::string>::iterator itExp;
  
  gsDebugMsg("input of manipulateAssignmentStat: %T \n \t %T\n", input_id, input_exp);
  if(ATgetLength( input_id ) != ATgetLength( input_exp ))
  {
    gsErrorMsg("Assignment to %T contains a number of assignments not equal to the number of variables\n", input_id);
    exit(1);
  }

  identifiers = getVariablesNamesFromList( input_id);  
  expressions = getExpressionsFromList( input_exp); 
  
  itExp = expressions.begin(); 
  for(it = identifiers.begin(); it != identifiers.end(); ++it)
  {
    result[*it] = *itExp; 
    ++itExp;
  }
 
  return result;
} 

std::string CAsttransform::getResult() 
{
  return mcrl2_result;
}

