// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsviewapp.cpp
/// \brief Add your file description here.

#include <string>

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
  c.add_input_combination(fsm_file_for_input,tipi::mime_type("fsm",
				tipi::mime_type::text),tipi::tool::category::visualisation);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  //skip
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  if (c.input_exists(fsm_file_for_input)) {

    tipi::object input_object(c.get_input(fsm_file_for_input));
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
#include "ltsviewapp.h"
#include "markstateruledialog.h"
#include "fileloader.h"
#include "settings.h"
#include "aterm1.h"

using namespace std;
using namespace Utils;
IMPLEMENT_APP_NO_MAIN(LTSViewApp)

BEGIN_EVENT_TABLE(LTSViewApp, wxApp)
  EVT_CHAR(MainFrame::onKeyDown)
END_EVENT_TABLE()

bool LTSViewApp::OnInit() {
  lts = NULL;
  markStyle = NO_MARKS;
  settings = new Settings();
  rankStyle = ITERATIVE;
  mainFrame = new MainFrame(this,settings);
  visualizer = new Visualizer(this,settings);
  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);
  
  SetTopWindow(mainFrame);
  mainFrame->Show(true);
  glCanvas->initialize();
  mainFrame->Layout();

  wxInitAllImageHandlers();

  wxCmdLineParser cmdParser(argc,argv);

  cmdParser.AddParam(wxT("INFILE"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);

  if (cmdParser.Parse() == 0) {
    if (cmdParser.GetParamCount() > 0) {
      lts_file_argument = std::string(cmdParser.GetParam(0).fn_str());
    }
  }

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

  using namespace mcrl2::utilities::squadt;

  ATerm bot;

  ATinit(0,0,&bot); // XXX args?

#ifdef ENABLE_SQUADT_CONNECTIVITY
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
  using namespace mcrl2::utilities::squadt;

  ATerm bot;

  /* Initialise aterm library */
  ATinit(argc,argv,&bot);

#ifdef ENABLE_SQUADT_CONNECTIVITY
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
  if (lts!=NULL) delete lts;
  delete settings;
  delete visualizer;
  return 0;
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
  if (lts!=NULL) delete lts;
  lts = newlts;
  // first remove all unreachable states
  lts->trim();
  
  mainFrame->updateProgressDialog(17,"Applying ranking");
  lts->applyRanking(rankStyle);
  
  mainFrame->updateProgressDialog(33,"Merging superiors");
  lts->mergeSuperiorClusters(rankStyle);

  mainFrame->updateProgressDialog(50,"Setting mark info");
  lts->computeClusterLabelInfo();
  
  mainFrame->updateProgressDialog(67,"Positioning clusters");
  lts->positionClusters();

  visualizer->setLTS(lts,true);
  
  mainFrame->updateProgressDialog(83,"Positioning states");
  lts->positionStates();
  
  mainFrame->updateProgressDialog(100,"Done");
  visualizer->setMarkStyle(NO_MARKS);

  glCanvas->enableDisplay();
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo(lts->getNumStates(),
      lts->getNumTransitions(),lts->getNumClusters(),
      lts->getNumRanks());
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

      mainFrame->updateProgressDialog(0,"Applying ranking");
      lts->applyRanking(rankStyle);
      
      mainFrame->updateProgressDialog(20,"Merging superiors");
      lts->mergeSuperiorClusters(rankStyle);

      mainFrame->updateProgressDialog(40,"Setting mark info");
      lts->computeClusterLabelInfo();
      lts->markClusters();
      
      mainFrame->updateProgressDialog(60,"Positioning clusters");
      lts->positionClusters();

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

void LTSViewApp::addMarkRule() {
  if (lts != NULL) {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog(mainFrame,this,
				lts);
    msrDialog->CentreOnParent();
    if (msrDialog->ShowModal() == wxID_OK) {
      MarkRule* markrule = msrDialog->getMarkRule();
      if (markrule != NULL) {
				lts->addMarkRule(markrule);
				mainFrame->addMarkRule(msrDialog->getMarkRuleString());
				applyMarkStyle(MARK_STATES);
      }
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

void LTSViewApp::removeMarkRule(const int index) {
  lts->removeMarkRule(index);
  applyMarkStyle(MARK_STATES);
}

void LTSViewApp::editMarkRule(const int index) {
  if (lts != NULL) {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog(mainFrame,this,
				lts);
		bool oldActivated = lts->getMarkRule(index)->isActivated;
    msrDialog->setMarkRule(lts->getMarkRule(index));
    msrDialog->CentreOnParent();
    if (msrDialog->ShowModal() == wxID_OK) {
      MarkRule* markrule = msrDialog->getMarkRule();
      if (markrule != NULL) {
				markrule->isActivated = oldActivated;
				lts->replaceMarkRule(index,markrule);
				mainFrame->replaceMarkRule(index,msrDialog->getMarkRuleString());
				applyMarkStyle(MARK_STATES);
      }
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

void LTSViewApp::activateMarkRule( const int index, const bool activate ) {
  if (lts != NULL) {
    lts->activateMarkRule(index,activate);
    applyMarkStyle(MARK_STATES);
  }
}

void LTSViewApp::setMatchAnyMarkRule(bool b) {
  if (lts != NULL) {
    lts->setMatchAnyMarkRule(b);
    applyMarkStyle(MARK_STATES);
  }
}

void LTSViewApp::markAction(string label) {
  lts->markAction(label);
  applyMarkStyle(MARK_TRANSITIONS);
}

void LTSViewApp::unmarkAction(string label) {
  lts->unmarkAction(label);
  applyMarkStyle(MARK_TRANSITIONS);
}

void LTSViewApp::applyMarkStyle(MarkStyle ms) {

  if (lts == NULL) {
    return;
  }

  markStyle = ms;
  switch (markStyle) {
    case MARK_DEADLOCKS:
      mainFrame->setMarkedStatesInfo(lts->getNumDeadlocks());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_STATES:
      mainFrame->setMarkedStatesInfo(lts->getNumMarkedStates());
      mainFrame->setMarkedTransitionsInfo(0);
      break;
    case MARK_TRANSITIONS:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(lts->getNumMarkedTransitions());
      break;
    case NO_MARKS:
    default:
      mainFrame->setMarkedStatesInfo(0);
      mainFrame->setMarkedTransitionsInfo(0);
      break;
  }
  visualizer->setMarkStyle(markStyle);
  glCanvas->display();
}

void LTSViewApp::notifyRenderingStarted() {
  mainFrame->startRendering();
}

void LTSViewApp::notifyRenderingFinished() {
  mainFrame->stopRendering();
}

void LTSViewApp::startSim() {
  lts->getSimulation()->start();
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
  if (lts != NULL)
  {
    State* s = lts->selectStateByID(id);
    lts->getSimulation()->setInitialState(s);

    for(int i = 0; i < lts->getNumParameters(); ++i)
    {
      string paramName = lts->getParameterName(i);
      string paramValue = lts->getParameterValue(i, s->getParameterValue(i));
      mainFrame->setParameterValue(paramName, paramValue);
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
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
      lts->getNumTransitions(),lts->getNumClusters(),
      lts->getNumRanks());
  applyMarkStyle(markStyle);
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSViewApp::zoomInAbove()
{
  LTS* newLTS = lts->zoomIntoAbove();
  deselect();
  lts = newLTS;
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
     lts->getNumTransitions(),lts->getNumClusters(),
     lts->getNumRanks()); 
  applyMarkStyle(markStyle);
  glCanvas->setSim(lts->getSimulation());
  mainFrame->setSim(lts->getSimulation());
}

void LTSViewApp::zoomOut()
{
  LTS* oldLTS = lts;
  lts = oldLTS->zoomOut();
  oldLTS->deselect();
  visualizer->setLTS(lts,false);
  mainFrame->setNumberInfo(lts->getNumStates(),
    lts->getNumTransitions(),lts->getNumClusters(),
    lts->getNumRanks()); 
  applyMarkStyle(markStyle);
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

