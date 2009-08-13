// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file springlayout.cpp
/// \brief Implementation of the "spring embedder" optimization algorithm.

#include "wx.hpp" // precompiled headers

#include "springlayout.h"
#include <wx/sizer.h>
#include <wx/slider.h>
#include "ids.h"

using namespace IDS;

BEGIN_EVENT_TABLE(SpringLayout, wxEvtHandler)
  EVT_BUTTON(myID_START_OPTI, SpringLayout::onStart)
  EVT_BUTTON(myID_STOP_OPTI, SpringLayout::onStop)

  EVT_COMMAND_SCROLL(myID_NS_SLIDER, SpringLayout::onNodeStrength)
  EVT_COMMAND_SCROLL(myID_ES_SLIDER, SpringLayout::onEdgeStrength)
  EVT_COMMAND_SCROLL(myID_NL_SLIDER, SpringLayout::onTransLength)

END_EVENT_TABLE()


SpringLayout::SpringLayout(LTSGraph3d* owner)
{
  app = owner;
  edgeStiffness = 3;
  nodeStrength = 100000;
  naturalLength = 10;
  stopOpti = true;
  stopped = true;
  optimizeBtn = 0;
  stopBtn = 0;
}

void SpringLayout::setupPane(wxPanel* pane)
{
  int lflags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int rflags = wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL;

  wxFlexGridSizer* sizer = new wxFlexGridSizer(0, 1, 0, 0);

  wxSlider* sliderNodeStrength = new wxSlider(
                                        pane, myID_NS_SLIDER,
                                        nodeStrength,10000, 1000000,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSL_HORIZONTAL);
  wxSlider* sliderEdgeStiffness = new wxSlider(
                                        pane, myID_ES_SLIDER,
                                        edgeStiffness, 0, 15,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSL_HORIZONTAL);
  wxSlider* sliderNaturalLength = new wxSlider(
                                        pane, myID_NL_SLIDER,
                                        naturalLength, 1, 500,
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSL_HORIZONTAL);

  optimizeBtn = new wxButton(pane, myID_START_OPTI, wxT("Start"));
  stopBtn = new wxButton(pane, myID_STOP_OPTI, wxT("Stop"));
  stopBtn->Enable(false);

  sizer->Add(
    new wxStaticText(pane, wxID_ANY, wxT("State repulsion")),
    0, lflags, 4);
  sizer->Add(sliderNodeStrength, 0, rflags, 3);

  sizer->Add(
    new wxStaticText(pane, wxID_ANY, wxT("Transition attracting force")),
    0, lflags, 4);
  sizer->Add(sliderEdgeStiffness, 0, rflags, 3);

  sizer->Add(
    new wxStaticText(pane, wxID_ANY, wxT("Natural transition length")),
    0, lflags, 4);
  sizer->Add(sliderNaturalLength, 0, rflags, 3);

  wxFlexGridSizer* btnSizer = new wxFlexGridSizer(0, 2, 0, 0);
  btnSizer->Add(optimizeBtn);
  btnSizer->Add(stopBtn);


  sizer->Add(btnSizer);

  pane->SetSizer(sizer);
  pane->Fit();
  pane->Layout();

}




void SpringLayout::onStart(wxCommandEvent& /* event */)
{
  start();
}


void SpringLayout::onStop(wxCommandEvent& /* event */)
{
  stop();
}

