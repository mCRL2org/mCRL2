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
#include <list>
#include "CArray.h"
#include <stack>
#include <iterator>

using namespace ::mcrl2::utilities;
using namespace std;

bool CAsttransform::StrcmpIsFun( const char* str, ATermAppl aterm) 
{
  return ATgetAFun( aterm ) == 
	ATmakeAFun( str, ATgetArity( ATgetAFun( aterm ) ), ATisQuoted( ATgetAFun( aterm ) ) );
}

bool CAsttransform::translator(ATermAppl ast)
{
  gsDebugMsg("input of translator: %T\n", ast);
  std::string result;

  /**
    * Write the special Terminator actions for the mcrl2 specification
    *
    **/
  result = "act Terminator; \n ";

  if( StrcmpIsFun( "ChiSpec", ast ) )
  {
 
    ATermList to_process = (ATermList) ATgetArgument(ast, 1);
    while ( ATgetLength(to_process) > 0)
    {
      result.append(manipulateProcess((ATermAppl) ATgetFirst(to_process)));
      to_process = ATgetNext(to_process);
    }
 
  } else {
    gsErrorMsg("No valid AST input\n");
    exit(1);
  }

  /**
    * write initialisation
    *
    **/
  manipulateModel((ATermAppl) ATgetArgument(ast, 0 ));

  result.append("\ninit ");
  result.append(initialisation+";\n");

  mcrl2_result = result;
  return true;

}

void CAsttransform::manipulateModel(ATermAppl input)
{
  gsDebugMsg("input of manipulateModel: %T\n", input);
  vector<RPV> ModelSpecification;
  vector<RPV>::iterator itRPV;
  //Manipulate the procees variables declared and the statements in the model
  ModelSpecification = manipulateModelSpecification((ATermAppl) ATgetArgument(input, 1 )); 
  return;
}

