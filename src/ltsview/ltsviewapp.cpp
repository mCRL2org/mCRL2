#define GLUT_DISABLE_ATEXIT_HACK

#include "ltsviewapp.h"

IMPLEMENT_APP( LTSViewApp )

bool LTSViewApp::OnInit()
{
  // initialise the ATerm and glut libraries
  ATerm stackbot;
  ATinit( 0, NULL, &stackbot );
  int c = 0;
  char* v = "";
  glutInit( &c, &v );
  
  mainFrame	  = new MainFrame( this );
  fileLoader	  = new FileLoader( this );
  visualizer	  = new Visualizer( this );
  lts		  = NULL;
  glCanvas	  = mainFrame->getGLCanvas();
  
  mainFrame->setVisSettings( visualizer->getVisSettings() );
  mainFrame->Show( true );
  glCanvas->initialize();

  return true;
}

void LTSViewApp::openFile( string fileName )
{
  glCanvas->disableDisplay();

  mainFrame->createProgressDialog( "Opening file", "Parsing file" );
  mainFrame->updateProgressDialog( 0, "Parsing file" );
  try
  {
    LTS* newlts = new LTS( this );
    fileLoader->loadFile( fileName, newlts );
    if ( lts != NULL ) delete lts;
    lts = newlts;
  }
  catch ( string msg )
  {
    mainFrame->updateProgressDialog( 100, "Error loading file" );
    jobNames.clear();
    mainFrame->showMessage( "Error loading file", msg );
    return;
  }
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 20, "Applying ranking" );
  applyRanking( visualizer->getRankStyle() );
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 40, "Clustering comrades" );
  lts->clusterComrades();
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 60, "Merging superiors" );
  lts->mergeSuperiorClusters();
  
  visualizer->setLTS( lts );
  
  ++currentJobNr;
  mainFrame->updateProgressDialog( 80, "Positioning clusters" );
  visualizer->positionClusters();
  mainFrame->updateProgressDialog( 100, "Done" );
  visualizer->setMarkStyle( NO_MARKS );

  //lts->printStructure();

  //lts->printClusterSizesPositions();
  glCanvas->enableDisplay();
  glCanvas->display( false );
  glCanvas->setDefaultPosition( visualizer->getStructureWidth(),
      visualizer->getStructureHeight() );
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo( lts->getNumberOfStates(),
      lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
      lts->getNumberOfRanks() );
  mainFrame->resetMarkRules();
  mainFrame->setMarkedStatesInfo( 0 );
  mainFrame->setVisSettings( visualizer->getVisSettings() );
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

void LTSViewApp::drawLTS()
{
  visualizer->drawLTS();
}

void LTSViewApp::applyDefaultSettings()
{
  mainFrame->setVisSettings( visualizer->getDefaultVisSettings() );
  applySettings();
}

void LTSViewApp::applySettings()
{
  visualizer->setVisSettings( mainFrame->getVisSettings() );
  if ( lts != NULL )
  {
    glCanvas->display();
    glCanvas->setDefaultPosition( visualizer->getStructureWidth(),
	visualizer->getStructureHeight() );
  }
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
      mainFrame->updateProgressDialog( 25, "Clustering comrades" );
      lts->clusterComrades();
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 50, "Merging superiors" );
      lts->mergeSuperiorClusters();
      lts->markStates();
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 75, "Positioning clusters" );
      visualizer->positionClusters();
      mainFrame->updateProgressDialog( 100, "Done" );

      //lts->printStructure();

      //lts->printClusterSizesPositions();
      glCanvas->enableDisplay();
      glCanvas->display( false );
      glCanvas->setDefaultPosition( visualizer->getStructureWidth(),
	  visualizer->getStructureHeight() );
      glCanvas->resetView();
      mainFrame->setNumberInfo( lts->getNumberOfStates(),
	  lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
	  lts->getNumberOfRanks() );
      mainFrame->setVisSettings( visualizer->getVisSettings() );
    }
  }
}

void LTSViewApp::addMarkRule()
{
  if ( lts != NULL )
  {
    MarkStateRuleDialog msrDialog( mainFrame, this, lts->getStateVectorSpec() );
    msrDialog.CentreOnParent();
    if ( msrDialog.ShowModal() == wxID_OK )
    {
      MarkRule* markrule = msrDialog.getMarkRule();
      lts->addMarkRule( markrule );
      mainFrame->addMarkRule( msrDialog.getMarkRuleString() );
      lts->markStates();
      applyMarkStyle( MARK_STATES );
    }
  }
}

void LTSViewApp::removeMarkRules( const vector<int> &mrs )
{
  lts->removeMarkRules( mrs );
  lts->markStates();
  applyMarkStyle( MARK_STATES );
}

void LTSViewApp::setMatchAnyMarkRule( bool b )
{
  if ( lts != NULL )
  {
    if ( lts->getMatchAnyMarkRule() != b )
    {
      lts->setMatchAnyMarkRule( b );
      lts->markStates();
      applyMarkStyle( MARK_STATES );
    }
  }
}

void LTSViewApp::applyMarkStyle( MarkStyle ms )
{
  switch( ms )
  {
    case MARK_DEADLOCKS:
      mainFrame->setMarkedStatesInfo( lts->getNumberOfDeadlocks() );
      break;
    case MARK_STATES:
      mainFrame->setMarkedStatesInfo( lts->getNumberOfMarkedStates() );
      break;
    case NO_MARKS:
    default:
      mainFrame->setMarkedStatesInfo( 0 );
      break;
  }
  visualizer->setMarkStyle( ms );
  glCanvas->display();
}
