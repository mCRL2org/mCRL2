// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file translate.cpp

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <string.h>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/utilities/logger.h"
#include "translate.h"
#include <vector>
#include <sstream>
#include <math.h>
#include <list>
#include <stack>
#include <iterator>
#include <map>
#include "libstruct_core.h"
#include <limits.h>

#include "mcrl2/aterm/aterm_ext.h"

using namespace mcrl2::log;
using namespace mcrl2::utilities;
using namespace std;

bool CAsttransform::StrcmpIsFun(const char* str, ATermAppl aterm)
{
  return ATgetAFun(aterm) ==
         ATmakeAFun(str, ATgetArity(ATgetAFun(aterm)), ATisQuoted(ATgetAFun(aterm)));
}

bool CAsttransform::set_options(t_options options)
{
  no_statepar = options.no_statepar;
  return true;
}

bool CAsttransform::translator(ATermAppl ast)
{
  mCRL2log(debug) << "input of translator: " << atermpp::aterm( ast) << std::endl;
  std::string result;

  /**
    *
    *
    **/
  mCRL2log(verbose) << "Options: State parameter elimination: " <<  (no_statepar?"yes":"no") << std::endl;

  /**
    * Write the special Terminate actions for the mcrl2 specification
    *
    **/
  result = "\nact Terminate, skip; \n ";

  if (StrcmpIsFun("ChiSpec", ast))
  {

    ATermList to_process = (ATermList) ATgetArgument(ast, 1);
    while (ATgetLength(to_process) > 0)
    {
      result.append(manipulateProcess((ATermAppl) ATgetFirst(to_process)));
      to_process = ATgetNext(to_process);
    }

  }
  else
  {
    mCRL2log(error) << "No valid AST input" << std::endl;
    exit(1);
  }

  result.insert(0, prefixmCRL2spec);

  /**
    * write initialisation
    *
    **/
  manipulateModel((ATermAppl) ATgetArgument(ast, 0));

  /**
    * write structs
    *
    **/
  string prefix;
  for (std::map<ATermAppl, std::string>::iterator itSet = structset.begin();
       itSet != structset.end();
       ++itSet
      )
  {
    prefix.append("sort "+ itSet->second+ " " + printStructset(itSet->first)+";\n");
  }

  /**
    * Write chi channel ends as actions
    *
    **/

  /**
    * Reduce Hashed Channels
    *
    **/
  map< string,RC > ReduceChannels;
  for (std::map< pair< std::string, int >, RC>::iterator itMap = Channels.begin();
       itMap != Channels.end();
       ++itMap
      )
  {
    ReduceChannels[(itMap->first).first] = itMap->second;
  }

  if (!ReduceChannels.empty())
  {
    prefix.append("\nmap ");
    for (std::map< std::string, RC>::iterator itMap = ReduceChannels.begin();
         itMap != ReduceChannels.end();
         ++itMap
        )
    {
      if (itMap != ReduceChannels.begin())
      {
        prefix.append(", ");
      }
      prefix.append(itMap->first);
    }
    prefix.append(": Nat;\n");
  }

  int i = 0;
  for (std::map<std::string, RC>::iterator itMap = ReduceChannels.begin();
       itMap != ReduceChannels.end();
       ++itMap
      )
  {
    prefix.append("eqn "+itMap->first+"="+to_string(i)+";\n");
    ++i;
  }


  std::set<std::string> new_channels = DeclaredTypesForChannels;
  result.append("\n");
  for (std::map<std::string, RC>::iterator itMap = ReduceChannels.begin();
       itMap != ReduceChannels.end();
       ++itMap
      )
  {
    new_channels.insert(itMap->second.Type);
  }

  for (std::set<std::string>::iterator itSet = new_channels.begin();
       itSet != new_channels.end();
       ++itSet
      )
  {
    if (itSet->compare("Void") == 0)
    {
      prefix.append("act Send_"+*itSet+", Recv_"+*itSet+", Comm_"+*itSet+": Nat#Nat;\n");
    }
    else
    {
      prefix.append("act Send_"+*itSet+", Recv_"+*itSet+", Comm_"+*itSet+": Nat#Nat#"+*itSet+";\n");
    }
  }

  result.insert(0, prefix);

  result.append("init ");
  /*  result.append("\n block({");
    for( std::set<std::string>::iterator itSet = new_channels.begin();
          itSet != new_channels.end();
          ++itSet
    )
    {
      if(itSet != new_channels.begin())
      {
        result.append(",");
      }
      result.append("Send_"+*itSet+", Recv_"+*itSet);
    }
    result.append("},");
  */
  result.append("\n  hide({skip},");
  result.append("\n   allow({ Terminate, skip");
  for (std::set<std::string>::iterator itSet = new_channels.begin();
       itSet != new_channels.end();
       ++itSet
      )
  {
    result.append(", Comm_"+*itSet);
  }
  result.append("},");

  result.append("\n    comm({");
  for (std::set<std::string>::iterator itSet = new_channels.begin();
       itSet != new_channels.end();
       ++itSet
      )
  {
    if (itSet != new_channels.begin())
    {
      result.append(",");
    }
    result.append("Send_"+*itSet+"| Recv_"+*itSet+"->Comm_"+*itSet);
  }
  result.append("},\n     ");

  result.append(initialisation+"\n    )\n   )\n );\n");

  mcrl2_result = result;
  return true;

}

void CAsttransform::manipulateModel(ATermAppl input)
{
  mCRL2log(debug) << "input of manipulateModel: " << atermpp::aterm( input) << std::endl;
  vector<RPV> ModelSpecification;
  vector<RPV>::iterator itRPV;
  ChiDeclParameters.clear();

  //Manipulate the procees variables declared and the statements in the model
  ModelSpecification = manipulateModelSpecification((ATermAppl) ATgetArgument(input, 1));
  return;
}

