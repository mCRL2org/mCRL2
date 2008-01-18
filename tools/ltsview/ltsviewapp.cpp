// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsviewapp.cpp
/// \brief Add your file description here.

#include <string>

#define NAME "ltsview"
#define AUTHOR "Bas Ploeger and Carst Tankink"

std::string lts_file_argument;

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <wx/wx.h>
//SQuADT protocol interface
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor: public mcrl2::utilities::squadt::mcrl2_tool_interface {
  private:
    static const char*  fsm_file_for_input;  ///< file containing an LTS that can be imported
    // Wrapper for wxEntry invocation
    mcrl2::utilities::squadt::entry_wrapper& starter;

  public:
    // Constructor
    squadt_interactor(mcrl2::utilities::squadt::entry_wrapper&);
    // Configures tool capabilities.
    void set_capabilities(tipi::tool::capabilities&) const;
    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(tipi::configuration&);
    // Check an existing configuration object to see if it is usable
    bool check_configuration(tipi::configuration const&) const;
    // Performs the task specified by a configuration
    bool perform_task(tipi::configuration&);
};

const char* squadt_interactor::fsm_file_for_input = "fsm_in";

squadt_interactor::squadt_interactor(mcrl2::utilities::squadt::entry_wrapper& w): starter(w) {
  // skip 
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(fsm_file_for_input,tipi::mime_type("fsm",
        tipi::mime_type::text),tipi::tool::category::visualisation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  //skip
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  if (c.input_exists(fsm_file_for_input)) {

    tipi::configuration::object input_object(c.get_input(fsm_file_for_input));
    lts_file_argument = input_object.get_location();

    return true;
  }

  return false;
}

bool squadt_interactor::perform_task(tipi::configuration&) {
  return starter.perform_entry();
}
#endif

#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <mcrl2/utilities/aterm_ext.h>
#include "mcrl2/utilities/version_info.h"
#include "ltsviewapp.h"
#include "markstateruledialog.h"
#include "fileloader.h"
#include "settings.h"
#include "aterm1.h"

using namespace std;
using namespace Utils;
IMPLEMENT_APP_NO_MAIN(LTSViewApp)

BEGIN_EVENT_TABLE(LTSViewApp, wxApp)
END_EVENT_TABLE()

void LTSViewApp::printHelp(std::string const &name) {
  cout <<         "Usage: " << name << "[OPTION]... [INFILE]"
       << endl << "Start the LTSView application and open INFILE. If INFILE is not"
       << endl << "supplied then LTSView is started without opening an LTS."
       << endl
       << endl << "INFILE should be in the FSM format."
       << endl
       << endl << "Options:"
       << endl << "  -h, --help     display this help message and terminate"
       << endl << "      --version  display version information and terminate"
       << endl
       << endl << "Report bugs at <http://www.mcrl2.org/issuetracker>."
       << endl;
}

void LTSViewApp::printVersion() {
  cout << getVersionString();
}

bool LTSViewApp::OnInit() {
  wxCmdLineParser cmdParser(argc,argv);
  cmdParser.AddSwitch(wxT("h"),wxT("help"),
      wxT("display this help message and terminate"),wxCMD_LINE_OPTION_HELP);
  cmdParser.AddSwitch(wxEmptyString,wxT("version"),
      wxT("display version information and terminate"));
  cmdParser.AddParam(wxT("INFILE"),wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL);

  if (cmdParser.Parse(false) > 0) {
    return false;
  }
  if (cmdParser.Found(wxT("h"))) {
    printHelp(argv[0]);
    return false;
  }
  if (cmdParser.Found(wxT("version"))) {
    printVersion();
    return false;
  }
  if (cmdParser.GetParamCount() > 1) {
    cerr << "Too many command line parameters specified" << endl;
  }
  if (cmdParser.GetParamCount() > 0) {
    lts_file_argument = std::string(cmdParser.GetParam(0).fn_str());
  }

  lts = NULL;
  rankStyle = ITERATIVE;
  fsmStyle = false;
  colourCounter = 0;
  settings = new Settings();
  mainFrame = new MainFrame(this,settings);
  visualizer = new Visualizer(this,settings);
  markManager = new MarkManager();
  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);
  
  SetTopWindow(mainFrame);
  mainFrame->Show(true);
  glCanvas->initialize();
  mainFrame->Layout();

  wxInitAllImageHandlers();

  if (!lts_file_argument.empty()) {
    wxFileName fileName(wxString(lts_file_argument.c_str(), wxConvLocal));
    fileName.Normalize();
    mainFrame->setFileInfo(fileName);
    openFile(static_cast< string >(fileName.GetFullPath().fn_str()));
  }
  return true;
}

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

  return (0);
