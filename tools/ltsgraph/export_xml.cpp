// Author(s): Carst Tankink, Ali Deniz Aladagli, Frank Stappers
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
#include <iostream>
#include <wx/xml/xml.h>

ExporterXML::ExporterXML(Graph* g) : Exporter(g)
{

}

ExporterXML::~ExporterXML() {}

bool ExporterXML::export_to(wxString _filename)
{
  try
  {

    wxXmlDocument wx_doc;
    wx_doc.SetVersion(wxT("1.0"));
    wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE , wxT("Graph"));
    wx_doc.SetRoot(root);
    for (size_t i = graph->getNumberOfStates(); i > 0 ; --i)
    {
      wxXmlNode* state = new wxXmlNode(root, wxXML_ELEMENT_NODE , wxT("State"));
      State* s = graph->getState(i-1);
      wxColour c = s->getColour();

      size_t fromVal = s->getValue();
      state->AddAttribute(wxT("value"), wxString::Format(wxT("%lu"), fromVal));
      state->AddAttribute(wxT("isInitial"), wxString::Format(wxT("%i"), s->isInitialState()));
      state->AddAttribute(wxT("x"), wxString::Format(wxT("%f"), s->getX()));
      state->AddAttribute(wxT("y"), wxString::Format(wxT("%f"), s->getY()));
      state->AddAttribute(wxT("z"), wxString::Format(wxT("%f"), s->getZ()));
      state->AddAttribute(wxT("red"), wxString::Format(wxT("%i"), (int)c.Red()));
      state->AddAttribute(wxT("green"), wxString::Format(wxT("%i"), (int)c.Green()));
      state->AddAttribute(wxT("blue"), wxString::Format(wxT("%i"), (int)c.Blue()));

      std::map<std::string, std::string> params = s->getParameters();
      std::map<std::string, std::string>::iterator it;

      for (it = params.begin(); it != params.end(); ++it)
      {
        wxXmlNode* parameter = new wxXmlNode(state, wxXML_ELEMENT_NODE , wxT("Parameter"));
        parameter->AddAttribute(wxT("name"), wxString(it->first.c_str(), wxConvUTF8));
        new wxXmlNode(parameter , wxXML_TEXT_NODE , wxEmptyString, wxString(it->second.c_str(), wxConvUTF8));
      }

      for (size_t j = 0; j < s->getNumberOfTransitions(); ++j)
      {
        Transition* t = s->getTransition(j);
        wxXmlNode* transition = new wxXmlNode(root, wxXML_ELEMENT_NODE , wxT("Transition"));

        size_t toVal = t->getTo()->getValue();
        transition->AddAttribute(wxT("from"), wxString::Format(wxT("%lu"), fromVal));
        transition->AddAttribute(wxT("to"), wxString::Format(wxT("%lu"), toVal));
        transition->AddAttribute(wxT("label"), wxString(t->getLabel().c_str(), wxConvUTF8));

        double x, y, z;
        t->getControl(x, y, z);
        transition->AddAttribute(wxT("x"), wxString::Format(wxT("%f"), x));
        transition->AddAttribute(wxT("y"), wxString::Format(wxT("%f"), y));
        transition->AddAttribute(wxT("z"), wxString::Format(wxT("%f"), z));

      }

      for (size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
      {
        Transition* t = s->getSelfLoop(j);
        wxXmlNode* transition = new wxXmlNode(root, wxXML_ELEMENT_NODE , wxT("Transition"));

        transition->AddAttribute(wxT("from"), wxString::Format(wxT("%lu"), fromVal));
        transition->AddAttribute(wxT("to"), wxString::Format(wxT("%lu"), fromVal));
        transition->AddAttribute(wxT("label"), wxString(t->getLabel().c_str(), wxConvUTF8));

        double x, y, z;
        t->getControl(x, y, z);
        transition->AddAttribute(wxT("x"), wxString::Format(wxT("%f"), x));
        transition->AddAttribute(wxT("y"), wxString::Format(wxT("%f"), y));
        transition->AddAttribute(wxT("z"), wxString::Format(wxT("%f"), z));
      }
    }
    wx_doc.Save(_filename);
    return true;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception when exporting: " << e.what();
    return false;
  }

}

