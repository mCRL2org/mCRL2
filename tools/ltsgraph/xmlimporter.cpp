// Author(s): Carst Tankink, Ali Deniz Aladagli, Frank Stappers
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
#include <iostream>
#include "mcrl2/utilities/logger.h"
#include <wx/xml/xml.h>

Graph* XMLImporter::importFile(const std::string& filename)
{
  try
  {

    Graph* g = NULL;
    g = new Graph();

    wxXmlDocument wx_doc;
    wx_doc.Load(wxString(filename.c_str(), wxConvUTF8));

    wxXmlNode* node = wx_doc.GetRoot();

    if (node->GetName() != wxT("Graph"))
    {
      throw "Expected Graph to be root node";
    }

    wxXmlNode* graph_child = node->GetChildren();

    while (graph_child)
    {
      if (graph_child->GetName() == wxT("State"))
      {
        wxString ret_value;

        unsigned long value;
        graph_child->GetPropVal(wxT("value"), &ret_value);
        ret_value.ToULong(&value);

        bool isInitial;
        graph_child->GetPropVal(wxT("isInitial"), &ret_value);
        isInitial = (ret_value == wxT("1"));

        double x;
        graph_child->GetPropVal(wxT("x"), &ret_value);
        ret_value.ToDouble(&x);

        double y;
        graph_child->GetPropVal(wxT("y"), &ret_value);
        ret_value.ToDouble(&y);

        double z;
        graph_child->GetPropVal(wxT("z"), &ret_value);
        ret_value.ToDouble(&z);

        graph_child->GetPropVal(wxT("red"), &ret_value);
        int red = wxAtoi(ret_value);

        graph_child->GetPropVal(wxT("green"), &ret_value);
        int green = wxAtoi(ret_value);

        graph_child->GetPropVal(wxT("blue"), &ret_value);
        int blue = wxAtoi(ret_value);

        wxColour colour(red, green, blue);

        std::map<std::string, std::string> parameters;

        wxXmlNode* state_child = graph_child->GetChildren();
        while (state_child)
        {
          if (state_child->GetName() == wxT("Parameter"))
          {
            wxXmlNode* parameter_child = state_child->GetChildren();

            while (parameter_child)
            {
              wxString name;
              parameter_child->GetPropVal(wxT("name"), &name);
              wxString content = parameter_child->GetContent();


              std::pair<std::string, std::string> p(std::string(name.mb_str()), std::string(content.mb_str()));
              parameters.insert(p);

              parameter_child = parameter_child->GetNext();
            }
          }

          state_child = state_child->GetNext();
        }

        State* s = new State(value, isInitial);
        s->setX(x);
        s->setY(y);
        s->setZ(z);
        s->setColour(colour);
        s->setParameters(parameters);
        g->addState(s);

      }

      graph_child = graph_child->GetNext();
    }

    graph_child = node->GetChildren();

    // All states have been created, now iterate over the transitions
    while (graph_child)
    {
      if (graph_child->GetName() == wxT("Transition"))
      {
        wxString ret_value;

        unsigned long from;
        graph_child->GetPropVal(wxT("from"), &ret_value);
        ret_value.ToULong(&from);

        unsigned long to;
        graph_child->GetPropVal(wxT("to"), &ret_value);
        ret_value.ToULong(&to);

        graph_child->GetPropVal(wxT("label"), &ret_value);
        std::string label = std::string(ret_value.mb_str()) ;

        double x;
        graph_child->GetPropVal(wxT("x"), &ret_value);
        ret_value.ToDouble(&x);

        double y;
        graph_child->GetPropVal(wxT("y"), &ret_value);
        ret_value.ToDouble(&y);

        double z;
        graph_child->GetPropVal(wxT("z"), &ret_value);
        ret_value.ToDouble(&z);

        if (from == to)
        {
          State* s = g->getState(from);
          Transition* t = new Transition(s, s, label);
          t->setControl(x, y, z);
          s->addSelfLoop(t);
        }
        else
        {
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
      graph_child = graph_child->GetNext();
    }
    return g;
  }
  catch (std::exception& e)
  {
    mCRL2log(mcrl2::log::error) << "Could not load XML file: " << e.what();
    return NULL;
  }
}