#endif
}
#else
int main(int argc, char **argv) {

  MCRL2_ATERM_INIT(argc, argv)

#ifdef ENABLE_SQUADT_CONNECTIVITY
  using namespace mcrl2::utilities::squadt;

  if(!interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    return wxEntry(argc, argv);
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }

  return 0;
#endif
}
#endif

int LTSViewApp::OnExit() {
  if (lts != NULL) delete lts;
  delete settings;
  delete visualizer;
  return 0;
}

std::string LTSViewApp::getVersionString() {
  return get_version_information(NAME, AUTHOR);
}

void LTSViewApp::openFile(string fileName) {
  glCanvas->disableDisplay();

  mainFrame->createProgressDialog("Opening file","Parsing file");
  mainFrame->updateProgressDialog(0,"Parsing file");
  LTS* newlts = new LTS(this);
  try {
    FileLoader::loadFile(fileName,newlts);
  }
  catch (string msg) {
    delete newlts;
    mainFrame->updateProgressDialog(100,"Error loading file");
    mainFrame->showMessage("Error loading file",msg);
    return;
  }
  if (lts != NULL) delete lts;
  lts = newlts;
  // first remove all unreachable states
  lts->trim();
  
  mainFrame->updateProgressDialog(17,"Ranking states");
  lts->rankStates(rankStyle);
  
  mainFrame->updateProgressDialog(33,"Clustering states");
  lts->clusterStates(rankStyle);

  mainFrame->updateProgressDialog(50,"Setting cluster info");
  lts->computeClusterInfo();
  
  mainFrame->updateProgressDialog(67,"Positioning clusters");
  lts->positionClusters(fsmStyle);

  markManager->setLTS(lts,true);
  visualizer->setLTS(lts,true);
  
  mainFrame->updateProgressDialog(83,"Positioning states");
  lts->positionStates();
  
  mainFrame->updateProgressDialog(100,"Done");
  visualizer->notifyMarkStyleChanged();

  glCanvas->enableDisplay();
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo(lts->getNumStates(),
      lts->getNumTransitions(),lts->getNumClusters(),
      lts->getNumRanks());
  mainFrame->resetParameters();
  for (int i = 0; i < lts->getNumParameters(); ++i) {
    mainFrame->addParameter(i,lts->getParameterName(i));
  }
  mainFrame->resetMarkRules();
  
  vector< string > ls;
  lts->getActionLabels(ls);

  mainFrame->setSim(lts->getSimulation());
  mainFrame->setActionLabels(ls);

  glCanvas->setSim(lts->getSimulation());

  mainFrame->setMarkedStatesInfo(0);
  mainFrame->setMarkedTransitionsInfo(0);
}

void LTSViewApp::setRankStyle(RankStyle rs) {
  if (rankStyle != rs) {
    rankStyle = rs;
    if (lts != NULL) {
      glCanvas->disableDisplay();

      mainFrame->createProgressDialog("Structuring LTS","Applying ranking");

      mainFrame->updateProgressDialog(17,"Ranking states");
      lts->rankStates(rankStyle);
      
      mainFrame->updateProgressDialog(33,"Clustering states");
      lts->clusterStates(rankStyle);

      mainFrame->updateProgressDialog(40,"Setting cluster info");
      lts->computeClusterInfo();
      markManager->markClusters();
      
      mainFrame->updateProgressDialog(60,"Positioning clusters");
      lts->positionClusters(fsmStyle);

      visualizer->setLTS(lts,true);

      mainFrame->updateProgressDialog(80,"Positioning states");
      lts->positionStates();

      mainFrame->updateProgressDialog(100,"Done");

      glCanvas->enableDisplay();
      glCanvas->resetView();

      mainFrame->setNumberInfo(lts->getNumStates(),
        lts->getNumTransitions(),lts->getNumClusters(),
        lts->getNumRanks());
    }
  }
}

void LTSViewApp::setVisStyle(VisStyle vs) {
  if (visualizer->getVisStyle() != vs) {
    visualizer->setVisStyle(vs);
    glCanvas->display();
  }
}

void LTSViewApp::setFSMStyle(bool b) {
  if (b != fsmStyle) {
    fsmStyle = b;
    if (lts != NULL) {
      glCanvas->disableDisplay();
      lts->positionClusters(fsmStyle);
      visualizer->setLTS(lts,true);
      glCanvas->enableDisplay();
      glCanvas->resetView();
    }
  }
}

