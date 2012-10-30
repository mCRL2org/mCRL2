// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsgraph.cpp
/// \brief Implementation of application base class.

#include "wx.hpp" // precompiled headers
#include "ltsgraph.h"
#include "ltsimporter.h"
#include "xmlimporter.h"

#include "springlayout.h"

#include "mcrl2/lts/lts_io.h"

LTSGraph::LTSGraph() :
  super(
    "LTSGraph",
    "visualise an LTS as a graph and manipulate its layout in 2D and 3D", // what-is
    "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout.", // GUI specific description
    "Draw graphs and optimize their layout in a graphical environment. "
    "If INFILE is supplied, the tool will use this file as input for drawing.",
    std::vector<std::string>(1,
                             "Carst Tankink, Ali Deniz Aladagli"))
{
}

bool LTSGraph::run()
{
  wxApp::SetExitOnFrameDelete(true);
  colouring = false;
  brushColour = *wxRED;
  selectedState = NULL;
  selectedTransition = NULL;
  selectedLabel = NULL;
  graph = NULL;
  SpringLayout* springLayout = new SpringLayout(this);
  algorithms.push_back(springLayout);

  mainFrame = new MainFrame(this);
  visualizer = new Visualizer(this);

  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);

  SetTopWindow(mainFrame);
  mainFrame->Show();
  mainFrame->Layout();


  // Load a provided file.
  if (!m_input_filename.empty())
  {
    openFile(m_input_filename);
  }

  return true;
}

IMPLEMENT_APP_NO_MAIN(LTSGraph_gui_tool)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,
                              int nCmdShow)
{

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return wxEntry(argc, argv);
}
#endif