std::vector<RPV> CAsttransform::manipulateModelSpecification(ATermAppl input)
{
  mCRL2log(debug) << "input of manipulateModelSpecification: " << atermpp::aterm( input) << std::endl;
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
  mCRL2log(debug) << "input of manipulateProcess: " << atermpp::aterm( input) << std::endl;
  // INPUT: ProcDef( "ID", ProcDecl( ... ), ProcSpec( ... ))
  // First element process name
  // Used to set the variable prefix and to increase scope level
  // TODO: The process name is taken as prefix. This may causes problems when
  // having duplicate instantiations.
  pair< vector<RVT>, vector<RPC> > DeclaredProcessDefinition;
  pair< vector<RPV>, vector<RPC> > ProcessSpecification;
  vector<RVT>::iterator itRVT;
  vector<RPV>::iterator itRPV;
  vector<RAT>::iterator itRAT;
  vector<int>::iterator itint;
  string result;
  set<size_t> collection_of_used_counters;

  //Clear global variables that are exclusivly thoughout this process
  begin_state.clear(); //first:  parenthesis level
  end_state.clear();   //first:  parenthesis level
  endstates_per_parenthesis_level.clear();
  info_per_parenthesis_level_per_parenthesis.clear();
  streams_per_parenthesis_level.clear();
  all_streams.clear();
  ProcessVariableMap.clear();
  transitionSystem.clear();
  InstantiatedHashedChannels.clear();

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
  ChiDeclParameters.clear();

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
  DeclaredProcessDefinition = manipulateDeclaredProcessDefinition(ATAgetArgument(input, 1));
  //Empty Initial value is added:: RVT --> RPV
  for (itRVT = DeclaredProcessDefinition.first.begin(); itRVT != DeclaredProcessDefinition.first.end(); itRVT++)
  {
    RPV tmpRPV;
    tmpRPV.Name = itRVT->Name;
    tmpRPV.Type = itRVT->Type;
    //tmpRPV.InitValue = "";
    ProcessVariableMap.push_back(tmpRPV);
    DeclaredProcessDefinitionRPV.push_back(tmpRPV);
  }
  ProcessChannelMap = DeclaredProcessDefinition.second;

  //Manipulate the procees variables declared and the statements in the specification
  ProcessSpecification = manipulateProcessSpecification(ATAgetArgument(input, 2));
  for (itRPV = ProcessSpecification.first.begin(); itRPV != ProcessSpecification.first.end(); itRPV++)
  {
    ProcessVariableMap.push_back(*itRPV);
  }

  /**
    * Create Processes for Instantiation: these are used to match the instantation given in the model
    *
    **/

  Chi_interfaces[processName] = ChiDeclParameters;
  mCRL2log(debug) << "Added " <<  ChiDeclParameters.size() << " ChiDeclParameters for " <<  processName << std::endl;

  ProcessForInstantation[processName].DeclarationVariables = DeclaredProcessDefinitionRPV;
  ProcessForInstantation[processName].DeclarationChannels = DeclaredProcessDefinition.second;
  ProcessForInstantation[processName].SpecificationVariables =ProcessSpecification.first;
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
    mCRL2log(error) << "It is not allowed to use the parallel and alternative operator on the same parenthesis level." << std::endl;
    exit(1);
  }

  info_per_parenthesis_level_per_parenthesis[parenthesis_level].push_back(info);
  endstates_per_parenthesis_level[parenthesis_level].clear();

  /**
    * Print the states and if they terminate the parenthesis level of termination
    *
    **/
  for (itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++)
  {
    mCRL2log(debug) << "state:" <<  itRAT->state << "\t terminate:" <<  itRAT->terminate << "\t plvl:" <<  itRAT->parenthesis_level << "\t looped:" <<  itRAT->looped_state << " guardedloop:" <<  itRAT->guardedloop << "" << std::endl;
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
    * Propagate repetition over parenthesis if different parenthesis_levels have the same begin and end state
    *
    **/
  bool again = true;
  while (again)
  {
    again = false;
    for (std::map<size_t, std::vector<RPI > >::iterator itMapRPI = info_per_parenthesis_level_per_parenthesis.begin();
         itMapRPI != info_per_parenthesis_level_per_parenthesis.end();
         ++itMapRPI)
    {
      for (std::vector<RPI>::iterator itRPI = itMapRPI->second.begin();
           itRPI != itMapRPI->second.end();
           ++itRPI)
      {
        mCRL2log(debug) << "begin_state:" <<  itRPI->begin_state << "\t end_state:" << itRPI->end_state << "\t  looped parenthesis:" <<  itRPI->looped << "" << std::endl;
        for (std::map<size_t, std::vector<RPI > >::iterator itMapRPI2 = info_per_parenthesis_level_per_parenthesis.begin();
             itMapRPI2 != info_per_parenthesis_level_per_parenthesis.end();
             ++itMapRPI2)
        {
          for (std::vector<RPI>::iterator itRPI2 = itMapRPI2->second.begin();
               itRPI2 != itMapRPI2->second.end();
               ++itRPI2)
          {
            mCRL2log(debug) << "+----begin_state:" <<  itRPI2->begin_state << "\t end_state:" << itRPI2->end_state << "\t  looped parenthesis:" <<  itRPI2->looped << "" << std::endl;
            if (itRPI2->begin_state == itRPI->begin_state && itRPI2->end_state == itRPI->end_state /* && (itRPI != itRPI2) */)
            {
              mCRL2log(debug) << "Found Match" << std::endl;
              if (itRPI2->looped != itRPI->looped)
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
  mCRL2log(debug) << "Creating edges for terminating branches inside a parenthesis" << std::endl;
  for (std::map<size_t, std::vector<RPI > >::iterator itIntVecSet = info_per_parenthesis_level_per_parenthesis.begin();
       itIntVecSet != info_per_parenthesis_level_per_parenthesis.end();
       ++itIntVecSet)
  {
    mCRL2log(debug) << "parenthesis_lvl: " <<  itIntVecSet->first << "" << std::endl;
    int i = 0;
    for (std::vector<RPI>::iterator itVecSet = itIntVecSet->second.begin();
         itVecSet != itIntVecSet->second.end();
         ++itVecSet)
    {
      mCRL2log(debug) << "\t#" <<  ++i << "\t:: begin_state:" <<  itVecSet->begin_state << "\t end_state:" << itVecSet->end_state << "\t  looped parenthesis:" <<  itVecSet->looped << std::endl;
      mCRL2log(debug) << "\t\t   guarded loop:" << itVecSet->guardedloop
                      << "\t number_of_streams:" << itVecSet->streams.size() << std::endl;
      for (std::set<int>::iterator i = itVecSet->streams.begin();
           i != itVecSet->streams.end();
           ++i)
      {
        mCRL2log(debug) << "\t\t\tstream:" << *i << "" << std::endl;
      }
      if (itVecSet->looped)
      {
        //The parenthesis is looped: Point the end of the branch back to the beginning of the parenthesis
        for (std::set<size_t>::iterator itSet= (*itVecSet).endstates.begin(); itSet != (*itVecSet).endstates.end(); ++itSet)
        {
          if (itVecSet->guardedloop)
          {
            transitionSystem.at(*itSet-1).nextstate = itVecSet->begin_state-1;
          }
          else
          {
            transitionSystem.at(*itSet-1).nextstate = itVecSet->begin_state;
          }
        }
      }
      else
      {
        //The parenthesis is not looped: Point the end of the branch to the end of the parenthesis
        size_t last_state = determineEndState((*itVecSet).endstates, itIntVecSet->first);
        for (std::set<size_t>::iterator itSet= (*itVecSet).endstates.begin(); itSet != (*itVecSet).endstates.end(); ++itSet)
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
  for (vector<RAT>::iterator itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++)
  {
    if (itRAT->guardedloop)
    {
      RAT transition;
      vector<RAT>::iterator dubItRAT = itRAT;
      dubItRAT--;
      transition = *dubItRAT;
      if (itRAT->terminate)
      {
        for (std::vector<RPI>::iterator itRPI = info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].begin();
             itRPI != info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].end();
             ++itRPI)
        {
          if (itRPI->endstates.find(itRAT->state) !=  itRPI->endstates.end())
          {
            transition.nextstate= determineEndState(itRPI->endstates, itRAT->parenthesis_level);
          }
        }

      }
      else
      {
        transition.nextstate= transition.nextstate+1;
      }
      transition.guard= "!("+transition.guard+") ";

      XtraTransitionSystem.push_back(transition);

    }
  }
  mCRL2log(debug) << "Processing parenthesis levels for *>" << std::endl;
  //Create transistions for parenthesis levels
  for (std::map<size_t, std::vector<RPI > >::iterator itIntVecSet = info_per_parenthesis_level_per_parenthesis.begin();
       itIntVecSet != info_per_parenthesis_level_per_parenthesis.end();
       ++itIntVecSet)
  {
    mCRL2log(debug) << "parenthesis_lvl: " <<  itIntVecSet->first << "" << std::endl;
    size_t i = 0;
    for (std::vector<RPI>::iterator itVecSet = itIntVecSet->second.begin();
         itVecSet != itIntVecSet->second.end();
         ++itVecSet)
    {
      mCRL2log(debug) << "\t#" << ++i << "\t:: begin_state:" <<  itVecSet->begin_state << "\t end_state:" << itVecSet->end_state << "\t  looped parenthesis:" <<  itVecSet->looped << " guarded loop:" <<  itVecSet->guardedloop << "" << std::endl;
      if (itVecSet->guardedloop)
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
  if (!no_statepar)
  {
    result.append("\nproc \n  "+processName+"(");

    /**
      * Declare the used variables
      *
      **/

    for (std::vector<RPV>::iterator itRVT = ProcessVariableMap.begin(); itRVT != ProcessVariableMap.end(); itRVT++)
    {
      if (itRVT != ProcessVariableMap.begin())
      {
        result.append(", ");
      }
      result.append(itRVT->Name);
      result.append(": ");
      result.append(itRVT->Type);
    }

    for (std::vector<RPC>::iterator itRPC = ProcessChannelMap.begin(); itRPC != ProcessChannelMap.end(); itRPC++)
    {
      if (!(itRPC == ProcessChannelMap.begin() && ProcessVariableMap.empty()))
      {
        result.append(", ");
      }
      result.append(itRPC->Name);
      result.append(": Nat");
      /*if ((atoi(itRPC->HashCount.c_str()) > 0) &&
          (InstantiatedHashedChannels.find(itRPC->Name) == InstantiatedHashedChannels.end()))
      { */
      result.append(", ");
      result.append(itRPC->Name);
      result.append("_hash");
      result.append(": Nat");
      // }
    }

    /**
      * Declare the number of used streams
      *
      **/
    for (set<int>::iterator i = all_streams.begin();
         i != all_streams.end();
         ++i)
    {
      if (!(ProcessVariableMap.empty() && i == all_streams.begin() && ProcessChannelMap.empty()))
      {
        result.append(", ");
      }
      result.append("state_");
      result.append(to_string(*i));
      result.append(": Nat");
    }

    result.append(")= \n");
  }



  /**
    * orden transistionSystem
    *
    **/
  multimap<size_t,vector<RAT>::iterator> OrderTransistionSystem;
  for (itRAT = transitionSystem.begin(); itRAT != transitionSystem.end(); itRAT++)
  {
    OrderTransistionSystem.insert(pair<size_t, vector<RAT>::iterator>(itRAT->state, itRAT));
  }

  /**
    * Write the transitionSystem into an LPS with summands
    *
    **/
  size_t index = 0;
  for (multimap<size_t,vector<RAT>::iterator>::iterator itOrdRAT = OrderTransistionSystem.begin();
       itOrdRAT != OrderTransistionSystem.end();
       itOrdRAT++)
  {
    itRAT = itOrdRAT->second;

    if (itRAT != transitionSystem.begin())
    {
      if (!no_statepar)
      {
        result.append("\t+ ");
      };
    }
    else
    {
      result.append("\t  ");
    };

    /**
      * Write for each summand the guards followed by the valuation for the state
      *
      **/
    //Write guard of the summand;
    if (!no_statepar)
    {
      result.append("( ");
      if (!itRAT->guard.empty())
      {
        result.append(itRAT->guard+" && ");
      }
    }


    /**
      * Collect ending streams
      *
      **/
    std::set<int> collect_streams;
    for (std::map<size_t, std::vector<RPI> >::iterator itMapRPI = info_per_parenthesis_level_per_parenthesis.begin();
         itMapRPI != info_per_parenthesis_level_per_parenthesis.end();
         ++itMapRPI)
    {
      for (std::vector<RPI>::iterator itRPI = itMapRPI->second.begin();
           itRPI != itMapRPI->second.end();
           ++itRPI)
      {
        if (itRPI->end_state == itRAT->state)
        {
          collect_streams.insert(itRPI->streams.begin(), itRPI->streams.end());
        }
      }
    }
    for (std::set<int>::iterator itSetInt = collect_streams.begin();
         itSetInt != collect_streams.end();
         ++itSetInt)
    {
      if (itRAT->stream != *itSetInt)
      {
        result.append("state_");
        result.append(to_string(*itSetInt));
        result.append(" >= ");
        result.append(to_string(itRAT->state));
        result.append(" && ");
      }
    }
    collect_streams.clear();

    if (!no_statepar)
    {

      result.append("state_");
      result.append(to_string(itRAT->stream));
      result.append(" == ");
      result.append(to_string(itRAT->state));
      result.append(") -> ");

    }
    else
    {
      if (collection_of_used_counters.find(itRAT->state) == collection_of_used_counters.end())
      {
        result.append("\nproc "+processName+to_string(itRAT->state));
        collection_of_used_counters.insert(itRAT->state);
        /**
          * Declare the used variables
          *
          **/
        if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
        {
          result.append("(");
        }

        for (std::vector<RPV>::iterator itRVT = ProcessVariableMap.begin(); itRVT != ProcessVariableMap.end(); itRVT++)
        {
          if (itRVT != ProcessVariableMap.begin())
          {
            result.append(", ");
          }
          result.append(itRVT->Name);
          result.append(": ");
          result.append(itRVT->Type);
        }


        for (std::vector<RPC>::iterator itRPC = ProcessChannelMap.begin(); itRPC != ProcessChannelMap.end(); itRPC++)
        {
          if (!(ProcessVariableMap.empty() && (ProcessChannelMap.begin() == itRPC)))
          {
            result.append(", ");
          }
          result.append(itRPC->Name);
          result.append(": Nat");
          result.append(", ");
          result.append(itRPC->Name);
          result.append("_hash");
          result.append(": Nat");
        }
        if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
        {
          result.append(") ");
        }

        result.append("= ");
        /* Print Guard */

        if (!itRAT->guard.empty())
        {
          result.append(itRAT->guard+" -> ");
        }
      }
      else
      {
        result.append("\n\t+ ");
      }
    }

    //Write the valuation of the state for a summand
    result.append(itRAT->action);

    result.append(".");
    //Write the process name for a summand
    result.append(processName);

    if (no_statepar)
    {
      result.append(to_string(itRAT->nextstate));
    }

    //Write the process vector
    if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
    {
      result.append("(");
    }

    //write the variable valuation
    for (std::vector<RPV>::iterator itMap = ProcessVariableMap.begin(); itMap != ProcessVariableMap.end(); itMap++)
    {
      if (itMap != ProcessVariableMap.begin())
      {
        result.append(", ");
      }
      if (itRAT->vectorUpdate.find(itMap->Name) != itRAT->vectorUpdate.end())
      {
        result.append(itRAT->vectorUpdate[itMap->Name]);
      }
      else
      {
        result.append(itMap->Name);
      }
    }

    for (vector<RPC>::iterator itRPC = ProcessChannelMap.begin();
         itRPC != ProcessChannelMap.end();
         ++itRPC
        )
    {
      if (!(ProcessVariableMap.empty() &&  itRPC == ProcessChannelMap.begin()))
      {
        result.append(", ");
      }
      result.append(itRPC->Name);
      /*  if ((atoi(itRPC->HashCount.c_str()) > 0) &&
           (InstantiatedHashedChannels.find(itRPC->Name) == InstantiatedHashedChannels.end()))
        { */
      result.append(", ");
      result.append(itRPC->Name);
      result.append("_hash");
      // }
    }

    collect_streams.clear();
    for (std::vector<RPI>::iterator itRPI = info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].begin()
                                            ; itRPI !=  info_per_parenthesis_level_per_parenthesis[itRAT->parenthesis_level].end()
         ; ++itRPI)
    {
      // {all_streams}\{streams} for the states at index of transistion system need to proceed
      if (itRPI->begin_state <= index  && index <= itRPI->end_state)
      {
        collect_streams.insert(itRPI->streams.begin(), itRPI->streams.end());
      }
    }

    std::set<int> ResultDif;
    std::insert_iterator<std::set<int> > DifIter(ResultDif, ResultDif.begin());
    set_difference(all_streams.begin(), all_streams.end(),
                   collect_streams.begin(), collect_streams.end(),
                   DifIter
                  );

    if (!no_statepar)
    {
      //write the state valuation
      for (set<int>::iterator i = all_streams.begin();
           i != all_streams.end();
           ++i)
      {
        if (!(ProcessVariableMap.empty() && ProcessChannelMap.empty() && *i == 0))
        {
          result.append(", ");
        }

        if (itRAT->stream == *i || ResultDif.find(*i) != ResultDif.end())
        {
          //Determine if the state is looped
          if (itRAT->looped_state)
          {
            if (itRAT->guardedloop)
            {
              result.append(to_string(itRAT->state-1));
            }
            else
            {
              result.append(to_string(itRAT->state));
            }
          }
          else
          {
            result.append(to_string(itRAT->nextstate));
          }
        }
        else
        {
          result.append("state_");
          result.append(to_string(*i));
        }
      }
    }
    if (!no_statepar)
    {
      result.append(") <> delta\n");
    }
    else
    {
      if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
      {
        result.append(");");
      }

      /*multimap<size_t,vector<RAT>::iterator>::iterator tmp_mlt = itOrdRAT;
      ++tmp_mlt;
      if(tmp_mlt->first != itOrdRAT->first)
      {
      }*/
    }
    ++index;
  }

  /**
     * Collect ending streamsi and write terminator
     *
     **/
  std::set<int> collect_streams;

  for (vector<RAT>::iterator itRAT = transitionSystem.begin();
       itRAT != transitionSystem.end();
       ++itRAT)
  {
    if (itRAT->nextstate == terminate_state.state)
    {
      int stream = itRAT->stream;
      collect_streams.insert(stream);
    }
  }
  if (!collect_streams.empty())
  {
    if (!no_statepar)
    {
      result.append("\t+ ( ");
      for (std::set<int>::iterator itSetInt = collect_streams.begin();
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
      result.append(") -> Terminate.delta");
    }
    else
    {
      if (collect_streams.size() > 1)
      {
        mCRL2log(error) << "Option -n cannot be used when having parallel processes!" << std::endl;
        exit(0);
      }
      result.append("\nproc "+processName+to_string(terminate_state.state));
      if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
      {
        result.append("(");
      }

      /**
        * Construct process parameters for Terminate action
        *
        **/

      for (std::vector<RPV>::iterator itRVT = ProcessVariableMap.begin(); itRVT != ProcessVariableMap.end(); itRVT++)
      {
        if (itRVT != ProcessVariableMap.begin())
        {
          result.append(", ");
        }
        result.append(itRVT->Name);
        result.append(": ");
        result.append(itRVT->Type);
      }

      for (std::vector<RPC>::iterator itRPC = ProcessChannelMap.begin(); itRPC != ProcessChannelMap.end(); itRPC++)
      {
        if (!(ProcessVariableMap.empty() && (ProcessChannelMap.begin() == itRPC)))
        {
          result.append(", ");
        }
        result.append(itRPC->Name);
        result.append(": Nat");
        result.append(", ");
        result.append(itRPC->Name);
        result.append("_hash");
        result.append(": Nat");
      }

      if (!(ProcessChannelMap.empty() && ProcessVariableMap.empty()))
      {
        result.append(")");
      }
      result.append(" = Terminate.delta");
    }
  }

  result.append(";\n");
  return result;
}

std::string CAsttransform::printStructset(ATermAppl input)
{
  std::string result;
  mCRL2log(debug) << "input of printStructset: " << atermpp::aterm( input) << std::endl;
  if (StrcmpIsFun("Type", (ATermAppl) input))
  {
    mCRL2log(debug) << atermpp::aterm(input) << std::endl;
    return ATgetName(ATgetAFun(ATgetArgument(input,0)));
  }
  if (StrcmpIsFun("TupleType", (ATermAppl) input))
  {
    result.append(" =  struct tuple_"+structset[input]+"(");
    ATermList to_process =(ATermList) ATgetArgument(input,0);
    int i = 0;
    while (!ATisEmpty(to_process))
    {
      result.append("get_"+structset[input]+"_"+to_string(i++)+": ");
      if (structset.find((ATermAppl) ATgetFirst(to_process)) == structset.end())
      {
        result.append(printStructset((ATermAppl) ATgetFirst(to_process)));
      }
      else
      {
        result.append(structset[(ATermAppl) ATgetFirst(to_process)]);
      }
      to_process = ATgetNext(to_process);
      if (!ATisEmpty(to_process))
      {
        result.append(", ");
      }
    }
    result.append(")");
    return result;
  }
  if (StrcmpIsFun("ListType", (ATermAppl) input))
  {
    result.append(" =  List(");
    ATermAppl element = (ATermAppl) ATgetArgument(input, 0);
    if (structset.find(element) == structset.end())
    {
      result.append(printStructset(element));
    }
    else
    {
      result.append(structset[element]);
    }
    result.append(")");
    return result;
  }
  if (StrcmpIsFun("SetType", (ATermAppl) input))
  {
    /* SHOULD BE
    result.append(" =  Set(");
    ATermAppl element = (ATermAppl) ATgetArgument(input, 0);
    if(structset.find(element) == structset.end())
      {
        result.append(printStructset(element));
      } else {
        result.append(structset[element]);
      }
    result.append(")");
    return result;
    */
    result.append(" =  List(");
    ATermAppl element = (ATermAppl) ATgetArgument(input, 0);
    if (structset.find(element) == structset.end())
    {
      result.append(printStructset(element));
    }
    else
    {
      result.append(structset[element]);
    }
    result.append(")");
    return result;
  }
  return result;
}


pair< vector<RVT>, vector<RPC> > CAsttransform::manipulateDeclaredProcessDefinition(ATermAppl input)
{
  vector<RVT> result_var;
  vector<RPC> result_chan;
  vector<RVT> tmpRVTVar;
  vector<RPC> tmpRPCChan;
  mCRL2log(debug) << "input of manipulateDeclaredProcessDefinition: " << atermpp::aterm( input) << std::endl;
  // INPUT: ProcDecl( ... )
  // Arity is 1 because VarDecl the argument is of the form list*

  if (false /*ATisEmpty(input)*/) // XXX Fixme: was never true
  {
    mCRL2log(debug) << "No variables/channels are declare in the process definition" << std::endl;
    return make_pair(result_var, result_chan);
  }
  else
  {
    //Get first argument
    ATermList to_process = ATLgetArgument(input, 0);
    while (!ATisEmpty(to_process))
    {
      if (StrcmpIsFun("VarDecl", (ATermAppl) ATgetFirst(to_process)))
      {
        tmpRVTVar = manipulateDeclaredProcessVariables((ATermList) ATgetFirst(to_process));
        result_var.insert(result_var.end(), tmpRVTVar.begin(), tmpRVTVar.end());
      }
      if (StrcmpIsFun("ChanDecl", (ATermAppl) ATgetFirst(to_process)))
      {
        tmpRPCChan = manipulateDeclaredProcessChannels((ATermList) ATgetFirst(to_process));
        result_chan.insert(result_chan.end(), tmpRPCChan.begin(), tmpRPCChan.end());
      }
      to_process = ATgetNext(to_process);
    }
  }
  return make_pair(result_var, result_chan);
}

std::vector<RVT> CAsttransform::manipulateDeclaredProcessVariables(ATermList input)
{
  std::vector<RVT>::iterator it;
  std::vector<RVT> result;
  RVT tmpRVT;
  mCRL2log(debug) << "input of manipulateDeclaredProcessVariables: " << atermpp::aterm( input) << std::endl;
  // INPUT: VarDecl( ... ),VarDecl( ... ),...

  ATermList to_process = (ATermList) ATLgetFirst(input);
  while (!ATisEmpty(to_process))
  {
    ATerm element = ATgetFirst(to_process);
    if (!StrcmpIsFun("DataVarID", (ATermAppl) element))
    {
      mCRL2log(error) << "Expcted DataVarID: " << atermpp::aterm(element) << std::endl;
      exit(1);
    }

    tmpRVT.Name = ATgetName(ATgetAFun(ATgetArgument(element,0)));
    tmpRVT.Type = processType((ATermAppl) ATgetArgument(element,1));
    result.push_back(tmpRVT);

    //Add to table for higherlevel lookup
    ChiDeclParameters.push_back(element);
    to_process = ATgetNext(to_process);
  }
  return result;
}

std::vector<RPC> CAsttransform::manipulateDeclaredProcessChannels(ATermList input)
{
  std::vector<RPC>::iterator it;
  std::vector<RPC> result;
  RPC tmpRPC;
  mCRL2log(debug) << "input of manipulateDeclaredProcessChannels: " << atermpp::aterm( input) << std::endl;
  // INPUT: ChanDecl( ... ),...

  ATermList to_process = (ATermList) ATLgetFirst(input);
  while (!ATisEmpty(to_process))
  {
    ATerm element = ATgetFirst(to_process);
    if (!(StrcmpIsFun("ChannelTypedID", (ATermAppl) element)))
    {
      mCRL2log(error) << "Expected ChannelTypedID instead of: " << atermpp::aterm(element) << std::endl;
      exit(1);
    }

    tmpRPC.Name = ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(element,0),0)));
    tmpRPC.Type = processType((ATermAppl) ATgetArgument(element,1));
    //Number of Channels for a #
    //If no # is used the value is 0
    tmpRPC.HashCount = manipulateExpression((ATermAppl) ATgetArgument(element,2));
    result.push_back(tmpRPC);

    //Add to table for higherlevel lookup
    ChiDeclParameters.push_back(element);
    to_process = ATgetNext(to_process);
  }
  return result;
}


std::vector<std::string> CAsttransform::getVariablesNamesFromList(ATermList input)
{
  mCRL2log(debug) << "input of getVariablesNamesFromList: " << atermpp::aterm( input) << std::endl;
  // INPUT: [a,b,....]

  std::vector<std::string> result;
  ATermList to_process = input;
  while (!ATisEmpty(to_process))
  {
    result.push_back(ATgetName(ATgetAFun(ATgetFirst(to_process))));
    to_process = ATgetNext(to_process);
  }
  return result;
}


std::vector<std::string> CAsttransform::getExpressionsFromList(ATermList input)
{
  mCRL2log(debug) << "input of getVariablesNamesFromList: " << atermpp::aterm( input) << std::endl;
  // INPUT: [Expression(...), Expression(...)]

  std::vector<std::string> result;
  ATermList to_process = input;
  while (!ATisEmpty(to_process))
  {
    result.push_back(manipulateExpression((ATermAppl) ATgetFirst(to_process)));
    to_process = ATgetNext(to_process);
  }
  return result;
}


pair< std::vector<RPV>, std::vector<RPC> > CAsttransform::manipulateProcessSpecification(ATermAppl input)
{
  mCRL2log(debug) << "input of manipulateProcessSpecification: " << atermpp::aterm( input) << std::endl;
  //INPUT: ProcSpec( [...] , SepStat ( [...] ))
  std::vector<RPV> result_var;
  std::vector<RPC> result_chan;
  std::vector<RPV> tmpRPV;
  std::vector<RPC> tmpRPC;

  if (false /* ATisEmpty(input) */) // XXX Fixme. was never true
  {
    mCRL2log(debug) << "No variables/channels are declare in the process definition" << std::endl;
    return make_pair(result_var, result_chan);
  }
  else
  {
    //Get first argument
    ATermList to_process = ATLgetArgument(input, 0);
    while (!ATisEmpty(to_process))
    {
      if (StrcmpIsFun("VarSpec", (ATermAppl) ATgetFirst(to_process)))
      {
        //Merge vector with previous vector
        tmpRPV = manipulateProcessVariableDeclarations((ATermList) ATgetFirst(to_process));
        for (std::vector<RPV>::iterator it = tmpRPV.begin(); it != tmpRPV.end(); it++)
        {
          result_var.push_back(*it);
        }
      };
      if (StrcmpIsFun("ChanDecl", (ATermAppl) ATgetFirst(to_process)))
      {
        tmpRPC = manipulateDeclaredProcessChannels((ATermList) ATgetFirst(to_process));
        for (std::vector<RPC>::iterator it = tmpRPC.begin(); it != tmpRPC.end(); it++)
        {
          result_chan.push_back(*it);
        }

      }
      to_process = ATgetNext(to_process);
    }

    //Process Statements
    transitionSystem.clear();
    state = transitionSystem.size();
    parenthesis_level = 0;
    begin_state[parenthesis_level] = state;

    manipulateStatements((ATermAppl) ATAgetArgument(input, 1));
  }

  return make_pair(result_var, result_chan);
}

std::string CAsttransform::processType(ATermAppl input)
{
  mCRL2log(debug) << "input of processType: " << atermpp::aterm( input) << std::endl;
  if (StrcmpIsFun("Type", (ATermAppl) input))
  {
    return ATgetName(ATgetAFun(ATgetArgument(input,0)));
  }
  if (StrcmpIsFun("TupleType", (ATermAppl) input))
  {
    if (structset.find(input) == structset.end())
    {
      ATermList to_process =(ATermList) ATgetArgument(input,0);
      while (!ATisEmpty(to_process))
      {
        processType((ATermAppl) ATgetFirst(to_process));
        to_process = ATgetNext(to_process);
      }
      structset[input] = "S"+to_string(structset.size()) ;
    }
    return structset[input];
  }
  if (StrcmpIsFun("ListType", (ATermAppl) input))
  {
    if (structset.find(input) == structset.end())
    {
      structset[input] = "s"+to_string(structset.size());
    }
    return structset[input];
  }
  if (StrcmpIsFun("SetType", (ATermAppl) input))
  {
    if (structset.find(input) == structset.end())
    {
      structset[input] = "s"+to_string(structset.size());
    }
    return structset[input];
  }

  mCRL2log(error) << "Type is not supported " << atermpp::aterm(input) << std::endl;
  exit(1);
  return "";
}

std::string CAsttransform::initialValueVariable(string Type)
{
  /**
    * TODO: should eventually be replaced by free variables
    *
    **/
  mCRL2log(debug) << "initialValueVariable: " << Type << std::endl;
  for (std::map<ATermAppl, std::string>::iterator itSet=structset.begin();
       itSet != structset.end();
       ++itSet)
  {
    if (itSet->second == Type)
    {
      if (StrcmpIsFun("ListType", itSet->first))
      {
        return "[]";
      }
      if (StrcmpIsFun("SetType", itSet->first))
      {
        //SHOULD BE:
        //return "{}";
        return "[]";
      }
      if (StrcmpIsFun("TupleType", itSet->first))
      {
        string result;
        result.append("tuple_"+Type+"(");

        ATermList to_process =(ATermList) ATgetArgument(itSet->first,0);
        while (!ATisEmpty(to_process))
        {
          ATermAppl element = (ATermAppl) ATgetFirst(to_process);
          if (structset.find(element) != structset.end())
          {
            result.append(initialValueVariable(structset[element]));
          }
          else
          {
            result.append(initialValueVariable(ATgetName(ATgetAFun(ATgetArgument(element,0)))));
          }
          to_process = ATgetNext(to_process);
          if (!ATisEmpty(to_process))
          {
            result.append(", ");
          }
        }
        result.append(")");
        return result;
      }
    }
  }

  if (Type == "Bool")
  {
    return "false";
  }
  if (Type == "Nat")
  {
    return "0";
  }
  if (Type == "Real")
  {
    return "0.0";
  }
  mCRL2log(error) << "Cannot set initial value for Type " << Type << std::endl;
  exit(0);
  return "";
}

std::string CAsttransform::processValue(ATermAppl input)
{
  mCRL2log(debug) << "input of processDataVarIDValue: " << atermpp::aterm( input) << std::endl;
  std::string result;
  if (StrcmpIsFun("Expression", input))
  {
    return ATgetName(ATgetAFun(ATgetArgument(input,0)));
  }
  if (StrcmpIsFun("ListLiteral", input))
  {
    ATermList to_process =(ATermList) ATgetArgument(input,0);
    result = "[";
    while (!ATisEmpty(to_process))
    {
      ATermAppl element = (ATermAppl) ATgetFirst(to_process);
      result.append(processValue(element));
      to_process = ATgetNext(to_process);
      if (!ATisEmpty(to_process))
      {
        result.append(", ");
      }
    }
    result.append("]");
    return result;
  }
  if (StrcmpIsFun("SetLiteral", input))
  {
    set<ATermAppl> UsedATerms;
    ATermList to_process =(ATermList) ATgetArgument(input,0);
    while (!ATisEmpty(to_process))
    {
      ATermAppl element = (ATermAppl) ATgetFirst(to_process);
      UsedATerms.insert(element);
      to_process = ATgetNext(to_process);
    }

    to_process =(ATermList) ATgetArgument(input,0);
    result = "[";
    for (set<ATermAppl>::iterator itSet = UsedATerms.begin();
         itSet != UsedATerms.end();
         ++itSet)
    {
      if (itSet != UsedATerms.begin())
      {
        result.append(", ");
      }
      result.append(processValue(*itSet));
      to_process = ATgetNext(to_process);
    }
    result.append("]");
    return result;
  }
  if (StrcmpIsFun("TupleLiteral", input))
  {
    set<ATermAppl> UsedATerms;
    ATermList to_process =(ATermList) ATgetArgument(input,0);
    while (!ATisEmpty(to_process))
    {
      ATermAppl element = (ATermAppl) ATgetFirst(to_process);
      UsedATerms.insert(element);
      to_process = ATgetNext(to_process);
    }

    to_process =(ATermList) ATgetArgument(input,0);

    string type = processType((ATermAppl) ATgetArgument(input,1));

    result = "tuple_"+type+"(";
    for (set<ATermAppl>::iterator itSet = UsedATerms.begin();
         itSet != UsedATerms.end();
         ++itSet)
    {
      if (itSet != UsedATerms.begin())
      {
        result.append(", ");
      }
      result.append(processValue(*itSet));
      to_process = ATgetNext(to_process);
    }
    result.append(")");
    return result;
  }
  mCRL2log(error) << __FILE__ << ":" << __LINE__ << ": processDataVarIDValue "
                  << atermpp::aterm(input) << " not defined" << std::endl;
  exit(1);
  return "";
}

std::vector<RPV> CAsttransform::manipulateProcessVariableDeclarations(ATermList input)
{
  vector<RPV> result;
  mCRL2log(debug) << "input of manipulateProcessVariableDeclarations: " << atermpp::aterm( input) << std::endl;
  // INPUT: ProcDecl( ... )
  // Arity is 1 because VarDecl the argument is of the form list*

  if (ATisEmpty(input))
  {
    mCRL2log(debug) << "No variables/channels are declare in the process definition" << std::endl;
    return result;
  }
  else
  {
    ATermList to_process = (ATermList) ATLgetFirst(input);
    while (!ATisEmpty(to_process))
    {
      ATerm element = ATgetFirst(to_process);
      if (StrcmpIsFun("DataVarID", (ATermAppl) element))
      {
        RPV tmpRPV;
        tmpRPV.Name = ATgetName(ATgetAFun(ATgetArgument(element,0)));
        tmpRPV.Type = processType((ATermAppl) ATgetArgument(element,1));
        tmpRPV.InitValue = initialValueVariable(tmpRPV.Type);
        result.push_back(tmpRPV);
      }
      if (StrcmpIsFun("DataVarExprID", (ATermAppl) element))
      {
        ATerm sub_element = ATgetArgument(element, 0);
        RPV tmpRPV;
        tmpRPV.Name = ATgetName(ATgetAFun(ATgetArgument(sub_element,0)));
        tmpRPV.Type = processType((ATermAppl) ATgetArgument(sub_element,1));
        tmpRPV.InitValue = processValue((ATermAppl) ATgetArgument(element,1));
        result.push_back(tmpRPV);
      }

      if (!StrcmpIsFun("DataVarExprID", (ATermAppl) element) &&
          !StrcmpIsFun("DataVarID", (ATermAppl) element))
      {
        mCRL2log(error) << "Expected DataVarID or DataVarExprID: " << atermpp::aterm(element) << std::endl;
        exit(1);
      }
      to_process = ATgetNext(to_process);
    }
  }
  return result;

}

std::string CAsttransform::manipulateExpression(ATermAppl input)
{
  std::string result;
  mCRL2log(debug) << "input of manipulateExpression: " << atermpp::aterm( input) << std::endl;
  // INPUT: Expression(...)
  if (StrcmpIsFun("Expression", input))
  {
    return ATgetName(ATgetAFun(ATgetArgument(input,0)));
  }
  if (StrcmpIsFun("BinaryExpression", input))
  {
    /**
      * rewriting the syntax of mathematical expressions
      *
      **/
    bool processed = false;
    if (StrcmpIsFun("MIN",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("min(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(",");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }
    if (StrcmpIsFun("MAX",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("max(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(",");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }
    if (StrcmpIsFun("MOD",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" mod ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }
    if (StrcmpIsFun("DIV",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" div ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }
    if (StrcmpIsFun("^",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("exp(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(",");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }

    if (StrcmpIsFun("-",(ATermAppl) ATgetArgument(input, 0)) &&
        (strcmp("Nat", processType((ATermAppl) ATgetArgument(input, 1)).c_str()) == 0)
       )
    {
      result.append("max(0,");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" - ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      processed = true;
    }
    if (!processed)
    {
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(ATgetName(ATgetAFun(ATgetArgument(input , 0))));
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
    }
    return result;
  }
  if (StrcmpIsFun("UnaryExpression", input))
  {
    //Special unary operators
    if (StrcmpIsFun("()", (ATermAppl) ATgetArgument(input , 0)))
    {
      result.append("(");
    }
    else
    {
      result.append(ATgetName(ATgetAFun(ATgetArgument(input , 0))));
    }

    result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));

    if (StrcmpIsFun("()",(ATermAppl) ATgetArgument(input , 0)))
    {
      result.append(")");
    }
    return result;
  }
  if (StrcmpIsFun("ListLiteral", input))
  {
    return processValue(input);
  }
  if (StrcmpIsFun("SetLiteral", input))
  {
    return processValue(input);
  }

  if (StrcmpIsFun("TupleLiteral", input))
  {
    return processValue(input);
  }

  if (StrcmpIsFun("BinaryListExpression", input))
  {
    if (StrcmpIsFun("in",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" in ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      return result;
    }
    if (StrcmpIsFun("++",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" ++ ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      return result;
    }
    if (StrcmpIsFun("--",(ATermAppl) ATgetArgument(input, 0)))
    {
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (union_setTypes.find(type) == union_setTypes.end())
      {
        union_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap sub_list_"+type+": "+type+"#"+type+" -> "+type+";\n");
        prefixmCRL2spec.append("    sub_list_"+type+"': "+type+"#"+type+"#"+type+" -> "+type+";\n");
        prefixmCRL2spec.append("var xs,ys,zs: "+type+";\n");
        prefixmCRL2spec.append("    x, y: "+sub_type+";\n");
        prefixmCRL2spec.append("eqn sub_list_"+type+"(xs, ys) = sub_list_"+type+"'(xs, ys, []);\n");
        prefixmCRL2spec.append("    x == y -> sub_list_"+type+"'(x |> xs, y |> ys, zs ) = sub_list_"+type+"'(zs++xs, ys, []);\n");
        prefixmCRL2spec.append("    x != y -> sub_list_"+type+"'(x |> xs, y |> ys, zs ) = sub_list_"+type+"'(xs, y|> ys, zs <| x );\n");
        prefixmCRL2spec.append("    sub_list_"+type+"'([], y |> ys, zs ) = sub_list_"+type+"'(zs, ys, []);\n");
        prefixmCRL2spec.append("    sub_list_"+type+"'(xs, [], zs ) = zs++xs;\n");
      }
      result.append("sub_list_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      return result;
    }
  }

  if (StrcmpIsFun("BinarySetExpression", input))
  {
    if (StrcmpIsFun("in",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" in ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      return result;
    }
    if (StrcmpIsFun("\\/",(ATermAppl) ATgetArgument(input, 0)) || StrcmpIsFun("+",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" + ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (union_setTypes.find(type) == union_setTypes.end())
      {
        union_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap union_set_"+type+": "+type+"#"+type+" ->"+type+";\n");
        prefixmCRL2spec.append("var xs, ys :"+type+";\n");
        prefixmCRL2spec.append("         x :"+sub_type+";\n");
        prefixmCRL2spec.append("eqn union_set_"+type+"( [] , ys) = ys;\n");
        prefixmCRL2spec.append("    x in ys    -> union_set_"+type+"( x |> xs, ys ) = union_set_"+type+"(xs , ys );\n");
        prefixmCRL2spec.append("    !(x in ys) -> union_set_"+type+"( x |> xs, ys ) = x |> union_set_"+type+"(xs , ys );\n");
      }

      result.append("union_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
    if (StrcmpIsFun("/\\",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" - ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (intersection_setTypes.find(type) == intersection_setTypes.end())
      {
        intersection_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap intersection_set_"+type+": "+type+"#"+type+" ->"+type+";\n");
        prefixmCRL2spec.append("var xs, ys :"+type+";\n");
        prefixmCRL2spec.append("         x :"+sub_type+";\n");
        prefixmCRL2spec.append("eqn intersection_set_"+type+"( [] , ys) = [];\n");
        prefixmCRL2spec.append("    x in ys    -> intersection_set_"+type+"( x |> xs, ys ) = x |> intersection_set_"+type+"(xs , ys );\n");
        prefixmCRL2spec.append("    !(x in ys) -> intersection_set_"+type+"( x |> xs, ys ) = intersection_set_"+type+"(xs , ys );\n");
      }

      result.append("intersection_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
    if (StrcmpIsFun("sub",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" <= ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (sub_setTypes.find(type) == sub_setTypes.end())
      {
        sub_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap sub_set_"+type+": "+type+"#"+type+" -> Bool;\n");
        prefixmCRL2spec.append("var xs, ys :"+type+";\n");
        prefixmCRL2spec.append("         x :"+sub_type+";\n");
        prefixmCRL2spec.append("eqn sub_set_"+type+"( [] , ys) = true;\n");
        prefixmCRL2spec.append("    (x in ys)  -> sub_set_"+type+"( x |> xs, ys ) = sub_set_"+type+"(xs , ys );\n");
        prefixmCRL2spec.append("    !(x in ys) -> sub_set_"+type+"( x |> xs, ys ) = false;\n");
      }
      result.append("sub_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
    if (StrcmpIsFun("-",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" <= ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (dif_setTypes.find(type) == dif_setTypes.end())
      {
        dif_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap dif_set_"+type+": "+type+"#"+type+" -> "+type+";\n");
        prefixmCRL2spec.append("var xs, ys :"+type+";\n");
        prefixmCRL2spec.append("         x :"+sub_type+";\n");
        prefixmCRL2spec.append("eqn dif_set_"+type+"( [] , ys) = [];\n");
        prefixmCRL2spec.append("    (x in ys)  -> dif_set_"+type+"( x |> xs, ys ) = dif_set_"+type+"(xs , ys );\n");
        prefixmCRL2spec.append("    !(x in ys) -> dif_set_"+type+"( x |> xs, ys ) = x |> dif_set_"+type+"(xs , ys );\n");
      }

      result.append("dif_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
    if (StrcmpIsFun("==",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" = ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (equal_setTypes.find(type) == equal_setTypes.end())
      {
        equal_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap equal_set_"+type+": "+type+"#"+type+" -> Bool;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"': "+type+"#"+type+"#"+type+" -> Bool;\n");
        prefixmCRL2spec.append("var xs, ys, zs: "+type+";\n");
        prefixmCRL2spec.append("    x, y: "+sub_type+";\n");
        prefixmCRL2spec.append("eqn equal_set_"+type+"(xs, ys) = equal_set_"+type+"'(xs, ys, []);\n");
        prefixmCRL2spec.append("    x == y -> equal_set_"+type+"'(x |> xs, y |> ys, zs) = equal_set_"+type+"'(xs, ys++zs, []);\n");
        prefixmCRL2spec.append("    x != y -> equal_set_"+type+"'(x |> xs, y |> ys, zs) = equal_set_"+type+"'(x |> xs, ys, y |> zs);\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'([],[],[]) = true;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'(x |> xs,[],[]) = false;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'(x |> xs,[],y |> zs) = false;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'([],y |> ys,[]) = false;\n");

      }

      result.append("equal_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
    if (StrcmpIsFun("!=",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* ::SHOULD BE::
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(" = ");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 3) ) );
      */
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (equal_setTypes.find(type) == equal_setTypes.end())
      {
        equal_setTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }

        prefixmCRL2spec.append("\nmap equal_set_"+type+": "+type+"#"+type+" -> Bool;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"': "+type+"#"+type+"#"+type+" -> Bool;\n");
        prefixmCRL2spec.append("var xs, ys, zs: "+type+";\n");
        prefixmCRL2spec.append("    x, y: "+sub_type+";\n");
        prefixmCRL2spec.append("eqn equal_set_"+type+"(xs, ys) = equal_set_"+type+"'(xs, ys, []);\n");
        prefixmCRL2spec.append("    x == y -> equal_set_"+type+"'(x |> xs, y |> ys, zs) = equal_set_"+type+"'(xs, ys++zs, []);\n");
        prefixmCRL2spec.append("    x != y -> equal_set_"+type+"'(x |> xs, y |> ys, zs) = equal_set_"+type+"'(x |> xs, ys, y |> zs);\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'([],[],[]) = true;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'(x |> xs,[],[]) = false;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'(x |> xs,[],y |> zs) = false;\n");
        prefixmCRL2spec.append("    equal_set_"+type+"'([],y |> ys,[]) = false;\n");

      }

      result.append("!equal_set_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");

      return result;
    }
  }

  if (StrcmpIsFun("Function", input))
  {
    if (StrcmpIsFun("len",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("#");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      return result;
    }
    if (StrcmpIsFun("hd",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("head(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(")");
      return result;
    }
    if (StrcmpIsFun("tl",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("tail(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(")");
      return result;
    }
    if (StrcmpIsFun("hr",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("rhead(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(")");
      return result;
    }
    if (StrcmpIsFun("tr",(ATermAppl) ATgetArgument(input, 0)))
    {
      result.append("rtail(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(")");
      return result;
    }
    if (StrcmpIsFun("sort",(ATermAppl) ATgetArgument(input, 0)))
    {
      /* string type = processType( (ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (takeTypes.find(type) == takeTypes.end())
      {
        takeTypes.insert(type);
        prefixmCRL2spec.append("\nmap sort_"+type+": "+type+"->"+type+";\n");

        prefixmCRL2spec.append("var xs :"+type+";\n");

        prefixmCRL2spec.append("eqn sort_"+type+"(xs_"+type+", 0)= [];\n");
        prefixmCRL2spec.append("    n_"+type+">0 -> take_"+type+"( e_"+type+"|> xs_"+type+", n_"+type+") = e_"+type+" |> take_"+type+"(xs_"+type+", Int2Nat(n_"+type+" - 1 ));\n");
      }

      result.append("sort_"+type+"(");
      result.append(manipulateExpression( (ATermAppl) ATgetArgument(input , 2) ) );
      result.append(")");
      return result;*/
    }
  }
  if (StrcmpIsFun("Function2", input))
  {
    if (StrcmpIsFun("take",(ATermAppl) ATgetArgument(input, 0)))
    {

      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (takeTypes.find(type) == takeTypes.end())
      {
        takeTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }
        prefixmCRL2spec.append("\nmap take_"+type+": "+type+"#Nat ->"+type+";\n");

        prefixmCRL2spec.append("var xs :"+type+";\n");
        prefixmCRL2spec.append("    n  :Nat;\n");
        prefixmCRL2spec.append("    x  :"+sub_type+";\n");

        prefixmCRL2spec.append("eqn take_"+type+"(xs, 0)= [];\n");
        prefixmCRL2spec.append("    n>0 -> take_"+type+"( x |> xs, n ) = x |> take_"+type+"(xs , Int2Nat(n - 1 ));\n");
      }
      result.append("take_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      return result;
    }
    if (StrcmpIsFun("drop",(ATermAppl) ATgetArgument(input, 0)))
    {
      string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 2),1));
      if (dropTypes.find(type) == dropTypes.end())
      {
        dropTypes.insert(type);
        string sub_type;
        for (std::map<ATermAppl, std::string>::iterator itMap = structset.begin() ;
             itMap != structset.end();
             ++itMap)
        {
          if (itMap->second == type)
          {
            sub_type =processType((ATermAppl) ATgetArgument(itMap->first, 0));
          }
        }
        prefixmCRL2spec.append("\nmap drop_"+type+": "+type+"#Nat ->"+type+";\n");

        prefixmCRL2spec.append("var xs :"+type+";\n");
        prefixmCRL2spec.append("    n  :Nat;\n");
        prefixmCRL2spec.append("    x  :"+sub_type+";\n");

        prefixmCRL2spec.append("eqn drop_"+type+"(xs, 0)= xs;\n");
        prefixmCRL2spec.append("    n>0 -> drop_"+type+"( x |> xs , n ) = drop_"+type+"(xs , Int2Nat(n - 1 ));\n");
      }
      result.append("drop_"+type+"(");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 2)));
      result.append(" , ");
      result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 3)));
      result.append(")");
      return result;
    }
  }

  if (StrcmpIsFun("TupleDot", input))
  {
    string type = processType((ATermAppl) ATgetArgument(ATgetArgument(input, 0),1));
    string index = manipulateExpression((ATermAppl) ATgetArgument(input, 2));

    result.append("get_"+type+"_"+index+"(");
    result.append(manipulateExpression((ATermAppl) ATgetArgument(input , 0)));
    result.append(")");

    return result;
  }
  mCRL2log(error) << __FILE__ << ":" << __LINE__ << ": Encounterd unknown expression: " << atermpp::aterm(input) << std::endl;
  exit(1);
  return "";

}

void CAsttransform::manipulateModelStatements(ATermAppl input)
{
  mCRL2log(debug) << "input of manipulateModelStatements: " << atermpp::aterm( input) << std::endl;
  if (StrcmpIsFun("ParStat", input))
  {
    manipulateModelStatements((ATermAppl) ATgetArgument(input,0));
    initialisation.append(" || ");
    manipulateModelStatements((ATermAppl) ATgetArgument(input,1));
    return ;
  }
  if (StrcmpIsFun("Instantiation", input))
  {
    std::string processName;
    processName = ATgetName(ATgetAFun(ATgetArgument(input,0)));

    if (ProcessForInstantation.find(processName) == ProcessForInstantation.end())
    {
      mCRL2log(error) << "Instantation of process " << processName << " is not declared" << std::endl;
      exit(1);
    }

    //Determine the number of arguments of the instantation
    ATermList to_process = (ATermList) ATgetArgument(input,1);

    //Determine if the number of arguments match between instantations
    if (ATgetLength(to_process) != Chi_interfaces[processName].size())
    {
      mCRL2log(error) << "Number of arguments does not correspond for " << processName << std::endl;
      exit(1);
    }

    //Determine if the arguments are of the same type
    ATermList tmp_to_process = to_process;
    vector< pair < string, int > > local_channels;
    int i = 0;
    while (!ATisEmpty(tmp_to_process))
    {
      bool known = false;
      ATerm element = ATgetFirst(tmp_to_process);
      mCRL2log(debug) << atermpp::aterm(Chi_interfaces[processName].at(i)) << "  --  " << atermpp::aterm(element) << std::endl;

      //Checking for multiple channel-ends
      if (StrcmpIsFun("ChannelTypedID", (ATermAppl) Chi_interfaces[processName].at(i)) &&
          StrcmpIsFun("ChannelTypedID", (ATermAppl) ATgetFirst(tmp_to_process))
         )
      {
        mCRL2log(debug) << "Checking for multiple channel-ends" << std::endl;
        string first = ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(element, 0),0)));
        int second;
        mCRL2log(debug) << "" << atermpp::aterm( element) << std::endl;
        if (! StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(element, 2)))
        {
          second = atoi(ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(element, 2),0))));
        }
        else
        {
          second = 0; //INT_MIN;
        }

        pair<string, int > channelID(first, second);
        ATerm direction = ATgetArgument(ATgetArgument(Chi_interfaces[processName].at(i), 0),1);
        string local_string =ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(Chi_interfaces[processName].at(i), 0),0)));
        //string type = ATgetName(ATgetAFun(ATgetArgument(ATgetArgument(element, 1),0)));
        string type = processType((ATermAppl) ATgetArgument(element, 1));
        local_channels.push_back(channelID);

        // Set Receiving end for a channel
        if (StrcmpIsFun("Recv", (ATermAppl) direction))
        {
          if (Channels[channelID].recv_end.empty())
          {
            RC tmpRC = Channels[channelID];
            tmpRC.recv_end = "Recv_" + type;
            tmpRC.Type = type;
            known = true;
            Channels[channelID]= tmpRC;
          }
          else
          {
            mCRL2log(error) << "Multiple receiving ends for \"" << channelID.first << "\"" << std::endl;
            exit(1);
          }
        }

        // Set Sending end for a channel
        if (StrcmpIsFun("Send", (ATermAppl) direction))
        {
          if (Channels[channelID].send_end.empty())
          {
            RC tmpRC = Channels[channelID];
            tmpRC.send_end = "Send_" + type;
            tmpRC.Type = type;
            Channels[channelID]= tmpRC;
          }
          else
          {
            mCRL2log(error) << "Multiple sending ends for channel \"" << channelID.first << "\"" << std::endl;
            exit(1);
          }
        }
      }

      //TypeCheck interfaces between processes
      mCRL2log(debug) << "TypeCheck interfaces between processes" << std::endl;
      known = TypeChecking((ATermAppl) ATgetArgument(element, 1),
                           (ATermAppl) ATgetArgument(Chi_interfaces[processName].at(i),1));


      if (!known)
      {
        if (StrcmpIsFun("DataVarExpr", (ATermAppl) ATgetFirst(tmp_to_process)))
        {
          mCRL2log(error) << "Interfaces for " << atermpp::aterm(ATgetArgument(Chi_interfaces[processName].at(i),0))
                          << " and " << atermpp::aterm(ATgetArgument(ATgetFirst(tmp_to_process),0))
                          << " do not match for process " << processName << std::endl;
        }
        if (StrcmpIsFun("ChannelTypedID", (ATermAppl) ATgetFirst(tmp_to_process)))
        {
          mCRL2log(error) << "Interfaces for " << atermpp::aterm(ATgetArgument(ATgetArgument(Chi_interfaces[processName].at(i),0),0))
                                  << " and " << atermpp::aterm(ATgetArgument(ATgetArgument(ATgetFirst(tmp_to_process),0),0))
                                  << " do not match for process " << processName << std::endl;
        }
        exit(1);
      }

      tmp_to_process = ATgetNext(tmp_to_process);
      ++i;
    }

    //Write state vector
    initialisation.append(processName);
    if (no_statepar)
    {
      initialisation.append("0");
    }

    std::vector<std::string> arguments;

    while (!(ATisEmpty(to_process)))
    {

      mCRL2log(debug) << "" << atermpp::aterm( to_process) << std::endl;

      //Check if the paramter is either a Expression or a Channel
      if (StrcmpIsFun("BinaryExpression", (ATermAppl) ATgetFirst(to_process))
          || StrcmpIsFun("Expression", (ATermAppl) ATgetFirst(to_process))
          || StrcmpIsFun("UnaryExpression", (ATermAppl) ATgetFirst(to_process))
         )
      {

        arguments.push_back(manipulateExpression((ATermAppl) ATgetFirst(to_process)));
      }

      if (StrcmpIsFun("TypedChannels", (ATermAppl) ATgetFirst(to_process)))
      {
      };

      to_process = ATgetNext(to_process);
    }

    for (vector<RPV>::iterator itRPV = ProcessForInstantation[processName].SpecificationVariables.begin();
         itRPV != ProcessForInstantation[processName].SpecificationVariables.end();
         ++itRPV)
    {

      arguments.push_back(itRPV->InitValue);

    }

    for (vector< pair <string, int > >::iterator itRVT = local_channels.begin();
         itRVT != local_channels.end();
         ++itRVT)
    {
      arguments.push_back(to_string(itRVT->first));
      arguments.push_back(to_string(itRVT->second));
    }

    //Initial State is alway 0
    if (!no_statepar)
    {
      arguments.push_back("0");
    }

    if (!arguments.empty())
    {
      initialisation.append("(");
    }

    for (std::vector< std::string >::iterator i = arguments.begin() ; i != arguments.end(); ++i)
    {
      if (i != arguments.begin())
      {
        initialisation.append(", ");
      }
      initialisation.append(*i);
    }

    if (!arguments.empty())
    {
      initialisation.append(")");
    }

//      exit(0);

    return ;
  }
  mCRL2log(error) << atermpp::aterm(input) << " operator is not supported yet." << std::endl;
  exit(1);
  return;
}

void CAsttransform::manipulateStatements(ATermAppl input)
{
  mCRL2log(debug) << "input of manipulateStatements: " << atermpp::aterm( input) << std::endl;
  RAT transition;
  /**
    * Basic Statements
    *
    **/
  if (StrcmpIsFun("DeltaStat", input))
  {
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,0)))
    {
      transition.guard =  manipulateExpression((ATermAppl) ATgetArgument(input,0));
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
    if (terminate)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    loop = false;
    guardedloop = false;
    return ;
  }
  if (StrcmpIsFun("SkipStat", input))
  {
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,0)))
    {
      transition.guard =  manipulateExpression((ATermAppl) ATgetArgument(input,0));
    }
    transition.stream = stream_number;
    transition.originates_from_stream = originates_from_stream;
    transition.state = state;
    transition.nextstate = next_state;
    transition.action = "skip";
    transition.terminate = terminate;
    transition.parenthesis_level = parenthesis_level;
    transition.looped_state = loop;
    transition.guardedloop = guardedloop;
    transitionSystem.push_back(transition);
    if (terminate)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    loop = false;
    guardedloop = false;
    return;
  }
  if (StrcmpIsFun("AssignmentStat", input))
  {
    transition.state = state;
    transition.stream = stream_number;
    transition.originates_from_stream = originates_from_stream;
    transition.nextstate = next_state;
    transition.action = "skip";
    transition.terminate = terminate;
    transition.looped_state = loop;
    transition.guardedloop = guardedloop;
    transition.parenthesis_level = parenthesis_level;
    transition.vectorUpdate = manipulateAssignmentStat((ATermList) ATgetArgument(input, 2), (ATermList) ATgetArgument(input, 3));
    transitionSystem.push_back(transition);
    if (terminate)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    loop = false;
    guardedloop = false;
    return ;
  }
  /**
    * Communication Statements
    *
    **/
  if (StrcmpIsFun("RecvStat", input))
  {
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,0)))
    {
      transition.guard =  manipulateExpression((ATermAppl) ATgetArgument(input,0));
    }

    transition.state = state;
    transition.stream = stream_number;
    transition.originates_from_stream = originates_from_stream;
    transition.nextstate = next_state;
    transition.terminate = terminate;
    transition.looped_state = loop;
    transition.guardedloop = guardedloop;
    transition.parenthesis_level = parenthesis_level;

    ATermList to_process = (ATermList) ATgetArgument(input, 3);


    assert(ATgetLength(to_process) <= 1);
    transition.action = "";

    std::string ChannelHashValue;
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,2)))
    {
      ChannelHashValue = manipulateExpression((ATermAppl) ATgetArgument(input,2));
      InstantiatedHashedChannels.insert(ATgetName(ATgetAFun(ATgetArgument(input,1))));
    }
    else
    {
      ChannelHashValue = ATgetName(ATgetAFun(ATgetArgument(input,1))) ;
      ChannelHashValue.append("_hash");
    }

    int i = 0;
    if (ATisEmpty(to_process))
    {
      transition.action.append("Recv_Void("+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+", "+
                               ChannelHashValue+
                               ")"
                              )  ;
      DeclaredTypesForChannels.insert("Void");
    }
    while (ATgetLength(to_process) > 0)
    {
      string type = processType((ATermAppl) ATgetArgument(ATgetFirst(to_process), 1));
      transition.action.append("sum "+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+
                               to_string(i)+ ":" +
                               type +
                               ". "+
                               "Recv_"+
                               type +"("+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+", "+
                               ChannelHashValue+", "+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+
                               to_string(i)+
                               ")"
                              );
      transition.vectorUpdate[(std::string) ATgetName(ATgetAFun(ATgetArgument(ATgetFirst(to_process),0)))]=
        (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1))) + to_string(i);
      DeclaredTypesForChannels.insert(type);
      to_process = ATgetNext(to_process);
      i++;

    }
    transitionSystem.push_back(transition);
    if (terminate)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    loop = false;
    guardedloop = false;
    return ;
  }
  if (StrcmpIsFun("SendStat", input))
  {
    mCRL2log(debug) << "SendStat: " << atermpp::aterm( input) << std::endl;
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,0)))
    {
      transition.guard =  manipulateExpression((ATermAppl) ATgetArgument(input,0));
    }

    transition.state = state;
    transition.stream = stream_number;
    transition.originates_from_stream = originates_from_stream;
    transition.nextstate = next_state;
    transition.terminate = terminate;
    transition.looped_state = loop;
    transition.guardedloop = guardedloop;
    transition.parenthesis_level = parenthesis_level;
    ATermList to_process = (ATermList) ATgetArgument(input, 3);

    transition.action = "";

    std::string ChannelHashValue;
    if (!StrcmpIsFun("Nil", (ATermAppl) ATgetArgument(input,2)))
    {
      ChannelHashValue = manipulateExpression((ATermAppl) ATgetArgument(input,2));
      InstantiatedHashedChannels.insert(ATgetName(ATgetAFun(ATgetArgument(input,1))));
    }
    else
    {
      ChannelHashValue = ATgetName(ATgetAFun(ATgetArgument(input,1))) ;
      ChannelHashValue.append("_hash");
    }

    if (ATisEmpty(to_process))
    {
      transition.action.append("Send_Void("+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+", "+
                               ChannelHashValue+
                               ")"
                              )  ;
      DeclaredTypesForChannels.insert("Void");
    }

    while (ATgetLength(to_process) > 0)
    {
      string type = processType((ATermAppl) ATgetArgument(ATgetFirst(to_process), 1));
      transition.action.append("Send_"+
                               type+"("+
                               (std::string) ATgetName(ATgetAFun(ATgetArgument(input,1)))+", "+
                               ChannelHashValue+", "+
                               manipulateExpression((ATermAppl) ATgetFirst(to_process))+
                               ")"
                              );
      DeclaredTypesForChannels.insert(type);
      to_process = ATgetNext(to_process);
    }


    transitionSystem.push_back(transition);
    if (terminate)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    loop = false;
    guardedloop = false;
    return ;
  }
  /**
    * Control Statements
    *
    **/
  if (StrcmpIsFun("SepStat", input))
  {
    terminate = false;

    next_state = transitionSystem.size() + 1;
    manipulateStatements((ATermAppl) ATgetArgument(input,0));
    if (StrcmpIsFun("SepStat", (ATermAppl) ATgetArgument(input,1)))
    {
      terminate = false;
    }
    else
    {
      terminate = true;
      next_state = ATERM_NON_EXISTING_POSITION ;
    }
    state = transitionSystem.size();
    originates_from_stream = stream_number;
    manipulateStatements((ATermAppl) ATgetArgument(input,1));
    return ;
  }
  if (StrcmpIsFun("AltStat", input))
  {
    next_state = ATERM_NON_EXISTING_POSITION;
    terminate = true;
    alternative = true;
    originates_from_stream = stream_number;
    manipulateStatements((ATermAppl) ATgetArgument(input,0));
    state =  begin_state[parenthesis_level];
    originates_from_stream = stream_number;
    manipulateStatements((ATermAppl) ATgetArgument(input,1));
    return ;
  }
  if (StrcmpIsFun("StarStat", input))
  {
    loop = true;
    guardedloop = false;
    originates_from_stream = stream_number;
    manipulateStatements((ATermAppl) ATgetArgument(input,0));
    return ;
  }
  if (StrcmpIsFun("ParStat", input))
  {
    mCRL2log(error) << "Parallel processes are only supported in at model level" << std::endl;
    exit(0);
    int bypass_originates_from_stream = stream_number;
    terminate = true;
    parallel = true;
    originates_from_stream = stream_number;

    manipulateStatements((ATermAppl) ATgetArgument(input,0));

    ++stream_number;


    all_streams.insert(stream_number);

    originates_from_stream = bypass_originates_from_stream;
    streams_per_parenthesis_level.insert(stream_number);
    state = begin_state[parenthesis_level];
    manipulateStatements((ATermAppl) ATgetArgument(input,1));

    return ;
  }
  if (StrcmpIsFun("GuardedStarStat", input))
  {

    transition.guard =  manipulateExpression((ATermAppl) ATgetArgument(input,0));
    transition.stream = stream_number;
    transition.state = state;
    transition.guardedloop = false;
    transition.nextstate = transitionSystem.size()+1;
    transition.action = "skip";
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
    manipulateStatements((ATermAppl) ATgetArgument(input,1));
    return ;
  }

  if (StrcmpIsFun("ParenthesisedStat", input))
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
    begin_state[parenthesis_level]= state;

    manipulateStatements((ATermAppl) ATgetArgument(input,0));

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
    if (ParenthesisedStatIsTerminating)
    {
      endstates_per_parenthesis_level[parenthesis_level].insert(transitionSystem.size());
    }
    return;
  }

  mCRL2log(error) << atermpp::aterm(input) << " operator is not supported yet." << std::endl;
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

  mCRL2log(debug) << "input of manipulateAssignmentStat: " << atermpp::aterm(input_id) << std::endl
                  << " \t " << atermpp::aterm(input_exp) << std::endl;
  if (ATgetLength(input_id) != ATgetLength(input_exp))
  {
    mCRL2log(error) << "Assignment to " << atermpp::aterm(input_id) << " contains a number of assignments not equal to the number of variables" << std::endl;
    exit(1);
  }

  identifiers = getExpressionsFromList(input_id);
  expressions = getExpressionsFromList(input_exp);

  itExp = expressions.begin();
  for (it = identifiers.begin(); it != identifiers.end(); ++it)
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
size_t CAsttransform::determineEndState(std::set<size_t> org_set, size_t lvl)
{
  std::set<size_t> ResultSet;
  std::insert_iterator<std::set<size_t> > InsertIter(ResultSet, ResultSet.begin());
  size_t last_state = 0;

  if (lvl > 0)
  {
    --lvl;
    for (std::vector<RPI>::iterator itVecSet = info_per_parenthesis_level_per_parenthesis[lvl].begin()
         ; itVecSet !=  info_per_parenthesis_level_per_parenthesis[lvl].end()
         ; ++itVecSet)
    {
      set_intersection(org_set.begin(), org_set.end(),
                       (*itVecSet).endstates.begin(), (*itVecSet).endstates.end(),
                       InsertIter
                      );
      if (!ResultSet.empty())
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
  for (std::set<size_t>::iterator itSet= org_set.begin(); itSet != org_set.end(); ++itSet)
  {
    last_state = max(last_state, *itSet);
  }
  return last_state;
}

bool CAsttransform::transitionexists(RAT transition, std::vector<RAT> transitionvector)
{
  bool result = false;
  for (std::vector<RAT>::iterator itRAT = transitionvector.begin();
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

// Typechecking may only be used to see if the interfaces between processes match
bool CAsttransform::TypeChecking(ATermAppl arg1, ATermAppl arg2)
{
  mCRL2log(debug) << __FILE__ << ":" << __LINE__ << std::endl;
  if (arg1 == arg2)
  {
    mCRL2log(debug) << __FILE__ << ":" << __LINE__ << ": return true" << std::endl;
    return true;
  }

  mCRL2log(debug) << "TypeChecking: " << atermpp::aterm(arg1) << ", " << atermpp::aterm(arg2) << std::endl;
  if ((strcmp(ATgetName(ATgetAFun(arg1)), ATgetName(ATgetAFun(arg2)))==0)
      && (strcmp(ATgetName(ATgetAFun(arg1)), "ListType") == 0))
  {
    if (((ATermAppl) ATgetArgument(arg2,0) == gsMakeType(gsMakeNil())) ||
        ((ATermAppl) ATgetArgument(arg1,0) == gsMakeType(gsMakeNil()))
       )
    {
      mCRL2log(debug) << __FILE__ << ":" << __LINE__ << ": return true" << std::endl;
      return true;
    }
    return TypeChecking((ATermAppl) ATgetArgument(arg1,0), ATermAppl(ATgetArgument(arg2,0)));
  }

  if ((strcmp(ATgetName(ATgetAFun(arg1)),ATgetName(ATgetAFun(arg2)))==0)
      && (strcmp(ATgetName(ATgetAFun(arg1)), "Type") == 0)
     )
  {
    if (arg1 == arg2)
    {
      mCRL2log(debug) << __FILE__ << ":" << __LINE__ << ": return true" << std::endl;
      return true;
    }
    else
    {
      mCRL2log(debug) << __FILE__ << ":" << __LINE__ << ": return false" << std::endl;
      return false;
    }
  }
  return false;
}

