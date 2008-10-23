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

        State* s = new State(value, isInitial);
        s->setX(x);
        s->setY(y);

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
        double x, y;
        transition->GetAttribute("x", &x);
        transition->GetAttribute("y", &y);
        
        if(from == to)
        {
          State* s = g->getState(from);
          Transition* t = new Transition(s, s, label);
          t->setControl(x, y);
          s->addSelfLoop(t);
        }
        else{
          State* fromState = g->getState(from);
          State* toState = g->getState(to);
          Transition* t = new Transition(fromState, toState, label);

          fromState->addOutTransition(t);
          // Now set the transitions control, as the state's add method can 
          // change this.
          t->setControl(x,y);
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
};
