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
#include "ltsimporter.h"

using namespace mcrl2::lts;

Graph* LTSImporter::importFile(const std::string& fn)
{
  Graph* result = new Graph();
  mcrl2::lts::lts_fsm_t fileLTS;

  load_lts_as_fsm_file(fn,fileLTS);

  // lts_fsm_t fileLTS(fn,lts_none);
  // if(fileLTS.read_from(fn))
  {
    size_t initialState = fileLTS.initial_state();

    std::map<size_t, State*> numsToStates;
    // bool hasParams = fileLTS.has_process_parameters();
    std::vector<std::string> parameters;
    for (size_t i = 0; i < fileLTS.process_parameters().size(); ++i)
    {
      parameters.push_back(fileLTS.process_parameter(i).first);
    }


    for (unsigned int si = 0; si< fileLTS.num_states(); ++si)
    {
      std::map<std::string, std::string> stateValues;

      unsigned int stNum = si;
      State* s = new State(stNum,
                           stNum == initialState);
      result->addState(s);

      std::pair<unsigned int, State*> pNumToState(stNum, s);
      numsToStates.insert(pNumToState);

      if (s->isInitialState())
      {
        result->setInitialState(s);
      }

      for (size_t i = 0; i < parameters.size(); ++i)
      {
        std::pair<std::string, std::string> stateValue(
          parameters[i],
          fileLTS.state_element_value(i,fileLTS.state_label(stNum)[i]));
        stateValues.insert(stateValue);
      }
      s->setParameters(stateValues);

      // Generate a random position (x, y, z) for this state
      int x = static_cast<int>(
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );
      int y = static_cast<int>(
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );
      int z = static_cast<int>(
                (rand() / static_cast<float>(RAND_MAX) - .5) * 2000
              );

      s->setX(x);
      s->setY(y);
      s->setZ(z);
    }

    const std::vector<mcrl2::lts::transition> &trans=fileLTS.get_transitions();
    for (std::vector<mcrl2::lts::transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
    {
      size_t idFrom, idTo;
      State* stFrom, *stTo;

      std::string label = mcrl2::lts::detail::pp(fileLTS.action_label(r->label()));
      idFrom = r->from();
      idTo = r->to();

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

    size_t numStates = fileLTS.num_states();
    size_t numLabels = fileLTS.num_action_labels();
    size_t numTrans = fileLTS.num_transitions();

    result->setInfo(initialState, numStates, numTrans, numLabels);
  }

  return result;
}