void LTSGraph::openFile(std::string const& path)
{
  try
  {
    // Set fileName
    fileName = path;
    // Find out file format based on extension
    std::string ext = path.substr(path.find_last_of('.') + 1);

    // Get file's basename

    std::auto_ptr<Importer> imp;
    // Create (on stack) appropriate importer imp
    if (ext == "xml")
    {
      // path points to an XML layout file, so create an XML importer
      imp.reset(new XMLImporter());
    }
    else
    {
      // Assume we have an LTS file, so create an LTS importer
      imp.reset(new LTSImporter());
    }
    Graph* graph_old = graph;

    graph = imp->importFile(path);

    size_t is = graph->getInitial();
    size_t ns = graph->getNumStates();
    size_t nt = graph->getNumTrans();
    size_t nl = graph->getNumLabels();

    if( graph->getNumStates() > 1000 )
    {
      wxMessageDialog* warning = new wxMessageDialog(NULL,
          wxString("The LTS contains more than 1000 states.\nThis may affect the performance of the tool.\n Are you sure you want to display the LTS?", wxConvUTF8), wxT("Warning"), wxYES_NO | wxICON_WARNING);
      if(warning->ShowModal() == wxID_NO)
      {
        graph = graph_old;
        return;
      }
    }
    mainFrame->setLTSInfo(is, ns, nt, nl);


  }
  catch (const std::exception& e)
  {
    //wxE e.what() << std::endl;
    wxMessageDialog* dial = new wxMessageDialog(NULL,
        wxString(e.what(), wxConvUTF8), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }

  // Setup graph in rest of tool.
}

Graph* LTSGraph::getGraph()
{
  return graph;
}

size_t LTSGraph::getNumberOfAlgorithms() const
{
  return algorithms.size();
}

LayoutAlgorithm* LTSGraph::getAlgorithm(size_t i) const
{
  return algorithms[i];
}

void LTSGraph::display()
{
  if (glCanvas)
  {
    // Create display event
    wxPaintEvent evt;

    wxEvtHandler* eh = glCanvas->GetEventHandler();

    if (eh)
    {
      eh->ProcessEvent(evt);
    }
  }
}

void LTSGraph::toggleVectorSelected()
{
  if (selectedState != NULL)
  {
    selectedState->setShowStateVector(!selectedState->getShowStateVector());
  }
  display();
}

void LTSGraph::moveObject(double invect[4])
{
  double trans[4];
  double theMtrx[16];
  glCanvas->getMdlvwMtrx(theMtrx);
  Utils::GLUnTransform(theMtrx, invect, trans);
  double x, y, z;
  if (selectedState != NULL)
  {
    x = selectedState->getX();
    y = selectedState->getY();
    z = selectedState->getZ();
  }
  else if (selectedTransition != NULL)
  {
    selectedTransition->getControl(x, y, z);
  }
  else if (selectedLabel != NULL)
  {
    selectedLabel->getLabelPos(x, y, z);
  }
  else
  {
    return;
  }
  double width, height, depth;
  glCanvas->getSize(width, height, depth);
  double rad = glCanvas->getPixelSize() * visualizer->getRadius();
  x = (x / 2000.0) * (width - rad * 2);
  y = (y / 2000.0) * (height - rad * 2);
  z = (z / 2000.0) * (depth - rad * 2);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glTranslated(x, y, z);
  glGetDoublev(GL_MODELVIEW_MATRIX, theMtrx);
  glPopMatrix();
  int pwidth, pheight;
  glCanvas->GetClientSize(&pwidth, &pheight);
  trans[0] = -trans[0] / (width - rad * 2) * theMtrx[14] * 3.0 * 550.0
             / double(pheight);
  trans[1] = -trans[1] / (height - rad * 2) * theMtrx[14] * 3.0 * 550.0
             / double(pheight);
  trans[2] = -trans[2] / (depth - rad * 2) * theMtrx[14] * 3.0 * 550.0
             / double(pheight);
  if (selectedState != NULL)
  {

    selectedState->setX(selectedState->getX() + trans[0]);
    selectedState->setY(selectedState->getY() + trans[1]);
    selectedState->setZ(selectedState->getZ() + trans[2]);
  }

  if (selectedTransition != NULL)
  {
    double prevX, prevY, prevZ;
    selectedTransition->getControl(prevX, prevY, prevZ);
    selectedTransition->setControl(prevX + trans[0], prevY + trans[1],
                                   prevZ + trans[2]);
  }
  if (selectedLabel != NULL)
  {
    double prevX, prevY, prevZ;
    selectedLabel->getLabelPos(prevX, prevY, prevZ);
    selectedLabel->setLabelPos(prevX + trans[0], prevY + trans[1], prevZ
                               + trans[2]);
  }
}

void LTSGraph::moveObject(double x, double y)
{
  double prevX, prevY, prevZ;
  if (selectedState != NULL)
  {
    prevX = selectedState->getX() + x;
    prevY = selectedState->getY() + y;
  }
  if (selectedTransition != NULL)
  {
    selectedTransition->getControl(prevX, prevY, prevZ);
    prevX = prevX + x;
    prevY = prevY + y;
  }
  if (selectedLabel != NULL)
  {
    selectedLabel->getLabelPos(prevX, prevY, prevZ);
    prevX = prevX + x;
    prevY = prevY + y;
  }
  if (selectedState != NULL || selectedTransition != NULL || selectedLabel
      != NULL)
  {
    if (prevX > 1000)
    {
      prevX = 1000;
    }

    if (prevX < -1000)
    {
      prevX = -1000;
    }

    if (prevY > 1000)
    {
      prevY = 1000;
    }

    if (prevY < -1000)
    {
      prevY = -1000;
    }
  }

  if (selectedState != NULL)
  {
    selectedState->setX(prevX);
    selectedState->setY(prevY);
  }
  if (selectedTransition != NULL)
  {
    selectedTransition->setControl(prevX, prevY, prevZ);
  }
  if (selectedLabel != NULL)
  {
    selectedLabel->setLabelPos(prevX, prevY, prevZ);
  }
}

void LTSGraph::lockObject()
{
  if (selectedState != NULL)
  {
    selectedState->lock();
  }
}

void LTSGraph::dragObject()
{
  if (selectedState != NULL)
  {
    selectedState->drag();
  }
}

void LTSGraph::stopDrag()
{
  if (selectedState != NULL)
  {
    selectedState->stopDrag();
  }
}

size_t LTSGraph::getNumberOfObjects()
{
  // TODO: needs to be adapted when new types of selectable objects are added
  return graph->getNumberOfStates();
}

void LTSGraph::deselect()
{
  if (selectedState != NULL)
  {
    selectedState->deselect();
  }

  if (selectedTransition != NULL)
  {
    selectedTransition->deselect();
  }

  if (selectedLabel != NULL)
  {
    selectedLabel->deselect();
  }

  selectedState = NULL;
  selectedTransition = NULL;
  selectedLabel = NULL;
  display();
}

void LTSGraph::uncolourState(size_t selectedObject)
{
  if (colouring)
  {
    graph->colourState(selectedObject);
  }
}

void LTSGraph::colourState(size_t selectedObject)
{
  if (colouring)
  {
    graph->colourState(selectedObject, brushColour);
  }
}

void LTSGraph::selectState(size_t selectedObject)
{
  if (!colouring)
  {
    selectedState = graph->selectState(selectedObject);
  }
}

void LTSGraph::selectTransition(size_t state, size_t transition)
{
  selectedTransition = graph->selectTransition(state, transition);
}

void LTSGraph::selectSelfLoop(size_t state, size_t transition)
{
  selectedTransition = graph->selectSelfLoop(state, transition);
}

void LTSGraph::selectLabel(size_t state, size_t transition)
{
  selectedLabel = graph->selectTransition(state, transition);
}

void LTSGraph::selectSelfLabel(size_t state, size_t transition)
{
  selectedLabel = graph->selectSelfLoop(state, transition);
}

void LTSGraph::setRadius(int radius)
{
  visualizer->setRadius(radius);
  display();
}

int LTSGraph::getRadius() const
{
  return visualizer->getRadius();
}

void LTSGraph::setTransLabels(bool value)
{
  visualizer->setTransLabels(value);
  display();
}

void LTSGraph::setStateLabels(bool value)
{
  visualizer->setStateLabels(value);
  display();
}

void LTSGraph::setCurves(bool value)
{
  visualizer->setCurves(value);
  display();
}

std::string LTSGraph::getFileName() const
{
  return fileName;
}

double LTSGraph::getAspectRatio() const
{
  return glCanvas->getAspectRatio();
}

void LTSGraph::setBrushColour(wxColour colour)
{
  brushColour = colour;
}

void LTSGraph::setTool(bool isColour)
{
  colouring = isColour;
}

void LTSGraph::getCanvasMdlvwMtrx(double* mtrx)
{
  glCanvas->getMdlvwMtrx(mtrx);
}

void LTSGraph::getCanvasCamPos(double& x, double& y, double& z)
{
  glCanvas->getCamPos(x, y, z);
}

bool LTSGraph::get3dMode()
{
  return glCanvas->get3D();
}

void LTSGraph::forceWalls()
{
  if (graph)
  {
    for (size_t i = 0; i < graph->getNumberOfStates(); ++i)
    {
      State* s = graph->getState(i);
      double newX, newY;
      newX = s->getX();
      newY = s->getY();
      if (newX > 1000)
      {
        newX = 1000;
      }

      if (newX < -1000)
      {
        newX = -1000;
      }

      if (newY > 1000)
      {
        newY = 1000;
      }

      if (newY < -1000)
      {
        newY = -1000;
      }

      s->setX(newX);
      s->setY(newY);
    }
    glCanvas->display();
  }
}