void SpringLayout::layoutGraph(Graph* graph)
{
  size_t nrStates = graph->getNumberOfStates();
  std::vector<float> sumFX(nrStates, 0.0f);
  std::vector<float> sumFY(nrStates, 0.0f);
  std::vector<float> sumFZ(nrStates, 0.0f);

  double windowWidth = 2000;
  double windowHeight = 2000;
  double windowDepth = 2000;

  for(size_t i = 0; i < nrStates; ++i)
  {
    State* s1 = graph->getState(i);

    // Calculate forces
    double x1 = s1->getX();
    double y1 = s1->getY();
	double z1 = s1->getZ();

    for (size_t j = i + 1; j < nrStates; ++j)
    {
      State* s2 = graph->getState(j);

      double x2 = s2->getX();
      double y2 = s2->getY();
	  double z2 = s2->getZ();
      double xDiff = x1 - x2;
      double yDiff = y1 - y2;
	  double zDiff = z1 - z2;

      // Euclidean distance
      double distance = sqrt (xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

      if (distance > 1)
      {
		double s = nodeStrength / (distance * distance * distance);

		sumFX[i] += s * xDiff;
		sumFY[i] += s * yDiff;
		sumFZ[i] += s * zDiff;
		sumFX[j] -= s * xDiff;
		sumFY[j] -= s * yDiff;
		sumFZ[j] -= s * zDiff;
	  }
      else
      {
        sumFX[j] += 5;
        sumFY[j] += 5;
		sumFZ[j] += 5;
        sumFX[i] += -5;
        sumFY[i] += -5;
		sumFZ[i] += -5;
      }
    }

    sumFX[i] += -2 * x1 / windowWidth;
    sumFY[i] += -2 * y1 / windowHeight;
	sumFZ[i] += -2 * z1 / windowDepth;

    for(size_t j = 0; j < s1->getNumberOfTransitions(); ++j)
    {
      State* s2 = s1->getTransition(j)->getTo();
      double x1 = s1->getX();
      double x2 = s2->getX();
      double y1 = s1->getY();
      double y2 = s2->getY();
	  double z1 = s1->getZ();
	  double z2 = s2->getZ();


      double xDiff = x2 - x1;
      double yDiff = y2 - y1;
	  double zDiff = z2 - z1;
      double distance = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

      if (distance > 0.1)
      {
        double s = (edgeStiffness * log (distance / naturalLength)) / distance;

        sumFX[i] += s * xDiff;
        sumFY[i] += s * yDiff;
		sumFZ[i] += s * zDiff;
      }
    }

    for(size_t j = 0; j < s1->getNumberOfInTransitions(); ++j)
    {
      State* s2 = s1->getInTransition(j)->getFrom();
      double x1 = s2->getX();
      double x2 = s1->getX();
      double y1 = s2->getY();
      double y2 = s1->getY();
	  double z1 = s2->getZ();
	  double z2 = s1->getZ();

      double xDiff = x2 - x1;
      double yDiff = y2 - y1;
	  double zDiff = z2 - z1;
      double distance = sqrt(xDiff * xDiff + yDiff * yDiff + zDiff * zDiff);

      if (distance > 0.1)
      {
        double s = (edgeStiffness * log(distance / naturalLength)) / distance;
        sumFX[i] -= s * xDiff;
        sumFY[i] -= s * yDiff;
		sumFZ[i] -= s * zDiff;
      }
    }
  }

  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    double newX = 0;
    double newY = 0;
	double newZ = 0;
    State* s = graph->getState(i);
    if (!(s->isLocked() || s->isDragged()))
    {
      newX = s->getX() + sumFX[i];
      newY = s->getY() + sumFY[i];
	  newZ = s->getZ() + sumFZ[i];

      s->setX(newX);
      s->setY(newY);
	  s->setZ(newZ);
    }
  }

}

void SpringLayout::onNodeStrength(wxScrollEvent& evt)
{
  nodeStrength = evt.GetPosition();
}

void SpringLayout::onEdgeStrength(wxScrollEvent& evt)
{
  edgeStiffness = evt.GetPosition();
}

void SpringLayout::onTransLength(wxScrollEvent& evt)
{
  naturalLength = evt.GetPosition();
}

void SpringLayout::toggle()
{
  if (stopOpti) {
    start();
  }
  else {
    stop();
  }
}

void SpringLayout::start()
{
  Graph* g = app->getGraph();
  if (g) {
    optimizeBtn->Enable(false);
    stopBtn->Enable(true);
    stopOpti = false;
    stopped = false;

    while(!stopOpti)
    {
      layoutGraph(g);

      if(app)
      {
        app->display();
      }

      wxYield();
    }

    stopped = true;

  }
}

void SpringLayout::stop()
{
  if(optimizeBtn)
  {
    optimizeBtn->Enable(true);
  }
  if(stopBtn)
  {
    stopBtn->Enable(false);
  }
  stopOpti = true;
}
