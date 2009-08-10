// Author(s): Carst Tankink
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

#include "mcrl2/lts/lts.h"

#ifdef ENABLE_SQUADT_CONNECTIVITY
// Configures tool capabilities.
void LTSGraph::set_capabilities(tipi::tool::capabilities& c) const {
  std::set< mcrl2::lts::lts_type > const& input_formats(mcrl2::lts::lts::supported_lts_formats());

  for (std::set< mcrl2::lts::lts_type >::const_iterator i = input_formats.begin(); i != input_formats.end(); ++i) {
    c.add_input_configuration("main-input", tipi::mime_type(mcrl2::lts::lts::mime_type_for_type(*i)), tipi::tool::category::visualisation);
  }
}

// Queries the user via SQuADt if needed to obtain configuration information
void LTSGraph::user_interactive_configuration(tipi::configuration&) { }

// Check an existing configuration object to see if it is usable
bool LTSGraph::check_configuration(tipi::configuration const& c) const {
  if (c.input_exists("main-input")) {
    /* The input object is present, verify whether the specified format is supported */
    if (mcrl2::lts::lts::parse_format(c.get_input("main-input").type().sub_type().c_str()) == mcrl2::lts::lts_none) {
      send_error("Invalid configuration: unsupported type `" +
          c.get_input("main-input").type().sub_type() + "' for main input");
    }
    else {
      return true;
    }
  }

  return false;
}

bool LTSGraph::perform_task(tipi::configuration& c) {
  m_input_filename = c.get_input("main-input").location();

  return super::perform_task(c);
}
#endif

LTSGraph::LTSGraph() : super("LTSGraph",
    "visualise an LTS as a graph and manipulate its layout", // what-is
    "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout.", // GUI specific description
    "Draw graphs and optimize their layout in a graphical environment. "
    "If INFILE is supplied, the tool will use this file as input for drawing.",
    std::vector< std::string >(1, "Carst Tankink"))
{ }

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
/*  GemLayout* gemLayout = new GemLayout(this);
  algorithms.push_back(gemLayout);*/

  mainFrame = new MainFrame(this);
  visualizer = new Visualizer(this);

  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);

  // Load a provided file.
  if (!m_input_filename.empty())
  {
    openFile(m_input_filename);
  }

  SetTopWindow(mainFrame);
  mainFrame->Show();
  glCanvas->initialize();
  mainFrame->Layout();

  return true;
}

IMPLEMENT_APP_NO_MAIN(LTSGraph)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                                  HINSTANCE hPrevInstance,
                                  wxCmdLineArgType lpCmdLine,
                                  int nCmdShow) {

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return wxEntry(argc, argv);
}
#endif

void LTSGraph::openFile(std::string const &path)
{
  // Set fileName
  fileName = path;
  // Find out file format based on extension
  std::string ext = path.substr(path.find_last_of( '.' ) + 1);

  // Get file's basename

  std::auto_ptr<Importer> imp;
  // Create (on stack) appropriate importer imp
  if ( ext == "xml")
  {
    // path points to an XML layout file, so create an XML importer
    imp.reset(new XMLImporter());
  }
  else
  {
    // Assume we have an LTS file, so create an LTS importer
    imp.reset(new LTSImporter());
  }

  graph = imp->importFile(path);

  int is = graph->getInitial();
  int ns = graph->getNumStates();
  int nt = graph->getNumTrans();
  int nl = graph->getNumLabels();

  mainFrame->setLTSInfo(is, ns, nt, nl);

  // Call the display routines. This is necessary to make sure the graph is
  // rendered on the Mac
  display();

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
  if(glCanvas)
  {
    // Create display event
    wxPaintEvent evt;

    wxEvtHandler* eh = glCanvas->GetEventHandler();

    if(eh)
    {
      eh->ProcessEvent(evt);
    }
  }
}

void LTSGraph::toggleVectorSelected() {
  if(selectedState != NULL)
  {
    selectedState->setShowStateVector(
      !selectedState->getShowStateVector());
  }
  display();
}

void LTSGraph::moveObject(double ox, double oy, double nx, double ny, double height, double width)
{
  if(selectedState != NULL)
  {
    double z = selectedState->getZ();
	oy = height - oy - 1;
	ny = height - ny - 1;
//	get2dCoords(ox, oy, z);
//	get2dCoords(nx, ny, z);
	double diffX = (nx - ox) / width * 2000;
	double diffY = (ny - oy) / height * 2000;
	z = 0;
    selectedState->setX(selectedState->getX() + diffX);
    selectedState->setY(selectedState->getY() + diffY);
	selectedState->setZ(selectedState->getZ() + z);
  }

  if(selectedTransition != NULL)
  {
    double prevX, prevY, prevZ;
    selectedTransition->getControl(prevX, prevY, prevZ);
	oy = height - oy - 1;
	ny = height - ny - 1;
//	get2dCoords(ox, oy, prevZ);
//	get2dCoords(nx, ny, prevZ);
	double diffX = (nx - ox) / width * 2000;
	double diffY = (ny - oy) / height * 2000;
	double z = 0;

    selectedTransition->setControl(prevX + diffX, prevY + diffY, prevZ + z);
  }
  if(selectedLabel != NULL)
  {
    double prevX, prevY, prevZ;
    selectedLabel->getLabelPos(prevX, prevY, prevZ);
	oy = height - oy - 1;
	ny = height - ny - 1;
//	get2dCoords(ox, oy, prevZ);
//	get2dCoords(nx, ny, prevZ);
	double diffX = (nx - ox) / width * 2000;
	double diffY = (ny - oy) / height * 2000;
	double z = 0;
    selectedLabel->setLabelPos(prevX + diffX, prevY + diffY, prevZ + z);
  }
}

void LTSGraph::lockObject()
{
  if(selectedState != NULL)
  {
    selectedState->lock();
  }
}

void LTSGraph::dragObject()
{
  if(selectedState != NULL)
  {
    selectedState->drag();
  }
}

void LTSGraph::stopDrag()
{
  if(selectedState != NULL)
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
  if(selectedState != NULL)
  {
    selectedState->deselect();
  }

  if(selectedTransition != NULL)
  {
    selectedTransition->deselect();
  }

  if(selectedLabel != NULL) {
    selectedLabel->deselect();
  }

  selectedState = NULL;
  selectedTransition = NULL;
  selectedLabel = NULL;
  display();
}

void LTSGraph::uncolourState(size_t selectedObject) 
{
  if(colouring) 
  {
    graph->colourState(selectedObject);
  }
}

void LTSGraph::colourState(size_t selectedObject) 
{
  if(colouring) 
  {
    graph->colourState(selectedObject, brushColour);
  }
}

void LTSGraph::selectState(size_t selectedObject)
{
  if(!colouring) 
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

void LTSGraph::getCanvasRots(double & _X, double & _Y, double & _Z)
{
  glCanvas->getRotations(_X, _Y, _Z);
}

void LTSGraph::get2dCoords(double &mx, double &my, double oz)
{
  double wx, wy, wz;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
  double rad = glCanvas->getMaxDepth();
  double depthR = (oz + 1000)/2000;
  gluUnProject(mx, my, depthR, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
  mx = wx;
  my = wy;
}