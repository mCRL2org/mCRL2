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

#include "ltsimporter.h"
#include <mcrl2/lts/lts.h>
#include <map>

using namespace mcrl2::lts;

Graph* LTSImporter::importFile(std::string fn)
{
  Graph* result = new Graph();

  lts fileLTS(fn);
  // if(fileLTS.read_from(fn))
  {
    unsigned int initialState = fileLTS.initial_state();

    std::map<unsigned int, State*> numsToStates;
    bool hasParams = fileLTS.has_state_parameters();
    std::vector<std::string> parameters;
    if(hasParams) {
      for(size_t i = 0; i < fileLTS.num_state_parameters(); ++i) {
        parameters.push_back(fileLTS.state_parameter_name_str(i));
      }
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

      if(hasParams) {
        for(size_t i = 0; i < parameters.size(); ++i) {
          std::pair<std::string, std::string> stateValue(
              parameters[i],
              fileLTS.get_state_parameter_value_str(stNum, i));
          stateValues.insert(stateValue);
        }
        s->setParameters(stateValues);
      }

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

      std::string label = fileLTS.label_value_str(
                            ti.label());
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
    int numLabels = fileLTS.num_labels();
    int numTrans = fileLTS.num_transitions();

    result->setInfo(initialState, numStates, numTrans, numLabels);
  }

  return result;
}
