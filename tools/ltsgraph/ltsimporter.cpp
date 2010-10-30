// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsimporter.cpp
/// \brief Importer for LTS files

#include <map>
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "ltsimporter.h"

using namespace mcrl2::lts;

template <class LTS_TYPE>
mcrl2::lts::lts_fsm_t read_lts_as_fsm_file(const std::string &fn)
{
  LTS_TYPE l;
  l.load(fn);
  mcrl2::lts::lts_fsm_t result_lts;
  mcrl2::lts::detail::lts_convert(l,result_lts);
  return result_lts;
}

Graph* LTSImporter::importFile(const std::string &fn)
{
  Graph* result = new Graph();
  mcrl2::lts::lts_fsm_t fileLTS;

  const mcrl2::lts::lts_type intype = mcrl2::lts::detail::guess_format(fn);
  switch (intype)
  {
    case lts_lts:
    {
      fileLTS=read_lts_as_fsm_file<lts_lts_t>(fn);
    }
    case lts_none:
      std::cerr << "Cannot determine type of input. Assuming .aut.\n";
    case lts_aut:
    {
      fileLTS=read_lts_as_fsm_file<lts_aut_t>(fn);
    }
    case lts_fsm:
    {
      fileLTS=read_lts_as_fsm_file<lts_fsm_t>(fn);
    }
#ifdef USE_BCG
    case lts_bcg:
    {
      fileLTS=read_lts_as_fsm_file<lts_bcg_t>(fn);
    }
#endif
    case lts_dot:
    {
      fileLTS=read_lts_as_fsm_file<lts_dot_t>(fn);
    }
    case lts_svc:
    { 
      fileLTS=read_lts_as_fsm_file<lts_svc_t>(fn);
    } 
  }


  // lts_fsm_t fileLTS(fn,lts_none);
  // if(fileLTS.read_from(fn))
  {
    unsigned int initialState = fileLTS.initial_state();

    std::map<unsigned int, State*> numsToStates;
    // bool hasParams = fileLTS.has_process_parameters();
    std::vector<std::string> parameters;
    for(size_t i = 0; i < fileLTS.process_parameters().size(); ++i) 
    {
      parameters.push_back(fileLTS.process_parameter(i).first);
    }


    for(unsigned int si = 0; si< fileLTS.num_states(); ++si)
    {
      std::map<std::string, std::string> stateValues;

      unsigned int stNum = si;
      State* s = new State(stNum,
                        stNum == initialState);
      result->addState(s);

      std::pair<unsigned int, State*> pNumToState(stNum, s);
      numsToStates.insert(pNumToState);

      if(s->isInitialState())
      {
        result->setInitialState(s);
      }

      for(size_t i = 0; i < parameters.size(); ++i) 
      {
          /* std::pair<std::string, std::string> stateValue(
              parameters[i],
              mcrl2::lts::detail::pp((ATermAppl)fileLTS.state_value(stNum)[i]));
          stateValues.insert(stateValue); */
          std::pair<std::string, std::string> stateValue(
              parameters[i],
              fileLTS.state_element_value(i,stNum));
          stateValues.insert(stateValue); 
      }
      s->setParameters(stateValues);

      // Generate a random position (x, y, z) for this state
      int x = static_cast<int> (
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );
      int y = static_cast<int> (
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );
      int z = static_cast<int>(
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );

      s->setX(x);
      s->setY(y);
	  s->setZ(z);
    }

    for(mcrl2::lts::transition_const_range r = fileLTS.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition ti=r.front();
      unsigned int idFrom, idTo;
      State *stFrom, *stTo;

      std::string label = mcrl2::lts::detail::pp(fileLTS.label_value(ti.label()));
      idFrom = ti.from();
      idTo = ti.to();

      stFrom = numsToStates.find(idFrom)->second;
      stTo = numsToStates.find(idTo)->second;

      Transition* t = new Transition(stFrom, stTo, label);

      if (idFrom != idTo)
      {
        // t is a transition from stFrom to stTo.
        stFrom->addOutTransition(t);
        stTo->addInTransition(t);
      }
      else
      {
        // t is a self-loop, stFrom and stTo are the same
        stFrom->addSelfLoop(t);
      }
    }

    int numStates = fileLTS.num_states();
    int numLabels = fileLTS.num_action_labels();
    int numTrans = fileLTS.num_transitions();

    result->setInfo(initialState, numStates, numTrans, numLabels);
  }

  return result;
}
