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
      openFile( static_cast< string > ( fileName.GetFullPath().fn_str() ) );
    }
  }

  return true;
}

void LTSViewApp::openFile( string fileName )
{
  glCanvas->disableDisplay();

  mainFrame->createProgressDialog( "Opening file", "Parsing file" );
  mainFrame->updateProgressDialog( 0, "Parsing file" );
  LTS* newlts = new LTS( this );
  try
  {
    fileLoader->loadFile( fileName, newlts );
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
  glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
      visualizer->getBoundingCylinderHeight() );
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

void LTSViewApp::drawLTS( Point3D viewpoint )
{
  visualizer->drawLTS( viewpoint );
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
    glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
	visualizer->getBoundingCylinderHeight() );
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
      glCanvas->setDefaultPosition( visualizer->getBoundingCylinderWidth(),
	  visualizer->getBoundingCylinderHeight() );
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
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog( mainFrame, this,
	lts->getStateVectorSpec() );
    msrDialog->CentreOnParent();
    if ( msrDialog->ShowModal() == wxID_OK )
    {
      MarkRule* markrule = msrDialog->getMarkRule();
      lts->addMarkRule( markrule );
      mainFrame->addMarkRule( msrDialog->getMarkRuleString() );
      lts->markStates();
      applyMarkStyle( MARK_STATES );
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

void LTSViewApp::removeMarkRule( const int index )
{
  lts->removeMarkRule( index );
  lts->markStates();
  applyMarkStyle( MARK_STATES );
}

void LTSViewApp::editMarkRule( const int index )
{
  if ( lts != NULL )
  {
    MarkStateRuleDialog* msrDialog = new MarkStateRuleDialog( mainFrame, this,
	lts->getStateVectorSpec() );
    msrDialog->setMarkRule( lts->getMarkRule( index ), lts->getStateVectorSpec() );
    msrDialog->CentreOnParent();
    if ( msrDialog->ShowModal() == wxID_OK )
    {
      MarkRule* markrule = msrDialog->getMarkRule();
      lts->replaceMarkRule( index, markrule );
      mainFrame->replaceMarkRule( index, msrDialog->getMarkRuleString() );
      lts->markStates();
      applyMarkStyle( MARK_STATES );
    }
    msrDialog->Close();
    msrDialog->Destroy();
  }
}

float LTSViewApp::getHalfStructureHeight() const
{
  return visualizer->getHalfStructureHeight();
}

RGB_Color LTSViewApp::getBackgroundColor() const
{
  return visualizer->getBackgroundColor();
}

void LTSViewApp::setMatchAnyMarkRule( bool b )
{
  if ( lts != NULL )
  {
    if ( lts->getMatchAnyMarkRule() != b )
    {
      lts->setMatchAnyMarkRule( b );
      lts->markStates();
    }
    applyMarkStyle( MARK_STATES );
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
