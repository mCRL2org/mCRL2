#include "ltsviewapp.h"

IMPLEMENT_APP( LTSViewApp )

bool LTSViewApp::OnInit()
{
  // initialize the ATerm library
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

  //lts->printStructure();

  //lts->printClusterSizesPositions();
  glCanvas->enableDisplay();
  glCanvas->display();
  glCanvas->setDefaultPosition( visualizer->getStructureWidth(),
      visualizer->getStructureHeight() );
  glCanvas->resetView();
  
  mainFrame->loadTitle();
  mainFrame->setNumberInfo( lts->getNumberOfStates(),
      lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
      lts->getNumberOfRanks() );
  mainFrame->setVisSettings( visualizer->getVisSettings() );
}

void LTSViewApp::applyRanking( RankStyle rs )
{
  switch ( rs )
  {
    case Iterative:
      lts->applyIterativeRanking();
      break;
    case Cyclic:
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

void LTSViewApp::setRankStyle( string rss )
{
  RankStyle rs;
  if ( rss == "Iterative" )
    rs = Iterative;
  else if ( rss == "Cyclic" )
    rs = Cyclic;
  else
    return;

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
      
      ++currentJobNr;
      mainFrame->updateProgressDialog( 75, "Positioning clusters" );
      visualizer->positionClusters();
      mainFrame->updateProgressDialog( 100, "Done" );

      //lts->printStructure();

      //lts->printClusterSizesPositions();
      glCanvas->enableDisplay();
      glCanvas->display();
      glCanvas->setDefaultPosition( visualizer->getStructureWidth(),
	  visualizer->getStructureHeight() );
      mainFrame->setNumberInfo( lts->getNumberOfStates(),
	  lts->getNumberOfTransitions(), lts->getNumberOfClusters(),
	  lts->getNumberOfRanks() );
      mainFrame->setVisSettings( visualizer->getVisSettings() );
    }
  }
}

void LTSViewApp::showMarkStateRuleDialog()
{
  if ( lts != NULL )
  {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog( mainFrame, this,
	lts->getStateVectorSpec() );
    if ( msrDialog->ShowModal() == wxOK )
    {
    }
    delete msrDialog;
  }
}