void LTSViewApp::addMarkRule() {
  if (lts != NULL) {
    MarkStateRuleDialog* msrdlg = new MarkStateRuleDialog(mainFrame,this,lts);
    msrdlg->CentreOnParent();
    if (msrdlg->ShowModal() == wxID_OK) {
      if (msrdlg->getParamIndex() != -1) {
        vector<bool> vals;
        msrdlg->getValues(vals);
        int mr = markManager->createMarkRule(msrdlg->getParamIndex(),
            msrdlg->getNegated(),msrdlg->getColor(),vals);
        mainFrame->addMarkRule(msrdlg->getMarkRuleString(),mr);

        if (markManager->getMarkStyle() != MARK_STATES) {
          setMarkStyle(MARK_STATES);
        } else {
          applyMarkStyle();
        }
      }
    }
    msrdlg->Close();
    msrdlg->Destroy();
  }
}

void LTSViewApp::removeMarkRule(int mr) {
  markManager->removeMarkRule(mr);
  if (markManager->getMarkStyle() != MARK_STATES) {
    setMarkStyle(MARK_STATES);
  } else {
    applyMarkStyle();
  }
}

void LTSViewApp::editMarkRule(int mr) {
  if (lts != NULL) {
    MarkStateRuleDialog* msrdlg = new MarkStateRuleDialog(mainFrame,this,lts);
    vector< bool > vals;
    markManager->getMarkRuleValues(mr,vals);
    msrdlg->setData(markManager->getMarkRuleParam(mr),
        markManager->getMarkRuleColor(mr),markManager->getMarkRuleNegated(mr),
        vals);
    msrdlg->CentreOnParent();
    if (msrdlg->ShowModal() == wxID_OK) {
      if (msrdlg->getParamIndex() != -1) {
        msrdlg->getValues(vals);
        markManager->setMarkRuleData(mr,msrdlg->getParamIndex(),
            msrdlg->getNegated(),msrdlg->getColor(),vals);
        mainFrame->replaceMarkRule(msrdlg->getMarkRuleString(),mr);
        if (markManager->getMarkStyle() != MARK_STATES) {
          setMarkStyle(MARK_STATES);
        } else {
          applyMarkStyle();
        }
      }
    }
    msrdlg->Close();
    msrdlg->Destroy();
  }
}

Utils::MarkStyle LTSViewApp::getMarkStyle() {
  return markManager->getMarkStyle();
}

Utils::MatchStyle LTSViewApp::getMatchStyle() {
  return markManager->getMatchStyle();
}

bool LTSViewApp::isMarked(State* s) {
  return markManager->isMarked(s);
}

bool LTSViewApp::isMarked(Cluster* c) {
  return markManager->isMarked(c);
}

bool LTSViewApp::isMarked(Transition* t) {
  return markManager->isMarked(t);
}

Utils::RGB_Color LTSViewApp::getMarkRuleColor(int mr) {
  return markManager->getMarkRuleColor(mr);
}

Utils::RGB_Color LTSViewApp::getNewRuleColour()
{
  // TODO implement
  colourCounter = (colourCounter + 1) % 9;

  RGB_Color result;

  switch(colourCounter)
  {
    case 0: 
      { result.r = 228;
        result.g = 26;
        result.b = 28;
        break;
      }
    case 1:
      { result.r = 55;
        result.g = 126;
        result.b = 184;
        break;
      }
    case 2:
      { result.r = 77;
        result.g = 175;
        result.b = 74;
        break;
      }
    case 3:
      { result.r = 152;
        result.g = 78;
        result.b = 163;
        break;
      }
    case 4:
      { result.r = 255;
        result.g = 127;
        result.b = 0;
        break;
      }
    case 5:
      { result.r = 255;
        result.g = 255;
        result.b = 51;
        break;
      }
    case 6:
      { result.r = 166;
        result.g = 86;
        result.b = 40;
        break;
      }
    case 7:
     { result.r = 247;
       result.g = 129;
       result.b = 191;
       break;
     }
    case 8:
     { result.r = 153;
       result.g = 153;
       result.b = 153;
       break;
     }
  }
  return result;
}

void LTSViewApp::activateMarkRule(int mr,bool activate) {
  if (lts != NULL) {
    markManager->setMarkRuleActivated(mr,activate);
    if (markManager->getMarkStyle() != MARK_STATES) {
      setMarkStyle(MARK_STATES);
    } else {
      applyMarkStyle();
    }
  }
}

