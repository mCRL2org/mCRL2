#include "wx.hpp" // precompiled headers

#define NAME "ltsgraph"
#define AUTHOR "Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp"

#include "gltsgraph.h"
#include "ltsimporter.h"
#include "xmlimporter.h"

#include "springlayout.h"

#include "mcrl2/lts/lts.h"
#include "mcrl2/core/aterm_ext.h"

#ifndef __MCRL2_UTILITIES_HPP__
#include "mcrl2/utilities/command_line_interface.h"
#endif

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
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::visualisation);
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("mcrl2-lts", tipi::mime_type::application), tipi::tool::category::visualisation);
#ifdef MCRL2_BCG
      c.add_input_configuration(lts_file_for_input,
                 tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::visualisation);
#endif
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

void parse_command_line(int argc, wxChar** argv)
{
  using namespace ::mcrl2::utilities;

  interface_description clinterface(std::string(wxString(argv[0], wxConvLocal).fn_str()),
        NAME, AUTHOR, "[OPTION]... [INFILE]\n",
    "Draw graphs and optimize their layout in a graphical environment. "
    "If INFILE is supplied, the tool will use this file as input for drawing.");

  command_line_parser parser(clinterface, argc, argv);

  if (0 < parser.arguments.size()) {
    lts_file_argument = parser.arguments[0];
  }
  if (1 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
}


bool GLTSGraph::OnInit()
{
  colouring = false;
  brushColour = *wxBLACK;
  selectedState = NULL;
  selectedTransition = NULL;
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

bool GLTSGraph::Initialize(int& argc, wxChar** argv) 
{
  try {
    parse_command_line(argc, argv);
  }
  catch (std::exception &e)
  {
    if(wxApp::Initialize(argc, argv)) 
    {
      parse_error = std::string(e.what()).
        append("\n\nNote that other command line options may have been ignored because of this error.");
    }
    else
    {
      std::cerr << e.what() <<std::endl;

      return false;
    }

    return true;
  }

  return wxApp::Initialize(argc, argv);
}


int GLTSGraph::OnExit() {
 
  return (wxApp::OnExit());
}

void GLTSGraph::printHelp(std::string const &name) {
  std::cout << "Usage: " << name << " [INFILE]" << std::endl 
       << "Draw graphs and optimize their layout in a graphical environment."
       << "If INFILE is supplied," 
       << "the tool will use this file" << std::endl
       << "as input for drawing." << std::endl << std::endl
       << "Use left click to drag the nodes and right click to fix the nodes."
       << std::endl
       << std::endl 
       << "Options:" << std::endl 
       << "  -h, --help            display this help message and terminate"
       << std::endl 

       << "      --version         displays version information and terminate"
       << std::endl
       << std::endl 
       << "Report bugs at <http://www.mcrl2.org/issuetracker>."
       << std::endl;
}

IMPLEMENT_APP_NO_MAIN(GLTSGraph)
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow) {

  int local_var;
  MCRL2_ATERM_INIT(local_var, lpCmdLine)

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

void GLTSGraph::openFile(std::string const &path)
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

Graph* GLTSGraph::getGraph()
{
  return graph;
}

std::string GLTSGraph::getVersion() {
  return mcrl2::utilities::version_tag;
}

std::string GLTSGraph::getRevision() {
  return std::string(MCRL2_REVISION);
}

size_t GLTSGraph::getNumberOfAlgorithms() const
{
  return algorithms.size();
}

LayoutAlgorithm* GLTSGraph::getAlgorithm(size_t i) const
{
  return algorithms[i];
}

void GLTSGraph::display()
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

void GLTSGraph::moveObject(double x, double y)
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

void GLTSGraph::lockObject()
{
  if(selectedState != NULL)
  {
    selectedState->lock();
  }
}

void GLTSGraph::dragObject()
{
  if(selectedState != NULL)
  {
    selectedState->drag();
  }
}

void GLTSGraph::stopDrag()
{
  if(selectedState != NULL)
  {
    selectedState->stopDrag();
  }
}

size_t GLTSGraph::getNumberOfObjects()
{
  // TODO: needs to be adapted when new types of selectable objects are added
  return graph->getNumberOfStates();
}

void GLTSGraph::deselect()
{
  if(selectedState != NULL)
  {
    selectedState->deselect();
  }

  if(selectedTransition != NULL)
  {
    selectedTransition->deselect();
  }
  
  selectedState = NULL;
  selectedTransition = NULL;
}

void GLTSGraph::selectState(size_t selectedObject)
{
  if(colouring) {
    graph->colourState(selectedObject, brushColour);
  }
  else {
    selectedState = graph->selectState(selectedObject);
  }


}

void GLTSGraph::selectTransition(size_t state, size_t transition)
{
  selectedTransition = graph->selectTransition(state, transition);
}

void GLTSGraph::selectSelfLoop(size_t state, size_t transition)
{
  selectedTransition = graph->selectSelfLoop(state, transition);
}

void GLTSGraph::selectLabel(size_t state, size_t transition) 
{
  selectedLabel = graph->selectTransition(state, transition);
}

void GLTSGraph::setRadius(int radius)
{
  visualizer->setRadius(radius);
  display();
}

int GLTSGraph::getRadius() const {
  return visualizer->getRadius();
}

void GLTSGraph::setCurves(bool value)
{
  visualizer->setCurves(value);
  display();
}

std::string GLTSGraph::getFileName() const
{
  return fileName;
}

double GLTSGraph::getAspectRatio() const
{
  return glCanvas->getAspectRatio();
}


void GLTSGraph::setBrushColour(wxColour colour) {
  brushColour = colour;
}

void GLTSGraph::setTool(bool isColour) {
  colouring = isColour;
}
