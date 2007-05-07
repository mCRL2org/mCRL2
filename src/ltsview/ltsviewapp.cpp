
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <wx/wx.h>
//SQuADT protocol interface
#include <utilities/mcrl2_squadt.h>

bool command_line = false;
std::string lts_file_argument;

class squadt_interactor: public mcrl2_squadt::tool_interface {
  private:
    static const char*  fsm_file_for_input;  ///< file containing an LTS that can be imported
    // Wrapper for wxEntry invocation
    squadt_utility::entry_wrapper& starter;

  public:
    // Constructor
    squadt_interactor(squadt_utility::entry_wrapper&);
    // Configures tool capabilities.
    void set_capabilities(sip::tool::capabilities&) const;
    // Queries the user via SQuADt if needed to obtain configuration information
    void user_interactive_configuration(sip::configuration&);
    // Check an existing configuration object to see if it is usable
    bool check_configuration(sip::configuration const&) const;
    // Performs the task specified by a configuration
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::fsm_file_for_input = "fsm_in";

squadt_interactor::squadt_interactor(squadt_utility::entry_wrapper& w): starter(w) {
  // skip 
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(fsm_file_for_input,sip::mime_type("fsm",
				sip::mime_type::text),sip::tool::category::visualisation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  //skip
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  if (c.input_exists(fsm_file_for_input)) {
		/* The input object is present, verify whether the specified format is
		 * supported */
    sip::object input_object(c.get_input(fsm_file_for_input));
    lts_file_argument = input_object.get_location();
    /* lts_type t = lts::parse_format(input_object.get_format().c_str());
    if (t == lts_none) {
      send_error(boost::str(boost::format(
				"Invalid configuration: unsupported type `%s' for main input") 
				% lts::string_for_type(t)));
      return false;
    }*/
  }
  else {
    return false;
  }
  return true;
}

bool squadt_interactor::perform_task(sip::configuration&) {
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

using namespace std;
using namespace Utils;
IMPLEMENT_APP_NO_MAIN(LTSViewApp)

bool LTSViewApp::OnInit() {
  lts = NULL;
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

  wxString lts_file_argument;
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (command_line) {
#endif
    // parse command line and check for specified input file
    wxCmdLineEntryDesc cmdLineDesc[] = {
      {wxCMD_LINE_PARAM,NULL,NULL,wxT("INFILE"),wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_NONE,NULL,NULL,NULL,wxCMD_LINE_VAL_NONE,0}
    };
    wxCmdLineParser cmdParser(cmdLineDesc,argc,argv);
    if (cmdParser.Parse() == 0) {
      if (cmdParser.GetParamCount() > 0) {
        lts_file_argument = cmdParser.GetParam(0);
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
  if (!lts_file_argument.empty()) {
    wxFileName fileName(lts_file_argument);
    fileName.Normalize();
    mainFrame->setFileInfo(fileName);
    openFile(static_cast< string >(fileName.GetFullPath().fn_str()));
  }
  return true;
}

#ifdef __WINDOWS__

extern "C" int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,int nCmdShow) {
#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_utility::entry_wrapper starter(hInstance,hPrevInstance,lpCmdLine,
      nCmdShow);
  squadt_interactor c(starter);
  if (!c.try_interaction(lpCmdLine)) {
#endif
    return wxEntry(hInstance,hPrevInstance,lpCmdLine,nCmdShow);    

#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
  return 0;
#endif
  return wxEntry(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
}
#else
int main(int argc, char **argv) {
#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_utility::entry_wrapper starter(argc, argv);
  squadt_interactor c(starter);
  if(!c.try_interaction(argc, argv)) {
    command_line = true;
#endif
  return wxEntry(argc, argv);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
  return 0;
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
  
  mainFrame->updateProgressDialog(14,"Applying ranking");
  applyRanking(rankStyle);
  
  mainFrame->updateProgressDialog(28,"Clustering comrades");
  lts->clusterComrades();
  
  mainFrame->updateProgressDialog(42,"Merging superiors");
  lts->mergeSuperiorClusters();

  mainFrame->updateProgressDialog(57,"Setting mark info");
  lts->computeClusterLabelInfo();
  
  mainFrame->updateProgressDialog(71,"Positioning clusters");
  lts->positionClusters();

  visualizer->setLTS(lts);
  
  mainFrame->updateProgressDialog(85,"Positioning states");
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

void LTSViewApp::applyRanking(RankStyle rs) {
  switch (rs) {
    case ITERATIVE:
      lts->applyIterativeRanking();
      break;
    case CYCLIC:
      lts->applyCyclicRanking();
      break;
    default:
      break;
  }
}

void LTSViewApp::setRankStyle(RankStyle rs) {
  if (rankStyle != rs) {
    rankStyle = rs;
    if (lts != NULL) {
      glCanvas->disableDisplay();

      mainFrame->createProgressDialog("Structuring LTS","Applying ranking");

      mainFrame->updateProgressDialog(0,"Applying ranking");
      applyRanking(rankStyle);
      
      mainFrame->updateProgressDialog(17,"Clustering comrades");
      lts->clusterComrades();
      
      mainFrame->updateProgressDialog(33,"Merging superiors");
      lts->mergeSuperiorClusters();

      mainFrame->updateProgressDialog(50,"Setting mark info");
      lts->computeClusterLabelInfo();
      lts->markClusters();
      
      mainFrame->updateProgressDialog(67,"Positioning clusters");
      lts->positionClusters();

      visualizer->setLTS(lts);

      mainFrame->updateProgressDialog(84,"Positioning states");
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
  switch (ms) {
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
  visualizer->setMarkStyle(ms);
  glCanvas->display();
}

void LTSViewApp::notifyRenderingStarted() {
  mainFrame->startRendering();
}

void LTSViewApp::notifyRenderingFinished() {
  mainFrame->stopRendering();
}

void LTSViewApp::startSim() {
  State* firstState = lts->getInitialState();
  lts->getSimulation()->start(firstState);
}

int LTSViewApp::getNumberOfParams() const {
  return lts->getNumParameters();
}
string LTSViewApp::getActionLabel(const int i) const {
  return lts->getLabel(i);
}

string LTSViewApp::getParName(const int i) const {
  return lts->getParameterName(i);
}

string LTSViewApp::getParValue(const int i, const int j) const {
  return lts->getParameterValue(i, j);
}