std::vector<RPV> CAsttransform::manipulateModelSpecification(ATermAppl input)  
{
  gsDebugMsg("input of manipulateModelSpecification: %T\n", input);
  //INPUT: ProcSpec( [...] , SepStat ( [...] ))
  std::vector<RPV> result;
  std::vector<RPV> tmpRPV;
  std::vector<RPV>::iterator it;
	
  //Process Statements
  manipulateModelStatements((ATermAppl) ATgetArgument(input, 1));

  return result; 
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
  vector<int>::iterator itint;
  string result;

  //Clear global variables that are exclusivly thoughout this process
  begin_state.clear(); //first:  parenthesis level
  end_state.clear();   //first:  parenthesis level
  endstates_per_parenthesis_level.clear();
  info_per_parenthesis_level_per_parenthesis.clear();
  streams_per_parenthesis_level.clear();
  all_streams.clear();
  ProcessVariableMap.clear();
  transitionSystem.clear();

  //Set the initial variables for this process
  parallel = false;
  alternative = false;
  terminate = true;
  loop = false;
  guardedloop=false;
  guardedStarBeginState = 0;
  stream_number = 0;
  originates_from_stream = 0;
  parenthesis_level = 0;
  all_streams.insert(stream_number);
  streams_per_parenthesis_level.insert(stream_number);

  /**
    * Start processing the process
    *
    **/
 
  variable_prefix = ATgetName(ATgetAFun(ATgetArgument(input , 0)));
  std::string processName =  ATgetName(ATgetAFun(ATgetArgument(input , 0)));
  // 
  // WARNING: if ExplicedTemplates are added shift  ATgetArgument(input, +1)
  // manipulateExplicitTemplates(ATgetArgument(input, 1))

  //Manipulate the process variables declared in the definition
  vector<RPV> DeclaredProcessDefinitionRPV;
  DeclaredProcessDefinition = manipulateDeclaredProcessDefinition((ATermAppl) ATgetArgument(input, 1 )); 
    for( itRVT = DeclaredProcessDefinition.begin(); itRVT != DeclaredProcessDefinition.end(); itRVT++ ) 
      { tmpRPV.Name = itRVT->Name; 
        tmpRPV.Type = itRVT->Type;
        tmpRPV.InitValue = "";
        ProcessVariableMap.push_back(tmpRPV);
        DeclaredProcessDefinitionRPV.push_back(tmpRPV);
      }
  //Manipulate the procees variables declared and the statements in the specification
  ProcessSpecification = manipulateProcessSpecification((ATermAppl) ATgetArgument(input, 2 )); 
    for( itRPV = ProcessSpecification.begin(); itRPV != ProcessSpecification.end(); itRPV++ ) 
      { 
        ProcessVariableMap.push_back(*itRPV);
      }

  /**
    * Create Processes for Instantiation: these are used to match the instantation given in the model
    *
    **/
  
  ProcessForInstantation[processName].DeclarationVariables =DeclaredProcessDefinitionRPV;
  ProcessForInstantation[processName].SpecificationVariables =ProcessSpecification;
  ProcessForInstantation[processName].NumberOfStreams = all_streams.size();
 
  /**
    * + endstates_per_parenthesis_level_per_parenthesis stores per parenthesis level the terminating statements
    * + endstates_per_parenthesis_level stores the terminating statemens until they are added to 
    *   endstates_per_parenthesis_level_per_parenthesis
    **/
 
  RPI info;
  endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size()); 
  info.endstates = endstates_per_parenthesis_level[parenthesis_level];
  info.looped = false;
  info.guardedloop = false;
  info.begin_state = 0;
  info.end_state = transitionSystem.size();
  info.streams = streams_per_parenthesis_level;
  info.parallel = parallel;
  info.alternative = alternative;
  streams_per_parenthesis_level.clear();

  if (info.parallel && info.alternative)
  {
    gsErrorMsg("It is not allowed to use the parallel and alternative operator on the same parenthesis level.");
    exit(1);
  }

  info_per_parenthesis_level_per_parenthesis[parenthesis_level].push_back(info);
  endstates_per_parenthesis_level[parenthesis_level].clear();

  /**
    * Print the states and if they terminate the parenthesis level of termination 
    *
    **/
  for( itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++ )
  {
    gsDebugMsg("state:%d\t terminate:%d\t plvl:%d\t looped:%d guardedloop:%d\n", itRAT->state, itRAT->terminate, itRAT->parenthesis_level, itRAT->looped_state, itRAT->guardedloop);
  }


  /**
    * Add a special terminating state for the transitionSystem
    * This node not added to the transitionSystem
    *
    **/
  RAT terminate_state;
  terminate_state.state = transitionSystem.size(); 
  end_state[parenthesis_level] = terminate_state.state;


  /**
    * Propegate repetition over parenthesis if different parenthesis_levels have the same begin and end state 
    * 
    **/
  bool again = true;
  while(again)
  {
    again = false;
    for(std::map<int, std::vector<RPI > >::iterator itMapRPI = info_per_parenthesis_level_per_parenthesis.begin();
      itMapRPI != info_per_parenthesis_level_per_parenthesis.end();
      ++itMapRPI)
      {
        for(std::vector<RPI>::iterator itRPI = itMapRPI->second.begin();
            itRPI != itMapRPI->second.end();
            ++itRPI)
        {
          gsDebugMsg("begin_state:%d\t end_state:%d\t  looped parenthesis:%d\n", itRPI->begin_state,itRPI->end_state, itRPI->looped);
          for(std::map<int, std::vector<RPI > >::iterator itMapRPI2 = info_per_parenthesis_level_per_parenthesis.begin();
              itMapRPI2 != info_per_parenthesis_level_per_parenthesis.end();
              ++itMapRPI2)
             {
               for(std::vector<RPI>::iterator itRPI2 = itMapRPI2->second.begin();
                     itRPI2 != itMapRPI2->second.end();
                     ++itRPI2)
                 {
                   gsDebugMsg("+----begin_state:%d\t end_state:%d\t  looped parenthesis:%d\n", itRPI2->begin_state,itRPI2->end_state, itRPI2->looped);
                   if(itRPI2->begin_state == itRPI->begin_state && itRPI2->end_state == itRPI->end_state && itRPI !=itRPI2)
                   {
                     gsDebugMsg("Found Match\n");
                     if(itRPI2->looped != itRPI->looped)
                     {
                       itRPI->looped=true;
                       itRPI->looped=true;
                       again = true;
                     }
                   }
                }
             }
          }
      } 
  }

  /**
    * Determine the end states for branches of the graph that have local terminating inside a parenthesis
    *
    **/
  gsDebugMsg("Creating edges for terminating branches inside a parenthesis\n");
  for(std::map<int, std::vector<RPI > >::iterator itIntVecSet = info_per_parenthesis_level_per_parenthesis.begin();
      itIntVecSet != info_per_parenthesis_level_per_parenthesis.end();
      ++itIntVecSet){
        gsDebugMsg("parenthesis_lvl: %d\n", itIntVecSet->first);
        int i = 0;
        for(std::vector<RPI>::iterator itVecSet = itIntVecSet->second.begin();
            itVecSet != itIntVecSet->second.end();
            ++itVecSet)
        {
             gsDebugMsg("\t#%d\t:: begin_state:%d\t end_state:%d\t  looped parenthesis:%d \n", ++i, itVecSet->begin_state,itVecSet->end_state, itVecSet->looped);
             gsDebugMsg("\t\t   guarded loop:%d\t number_of_streams:%d:\n",  itVecSet->guardedloop, itVecSet->streams.size());
             for(std::set<int>::iterator i = itVecSet->streams.begin(); 
                                         i != itVecSet->streams.end();
                                         ++i )
             {
               gsDebugMsg("\t\t\tstream:%d\n",*i);
             }
             if (itVecSet->looped)
             {
               //The parenthesis is looped: Point the end of the branch back to the beginning of the parenthesis
               for(std::set<int>::iterator itSet= (*itVecSet).endstates.begin(); itSet != (*itVecSet).endstates.end(); ++itSet )
               {
                 if(itVecSet->guardedloop)
                 {
                   transitionSystem.at(*itSet-1).nextstate = itVecSet->begin_state-1; 
                 } else { 
                   transitionSystem.at(*itSet-1).nextstate = itVecSet->begin_state; 
                 }
               }
             } else
             { 
               //The parenthesis is not looped: Point the end of the branch to the end of the parenthesis
               int last_state = determineEndState((*itVecSet).endstates, itIntVecSet->first);
               for(std::set<int>::iterator itSet= (*itVecSet).endstates.begin(); itSet != (*itVecSet).endstates.end(); ++itSet )
               {
                 transitionSystem.at(*itSet-1).nextstate = last_state; 
               }
             }
        } 
  }

  /** 
    * Create extra Summands for terminating repetitions 
    *
    **/
  //Create transitions for states
  vector<RAT> XtraTransitionSystem;
  for(vector<RAT>::iterator itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++ )
  {
    if(itRAT->guardedloop)
    {
      RAT transition;
      vector<RAT>::iterator dubItRAT = itRAT;
      dubItRAT--;
      transition = *dubItRAT;
      if (itRAT->terminate)
      {
        for(std::vector<RPI>::iterator itRPI = info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].begin();
                                   itRPI != info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].end();
                                   ++itRPI)
        {
          if (itRPI->endstates.find(itRAT->state) !=  itRPI->endstates.end())
           {transition.nextstate= determineEndState(itRPI->endstates, itRAT->parenthesis_level);
           }
        }
             
      } else {
        transition.nextstate= transition.nextstate+1;
      }
      transition.guard= "!("+transition.guard+") ";
 
      XtraTransitionSystem.push_back(transition); 

    }
  }
  gsDebugMsg("Processing parenthesis levels for *>\n");
  //Create transistions for parenthesis levels
  for(std::map<int, std::vector<RPI > >::iterator itIntVecSet = info_per_parenthesis_level_per_parenthesis.begin();
      itIntVecSet != info_per_parenthesis_level_per_parenthesis.end();
      ++itIntVecSet){
        gsDebugMsg("parenthesis_lvl: %d\n", itIntVecSet->first);
        int i = 0;
        for(std::vector<RPI>::iterator itVecSet = itIntVecSet->second.begin();
            itVecSet != itIntVecSet->second.end();
            ++itVecSet)
        {
             gsDebugMsg("\t#%d\t:: begin_state:%d\t end_state:%d\t  looped parenthesis:%d guarded loop:%d\n",++i, itVecSet->begin_state,itVecSet->end_state, itVecSet->looped, itVecSet->guardedloop);
             if(itVecSet->guardedloop)
             {
               RAT transition;
               transition = transitionSystem.at(itVecSet->begin_state-1);
               transition.nextstate = itVecSet->end_state;
               transition.terminate = transitionSystem.at(itVecSet->end_state-1).terminate;
               transition.guard= "!("+transition.guard+")";
               if (itVecSet->endstates.find(transition.nextstate) !=  itVecSet->endstates.end()) 
               { 
                 transition.nextstate= determineEndState(itVecSet->endstates, itIntVecSet->first);
                 {
                   XtraTransitionSystem.push_back(transition);
                 } 
               }   
             }

        } 
  }
  transitionSystem.insert(transitionSystem.end(),XtraTransitionSystem.begin(),XtraTransitionSystem.end());

  /**
    * Write the mcrl2 process specification
    *
    **/
  result.append("\n proc \n  "+processName+"(");
  /**
    * Declare the used variables
    *
    **/

  for(std::vector<RPV>::iterator itRVT = ProcessVariableMap.begin(); itRVT != ProcessVariableMap.end(); itRVT++)
    {
      if (itRVT != ProcessVariableMap.begin())
        {
          result.append(", ");
        }
      result.append(itRVT->Name);
      result.append(": ");
      result.append(itRVT->Type);
    }
  /**
    * Declare the number of used streams
    *
    **/
  for(set<int>::iterator i = all_streams.begin(); 
    i != all_streams.end(); 
    ++i )
  {
    if (!(ProcessVariableMap.empty() && i == all_streams.begin()) )
    {
      result.append(", ");
    }
    result.append("state_");
    result.append(to_string(*i));
    result.append(": Nat");
  }
  result.append(")= \n");
  

  /**
    * Write the transitionSystem into an LPS with summands
    *
    **/  
  int index = 0;
  for( itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++ ) 
    { 
      if (itRAT != transitionSystem.begin()){
         result.append("\t+ ");
      } else {
         result.append("\t  ");
      };
      
      /**
        * Write for each summand the guards followed by the valuation for the state   
        *
        **/
      //Write guard of the summand;
      result.append("( ");
      if (!itRAT->guard.empty())
      {
        result.append(itRAT->guard+" && ");
      }
      
      /**
        * Collect ending streams
        *
        **/
      std::set<int> collect_streams;
      for(std::map<int, std::vector<RPI> >::iterator itMapRPI = info_per_parenthesis_level_per_parenthesis.begin();
        itMapRPI != info_per_parenthesis_level_per_parenthesis.end();
        ++itMapRPI)
      {
        for(std::vector<RPI>::iterator itRPI = itMapRPI->second.begin();
            itRPI != itMapRPI->second.end();
            ++itRPI)
        {
          if (itRPI->end_state == itRAT->state)
          { 
            collect_streams.insert(itRPI->streams.begin(), itRPI->streams.end()); 
          }
        }
      }
      for(std::set<int>::iterator itSetInt = collect_streams.begin();
        itSetInt != collect_streams.end();
        ++itSetInt)
      {
        if(itRAT->stream != *itSetInt)
        {
          result.append("state_");
          result.append(to_string(*itSetInt));
          result.append(" >= ");
          result.append(to_string(itRAT->state));
          result.append(" && ");
        }
      }      
      collect_streams.clear();

      result.append("state_");
      result.append(to_string(itRAT->stream));
      result.append(" == ");
      result.append(to_string(itRAT->state));
      result.append(") -> ");
      
      //Write the valuation of the state for a summand
      result.append(itRAT->action);

      result.append(".");
      //Write the process name for a summand
      result.append(processName);
      //Write the process vector
      result.append("(");

      //write the variable valuation
	  for(std::vector<RPV>::iterator itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
		{
		  if(itMap != ProcessVariableMap.begin())
          {
            result.append(", ");
          }
          if( itRAT->vectorUpdate.find(itMap->Name) != itRAT->vectorUpdate.end() ) 
          {
            result.append(itRAT->vectorUpdate[itMap->Name]);
          } else {
            result.append(itMap->Name);
          } 
		}
      
      collect_streams.clear();
      for(std::vector<RPI>::iterator itRPI = info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].begin()
           ; itRPI !=  info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].end()
           ; ++itRPI)
      {
         // {all_streams}\{streams} for the states at index of transistion system need to proceed
         if ( itRPI->begin_state <= index  && index <= itRPI->end_state)
         {
           collect_streams.insert(itRPI->streams.begin(), itRPI->streams.end());
         }  
      }

      std::set<int> ResultDif;
      std::insert_iterator<std::set<int> > DifIter( ResultDif, ResultDif.begin() ); 
      set_difference(all_streams.begin(), all_streams.end(), 
                       collect_streams.begin(), collect_streams.end(),
                       DifIter
                      );
      
       //write the state valuation  
       for(set<int>::iterator i = all_streams.begin(); 
         i != all_streams.end(); 
         ++i )
         {
	     if (!(ProcessVariableMap.empty() && *i == 0) )
         {
           result.append(", ");
         }
         if (itRAT->stream == *i || ResultDif.find(*i) != ResultDif.end() )
         {
           //Determine if the state is looped
           if(itRAT->looped_state)
           {
             if(itRAT->guardedloop)
             {
               result.append(to_string(itRAT->state-1));
             } else {
               result.append(to_string(itRAT->state));
             }
           } else {
             result.append(to_string(itRAT->nextstate));
           }
         } else {
           result.append("state_");
           result.append(to_string(*i));
         }
       }  
       result.append(")\n");
       ++index;
    }

     /**
        * Collect ending streams
        *
        **/
      std::set<int> collect_streams;

      for(vector<RAT>::iterator itRAT = transitionSystem.begin();
                                itRAT != transitionSystem.end();
                                ++itRAT)
      { if (itRAT->nextstate == terminate_state.state)
        {
          collect_streams.insert(itRAT->stream);
        }
      } 
      if (!collect_streams.empty())
      {
        result.append("\t+ ( ");
        for(std::set<int>::iterator itSetInt = collect_streams.begin();
          itSetInt != collect_streams.end();
          ++itSetInt)
        {
          if (itSetInt != collect_streams.begin())
          {
            result.append(" && ");
          }
          result.append("state_");
          result.append(to_string(*itSetInt));
          result.append(" == ");
          result.append(to_string(terminate_state.state));
        }
      result.append(") -> Terminator\n");
      }      


  result.append("\t;\n");


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
    transitionSystem.clear();
    state = transitionSystem.size();
    parenthesis_level = 0;
    begin_state[parenthesis_level] = state;

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

