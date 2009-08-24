// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file xmlimporter.cpp
/// \brief Importer for XML files.


#include "xmlimporter.h"
#include "ticpp.h"
#include <iostream>

Graph* XMLImporter::importFile(std::string filename)
{
  try
  {

    Graph* g = NULL;
    g = new Graph();

    ticpp::Document doc(filename);
    doc.LoadFile();


    ticpp::Iterator<ticpp::Element> graph("Graph");


    for( graph = graph.begin(&doc); graph != graph.end(); ++graph)
    {
      // Iterate over all states within the graph
      ticpp::Iterator<ticpp::Element> state("State");
      for(state = state.begin(graph.Get()); state!= state.end(); ++state)
      {
        size_t value;
        state->GetAttribute("value", &value);

        bool isInitial;
        state->GetAttribute("isInitial", &isInitial);

        double x;
        state->GetAttribute("x", &x);

        double y;
        state->GetAttribute("y", &y);

		double z;
		state->GetAttribute("z", &z);

        int red;
        state->GetAttribute("red", &red);

        int green;
        state->GetAttribute("green", &green);

        int blue;
        state->GetAttribute("blue", &blue);

        wxColour colour(red, green, blue);

        std::map<std::string, std::string> parameters;

        ticpp::Iterator<ticpp::Element> param("Parameter");
        for(param  = param.begin(state.Get());
            param != param.end(); ++param)
        {
          std::string name;
          param->GetAttribute("name", &name);

          std::string parValue;
          param->GetText(&parValue);
          std::pair<std::string, std::string> p(name, parValue);
          parameters.insert(p);
        }


        State* s = new State(value, isInitial);
        s->setX(x);
        s->setY(y);
		s->setZ(z);
        s->setColour(colour);
        s->setParameters(parameters);
        g->addState(s);
      }

      // All states have been created, now iterate over the transitions
      ticpp::Iterator<ticpp::Element> transition("Transition");
      for(transition  = transition.begin(graph.Get());
          transition != transition.end(); ++transition)
      {
        size_t from, to;
        transition->GetAttribute("from", &from);
        transition->GetAttribute("to", &to);

        std::string label = transition->GetAttribute("label");
        double x, y, z;
        transition->GetAttribute("x", &x);
        transition->GetAttribute("y", &y);
		transition->GetAttribute("z", &z);

        if(from == to)
        {
          State* s = g->getState(from);
          Transition* t = new Transition(s, s, label);
          t->setControl(x, y, z);
          s->addSelfLoop(t);
        }
        else{
          State* fromState = g->getState(from);
          State* toState = g->getState(to);
          Transition* t = new Transition(fromState, toState, label);

          fromState->addOutTransition(t);
          // Now set the transitions control, as the state's add method can
          // change this.
          t->setControl(x,y,z);
          toState->addInTransition(t);
        }
      }
    }
    return g;
  }
  catch(ticpp::Exception e)
  {
    std::cerr << "Exception by ticpp: " << e.what();
    return NULL;
  }
}
