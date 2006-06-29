#include "ltsviewapp.h"

IMPLEMENT_APP_NO_MAIN( LTSViewApp )

bool LTSViewApp::OnInit()
{
  mainFrame	  = new MainFrame( this );
  visualizer	  = new Visualizer( this );
  lts		  = NULL;
  glCanvas	  = mainFrame->getGLCanvas();
  
  SetTopWindow( mainFrame );
  mainFrame->Show( true );
  glCanvas->initialize();
  mainFrame->setVisSettings( visualizer->getVisSettings() );
  mainFrame->setBackgroundColor( glCanvas->getBackgroundColor() );

  // parse command line and check for specified input file
  wxCmdLineEntryDesc cmdLineDesc[] = 
  {
    { wxCMD_LINE_PARAM, NULL, NULL, wxT("INFILE"), wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0 }
  };
  wxCmdLineParser cmdParser( cmdLineDesc, argc, argv );
  if ( cmdParser.Parse() == 0 )
  {
    if ( cmdParser.GetParamCount() > 0 )
    {
      wxFileName fileName( cmdParser.GetParam(0) );
      fileName.Normalize( wxPATH_NORM_LONG | wxPATH_NORM_DOTS |
	  wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE );
      mainFrame->setFileInfo( fileName );
      openFile( static_cast< string > ( fileName.GetFullPath().fn_str() ) );
    }
  }

  return true;
}

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,                    
                                  HINSTANCE hPrevInstance,                
                                  wxCmdLineArgType lpCmdLine,             
                                  int nCmdShow) {                                                                     
  ATerm stackbot;

  // initialise the ATerm library
  ATinit(NULL,NULL,&stackbot); // XXX args?

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);    
}
#else
int main(int argc, char **argv) {
  ATerm stackbot;

  // initialise the ATerm library
  ATinit(argc,argv,&stackbot);

  return wxEntry(argc, argv);
}
#endif

int LTSViewApp::OnExit()
{
  if ( lts != NULL ) delete lts;
  delete visualizer;
  return 0;
}

void LTSViewApp::openFile( string fileName )
{
  glCanvas->disableDisplay();

  mainFrame->createProgressDialog( "Opening file", "Parsing file" );
  mainFrame->updateProgressDialog( 0, "Parsing file" );
  LTS* newlts = new LTS( this );
  try
  {
    FileLoader::loadFile( fileName, newlts );
  }
  catch ( string msg )
  {
    delete newlts;
    jobNames.clear();
    mainFrame->updateProgressDialog( 100, "Error loading file" );
    mainFrame->showMessage( "Error loading file", msg );
    return;
  }
  if ( lts != NULL ) delete lts;
  lts = newlts;
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 14, "Applying ranking" );
  applyRanking( visualizer->getRankStyle() );
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 28, "Clustering comrades" );
  lts->clusterComrades();
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 42, "Merging superiors" );
  lts->mergeSuperiorClusters();

  mainFrame->updateProgressDialog( 57, "Setting mark info" );
  lts->computeClusterLabelInfo();
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 71, "Positioning clusters" );
  lts->positionClusters();

  visualizer->setLTS( lts );
  visualizer->computeClusterHeight();
  
  mainFrame->updateProgressDialog( 85, "Positioning states" );
  lts->positionStates();
  
  mainFrame->updateProgressDialog( 100, "Done" );
  visualizer->setMarkStyle( NO_MARKS );

  //lts->printStructure();

  //lts->printClusterSizesPositions();
  glCanvas->enableDisplay();
  visualizer->computeBoundsInfo();
  glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
      visualizer->getBoundingCylinderHeight() );
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo( lts->getNumberOfStates(),
      lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
      lts->getNumberOfRanks() );
  mainFrame->resetMarkRules();
  
  vector< ATerm > ls;
  lts->getActionLabels( ls );
  mainFrame->setActionLabels( ls );

  mainFrame->setMarkedStatesInfo( 0 );
  mainFrame->setMarkedTransitionsInfo( 0 );
  mainFrame->setVisSettings( visualizer->getVisSettings() );
  mainFrame->setBackgroundColor( glCanvas->getBackgroundColor() );
}

void LTSViewApp::applyRanking( RankStyle rs )
{
  switch ( rs )
  {
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

void LTSViewApp::drawLTS( Point3D viewpoint )
{
  visualizer->drawLTS( viewpoint );
}

void LTSViewApp::applyDefaultSettings()
{
  mainFrame->setVisSettings( visualizer->getDefaultVisSettings() );
  mainFrame->setBackgroundColor( glCanvas->getDefaultBackgroundColor() );
  applySettings();
}

void LTSViewApp::applySettings()
{
  bool refreshBoundCyl = visualizer->setVisSettings( mainFrame->getVisSettings() );
  glCanvas->setBackgroundColor( mainFrame->getBackgroundColor() );
  if ( refreshBoundCyl )
  {
    visualizer->computeBoundsInfo();
  }
  glCanvas->display();
  glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
      visualizer->getBoundingCylinderHeight() );
}

void LTSViewApp::setRankStyle( RankStyle rs )
{
  if ( visualizer->getRankStyle() != rs )
  {
    visualizer->setRankStyle( rs );
     
    if ( lts != NULL )
    {
      glCanvas->disableDisplay();

      mainFrame->createProgressDialog( "Structuring LTS", "Applying ranking" );
      mainFrame->updateProgressDialog( 0, "Applying ranking" );

      applyRanking( rs );
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 17, "Clustering comrades" );
      lts->clusterComrades();
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 33, "Merging superiors" );
      lts->mergeSuperiorClusters();

      mainFrame->updateProgressDialog( 50, "Setting mark info" );
      lts->computeClusterLabelInfo();
      lts->markClusters();
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 67, "Positioning clusters" );
      lts->positionClusters();
      visualizer->computeClusterHeight();

      mainFrame->updateProgressDialog( 84, "Positioning states" );
      lts->positionStates();
      mainFrame->updateProgressDialog( 100, "Done" );

      //lts->printStructure();

      //lts->printClusterSizesPositions();
      glCanvas->enableDisplay();
      visualizer->computeBoundsInfo();
      glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
	  visualizer->getBoundingCylinderHeight() );
      glCanvas->resetView();
      mainFrame->setNumberInfo( lts->getNumberOfStates(),
	  lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
	  lts->getNumberOfRanks() );
      mainFrame->setVisSettings( visualizer->getVisSettings() );
      mainFrame->setBackgroundColor( glCanvas->getBackgroundColor() );
    }
  }
}

void LTSViewApp::setVisStyle( VisStyle vs )
{
  if ( visualizer->getVisStyle() != vs )
  {
    visualizer->setVisStyle( vs );
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

float LTSViewApp::getHalfStructureHeight() const
{
  return visualizer->getHalfStructureHeight();
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

void LTSViewApp::toggleDisplayStates()
{
  visualizer->toggleDisplayStates();
  glCanvas->display();
}

void LTSViewApp::toggleDisplayWireframe()
{
  visualizer->toggleDisplayWireframe();
  glCanvas->display();
}

void LTSViewApp::notifyRenderingStarted()
{
  mainFrame->startRendering();
}

void LTSViewApp::notifyRenderingFinished()
{
  mainFrame->stopRendering();
}