void CAsttransform::manipulateModelStatements(ATermAppl input)
{
  gsDebugMsg("input of manipulateModelStatements: %T\n", input);
  if ( StrcmpIsFun( "ParStat", input ) )
    {
      manipulateModelStatements((ATermAppl) ATgetArgument(input,0));
      initialisation.append(" || ");
      manipulateModelStatements((ATermAppl) ATgetArgument(input,1));
      return ;
    } 
  if ( StrcmpIsFun( "Instantiation", input ) )
    {
      std::string processName;
      processName = ATgetName(ATgetAFun(ATgetArgument(input,0)));
      
      if (ProcessForInstantation.find(processName) == ProcessForInstantation.end())
      {
        gsErrorMsg("Instantation of process %s is not declared\n", processName.c_str());
        exit(1);
      }

      //Determine the number of arguments of the instantation
      ATermList to_process = (ATermList) ATgetArgument(input,1);
       
      if(ATgetLength(to_process) != ProcessForInstantation[processName].DeclarationVariables.size())   
      {
        gsDebugMsg("%T\n",to_process);
        gsDebugMsg("Declared variables from process\n");
        for(vector<RPV>::iterator itRPV =ProcessForInstantation[processName].DeclarationVariables.begin();
             itRPV != ProcessForInstantation[processName].DeclarationVariables.end();
             ++itRPV
           )
        {
          gsDebugMsg("%s\n,", itRPV->Name.c_str());
        }
        gsErrorMsg("Number of arguments does not correspond for %s\n", processName.c_str());
        exit(1);
      }
 
      //Determine if the arguments are of the same type 
      vector<RPV>::iterator itRPV = ProcessForInstantation[processName].DeclarationVariables.begin();
      ATermList tmp_to_process = to_process;
      while (ATgetLength(tmp_to_process)>0)
      {
        if (strcmp(itRPV->Type.c_str(),
            ATgetName(ATgetAFun(ATgetArgument(ATgetArgument( ATgetFirst(tmp_to_process), 1 ), 0))))
            != 0
           )
        {
          gsDebugMsg("%T\n",to_process);
          gsDebugMsg("Declared Types from process\n");
          for(vector<RPV>::iterator itRPV =ProcessForInstantation[processName].DeclarationVariables.begin();
             itRPV != ProcessForInstantation[processName].DeclarationVariables.end();
             ++itRPV
             )
          {
            gsDebugMsg("%s\n,", itRPV->Type.c_str());
          }
          gsErrorMsg("Arguments do not correspond in types for %s\n", processName.c_str());
          exit(1);
        }
        tmp_to_process = ATgetNext(tmp_to_process);
        ++itRPV;
      }
           
 
      //Write state vector
      initialisation.append(processName);
      initialisation.append("(");
      while (ATgetLength(to_process)>0)
      {
        initialisation.append(manipulateExpression( (ATermAppl) ATgetFirst(to_process))+", ");
        to_process = ATgetNext(to_process);
      }
      for(vector<RPV>::iterator itRPV = ProcessForInstantation[processName].SpecificationVariables.begin();
          itRPV != ProcessForInstantation[processName].SpecificationVariables.end();
          ++itRPV)
      {
        initialisation.append(itRPV->InitValue+", ");
      }
      initialisation.append("0)"); 
      return ;
    } 
  gsErrorMsg("%T operator is not supported yet.\n",  input ) ;
  exit(1);
  return;
}

