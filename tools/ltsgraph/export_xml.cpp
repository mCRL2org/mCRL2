// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file export_xml.cpp
/// \brief Implementation of XML exporter.

#include "export_xml.h"
#include "ticpp.h"

ExporterXML::ExporterXML(Graph* g) : Exporter(g)
{

};

ExporterXML::~ExporterXML(){};

bool ExporterXML::export_to(wxString _filename)
{
  try
  {

    ticpp::Document xmlDoc(_filename.fn_str());
  
    ticpp::Declaration* decl = new ticpp::Declaration("1.0", "", "");
    xmlDoc.LinkEndChild(decl);
    

    // Add a graph element to the document
    ticpp::Element* graphEl = new ticpp::Element("Graph");
    graphEl->SetAttribute("name", "Foo"); // TODO: Graph names
    xmlDoc.LinkEndChild(graphEl);
    
    // A Graph has two types of children, states and transitions.
    for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);
      wxColour c = s->getColour();
      ticpp::Element* state = new ticpp::Element("State");

      size_t fromVal = s->getValue();

      state->SetAttribute("value", fromVal);
      state->SetAttribute("isInitial", s->isInitialState());
      state->SetAttribute("x", s->getX());
      state->SetAttribute("y", s->getY());
      state->SetAttribute("red", (int)c.Red());
      state->SetAttribute("green", (int)c.Green());
      state->SetAttribute("blue", (int)c.Blue());

      graphEl->LinkEndChild(state);
      
      for(size_t j = 0; j < s->getNumberOfTransitions(); ++j)
      {
        Transition* t = s->getTransition(j);
        ticpp::Element* transition = new ticpp::Element("Transition");
        
        size_t toVal = t->getTo()->getValue();

        transition->SetAttribute("from", fromVal);
        transition->SetAttribute("to", toVal);
        transition->SetAttribute("label", t->getLabel());
        
        double x, y;
        t->getControl(x, y);
        transition->SetAttribute("x", x);
        transition->SetAttribute("y", y);

        graphEl->LinkEndChild(transition);
      }

      for(size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
      {
        Transition* t = s->getSelfLoop(j);
        ticpp::Element* transition = new ticpp::Element("Transition");

        transition->SetAttribute("from", fromVal);
        transition->SetAttribute("to", fromVal);
        transition->SetAttribute("label", t->getLabel());

        double x, y;
        t->getControl(x, y);
        transition->SetAttribute("x", x);
        transition->SetAttribute("y", y);

        graphEl->LinkEndChild(transition);
      }
    }
    
    xmlDoc.SaveFile();

    return true;
  }
  catch(ticpp::Exception e)
  {
    std::cerr << "Exception by ticpp: " << e.what();
    return false;
  }

}

