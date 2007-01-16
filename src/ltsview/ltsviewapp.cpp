
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <wx/wx.h>
//SQuADT protocol interface
#include <utility/squadt_utility.h>

bool command_line = false;
std::string lts_file_argument;

class squadt_interactor: public squadt_tool_interface {
  
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

const char* squadt_interactor::fsm_file_for_input  = "fsm_in";

squadt_interactor::squadt_interactor(squadt_utility::entry_wrapper& w): starter(w) {
  // skip 
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(fsm_file_for_input, sip::mime_type("fsm", sip::mime_type::text), sip::tool::category::visualisation);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  //skip
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  if (c.input_exists(fsm_file_for_input)) {
    /* The input object is present, verify whether the specified format is supported */
    sip::object input_object(c.get_input(fsm_file_for_input));
    lts_file_argument = input_object.get_location();


    /* lts_type t = lts::parse_format(input_object.get_format().c_str());

    if (t == lts_none) {
      send_error(boost::str(boost::format("Invalid configuration: unsupported type `%s' for main input") % lts::string_for_type(t)));
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
#include <iostream>
#include "ltsviewapp.h"
#include "aterm1.h"
#include "markstateruledialog.h"
#include "fileloader.h"

using namespace std;
using namespace Utils;
IMPLEMENT_APP_NO_MAIN(LTSViewApp)

bool LTSViewApp::OnInit() {
  lts = NULL;
  rankStyle = ITERATIVE;
  mainFrame = new MainFrame(this);
  visualizer = new Visualizer(this);
  glCanvas = mainFrame->getGLCanvas();
  glCanvas->setVisualizer(visualizer);
  
  SetTopWindow(mainFrame);
  mainFrame->Show(true);
  glCanvas->initialize();
  mainFrame->setVisSettings(visualizer->getVisSettings());
  mainFrame->setBackgroundColor(glCanvas->getBackgroundColor());
  mainFrame->Layout();

  wxInitAllImageHandlers();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (command_line) {
#else
    std::string lts_file_argument;
#endif
    // parse command line and check for specified input file
    wxCmdLineEntryDesc cmdLineDesc[] = {
      {wxCMD_LINE_PARAM,NULL,NULL,wxT("INFILE"),wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_OPTIONAL},
      {wxCMD_LINE_NONE,NULL,NULL,NULL,wxCMD_LINE_VAL_NONE,0}
    };
    wxCmdLineParser cmdParser(cmdLineDesc, argc,argv);
    if (cmdParser.Parse() == 0) {
      if (cmdParser.GetParamCount() > 0) {
        lts_file_argument = std::string(cmdParser.GetParam(0).fn_str());
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif
  wxString wx_file_string(lts_file_argument.c_str(),wxConvLocal);
  if (!wx_file_string.IsEmpty()) {
    wxFileName fileName(wx_file_string);
    fileName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS |
        wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
    mainFrame->setFileInfo(fileName);
    openFile(static_cast< string >(fileName.GetFullPath().fn_str()));
  }
  return true;
}

#ifdef __WINDOWS__

extern "C" int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                              wxCmdLineArgType lpCmdLine,int nCmdShow) {
  // initialise the ATerm library
  ATerm stackbot;
  ATinit(0,0,&stackbot); // XXX args?

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
  // initialise the ATerm library
  ATerm stackbot;
  ATinit(argc,argv,&stackbot);

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
  visualizer->computeBoundsInfo();
  glCanvas->setDefaultPosition(visualizer->getBoundingCylinderWidth(),
      visualizer->getBoundingCylinderHeight());
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo(lts->getNumberOfStates(),
      lts->getNumberOfTransitions(),lts->getNumberOfClusters(),
      lts->getNumberOfRanks());
  mainFrame->resetMarkRules();
  
  vector< ATerm > ls;
  lts->getActionLabels(ls);
  mainFrame->setActionLabels(ls);

  mainFrame->setMarkedStatesInfo(0);
  mainFrame->setMarkedTransitionsInfo(0);
  mainFrame->setVisSettings(visualizer->getVisSettings());
  mainFrame->setBackgroundColor(glCanvas->getBackgroundColor());
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

void LTSViewApp::applyDefaultSettings() {
  mainFrame->setVisSettings(visualizer->getDefaultVisSettings());
  mainFrame->setBackgroundColor(glCanvas->getDefaultBackgroundColor());
  applySettings();
}

void LTSViewApp::applySettings() {
  visualizer->setVisSettings(mainFrame->getVisSettings());
  glCanvas->setBackgroundColor(mainFrame->getBackgroundColor());
  glCanvas->display();
  glCanvas->setDefaultPosition(visualizer->getBoundingCylinderWidth(),
      visualizer->getBoundingCylinderHeight() );
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
      visualizer->computeBoundsInfo();
      glCanvas->setDefaultPosition(visualizer->getBoundingCylinderWidth(),
        visualizer->getBoundingCylinderHeight());
      glCanvas->resetView();

      mainFrame->setNumberInfo(lts->getNumberOfStates(),
        lts->getNumberOfTransitions(),lts->getNumberOfClusters(),
        lts->getNumberOfRanks());
      mainFrame->setVisSettings(visualizer->getVisSettings());
      mainFrame->setBackgroundColor(glCanvas->getBackgroundColor());
    }
  }
}

void LTSViewApp::setVisStyle(VisStyle vs) {
  if (visualizer->getVisStyle() != vs) {
    visualizer->setVisStyle(vs);
    glCanvas->display();
  }
}

void LTSViewApp::addMarkRule()
{
  if ( lts != NULL )
  {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog( mainFrame, this,
	lts->getStateVectorSpec() );
    msrDialog->CentreOnParent();
    if ( msrDialog->ShowModal() == wxID_OK )
    {
      MarkRule* markrule = msrDialog->getMarkRule();
      if ( markrule != NULL )
      {
	lts->addMarkRule( markrule );
	mainFrame->addMarkRule( msrDialog->getMarkRuleString() );
	applyMarkStyle( MARK_STATES );
      }
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

void LTSViewApp::removeMarkRule( const int index )
{
  lts->removeMarkRule( index );
  applyMarkStyle( MARK_STATES );
}

void LTSViewApp::editMarkRule( const int index )
{
  if ( lts != NULL )
  {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog( mainFrame, this,
	lts->getStateVectorSpec() );
    bool oldActivated = lts->getMarkRule( index )->isActivated;
    msrDialog->setMarkRule( lts->getMarkRule( index ), lts->getStateVectorSpec() );
    msrDialog->CentreOnParent();
    if ( msrDialog->ShowModal() == wxID_OK )
    {
      MarkRule* markrule = msrDialog->getMarkRule();
      if ( markrule != NULL )
      {
	markrule->isActivated = oldActivated;
	lts->replaceMarkRule( index, markrule );
	mainFrame->replaceMarkRule( index, msrDialog->getMarkRuleString() );
	applyMarkStyle( MARK_STATES );
      }
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

void LTSViewApp::activateMarkRule( const int index, const bool activate )
{
  if ( lts != NULL )
  {
    lts->activateMarkRule( index, activate );
    applyMarkStyle( MARK_STATES );
  }
}

void LTSViewApp::setMatchAnyMarkRule( bool b )
{
  if ( lts != NULL )
  {
    lts->setMatchAnyMarkRule( b );
    applyMarkStyle( MARK_STATES );
  }
}

void LTSViewApp::markAction( string label )
{
  lts->markAction( label );
  applyMarkStyle( MARK_TRANSITIONS );
}

void LTSViewApp::unmarkAction( string label )
{
  lts->unmarkAction( label );
  applyMarkStyle( MARK_TRANSITIONS );
}

void LTSViewApp::applyMarkStyle( MarkStyle ms )
{
  if ( lts == NULL ) return;
  switch( ms )
  {
    case MARK_DEADLOCKS:
      mainFrame->setMarkedStatesInfo( lts->getNumberOfDeadlocks() );
      mainFrame->setMarkedTransitionsInfo( 0 );
      break;
    case MARK_STATES:
      mainFrame->setMarkedStatesInfo( lts->getNumberOfMarkedStates() );
      mainFrame->setMarkedTransitionsInfo( 0 );
      break;
    case MARK_TRANSITIONS:
      mainFrame->setMarkedStatesInfo( 0 );
      mainFrame->setMarkedTransitionsInfo( lts->getNumberOfMarkedTransitions() );
      break;
    case NO_MARKS:
    default:
      mainFrame->setMarkedStatesInfo( 0 );
      mainFrame->setMarkedTransitionsInfo( 0 );
      break;
  }
  visualizer->setMarkStyle( ms );
  glCanvas->display();
}

void LTSViewApp::notifyRenderingStarted() {
  mainFrame->startRendering();
}

void LTSViewApp::notifyRenderingFinished() {
  mainFrame->stopRendering();
}