void CAsttransform::manipulateStatements(ATermAppl input)
{
  gsDebugMsg("input of manipulateStatements: %T\n", input);
  RAT transition;

  if ( StrcmpIsFun( "DeltaStat", input ) )
    {
      if(!StrcmpIsFun( "Nil", (ATermAppl) ATgetArgument(input,0) ))
      {
        transition.guard =  manipulateExpression( (ATermAppl) ATgetArgument(input,0));
      }
      
      transition.state = state;
      transition.stream = stream_number;
      transition.originates_from_stream = originates_from_stream;
      transition.nextstate = next_state;
      transition.action = "delta";
      transition.terminate = terminate;
      transition.looped_state = loop;
      transition.guardedloop = guardedloop;
      transition.parenthesis_level = parenthesis_level;
      transitionSystem.push_back(transition);
      if(terminate)
      { 
        endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size()); 
      }
      loop = false;
      guardedloop = false;
      return ;
     }
  if ( StrcmpIsFun( "SkipStat", input ) )
    {
      if(!StrcmpIsFun( "Nil", (ATermAppl) ATgetArgument(input,0) ))
      {
        transition.guard =  manipulateExpression( (ATermAppl) ATgetArgument(input,0));
      }
      transition.stream = stream_number;
      transition.originates_from_stream = originates_from_stream;
      transition.state = state;
      transition.nextstate = next_state;
      transition.action = "tau";
      transition.terminate = terminate;
      transition.parenthesis_level = parenthesis_level;
      transition.looped_state = loop;
      transition.guardedloop = guardedloop;
      transitionSystem.push_back(transition);
      if(terminate)
      { 
        endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size()); 
      }
      loop = false;
      guardedloop = false;
      return;
    }
  if ( StrcmpIsFun( "AssignmentStat", input ) )
    {
      transition.state = state;
      transition.stream = stream_number;
      transition.originates_from_stream = originates_from_stream;
      transition.nextstate = next_state;
      transition.action = "tau";
      transition.terminate = terminate;
      transition.looped_state = loop; 
      transition.guardedloop = guardedloop;
      transition.parenthesis_level = parenthesis_level;
      transition.vectorUpdate = manipulateAssignmentStat((ATermList) ATgetArgument(input, 2), (ATermList) ATgetArgument(input, 3) );
      transitionSystem.push_back(transition);
      if(terminate)
      { 
        endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size()); 
      }
      loop = false;
      guardedloop = false;
      return ;
    }
  
  if ( StrcmpIsFun( "SepStat", input ) )
    {
      terminate = false;

      next_state = transitionSystem.size() + 1;
      manipulateStatements( (ATermAppl) ATgetArgument(input,0) );
      if (StrcmpIsFun("SepStat", (ATermAppl) ATgetArgument(input,1)))
      { 
        terminate = false;
      } else {
        terminate = true;
        next_state = -1 ;
      } 
      state = transitionSystem.size();
      originates_from_stream = stream_number;
      manipulateStatements( (ATermAppl) ATgetArgument(input,1) );
      return ;
    }
  if ( StrcmpIsFun( "AltStat", input ) )
    {
      next_state = -1;
      terminate = true;
      alternative = true;
      originates_from_stream = stream_number;
      manipulateStatements( (ATermAppl) ATgetArgument(input,0) );
      state = begin_state[parenthesis_level];
      originates_from_stream = stream_number;
      manipulateStatements( (ATermAppl) ATgetArgument(input,1) );
      return ;
    }
  if ( StrcmpIsFun( "StarStat", input ) )
    {
      loop = true;
      guardedloop = false;
      originates_from_stream = stream_number;
      manipulateStatements( (ATermAppl) ATgetArgument(input,0) );
      return ;
    }
  if ( StrcmpIsFun( "ParStat", input ) )
    {
      gsErrorMsg("Parallel processes are only supported in at model level\n");
      exit(0);
      int bypass_originates_from_stream = stream_number; 
      terminate = true;
      parallel = true;
      originates_from_stream = stream_number;
      
      manipulateStatements( (ATermAppl) ATgetArgument(input,0) );

      ++stream_number;

      
      all_streams.insert(stream_number);
      
      originates_from_stream = bypass_originates_from_stream;
      streams_per_parenthesis_level.insert(stream_number);
      state = begin_state[parenthesis_level];
      manipulateStatements( (ATermAppl) ATgetArgument(input,1));
      
      return ;
    } 
  if ( StrcmpIsFun( "GuardedStarStat", input ) )
    {
      
      transition.guard =  manipulateExpression( (ATermAppl) ATgetArgument(input,0));
      transition.stream = stream_number;
      transition.state = state;
      transition.guardedloop = false;
      transition.nextstate = transitionSystem.size()+1;
      transition.action = "tau";
      transition.terminate = false;
      transition.parenthesis_level = parenthesis_level;
      transition.looped_state = false;
      transitionSystem.push_back(transition);
      
      guardedStarBeginState= state;
      guardedStarExpression = "!";
      guardedStarExpression.append(transition.guard);        
      loop = true;
      guardedloop = true;
      state = transitionSystem.size();
      manipulateStatements( (ATermAppl) ATgetArgument(input,1) );
      return ;
    }

  if ( StrcmpIsFun( "ParenthesisedStat", input ) )
    {
      //check if ParenthesisedStat is terminating
      bool ParenthesisedStatIsTerminating = terminate;
      bool ParenthesisedLoop = loop;
      loop = false;
      bool PassGuardedLoop = guardedloop;
      guardedloop = false;
      bool PassAlternative = alternative;
      parallel = false;
      bool PassParallel = parallel;  
      alternative = false;    
      int PassStreamNumber = stream_number; 

      ++parenthesis_level;
      begin_state[parenthesis_level]= transitionSystem.size();

      manipulateStatements( (ATermAppl) ATgetArgument(input,0));

      RPI info;
      info.endstates = endstates_per_parenthesis_level[parenthesis_level];
      info.endstates.insert(transitionSystem.size());
      info.looped = ParenthesisedLoop;
      info.begin_state = begin_state[parenthesis_level];
      info.end_state = transitionSystem.size();
      info.guardedloop = PassGuardedLoop; 
      info.parallel = PassAlternative;
      info.alternative = PassAlternative;
      info.streams = streams_per_parenthesis_level;
      
      info_per_parenthesis_level_per_parenthesis[parenthesis_level].push_back(info);
      endstates_per_parenthesis_level[parenthesis_level].clear();
      streams_per_parenthesis_level.clear();

      parallel = PassParallel;
      alternative = PassAlternative ;  
      stream_number = PassStreamNumber ; 
      streams_per_parenthesis_level.insert(stream_number);
      --parenthesis_level;
      if(ParenthesisedStatIsTerminating)
      {
        endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size()); 
      }
      return;
    }

  gsErrorMsg("%T operator is not supported yet.\n",  input ) ;
  exit(1);
  return;
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

  identifiers = getExpressionsFromList( input_id);  
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