void LTSViewApp::setMatchStyle(Utils::MatchStyle ms) {
  if (lts == NULL) return;
  markManager->setMatchStyle(ms);
  if (markManager->getMarkStyle() != MARK_STATES) {
    setMarkStyle(MARK_STATES);
  } else {
    applyMarkStyle();
  }
}

void LTSViewApp::setActionMark(string label,bool b) {
  markManager->setActionMark(label,b);
  setMarkStyle(MARK_TRANSITIONS);
}

void LTSViewApp::setMarkStyle(Utils::MarkStyle ms) {
  markManager->setMarkStyle(ms);
  applyMarkStyle();
}

void LTSViewApp::applyMarkStyle() {
  if (lts == NULL) return;

  switch (markManager->getMarkStyle()) {
    case MARK_DEADLOCKS:
      mainFrame->setMarkedStatesInfo(lts->getNumDeadlocks());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_STATES:
      mainFrame->setMarkedStatesInfo(markManager->getNumMarkedStates());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_TRANSITIONS:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(
          markManager->getNumMarkedTransitions());
      break;
    case NO_MARKS:
    default:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(0);
      break;
  }
  visualizer->notifyMarkStyleChanged();
  glCanvas->display();
}

void LTSViewApp::notifyRenderingStarted() {
  mainFrame->startRendering();
}

void LTSViewApp::notifyRenderingFinished() {
  mainFrame->stopRendering();
}

void LTSViewApp::startSim() {
  Simulation* sim = lts->getSimulation();

  sim->start();
  
  // Select/deselect initial state of simulation, to initialize state info pane.
  selectStateByID(sim->getCurrState()->getID());
}

int LTSViewApp::getNumberOfParams() const {
  if (lts != NULL)
  {
    return lts->getNumParameters();
  }
  else
  {
    return 0;
  }
}
string LTSViewApp::getActionLabel(const int i) const {
  if (lts != NULL)
  {
    return lts->getLabel(i);
  }
  else 
  {
    return "";
  }
}

string LTSViewApp::getParName(const int i) const {
  if (lts != NULL)
  {
    return lts->getParameterName(i);
  }
  else 
  {
    return "";
  }
}

string LTSViewApp::getParValue(const int i, const int j) const {
  if (lts != NULL)
  {
    return lts->getParameterValue(i, j);
  }
  else
  {
    return "";
  }
}


void LTSViewApp::selectStateByID(const int id) {
  if (lts != NULL) {
    State* s = lts->selectStateByID(id);
    lts->getSimulation()->setInitialState(s);
    for (int i = 0; i < lts->getNumParameters(); ++i) {
      mainFrame->setParameterValue(i,lts->getParameterValue(i,
            s->getParameterValue(i)));
    }
  }
}

void LTSViewApp::selectCluster(const int rank, const int pos) 
{
  if (lts != NULL) 
  {
    lts->selectCluster(rank, pos);
  }
}

void LTSViewApp::deselect() {
  if (lts != NULL)
  {
    lts->deselect();
  }
}

int LTSViewApp::getNumberOfObjects() {
  int result = 0;

  if (lts != NULL) {
    result += lts->getNumClusters();
    result += lts->getNumStates();
  }

  return result;
}

void LTSViewApp::zoomInBelow()
{
  LTS* newLTS = lts->zoomIntoBelow();
  deselect();
  lts = newLTS;
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
      lts->getNumTransitions(),lts->getNumClusters(),
      lts->getNumRanks());
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSViewApp::zoomInAbove()
{
  LTS* newLTS = lts->zoomIntoAbove();
  deselect();
  lts = newLTS;
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
     lts->getNumTransitions(),lts->getNumClusters(),
     lts->getNumRanks()); 
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSViewApp::zoomOutTillTop()
{
  LTS* oldLTS = lts;
  do
  {
    zoomOut();
    oldLTS = lts;
  }  while (lts != oldLTS);
}

void LTSViewApp::zoomOut()
{
  LTS* oldLTS = lts;
  lts = oldLTS->zoomOut();
  oldLTS->deselect();
  markManager->setLTS(lts,false);
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
    lts->getNumTransitions(),lts->getNumClusters(),
    lts->getNumRanks()); 
  applyMarkStyle();
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());

  if (oldLTS != lts) 
  {
    delete oldLTS;
  }
}

void LTSViewApp::loadTrace(std::string const& path)
{
  lts->loadTrace(path);
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSViewApp::reportError(std::string const& error) 
{
  mainFrame->reportError(error);
}


void LTSViewApp::generateBackTrace()
{
  lts->generateBackTrace();
}

