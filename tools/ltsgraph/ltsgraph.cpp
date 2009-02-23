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

#define NAME "ltsgraph"
#define AUTHOR "Carst Tankink"

#include "ltsgraph.h"
#include "ltsimporter.h"
#include "xmlimporter.h"

#include "springlayout.h"

#include "mcrl2/lts/lts.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"

std::string lts_file_argument;

#ifdef ENABLE_SQUADT_CONNECTIVITY
// On OS X, a macro called `check' is imported with one of the include directives above
// which causes compilation failures in Boost header files.
#undef check

// SQuADT protocol interface
# include <mcrl2/utilities/mcrl2_squadt_interface.h>
using namespace mcrl2::utilities::squadt;
using namespace mcrl2::lts;
const char* lts_file_for_input  = "lts_in";

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_wx_tool_interface {

  public:
    // Configures tool capabilities.
    void set_capabilities(tipi::tool::capabilities& c) const {
      std::set< mcrl2::lts::lts_type > const& input_formats(mcrl2::lts::lts::supported_lts_formats());

      for (std::set< mcrl2::lts::lts_type >::const_iterator i = input_formats.begin(); i != input_formats.end(); ++i) {
        c.add_input_configuration(lts_file_for_input, tipi::mime_type(mcrl2::lts::lts::mime_type_for_type(*i)), tipi::tool::category::visualisation);
      }
    }

    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(tipi::configuration&) { }

    // Check an existing configuration object to see if it is usable
    bool check_configuration(tipi::configuration const& c) const {
      if (c.input_exists(lts_file_for_input)) {
        /* The input object is present, verify whether the specified format is supported */
        if (lts::parse_format(c.get_input(lts_file_for_input).type().sub_type().c_str()) == lts_none) {
          send_error("Invalid configuration: unsupported type `" +
              c.get_input(lts_file_for_input).type().sub_type() + "' for main input");
        }
        else {
          return true;
        }
      }

      return false;
    }

    bool perform_task(tipi::configuration& c) {
      lts_file_argument = c.get_input(lts_file_for_input).location();

      return mcrl2_wx_tool_interface::perform_task(c);
    }
};
#endif

bool LTSGraph::parse_command_line(int argc, wxChar** argv)
{
  using namespace ::mcrl2::utilities;

  interface_description clinterface(std::string(wxString(argv[0], wxConvLocal).fn_str()),
    NAME, AUTHOR,
    "visualise an LTS as a graph and manipulate its layout",
    "[OPTION]... [INFILE]\n",
    "Draw graphs and optimize their layout in a graphical environment. "
    "If INFILE is supplied, the tool will use this file as input for drawing.");

  command_line_parser parser(clinterface, argc, argv);

  if (parser.continue_execution()) {
    if (0 < parser.arguments.size()) {
      lts_file_argument = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
  }

  return parser.continue_execution();
}

LTSGraph::LTSGraph() : mcrl2::utilities::wx::tool< LTSGraph >("LTSGraph",
    "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout.",
    std::vector< std::string >(1, "Carst Tankink")) {
}

bool LTSGraph::DoInit()
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
  if (!lts_file_argument.empty())
  {
    openFile(lts_file_argument);
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

  MCRL2_ATERM_INIT(0, lpCmdLine)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  using namespace mcrl2::utilities::squadt;

  if(!interactor< squadt_interactor >::free_activation(hInstance, hPrevInstance, lpCmdLine, nCmdShow)) {
#endif
      return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#ifdef ENABLE_SQUADT_CONNECTIVITY
    }

    return 0;
#endif
}
#else
int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

# ifdef ENABLE_SQUADT_CONNECTIVITY
  using namespace mcrl2::utilities::squadt;

  if(!interactor< squadt_interactor >::free_activation(argc, argv)) {
# endif
    return wxEntry(argc, argv);
# ifdef ENABLE_SQUADT_CONNECTIVITY
  }

  return 0;
# endif
}
#endif

void LTSGraph::openFile(std::string const &path)
{
  // Set fileName
  fileName = path;
  // Find out file format based on extension
  std::string ext = path.substr(path.find_last_of( '.' ) + 1);

  // Get file's basename

  Importer* imp;
  // Create (on stack) appropriate importer imp
  if ( ext == "xml")
  {
    // path points to an XML layout file, so create an XML importer
    imp = new XMLImporter();
  }
  else
  {
    // Assume we have an LTS file, so create an LTS importer
    imp = new LTSImporter();
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

void LTSGraph::moveObject(double x, double y)
{
  if(selectedState != NULL)
  {
    selectedState->setX(selectedState->getX() + x);
    selectedState->setY(selectedState->getY() + y);
  }

  if(selectedTransition != NULL)
  {
    double prevX, prevY;
    selectedTransition->getControl(prevX, prevY);

    selectedTransition->setControl(prevX + x, prevY + y);
  }
  if(selectedLabel != NULL)
  {
    double prevX, prevY;
    selectedLabel->getLabelPos(prevX, prevY);

    selectedLabel->setLabelPos(prevX + x, prevY + y);
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

void LTSGraph::uncolourState(size_t selectedObject) {
  if(colouring) {
    graph->colourState(selectedObject);
  }
}

void LTSGraph::colourState(size_t selectedObject) {
  if(colouring) {
    graph->colourState(selectedObject, brushColour);
  }
}

void LTSGraph::selectState(size_t selectedObject)
{
  if(!colouring) {
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

void LTSGraph::selectSelfLabel(size_t state, size_t transition) {
  selectedLabel = graph->selectSelfLoop(state, transition);
}

void LTSGraph::setRadius(int radius)
{
  visualizer->setRadius(radius);
  display();
}

int LTSGraph::getRadius() const {
  return visualizer->getRadius();
}

void LTSGraph::setTransLabels(bool value) {
  visualizer->setTransLabels(value);
  display();
}

void LTSGraph::setStateLabels(bool value) {
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


void LTSGraph::setBrushColour(wxColour colour) {
  brushColour = colour;
}

void LTSGraph::setTool(bool isColour) {
  colouring = isColour;
}