/**
  * determineEndState determinse the end state for a given set of terminating processes
  * for a given parenthesis level 
  *
  **/
int CAsttransform::determineEndState(std::set<int> org_set, int lvl)
{
  std::set<int> ResultSet;
  std::insert_iterator<std::set<int> > InsertIter( ResultSet, ResultSet.begin() ); 
  int last_state = 0;

  if(lvl > 0){
    --lvl;
    for(std::vector<RPI>::iterator itVecSet = info_per_parenthesis_level_per_parenthesis[lvl].begin()
         ; itVecSet !=  info_per_parenthesis_level_per_parenthesis[lvl].end()
         ; ++itVecSet)
    {
      set_intersection(org_set.begin(), org_set.end(), 
                       (*itVecSet).endstates.begin(), (*itVecSet).endstates.end(),
                       InsertIter
                      );
      if(!ResultSet.empty())
      {
        ResultSet.clear();
        set_union(org_set.begin(), org_set.end(),
                  (*itVecSet).endstates.begin(), (*itVecSet).endstates.end(),
                  InsertIter
                 );
        return determineEndState(ResultSet, lvl);  
      }
    }
  }
  for(std::set<int>::iterator itSet= org_set.begin(); itSet != org_set.end(); ++itSet )
  {
      last_state = max(last_state, *itSet);
  }
  return last_state;      
}
     
bool CAsttransform::transitionexists(RAT transition, std::vector<RAT> transitionvector)
{
   bool result = false;
   for(std::vector<RAT>::iterator itRAT = transitionvector.begin();
       itRAT != transitionvector.end();
       ++itRAT 
      )
   {
     result = result || (itRAT->state == transition.state);
     result = result || (itRAT->nextstate == transition.nextstate);
     result = result || (itRAT->guard == transition.guard);
     result = result || (itRAT->stream == transition.stream);
   }
   return result;
} 
