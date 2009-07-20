// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagraph.cpp

#include "wx.hpp" // precompiled headers

#define NAME "diagraphica"
#define AUTHOR "A. Johannes Pretorius"

#include <wx/wx.h>
#include <wx/sysopt.h>
#include <wx/clrpicker.h>
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/wx_tool.h"
#include "diagraph.h"

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"
#include "mcrl2/lts/lts.h"

// windows debug libraries
#ifdef _MSC_VER
  #define _CRTDBG_MAP_ALLOC
  #include <stdlib.h>
  #include <crtdbg.h>
#endif

using namespace std;

#ifdef ENABLE_SQUADT_CONNECTIVITY
// Configures tool capabilities.
void DiaGraph::set_capabilities(tipi::tool::capabilities& c) const {
  std::set< mcrl2::lts::lts_type > const& input_formats(mcrl2::lts::lts::supported_lts_formats());

  for (std::set< mcrl2::lts::lts_type >::const_iterator i = input_formats.begin(); i != input_formats.end(); ++i) {
    c.add_input_configuration("main-input", tipi::mime_type(mcrl2::lts::lts::mime_type_for_type(*i)), tipi::tool::category::visualisation);
  }
}

// Queries the user via SQuADt if needed to obtain configuration information
void DiaGraph::user_interactive_configuration(tipi::configuration&) { }

// Check an existing configuration object to see if it is usable
bool DiaGraph::check_configuration(tipi::configuration const& c) const {
  if (c.input_exists("main-input")) {
    /* The input object is present, verify whether the specified format is supported */
    if (mcrl2::lts::lts::parse_format(c.get_input("main-input").type().sub_type().c_str()) == mcrl2::lts::lts_none) {
      send_error("Invalid configuration: unsupported type `" +
          c.get_input("main-input").type().sub_type() + "' for main input");
    }
    else {
      return true;
    }
  }

  return false;
}

bool DiaGraph::perform_task(tipi::configuration& c) {
  m_input_filename = c.get_input("main-input").location();

  return super::perform_task(c);
}
#endif

DiaGraph::DiaGraph() : super("DiaGraph",
      "interactive visual analysis of an LTS", // what-is
      "You are free to use images produced with DiaGraphica.\n" // gui-specific description
      "In this case, image credits would be much appreciated.\n" 
      "\n" 
      "DiaGraphica was built with wxWidgets (www.wxwidgets.org) and \n" 
      "uses the TinyXML parser (tinyxml.sourceforge.net). \n" 
      "Color schemes were chosen with ColorBrewer (www.colorbrewer.org).",
      "Multivariate state visualisation and simulation analysis for labelled "
      "transition systems (LTS's) in the FSM format. If an INFILE is not supplied then "
      "DiaGraphica is started without opening an LTS.",
    std::vector< std::string >(1, "Hannes Pretorius")), graph(0)
{ }

bool DiaGraph::run()
{
    // windows debugging
    #ifdef _MSC_VER
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    //_CrtSetBreakAlloc( 4271 );
    #endif

    wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), 1);

    // set mode
    mode = MODE_ANALYSIS;

    // set view
    view = VIEW_SIM;

    // init colleagues
    initColleagues();

    clustered = false;
    critSect = false;

    if (!lts_file_argument.empty()) {
      openFile(lts_file_argument);
    }

    // start event loop
    return true;
}

IMPLEMENT_APP_NO_MAIN( DiaGraph )
IMPLEMENT_WX_THEME_SUPPORT

#ifdef __WINDOWS__
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
                                  HINSTANCE hPrevInstance,
                                  wxCmdLineArgType lpCmdLine,
                                  int nCmdShow) {

  MCRL2_ATERMPP_INIT(0, lpCmdLine)

  return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return wxEntry(argc, argv);
}
#endif
// -- * -------------------------------------------------------------

// -- functions inherited from wxApp --------------------------------

// -------------------
int DiaGraph::OnExit()
// -------------------
{
    if (graph != 0) {
      // clear colleagues
      clearColleagues();
    }

    // normal exit
    return 0;
}


// -- load & save data ----------------------------------------------


// ------------------------------------------
void DiaGraph::openFile( const std::string &path )
// ------------------------------------------
{
    Parser* parser   = NULL;
    std::string fileName  = "";
    std::string delims    = "\\/";
    std::string::size_type begIdx;
    std::string::size_type endIdx;
    int    fileSize  = 0;

    // get filename
    begIdx = path.find_last_of( delims );
    if ( begIdx == std::string::npos )
        begIdx = 0;
    else
        begIdx += 1;
    endIdx   = path.size();
    fileName = path.substr( begIdx, endIdx-begIdx );

    // init parser
    parser = new Parser( this );
    try
    {
        // get file size
        fileSize = parser->getFileSize( path );
    }
    catch ( const mcrl2::runtime_error& e )
    {
        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    try
    {
        critSect = true;

        // delete visualizers
        if ( arcDgrm != NULL )
        {
            delete arcDgrm;
            arcDgrm = NULL;
        }
        canvasArcD->Refresh();

        if ( simulator != NULL )
        {
            delete simulator;
            simulator = NULL;
        }
        if ( view == VIEW_SIM )
            canvasSiml->Refresh();

        if ( timeSeries != NULL )
        {
            delete timeSeries;
            timeSeries = NULL;
        }
        if ( view == VIEW_TRACE )
            canvasTrace->Refresh();

        if ( examiner != NULL )
        {
            delete examiner;
            examiner = NULL;
        }
        canvasExnr->Refresh();

        if ( editor != NULL )
        {
            delete editor;
            editor = NULL;
        }

        // delete old graph
        if ( graph != NULL )
        {
            delete graph;
            graph = NULL;
        }

        // create new graph
        graph = new Graph( this );
        graph->setFileName( fileName );

        // parse file
        initProgress(
            "Opening file",
            "Opening " + fileName,
            fileSize );
        parser->parseFSMFile(
            path,
            graph );
        closeProgress();

        // init graph
        graph->initGraph();

        // set up frame output
        frame->clearOuput();
        frame->setTitleText( fileName );
        frame->setFileOptionsActive();
        frame->displNumNodes( graph->getSizeNodes() );
        frame->displNumEdges( graph->getSizeEdges() );

        // display attributes
        displAttributes();

        // init new visualizers
        arcDgrm = new ArcDiagram(
            this,
            graph,
            canvasArcD );
        if ( mode == MODE_ANALYSIS )
        {
            canvasArcD->Refresh();
            canvasSiml->Refresh();
            canvasExnr->Refresh();
        }

        simulator = new Simulator(
            this,
            graph,
            canvasSiml );

        timeSeries = new TimeSeries(
            this,
            graph,
            canvasTrace );

        examiner = new Examiner(
            this,
            graph,
            canvasExnr );

        editor = new DiagramEditor(
            this,
            graph,
            canvasEdit );
        if ( mode == MODE_EDIT )
            canvasEdit->Refresh();

        arcDgrm->setDiagram( editor->getDiagram() );
        simulator->setDiagram( editor->getDiagram() );
        timeSeries->setDiagram( editor->getDiagram() );
        examiner->setDiagram( editor->getDiagram() );

        critSect = false;
    }
    catch ( const mcrl2::runtime_error& e )
    {
        delete progressDialog;
        progressDialog = NULL;

        wxLogError( wxString( e.what(), wxConvUTF8 ) );

        critSect = false;
    }

    // delete parser
    delete parser;
    parser = NULL;

    // clear status msg
    frame->setStatusText( "" );

    // enable edit mode
    frame->enableEditMode( true );
}


// ------------------------------------------
void DiaGraph::saveFile( const std::string &path )
// ------------------------------------------
{
    // init parser
    Parser* parser = new Parser( this );

    // do parsing
    try
    {
        parser->writeFSMFile(
            path,
            graph );
    }
    catch ( const mcrl2::runtime_error& e )
    {
        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    // delete parser
    delete parser;
    parser = NULL;
}


// ------------------------------------------------------
void DiaGraph::handleLoadAttrConfig( const std::string &path )
// ------------------------------------------------------
{
    // init parser
    Parser* parser = new Parser( this );

    // do parsing
    try
    {
        map< int, int > attrIdxFrTo;
        map< int, vector< std::string > > attrCurDomains;
        map< int, map< int, int  > > attrOrigToCurDomains;

        parser->parseAttrConfig(
            path,
            graph,
            attrIdxFrTo,
            attrCurDomains,
            attrOrigToCurDomains );

        graph->configAttributes(
            attrIdxFrTo,
            attrCurDomains,
            attrOrigToCurDomains );
        displAttributes();

        /*
        ! also need to close any other vis windows e.g. corrlplot... !
        */
    }
    catch ( const mcrl2::runtime_error& e )
    {
        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    // delete parser
    delete parser;
    parser = NULL;
}


// ------------------------------------------------------
void DiaGraph::handleSaveAttrConfig( const std::string &path )
// ------------------------------------------------------
{
    // init parser
    Parser* parser = new Parser( this );

    // do parsing
    try
    {
        parser->writeAttrConfig(
            path,
            graph );
    }
    catch ( const mcrl2::runtime_error& e )
    {
        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    // delete parser
    delete parser;
    parser = NULL;
}


// ---------------------------------------------------
void DiaGraph::handleLoadDiagram( const std::string &path )
// ---------------------------------------------------
{
    // init parser
    Parser*  parser  = new Parser( this );
    // init diagrams
    Diagram* dgrmOld = editor->getDiagram();
    Diagram* dgrmNew = new Diagram( this/*, canvasEdit*/ );

    // do parsing
    try
    {
        parser->parseDiagram(
            path,
            graph,
            dgrmOld,
            dgrmNew );
        editor->setDiagram( dgrmNew );

        arcDgrm->setDiagram( dgrmNew );
        arcDgrm->hideAllDiagrams();

        simulator->clearData();
        simulator->setDiagram( dgrmNew );

        timeSeries->clearData();
        timeSeries->setDiagram( dgrmNew );

        examiner->clearData();
        examiner->setDiagram( dgrmNew );

        if ( mode == MODE_EDIT && canvasEdit != NULL )
            canvasEdit->Refresh();

        if ( mode == MODE_ANALYSIS )
        {
            if ( canvasArcD != NULL )
                canvasArcD->Refresh();
            if ( canvasSiml != NULL )
                canvasSiml->Refresh();
            if ( canvasExnr != NULL )
                canvasExnr->Refresh();
        }

    }
    catch ( const mcrl2::runtime_error& e )
    {
        delete dgrmNew;
        dgrmNew = NULL;

        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    // delete parser
    delete parser;
    parser = NULL;

    dgrmOld = NULL;
    dgrmNew = NULL;
}


// ---------------------------------------------------
void DiaGraph::handleSaveDiagram( const std::string &path )
// ---------------------------------------------------
{
    // init parser
    Parser* parser   = new Parser( this );

    // do parsing
    try
    {
        parser->writeDiagram(
            path,
            graph,
            editor->getDiagram() );
    }
    catch ( const mcrl2::runtime_error& e )
    {
        wxLogError( wxString( e.what(), wxConvUTF8 ) );
    }

    // delete parser
    delete parser;
    parser = NULL;
}


// -- general input & output ------------------------------------


// -------------------------
void DiaGraph::initProgress(
    const std::string &title,
    const std::string &msg,
    const int &max )
// -------------------------
{
    if ( progressDialog != NULL )
    {
        delete progressDialog;
        progressDialog = NULL;
    }

    // display progress dialog
    progressDialog = new wxProgressDialog(
            wxString( title.c_str(), wxConvUTF8 ),
            wxString( msg.c_str(), wxConvUTF8 ),
            max,
            frame );
    // set status message
    frame->setStatusText( msg );
}


// --------------------------------------------
void DiaGraph::updateProgress( const int &val )
// --------------------------------------------
{
    if ( progressDialog != NULL )
        progressDialog->Update( val );
}


// ---------------------------
void DiaGraph::closeProgress()
// ---------------------------
{
    if ( progressDialog != NULL )
    {
        delete progressDialog;
        progressDialog = NULL;
    }

    frame->setStatusText( "" );
}


// ----------------------------------------------
void DiaGraph::setOutputText( const std::string &msg )
// ----------------------------------------------
// ------------------------------------------------------------------
// Clear existing text output and display 'msg'.
// ------------------------------------------------------------------
{
    frame->appOutputText( msg );
}


// -------------------------------------------
void DiaGraph::setOutputText( const int &val )
// -------------------------------------------
// ------------------------------------------------------------------
// Clear existing text output and display 'val'.
// ------------------------------------------------------------------
{
  std::string msg = Utils::intToStr( val );
    frame->appOutputText( msg );
}


// ----------------------------------------------
void DiaGraph::appOutputText( const std::string &msg )
// ----------------------------------------------
// ------------------------------------------------------------------
// Append 'msg' to the current text output without clearing it first.
// ------------------------------------------------------------------
{
    frame->appOutputText( msg );
}


// -------------------------------------------
void DiaGraph::appOutputText( const int &val )
// -------------------------------------------
// ------------------------------------------------------------------
// Append 'val' to the current text output without clearing it first.
// ------------------------------------------------------------------
{
  std::string msg = Utils::intToStr( val );
    frame->appOutputText( msg );
}

// -------------------------------------
void DiaGraph::getColor( ColorRGB &col )
// -------------------------------------
{
    wxColourData   data;
    wxColourDialog dialog( frame, &data );

    if ( dialog.ShowModal() == wxID_OK )
    {
        data   = dialog.GetColourData();
        wxColour colTmp = data.GetColour();

        col.r = colTmp.Red()/255.0;
        col.g = colTmp.Green()/255.0;
        col.b = colTmp.Blue()/255.0;
    }
}


// ---------------------------------------------
void DiaGraph::handleCloseFrame( PopupFrame* f )
// ---------------------------------------------
{
    frame->handleCloseFrame( f );
}


// -- interaction with attributes & domains -------------------------


// ------------------------------------------------
void DiaGraph::handleAttributeSel( const int &idx )
// ------------------------------------------------
{
    // display domain
    displAttrDomain( idx );
}


// ---------------------------
void DiaGraph::handleMoveAttr(
    const int &idxFr,
    const int &idxTo )
// ---------------------------
{
    if ( 0 <= idxFr && idxFr < graph->getSizeAttributes() &&
         0 <= idxTo && idxTo < graph->getSizeAttributes() )
    {
        graph->moveAttribute( idxFr, idxTo );
        displAttributes();
        frame->selectAttribute( idxTo );
    }
}


// ------------------------------------------------------------------
void DiaGraph::handleAttributeDuplicate( const vector< int > &indcs )
// ------------------------------------------------------------------
{
    // duplicate attributes
    graph->duplAttributes( indcs );

    // display attributes
    displAttributes();
}

/*
// ---------------------------------------------------------------
void DiaGraph::handleAttributeDelete( const vector< int > &indcs )
// ---------------------------------------------------------------
{
    // reset simulator, timeSeries & examiner
    if ( simulator != NULL )
    {
        simulator->clearData();
        simulator->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_SIM && canvasSiml != NULL ) )
            canvasSiml->Refresh();
    }

    if ( timeSeries != NULL )
    {
        timeSeries->clearData();
        timeSeries->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_TRACE && canvasSiml != NULL ) )
            canvasTrace->Refresh();
    }

    if ( examiner != NULL )
    {
        examiner->clearData();
        examiner->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }

    // delete attributes
    for ( int i = 0; i < indcs.size(); ++i )
    {
        editor->clearLinkAttrDOF( indcs[i] );
        graph->deleteAttribute( indcs[i] );
    }

    // display attributes
    displAttributes();
    clearAttrDomain();
}
*/

// ---------------------------------------------------
void DiaGraph::handleAttributeDelete( const int &idx )
// ---------------------------------------------------
{
    // reset simulator, timeSeries & examiner
    if ( simulator != NULL )
    {
        simulator->clearData();
        simulator->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_SIM && canvasSiml != NULL ) )
            canvasSiml->Refresh();
    }

    if ( timeSeries != NULL )
    {
        timeSeries->clearData();
        timeSeries->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_TRACE && canvasSiml != NULL ) )
            canvasTrace->Refresh();
    }

    if ( examiner != NULL )
    {
        examiner->clearData();
        examiner->setDiagram( editor->getDiagram() );

        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }

    // update cluster and time series if necessary
    Attribute* attr = graph->getAttribute( idx );
    // get attributes currently clustered on
    int posFoundClust = -1;
    vector< int > attrsClust;
    arcDgrm->getAttrsTree( attrsClust );
    for ( size_t i = 0; i < attrsClust.size(); ++i )
    {
        if ( attrsClust[i] == attr->getIndex() )
        {
            posFoundClust = i;
            break;
        }
    }

    // recluster if necessary
    if ( posFoundClust >= 0 )
    {
        attrsClust.erase( attrsClust.begin() + posFoundClust );
        handleAttributeCluster( attrsClust );
    }

    // get attributes currently in time series
    int posFoundTimeSeries = -1;
    vector< int > attrsTimeSeries;
    timeSeries->getAttrIdcs( attrsTimeSeries );
    for ( size_t i = 0; i < attrsTimeSeries.size(); ++i )
    {
        if ( attrsTimeSeries[i] == attr->getIndex() )
        {
            posFoundTimeSeries = i;
            break;
        }
    }

    // re-initiate time series if necessary
    if ( posFoundTimeSeries >= 0 )
    {
        attrsTimeSeries.erase( attrsTimeSeries.begin() + posFoundTimeSeries );
        initTimeSeries( attrsTimeSeries );
    }

    // display results
    displAttributes();
    displAttrDomain( attr->getIndex() );
    attr = NULL;

    // delete attribute
    editor->clearLinkAttrDOF( idx );
    graph->deleteAttribute( idx );

    // display attributes
    displAttributes();
    clearAttrDomain();
}


// ----------------------------------
void DiaGraph::handleAttributeRename(
        const int &idx,
        const std::string &name )
// ----------------------------------
{
    if ( 0 <= idx && idx < graph->getSizeAttributes() )
    {
        graph->getAttribute( idx )->setName( name );
        displAttributes();
    }
}


// ----------------------------------------------------------------
void DiaGraph::handleAttributeCluster( const vector< int > &indcs )
// ----------------------------------------------------------------
{
    bool zeroCard = false;
    clustered = true;

    for ( size_t i = 0; i < indcs.size() && zeroCard == false; ++i )
    {
        if ( graph->getAttribute( indcs[i] )->getSizeCurValues() == 0 )
            zeroCard = true;
    }

    if ( indcs.size() > 0 )
    {
        if ( zeroCard == true )
        {
            wxLogError(
                wxString( wxT("Error clustering.\nAt least one attribute has no domain defined.") ) );
        }
        else
        {
            critSect = true;

            graph->clustNodesOnAttr( indcs );
            arcDgrm->setAttrsTree( indcs );

            arcDgrm->setDataChanged( true );
            handleMarkFrameClust( timeSeries );

            critSect = false;

            if ( canvasArcD != NULL && mode == MODE_ANALYSIS )
                canvasArcD->Refresh();
        }
    }
    else
    {
        vector< int > coord;
        coord.push_back( 0 );

        critSect = true;

        graph->clearSubClusters( coord );
        vector< int > empty;
        arcDgrm->setAttrsTree( empty );
        arcDgrm->setDataChanged( true );

        critSect = false;

        if ( canvasArcD != NULL && mode == MODE_ANALYSIS )
            canvasArcD->Refresh();
    }
}


// -----------------------------------------------------
void DiaGraph::handleAttrPartition( const int &attrIdx )
// -----------------------------------------------------
{
    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        Attribute* attr  = graph->getAttribute( attrIdx );
        if ( attr != NULL )
        {
            tempAttr = attr;
            frame->displAttrInfoPart(
                tempAttr->getName(),
                0,
                graph->getSizeNodes(),
                attr->getSizeCurValues() );

            attr = NULL;
        }
    }
}


// --------------------------------
void DiaGraph::handleAttrPartition(
    const int &numParts,
    const int &method )
// --------------------------------
{
    if ( tempAttr != NULL )
    {
        if ( tempAttr->getAttrType() == Attribute::ATTR_TYPE_CONTI )
        {
            // get attributes currently clustered on
            int posFound;
            vector< int > attrsClust;

            posFound = -1;
            arcDgrm->getAttrsTree( attrsClust );
            for ( size_t i = 0; i < attrsClust.size(); ++i )
            {
                if ( attrsClust[i] == tempAttr->getIndex() )
                {
                    posFound = i;
                    break;
                }
            }

            // get attributes currently in time series
            vector< int > attrsTimeSeries;
            timeSeries->getAttrIdcs( attrsTimeSeries );

            // perform partitioning
            if ( method == Attribute::PART_METH_EQUAL_INTERVALS )
                tempAttr->classifyEqualIntervals( numParts );
            else if ( method == Attribute::PART_METH_QUANTILES )
                tempAttr->classifyQuantiles( numParts );
            else if ( method == Attribute::PART_METH_MEAN_STANDARD_DEVIATION )
                tempAttr->classifyMeanStandardDeviation( numParts );

            // recluster if necessary
            if ( posFound >= 0 )
            {
                if ( tempAttr->getSizeCurValues() == 0 )
                    attrsClust.erase( attrsClust.begin() + posFound );
                handleAttributeCluster( attrsClust );
            }

            // re-initiate time series if necessary
            if ( attrsTimeSeries.size() > 0 )
                initTimeSeries( attrsTimeSeries );

            // display results
            displAttributes( tempAttr->getIndex() );
            displAttrDomain( tempAttr->getIndex() );
        }
        tempAttr = NULL;
    }
}


// -------------------------------------------------------
void DiaGraph::handleAttrDepartition( const int &attrIdx )
// -------------------------------------------------------
{
    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        Attribute* attr = graph->getAttribute( attrIdx );
        if ( attr->getAttrType() == Attribute::ATTR_TYPE_CONTI )
        {
            // get attributes currently clustered on
            int posFound;
            vector< int > attrsClust;

            posFound = -1;
            arcDgrm->getAttrsTree( attrsClust );
            for ( size_t i = 0; i < attrsClust.size(); ++i )
            {
                if ( attrsClust[i] == attr->getIndex() )
                {
                    posFound = i;
                    break;
                }
            }

            // get attributes currently in time series
            vector< int > attrsTimeSeries;
            timeSeries->getAttrIdcs( attrsTimeSeries );

            // perform departitioning
            attr->removeClassification();

            // recluster if necessary
            if ( posFound >= 0 )
            {
                attrsClust.erase( attrsClust.begin() + posFound );
                handleAttributeCluster( attrsClust );
            }

            // re-initiate time series if necessary
            if ( attrsTimeSeries.size() > 0 )
                initTimeSeries( attrsTimeSeries );

            // display results
            displAttributes();
            displAttrDomain( attr->getIndex() );
        }
        attr = NULL;
    }
}

// -------------------------------------------
void DiaGraph::handleAttrPartitionCloseFrame()
// -------------------------------------------
{
    tempAttr = NULL;
}


// --------------------------
void DiaGraph::getAttrValues(
    const int &attrIdx,
    vector< double > &vals )
// --------------------------
{
    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        vals.clear();
        Node* node;
        for ( int i = 0; i < graph->getSizeNodes(); ++i )
        {
            node = graph->getNode( i );
            vals.push_back( node->getTupleVal( attrIdx ) );
        }
        node = NULL;
    }
}


// --------------------------
void DiaGraph::getAttrValues(
    const int &attrIdx,
    set< double > &vals )
// --------------------------
{
    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        vals.clear();
        Node* node;
        for ( int i = 0; i < graph->getSizeNodes(); ++i )
        {
            node = graph->getNode( i );
            vals.insert( node->getTupleVal( attrIdx ) );
        }
        node = NULL;
    }
}


// -----------------------------
void DiaGraph::handleMoveDomVal(
    const int &idxAttr,
    const int &idxFr,
    const int &idxTo )
// -----------------------------
{
    if ( 0 <= idxAttr && idxAttr < graph->getSizeAttributes() )
    {
        Attribute* attr = graph->getAttribute( idxAttr );

        if ( attr->getAttrType() == Attribute::ATTR_TYPE_DISCR )
        {
            if ( 0 <= idxFr && idxFr < attr->getSizeCurValues() &&
                 0 <= idxTo && idxTo < attr->getSizeCurValues() )
            {
                attr->moveValue( idxFr, idxTo );
                displAttrDomain( idxAttr );
                frame->selectDomainVal( idxTo );
            }
        }
        else if ( attr->getAttrType() == Attribute::ATTR_TYPE_CONTI )
        {
            if ( attr->getSizeCurValues() > 0 )
            {
                if ( 0 <= idxFr && idxFr < attr->getSizeCurValues() &&
                     0 <= idxTo && idxTo < attr->getSizeCurValues() )
                {
                    attr->moveValue( idxFr, idxTo );
                    displAttrDomain( idxAttr );
                    frame->selectDomainVal( idxTo );
                }
            }
        }

        attr = NULL;
    }
}


// ------------------------------
void DiaGraph::handleDomainGroup(
    const int &attrIdx,
    const vector< int > domIndcs,
    const std::string &newValue )
// ------------------------------
{
    Attribute *attr;

    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        attr = graph->getAttribute( attrIdx );
        attr->clusterValues( domIndcs, newValue );

        displAttributes();
        frame->selectAttribute( attrIdx );
        displAttrDomain( attrIdx );
    }

    attr = NULL;
}


// -----------------------------------------------------
void DiaGraph::handleDomainUngroup( const int &attrIdx )
// -----------------------------------------------------
{
    Attribute *attr;

    if ( 0 <= attrIdx && attrIdx < graph->getSizeAttributes() )
    {
        attr = graph->getAttribute( attrIdx );
        attr->clearClusters();

        displAttributes();
        frame->selectAttribute( attrIdx );
        displAttrDomain( attrIdx );
    }

    attr = NULL;
}


// ------------------------------
void DiaGraph::getAttributeNames(
    const vector< int > &indcs,
    vector< wxString > &names )
// ------------------------------
{
    names.clear();
    for ( size_t i = 0; i < indcs.size(); ++i )
    {
        if ( 0 <= indcs[i] && indcs[i] < graph->getSizeAttributes() )
            names.push_back(
				wxString(
					graph->getAttribute( indcs[i] )->getName().c_str(),
					wxConvUTF8 ) );
    }
}


// ---------------------------------------------
int DiaGraph::getAttributeType( const int &idx )
// ---------------------------------------------
{
    int result = -1;
    if ( 0 <= idx && idx < graph->getSizeAttributes() )
        result = graph->getAttribute( idx )->getAttrType();
    return result;
}


// -------------------------------------------------
int DiaGraph::getAttrSizeCurDomain( const int &idx )
// -------------------------------------------------
{
    int result = 0;
    if ( 0 <= idx && idx < graph->getSizeAttributes() )
        result = graph->getAttribute( idx )->getSizeCurValues();
    return result;
}


// -- attribute plots -----------------------------------------------


// -------------------------------------------------
void DiaGraph::handleAttributePlot( const int &idx )
// -------------------------------------------------
{
    if ( canvasDistr == NULL )
    {
        canvasDistr = frame->getCanvasDistr();
        distrPlot = new DistrPlot(
            this,
            graph,
            canvasDistr );
        distrPlot->setDiagram( editor->getDiagram() );
    }

    vector< int > number;

    // display domain
    displAttrDomain( idx );

    // visualize distribution of domain
    graph->calcAttrDistr( idx, number );
    distrPlot->setValues(
        idx,
        number );
    canvasDistr->Refresh();
}


// --------------------------------
void DiaGraph::handleAttributePlot(
    const int &idx1,
    const int &idx2 )
// --------------------------------
{
    if ( canvasCorrl == NULL )
    {
        canvasCorrl = frame->getCanvasCorrl();
        corrlPlot = new CorrlPlot(
            this,
            graph,
            canvasCorrl );
        corrlPlot->setDiagram( editor->getDiagram() );
    }

    vector< int > indices;
    vector< std::string > vals1;
    vector< vector< int > > corrlMap;
    vector< vector< int > > number;

    // display correlation between 2 attr's
    graph->calcAttrCorrl(
        idx1,
        idx2,
        corrlMap,
        number );
    corrlPlot->setValues(
        idx1,
        idx2,
        corrlMap,
        number );
    canvasCorrl->Refresh();
}


// -------------------------------------------------------------
void DiaGraph::handleAttributePlot( const vector< int > &indcs )
// -------------------------------------------------------------
{
   if ( canvasCombn == NULL )
    {
        canvasCombn = frame->getCanvasCombn();
        combnPlot = new CombnPlot(
            this,
            graph,
            canvasCombn );
        combnPlot->setDiagram( editor->getDiagram() );
    }
    /*
    int combinations     = 1;
    Attribute* attribute = NULL;
    int cardinality      = 0;

    for ( int i = 0; i < indcs.size(); ++i )
    {
        attribute   = graph->getAttribute( indcs[i] );
        cardinality = attribute->getSizeCurValues();
        if ( cardinality > 0 )
            combinations *= cardinality;
    }
    attribute = NULL;
    */
    /*
    if ( combinations < 2048 )
    {
    */
        vector< vector< int > > combs;
        vector< int > number;

        if ( indcs.size() > 0 )
        {
            clearAttrDomain();

            graph->calcAttrCombn(
                indcs,
                combs,
                number );

            combnPlot->setValues(
                indcs,
                combs,
                number );
            canvasCombn->Refresh();
        }
    /*
    }
    else
    {
    std::string msg;
        msg.append( "More than 2048 combinations. " );
        msg.append( "Please select fewer attributes " );
        msg.append( "or group their domains." );
        wxLogError( wxString( msg->c_str(), wxConvUTF8 ) );
    }
    */
}


// ------------------------------------
void DiaGraph::handlePlotFrameDestroy()
// ------------------------------------
{
    if ( distrPlot != NULL )
    {
        delete distrPlot;
        distrPlot = NULL;
    }
    canvasDistr = NULL;

    if ( corrlPlot != NULL )
    {
        delete corrlPlot;
        corrlPlot = NULL;
    }
    canvasCorrl = NULL;

    if ( combnPlot != NULL )
    {
        delete combnPlot;
        combnPlot = NULL;
    }
    canvasCombn = NULL;
}


// -----------------------------------------
void DiaGraph::handleEditClust( Cluster* c )
// -----------------------------------------
{
    frame->displClustMenu();
    tempClust = c;
}


// -------------------------------------
void DiaGraph::handleClustFrameDisplay()
// -------------------------------------
{
    vector< int >    attrIdcs;
    vector< std::string > attrNames;

    for ( int i = 0; i < graph->getSizeAttributes(); ++i )
    {
        attrIdcs.push_back( graph->getAttribute(i)->getIndex() );
        attrNames.push_back( graph->getAttribute(i)->getName() );
    }

    frame->displAttrInfoClust(
        attrIdcs,
        attrNames );
}


// ---------------------------------------------------------
void DiaGraph::handleClustPlotFrameDisplay( const int &idx )
// ---------------------------------------------------------
{
    if ( canvasDistr == NULL )
    {
        canvasDistr = frame->getCanvasDistr();
        distrPlot = new DistrPlot(
            this,
            graph,
            canvasDistr );
    }

    // visualize distribution of domain
    /*
    vector< int > distr;
    graph->calcAttrDistr( tempClust, idx, distr );
    distrPlot->setValues(
        idx,
        distr );
    canvasDistr->Refresh();
    */
}


// ----------------------------------------
void DiaGraph::handleClustPlotFrameDisplay(
    const int &idx1,
    const int &idx2 )
// ----------------------------------------
{
    if ( canvasCorrl == NULL )
    {
        canvasCorrl = frame->getCanvasCorrl();
        corrlPlot = new CorrlPlot(
            this,
            graph,
            canvasCorrl );
    }

    vector< int > indices;
    vector< std::string > vals1;
    vector< vector< int > > corrlMap;
    vector< vector< int > > number;

    // display correlation between 2 attr's
    graph->calcAttrCorrl(
        tempClust,
        idx1,
        idx2,
        corrlMap,
        number );
    corrlPlot->setValues(
        idx1,
        idx2,
        corrlMap,
        number );
    canvasCorrl->Refresh();

}


// ---------------------------------------------------------------------
void DiaGraph::handleClustPlotFrameDisplay( const vector< int > &indcs )
// ---------------------------------------------------------------------
{
    if ( canvasCombn == NULL )
    {
        canvasCombn = frame->getCanvasCombn();
        combnPlot = new CombnPlot(
            this,
            graph,
            canvasCombn );
    }

    vector< vector< int > > combs;
    vector< int > number;

    if ( indcs.size() > 0 )
    {
        graph->calcAttrCombn(
            tempClust,
            indcs,
            combs,
            number );

        combnPlot->setValues(
            indcs,
            combs,
            number );
        canvasCombn->Refresh();
    }
}


// ----------------------------------------
void DiaGraph::setClustMode( const int &m )
// ----------------------------------------
{
    clustMode = m;
}


// -------------------------
int DiaGraph::getClustMode()
// -------------------------
{
    return clustMode;
}


// -- global mode changes -------------------------------------------


// -----------------------------------
void DiaGraph::handleSetModeAnalysis()
// -----------------------------------
{
    mode = MODE_ANALYSIS;
    if ( editor != NULL )
    {
        editor->setEditModeSelect();
        editor->deselectAll();
    }

    frame->clearDOFInfo();
    canvasColChooser = NULL;
    if ( colChooser != NULL )
    {
        delete colChooser;
        colChooser = NULL;
    }
    canvasOpaChooser = NULL;
    if ( opaChooser != NULL )
    {
        delete opaChooser;
        opaChooser = NULL;
    }
    frame->setEditModeSelect();

    if ( arcDgrm != NULL )
        arcDgrm->updateDiagramData();

    if ( canvasExnr != NULL )
        canvasExnr->Refresh();
}


// -------------------------------
void DiaGraph::handleSetModeEdit()
// -------------------------------
{
    mode = MODE_EDIT;
    editor->reGenText();

    if ( canvasExnr != NULL )
        canvasExnr->Refresh();
}


// --------------------
int DiaGraph::getMode()
// --------------------
{
    return mode;
}


// ------------------------------
void DiaGraph::handleSetViewSim()
// ------------------------------
{
    view = VIEW_SIM;
    if ( arcDgrm != NULL )
        arcDgrm->unmarkLeaves();
    canvasArcD->Refresh();
}


// --------------------------------
void DiaGraph::handleSetViewTrace()
// --------------------------------
{
    view = VIEW_TRACE;
    handleMarkFrameClust( timeSeries );
    canvasArcD->Refresh();
}


// --------------------
int DiaGraph::getView()
// --------------------
{
    return view;
}


// --------------------
bool DiaGraph::getClustered()
// --------------------
{
    return clustered;
}



// -- diagram editor ------------------------------------------------


// ----------------------------------
void* DiaGraph::getGraph()
// ----------------------------------
{
    return graph;
}


// ----------------------------------
void DiaGraph::handleNote( const int &shapeId, const std::string &msg )
// ----------------------------------
{
    frame->handleNote( shapeId, msg );
}


// ----------------------------------
void DiaGraph::handleEditModeSelect()
// ----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeSelect();

    if( colChooser != NULL )
    {
        delete colChooser;
        colChooser = NULL;
    }
    canvasColChooser = NULL;

    if ( opaChooser != NULL )
    {
        delete opaChooser;
        opaChooser = NULL;
    }
    canvasOpaChooser = NULL;
}


// ----------------------------------
void DiaGraph::handleEditModeNote()
// ----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeNote();
}


// ---------------------------------------------
void DiaGraph::handleEditModeDOF( Colleague* c )
// ---------------------------------------------
{
    if ( c == frame )
    {
        if ( mode == MODE_EDIT && editor != NULL )
            editor->setEditModeDOF();
    }
    else if ( c == editor )
    {
        frame->setEditModeDOF();
    }
}


// --------------------------------
void DiaGraph::handleEditModeRect()
// --------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeRect();
}


// -----------------------------------
void DiaGraph::handleEditModeEllipse()
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeEllipse();
}


// --------------------------------
void DiaGraph::handleEditModeLine()
// --------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeLine();
}


// ---------------------------------
void DiaGraph::handleEditModeArrow()
// ---------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeArrow();
}


// ----------------------------------
void DiaGraph::handleEditModeDArrow()
// ----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setEditModeDArrow();
}


// -----------------------------------
void DiaGraph::handleEditModeFillCol()
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setFillCol();
}


// -----------------------------------
void DiaGraph::handleEditModeLineCol()
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setLineCol();
}


// --------------------------------------------------
void DiaGraph::handleEditShowGrid( const bool &flag )
// --------------------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setShowGrid( flag );
}


// --------------------------------------------------
void DiaGraph::handleEditSnapGrid( const bool &flag )
// --------------------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->setSnapGrid( flag );
}


// ----------------------------
void DiaGraph::handleEditShape(
    const bool &cut,
    const bool &copy,
    const bool &paste,
    const bool &clear,
    const bool &bringToFront,
    const bool &sendToBack,
    const bool &bringForward,
    const bool &sendBackward,
    const bool &editDOF,
    const int  &checkedItem )
// ----------------------------
{
    frame->displShapeMenu(
        cut,
        copy,
        paste,
        clear,
        bringToFront,
        sendToBack,
        bringForward,
        sendBackward,
        editDOF,
        checkedItem );
}


// ----------------------------
void DiaGraph::handleShowVariable(
    			const std::string &variable,
    			const int    &variableId)
// ----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleShowVariable( variable, variableId );
}


// ----------------------------
void DiaGraph::handleShowNote( const std::string &variable, const int &shapeId)
// ----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleShowNote( variable, shapeId );
}


// ----------------------------
void DiaGraph::handleAddText( std::string &variable, int &shapeId)
// ----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        return editor->handleAddText( variable, shapeId);
}


// ----------------------------
void DiaGraph::handleTextSize( int &textSize, int &shapeId )
// ----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        return editor->handleTextSize( textSize, shapeId);
}


// ----------------------------
void DiaGraph::handleSetTextSize( int &textSize, int &shapeId )
// ----------------------------
{
	if ( mode == MODE_EDIT && editor != NULL )
        	editor->handleSetTextSize( textSize, shapeId );
}


// ----------------------------
void DiaGraph::handleCutShape()
// ----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleCut();
}


// -----------------------------
void DiaGraph::handleCopyShape()
// -----------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleCopy();
}


// ------------------------------
void DiaGraph::handlePasteShape()
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handlePaste();
}


// -------------------------------
void DiaGraph::handleDeleteShape()
// -------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDelete();
}


// -------------------------------------
void DiaGraph::handleBringToFrontShape()
// -------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleBringToFront();
}


// -----------------------------------
void DiaGraph::handleSendToBackShape()
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleSendToBack();
}


// -------------------------------------
void DiaGraph::handleBringForwardShape()
// -------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleBringForward();
}


// -------------------------------------
void DiaGraph::handleSendBackwardShape()
// -------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleSendBackward();
}


// --------------------------------
void DiaGraph::handleEditDOFShape()
// --------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
    {
        editor->handleEditDOF();
    }
}


// --------------------------------
void DiaGraph::handleSetDOF( const int &attrIdx )
// --------------------------------
{
	if ( mode == MODE_EDIT && editor != NULL )
	{
        	editor->handleSetDOF( attrIdx );
	}
}


// --------------------------------
void DiaGraph::handleCheckedVariable( const int &idDOF, const int &variableId )
// --------------------------------
{
	if ( mode == MODE_EDIT && editor != NULL )
	{
        	editor->handleCheckedVariable( idDOF, variableId );
	}
}

// ------------------------------------
void DiaGraph::handleEditDOF(
    const vector< int > &degsOfFrdmIds,
    const vector< std::string > &degsOfFrdm,
    const vector< int > &attrIndcs,
    const int &selIdx )
// ------------------------------------
{
    // init attrIndcs
    vector< std::string > attributes;
    {
    for ( size_t i = 0; i < attrIndcs.size(); ++i )
    {
        if ( attrIndcs[i] < 0 )
            attributes.push_back( "" );
        else
            attributes.push_back(
                graph->getAttribute(
                    attrIndcs[i] )->getName() );
    }
    }

    frame->displDOFInfo(
        degsOfFrdmIds,
        degsOfFrdm,
        attributes,
        selIdx );

    // color chooser
    if ( colChooser != NULL )
    {
        delete colChooser;
        colChooser = NULL;
    }
    canvasColChooser = frame->getCanvasColDOF();
    colChooser = new ColorChooser(
        this,
        graph,
        canvasColChooser );
	canvasColChooser->Refresh();

    // opacity chooser
    if ( opaChooser != NULL )
    {
        delete opaChooser;
        opaChooser = NULL;
    }
    canvasOpaChooser = frame->getCanvasOpaDOF();
    opaChooser = new OpacityChooser(
        this,
        graph,
        canvasOpaChooser );
		canvasOpaChooser->Refresh();
}

// -------------------------------------------
void DiaGraph::setDOFColorSelected( )
// -------------------------------------------
// ------------------------------------------------------------------
// Select the Color Item in the Edit DOF Menu
// ------------------------------------------------------------------
{
    frame->setDOFColorSelected();
}

// -------------------------------------------
void DiaGraph::setDOFOpacitySelected( )
// -------------------------------------------
// ------------------------------------------------------------------
// Select the Opacity Item in the Edit DOF Menu
// ------------------------------------------------------------------
{
    frame->setDOFOpacitySelected();
}


// ---------------------------------------------
void DiaGraph::handleDOFSel( const int &DOFIdx )
// ---------------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFSel( DOFIdx );
}


// -----------------------------------
void DiaGraph::handleSetDOFTextStatus(
    const int &DOFIdx,
    const int &status )
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFSetTextStatus( DOFIdx, status );
}


// ------------------------------------------------------
int DiaGraph::handleGetDOFTextStatus( const int &DOFIdx )
// ------------------------------------------------------
{
    int result = -1;
    if ( mode == MODE_EDIT && editor != NULL )
        result = editor->handleDOFGetTextStatus( DOFIdx );
    return result;
}


// ----------------------------------
void DiaGraph::handleDOFColActivate()
// ----------------------------------
{
    if ( colChooser != NULL )
    {
        colChooser->setActive( true );

        if ( canvasColChooser != NULL )
            canvasColChooser->Refresh();
    }
}


// ------------------------------------
void DiaGraph::handleDOFColDeactivate()
// ------------------------------------
{
    if ( colChooser != NULL )
    {
        colChooser->setActive( false );

        if ( canvasColChooser != NULL )
            canvasColChooser->Refresh();
    }
}


// ------------------------------
void DiaGraph::handleDOFColAdd(
    const double &hue,
    const double &y )
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFColAdd( hue, y );
}


// -------------------------------
void DiaGraph::handleDOFColUpdate(
    const int &idx,
    const double &hue,
    const double &y )
// -------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFColUpdate( idx, hue, y );
}


// ------------------------------
void DiaGraph::handleDOFColClear(
    const int &idx )
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
       editor->handleDOFColClear( idx );
}


// -------------------------------------
void DiaGraph::handleDOFColSetValuesEdt(
    const vector< double > &hue,
    const vector< double > &y )
// -------------------------------------
{
    if ( colChooser != NULL )
    {
        colChooser->setPoints( hue, y );
    }
}


// ----------------------------------
void DiaGraph::handleDOFOpaActivate()
// ----------------------------------
{
    if ( opaChooser != NULL )
    {
        opaChooser->setActive( true );

        if ( canvasOpaChooser != NULL )
            canvasOpaChooser->Refresh();
    }
}


// ------------------------------------
void DiaGraph::handleDOFOpaDeactivate()
// ------------------------------------
{
    if ( opaChooser != NULL )
    {
        opaChooser->setActive( false );

        if ( canvasOpaChooser != NULL )
            canvasOpaChooser->Refresh();
    }
}


// ------------------------------
void DiaGraph::handleDOFOpaAdd(
    const double &opa,
    const double &y )
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFOpaAdd( opa, y );
}


// -------------------------------
void DiaGraph::handleDOFOpaUpdate(
    const int &idx,
    const double &opa,
    const double &y )
// -------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->handleDOFOpaUpdate( idx, opa, y );
}


// ------------------------------
void DiaGraph::handleDOFOpaClear(
    const int &idx )
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
       editor->handleDOFOpaClear( idx );
}


// -------------------------------------
void DiaGraph::handleDOFOpaSetValuesEdt(
    const vector< double > &opa,
    const vector< double > &y )
// -------------------------------------
{
    if ( opaChooser != NULL )
    {
        opaChooser->setPoints( opa, y );
    }
}


// ------------------------------
void DiaGraph::handleLinkDOFAttr(
    const int DOFIdx,
    const int attrIdx )
// ------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
    {
        /*
        if ( graph->getAttribute( attrIdx )->getSizeCurValues() == 0 )
            wxLogError(
                wxString( "Error linking DOF.\nAt least one attribute has an undefined or real-valued domain." ) );
        else
        */
            editor->setLinkDOFAttr( DOFIdx, attrIdx );
    }
}


// ---------------------------------------------------
void DiaGraph::handleUnlinkDOFAttr( const int DOFIdx )
// ---------------------------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
        editor->clearLinkDOFAttr( DOFIdx );
}


// -----------------------------------
void DiaGraph::handleDOFFrameDestroy()
// -----------------------------------
{
    if ( mode == MODE_EDIT && editor != NULL )
    {
        editor->setEditModeSelect();
        editor->deselectAll();
        frame->setEditModeSelect();
    }

    // color chooser
    canvasColChooser = NULL;
    delete colChooser;
    colChooser = NULL;

    // opacity chooser
    canvasOpaChooser = NULL;
    delete opaChooser;
    opaChooser = NULL;
}


// -------------------------------
void DiaGraph::handleDOFDeselect()
// -------------------------------
{
    frame->clearDOFInfo();
}


// -- simulator, time series & examiner -----------------------------

/*
// ------------------------------------
void DiaGraph::handleSetSimCurrFrame(
    Cluster* frame,
    const vector< Attribute* > &attrs )
// ------------------------------------
{
    if ( simulator != NULL )
    {
        simulator->setFrameCurr( frame, attrs );

        if ( mode == MODE_ANALYSIS && canvasPrev != NULL )
            canvasPrev->Refresh();
        if ( mode == MODE_ANALYSIS && canvasCurr != NULL )
            canvasCurr->Refresh();
        if ( mode == MODE_ANALYSIS && canvasNext != NULL )
            canvasNext->Refresh();
        if ( mode == MODE_ANALYSIS && canvasHist != NULL )
            canvasHist->Refresh();
    }
}
*/

// ------------------------------------
void DiaGraph::initSimulator(
    Cluster* currFrame,
    const vector< Attribute* > &attrs )
// ------------------------------------
{
    if ( simulator != NULL )
    {
        simulator->initFrameCurr( currFrame, attrs );

        if ( mode == MODE_ANALYSIS && canvasSiml != NULL )
            canvasSiml->Refresh();
    }
}


// ----------------------------------------------------------
void DiaGraph::initTimeSeries( const vector< int > attrIdcs )
// ----------------------------------------------------------
{
    if ( timeSeries != NULL )
        timeSeries->initAttributes( attrIdcs );

    if ( view == VIEW_TRACE && ( mode == MODE_ANALYSIS && canvasTrace != NULL ) )
            canvasTrace->Refresh();
}


// ---------------------------
void DiaGraph::markTimeSeries(
    Colleague* sender,
    Cluster* currFrame )
// ---------------------------
{
    if ( timeSeries != NULL )
    {
        timeSeries->markItems( currFrame );
        handleMarkFrameClust( timeSeries );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_TRACE && canvasTrace != NULL ) )
        {
            canvasArcD->Refresh();
            canvasTrace->Refresh();
        }
    }
}


// ----------------------------------
void DiaGraph::markTimeSeries(
    Colleague* sender,
    const vector< Cluster* > frames )
// ----------------------------------
{
    if ( timeSeries != NULL )
    {
        timeSeries->markItems( frames );
        handleMarkFrameClust( timeSeries );

        if ( mode == MODE_ANALYSIS && ( view == VIEW_TRACE && canvasTrace != NULL ) )
            canvasTrace->Refresh();
    }
}


// ------------------------------------
void DiaGraph::addToExaminer(
    Cluster* currFrame,
    const vector< Attribute* > &attrs )
// ------------------------------------
{
    if ( examiner != NULL )
    {
        examiner->addFrameHist( currFrame, attrs );

        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }
}


// ------------------------------------
void DiaGraph::addToExaminer(
    const vector< Cluster* > frames,
    const vector< Attribute* > &attrs )
// ------------------------------------
{
    if ( examiner != NULL )
    {
        for ( size_t i = 0; i < frames.size(); ++i )
            examiner->addFrameHist( frames[i], attrs );

        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }
}


void DiaGraph::handleShowClusterMenu()
{
	frame->displClusterMenu();
}


// ----------------------------
void DiaGraph::handleSendDgrm(
    Colleague* sender,
    const bool &sendSglToSiml,
    const bool &sendSglToTrace,
    const bool &sendSetToTrace,
    const bool &sendSglToExnr,
    const bool &sendSetToExnr )
// ----------------------------
{
    dgrmSender = sender;

    frame->displDgrmMenu(
        sendSglToSiml,
        sendSglToTrace,
        sendSetToTrace,
        sendSglToExnr,
        sendSetToExnr );
}


// -------------------------------------
void DiaGraph::handleSendDgrmSglToSiml()
// -------------------------------------
{
    if ( dgrmSender == arcDgrm )
        arcDgrm->handleSendDgrmSglToSiml();
    else if ( dgrmSender == simulator )
        *this << "Simulator sending single to siml\n";
    else if ( dgrmSender == examiner )
        examiner->handleSendDgrmSglToSiml();
}


// --------------------------------------
void DiaGraph::handleSendDgrmSglToTrace()
// --------------------------------------
{
    if ( dgrmSender == arcDgrm )
        arcDgrm->handleSendDgrmSglToTrace();
    else if ( dgrmSender == examiner )
        examiner->handleSendDgrmSglToTrace();
}


// --------------------------------------
void DiaGraph::handleSendDgrmSetToTrace()
// --------------------------------------
{
    if ( dgrmSender == arcDgrm )
        arcDgrm->handleSendDgrmSetToTrace();
    else if ( dgrmSender == examiner )
        examiner->handleSendDgrmSetToTrace();
}


// -------------------------------------
void DiaGraph::handleSendDgrmSglToExnr()
// -------------------------------------
{
    if ( dgrmSender == arcDgrm )
        arcDgrm->handleSendDgrmSglToExnr();
    else if ( dgrmSender == simulator )
        simulator->handleSendDgrmSglToExnr();
    else if ( dgrmSender == timeSeries )
        timeSeries->handleSendDgrmSglToExnr();
}


// -------------------------------------
void DiaGraph::handleSendDgrmSetToExnr()
// -------------------------------------
{
    if ( dgrmSender == arcDgrm )
        arcDgrm->handleSendDgrmSetToExnr();
}


// -----------------------------------------------
void DiaGraph::handleClearSim( Colleague* sender )
// -----------------------------------------------
{
    if ( sender == simulator )
    {
        frame->displSimClearDlg();
    }
    else if ( sender == frame )
    {
        if ( arcDgrm != NULL )
        {
            if ( mode == MODE_ANALYSIS && canvasArcD != NULL )
                canvasArcD->Refresh();
        }

        if ( simulator != NULL )
        {
            simulator->clearData();

            if ( mode == MODE_ANALYSIS && canvasSiml != NULL )
                canvasSiml->Refresh();
        }
    }
}


// ------------------------------------------------
void DiaGraph::handleClearExnr( Colleague* sender )
// ------------------------------------------------
{
    if ( sender == examiner )
        frame->displExnrClearDlg();
    else if ( sender == frame )
    {
        if ( arcDgrm != NULL )
        {
            if ( mode == MODE_ANALYSIS && canvasArcD != NULL )
                canvasArcD->Refresh();
        }

        if ( examiner != NULL )
        {
            examiner->clrFrameHist();

            if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
                canvasExnr->Refresh();
        }
    }
}


// ---------------------------------------------------
void DiaGraph::handleClearExnrCur( Colleague* sender )
// ---------------------------------------------------
{
    if ( sender == examiner )
        frame->displExnrFrameMenu( true );
    else if ( sender == frame )
    {
        if ( examiner != NULL )
        {
            examiner->clrFrameHistCur();

            if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
                canvasExnr->Refresh();
        }
    }
}

/*
// -----------------------------------------------------
void DiaGraph::handleAnimFrameBundl( Colleague* sender )
// -----------------------------------------------------
{
    if ( arcDgrm != NULL && canvasArcD != NULL )
    {
        arcDgrm->unmarkLeaves();
        if ( sender == timeSeries )
        {
            int idx;
            set< int > idcs;
            ColorRGB col;

            timeSeries->getAnimIdxDgrm( idxLeaf, idxBndl, col );

//            arcDgrm->unmarkLeaves();
//            arcDgrm->unmarkBundles();
//            arcDgrm->markBundle( idxBndl );
//            canvasArcD->Refresh();
        }
    }
}
*/

// -----------------------------------------------------
void DiaGraph::handleAnimFrameClust( Colleague* sender )
// -----------------------------------------------------
{
    if ( arcDgrm != NULL && canvasArcD != NULL )
    {
        arcDgrm->unmarkLeaves();
        if ( sender == timeSeries )
        {
            int idx;
            set< int > idcs;
            ColorRGB col;

            timeSeries->getAnimIdxDgrm( idx, idcs, col );

            arcDgrm->unmarkBundles();
            arcDgrm->unmarkLeaves();
            for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                arcDgrm->markBundle( *it );
            arcDgrm->markLeaf( idx, col );

            canvasArcD->Refresh();
        }
    }
}


// ------------------------------------------------------
void DiaGraph::handleMarkFrameClust( Colleague* sender  )
// ------------------------------------------------------
{
    if ( arcDgrm != NULL )
    {
        arcDgrm->unmarkLeaves();
        if ( sender == simulator )
        {
            ColorRGB col = simulator->getColorSel();
            int      idx = simulator->getIdxClstSel();

            arcDgrm->markLeaf( idx, col );
        }
        else if ( sender == timeSeries )
        {
            ColorRGB col;
            set< int > idcs;
            int idx;

            // clear bundles
            arcDgrm->unmarkBundles();

            // marked nodes
            timeSeries->getIdcsClstMarked( idcs, col );
            for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                arcDgrm->markLeaf( *it, col );

            // mouse over
            timeSeries->getIdxMseOver( idx, idcs, col );
            if ( idx >= 0 )
            {
                arcDgrm->markLeaf( idx, col );
                for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                    arcDgrm->markBundle( *it );
            }

            // current diagram
            timeSeries->getCurrIdxDgrm( idx, idcs, col );
            if ( idx >= 0 )
            {
                arcDgrm->markLeaf( idx, col );
                for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                    arcDgrm->markBundle( *it );
            }

            // examiner view
            idx = examiner->getIdxClstSel();
            if ( idx >= 0 )
            {
                col = examiner->getColorSel();
                arcDgrm->markLeaf( idx, col );
            }

        }
        else if ( sender == examiner )
        {
            ColorRGB col;
            int      idx;

            // trace view
            if ( view == VIEW_TRACE )
            {
                set< int > idcs;
                timeSeries->getIdcsClstMarked( idcs, col );
                for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                    arcDgrm->markLeaf( *it, col );
            }

            // examiner view
            col = examiner->getColorSel();
            idx = examiner->getIdxClstSel();
            arcDgrm->markLeaf( idx, col );
        }
    }
}


// --------------------------------------------------------
void DiaGraph::handleUnmarkFrameClusts( Colleague* sender )
// --------------------------------------------------------
{
    if ( arcDgrm != NULL )
    {
        arcDgrm->unmarkLeaves();
        if ( sender == simulator )
        {
            ColorRGB col = examiner->getColorSel();
            int      idx = examiner->getIdxClstSel();

            arcDgrm->markLeaf( idx, col );
        }
        else if ( sender == examiner )
        {
            // trace view
            if ( view == VIEW_TRACE )
            {
                ColorRGB col;
                set< int > idcs;
                timeSeries->getIdcsClstMarked( idcs, col );

                for ( set< int >::iterator it = idcs.begin(); it != idcs.end(); ++it )
                    arcDgrm->markLeaf( *it, col );
            }
        }

        if ( mode == MODE_ANALYSIS && canvasArcD != NULL )
            canvasArcD->Refresh();
    }
}

// -----------------------------------
void DiaGraph::handleShowFrame(
    Cluster* frame,
    const vector< Attribute* > &attrs,
    ColorRGB &col )
// -----------------------------------
{
    if ( examiner != NULL )
    {
        examiner->setFrame( frame, attrs, col );
        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }
}


// -------------------------------
void DiaGraph::handleUnshowFrame()
// -------------------------------
{
    if ( examiner != NULL )
    {
        examiner->clrFrame();
        if ( mode == MODE_ANALYSIS && canvasExnr != NULL )
            canvasExnr->Refresh();
    }
}


// -- visualization settings ----------------------------------------


// -------------------------------
void DiaGraph::setSettingsGeneral(
    const wxColour &colClr,
    const wxColour &colTxt,
    const int &szeTxt,
    const double &spdAnim )
// -------------------------------
{
    ColorRGB colTmp;

    colTmp.r = colClr.Red()/255.0;
    colTmp.g = colClr.Green()/255.0;
    colTmp.b = colClr.Blue()/255.0;
    colTmp.a = 1.0;
    ArcDiagram::setColorClr( colTmp );
    Simulator::setColorClr( colTmp );
    TimeSeries::setColorClr( colTmp );
    Examiner::setColorClr( colTmp );

    colTmp.r = colTxt.Red()/255.0;
    colTmp.g = colTxt.Green()/255.0;
    colTmp.b = colTxt.Blue()/255.0;
    colTmp.a = 1.0;
    ArcDiagram::setColorTxt( colTmp );
    Simulator::setColorTxt( colTmp );
    TimeSeries::setColorTxt( colTmp );
    Examiner::setColorTxt( colTmp );

    ArcDiagram::setSizeTxt( szeTxt );
    Simulator::setSizeTxt( szeTxt );
    TimeSeries::setSizeTxt( szeTxt );
    Examiner::setSizeTxt( szeTxt );

    ArcDiagram::setIntervAnim( (int)(1000.0/spdAnim) );

    if ( mode == MODE_ANALYSIS )
    {
        if ( canvasArcD != NULL )
            canvasArcD->Refresh();
        if ( canvasSiml != NULL )
            canvasSiml->Refresh();
        if ( canvasExnr != NULL )
            canvasExnr->Refresh();
    }
}


// ---------------------------------
void DiaGraph::setSettingsClustTree(
    const bool &show,
    const bool &annotate,
    const int &colMap )
// ---------------------------------
{
    if ( arcDgrm != NULL )
    {
        if ( (show != arcDgrm->getShowTree()) |
             (annotate != arcDgrm->getAnnotateTree()) |
             (colMap != arcDgrm->getColorMap()) )
            arcDgrm->setGeomChanged( true );
    }

    ArcDiagram::setShowTree( show );
    ArcDiagram::setAnnotateTree( annotate );
    ArcDiagram::setColorMap( colMap );

    if ( canvasArcD != NULL && mode == MODE_ANALYSIS )
        canvasArcD->Refresh();
}


// -------------------------------
void DiaGraph::setSettingsBarTree(
    const bool &show,
    const double &magn )
// -------------------------------
{
    if ( arcDgrm != NULL )
    {
        if ( (show != arcDgrm->getShowBarTree()) |
             (magn != arcDgrm->getMagnBarTree()) )
            arcDgrm->setGeomChanged( true );
    }

    ArcDiagram::setShowBarTree( show );
    ArcDiagram::setMagnBarTree( magn );

    if ( canvasArcD != NULL && mode == MODE_ANALYSIS )
        canvasArcD->Refresh();
}


// --------------------------------------------------------
void DiaGraph::setSettingsSimulator( const int &blendType )
// --------------------------------------------------------
{
    Simulator::setBlendType( blendType );
}


// ------------------------------------------------------
void DiaGraph::setSettingsTrace( const bool &useShading )
// ------------------------------------------------------
{
    TimeSeries::setUseShading( useShading );

    if ( canvasTrace != NULL && view == VIEW_TRACE )
        canvasTrace->Refresh();
}


// ----------------------------------
void DiaGraph::setSettingsArcDiagram(
    const bool &showNodes,
    const bool &showArcs,
    const wxColour &colArcs,
    const double &trspArcs )
// ----------------------------------
{
    if ( arcDgrm != NULL )
    {
        arcDgrm->setGeomChanged( true );
    }

    ArcDiagram::setShowLeaves( showNodes );
    ArcDiagram::setShowBundles( showArcs );

    ColorRGB colTmp;
    colTmp.r = colArcs.Red()/255.0;
    colTmp.g = colArcs.Green()/255.0;
    colTmp.b = colArcs.Blue()/255.0;
    colTmp.a = 1.0 - trspArcs;
    ArcDiagram::setColorBundles( colTmp );

    if ( canvasArcD != NULL && mode == MODE_ANALYSIS )
        canvasArcD->Refresh();
}


// --------------------------------------------------
void DiaGraph::getSettingsGeneral(
    wxColour &colClr,
    wxColour &colTxt,
    int &szeTxt,
    double &spdAnim )
// --------------------------------------------------
{
    ColorRGB colTmp;

    colTmp = ArcDiagram::getColorClr();
    colClr.Set( (int)(255*colTmp.r), (int)(255*colTmp.g), (int)(255*colTmp.b) );

    colTmp = ArcDiagram::getColorTxt();
    colTxt.Set( (int)(255*colTmp.r), (int)(255*colTmp.g), (int)(255*colTmp.b) );

    szeTxt = ArcDiagram::getSizeTxt();

    spdAnim = 1000.0/ArcDiagram::getIntervAnim();
}


// ---------------------------------
void DiaGraph::getSettingsClustTree(
    bool &show,
    bool &annotate,
    int &colMap )
// ---------------------------------
{
    show = ArcDiagram::getShowTree();
    annotate = ArcDiagram::getAnnotateTree();
    colMap = ArcDiagram::getColorMap();
}


// -------------------------------
void DiaGraph::getSettingsBarTree(
    bool &show,
    double &magn )
// -------------------------------
{
    show = ArcDiagram::getShowBarTree();
    magn = ArcDiagram::getMagnBarTree();
}


// --------------------------------------------------
void DiaGraph::getSettingsSimulator( int &blendType )
// --------------------------------------------------
{
    blendType = Simulator::getBlendType();
}


// ------------------------------------------------
void DiaGraph::getSettingsTrace( bool &useShading )
// ------------------------------------------------
{
    useShading = TimeSeries::getUseShading();
}


// ----------------------------------
void DiaGraph::getSettingsArcDiagram(
    bool &showNodes,
    bool &showArcs,
    wxColour &colArcs,
    double &trspArcs )
// ----------------------------------
{
    showNodes = ArcDiagram::getShowLeaves();
    showArcs = ArcDiagram::getShowBundles();

    ColorRGB colTmp;
    colTmp = ArcDiagram::getColorBundles();
    colArcs.Set( (int)(255*colTmp.r), (int)(255*colTmp.g), (int)(255*colTmp.b) );
    trspArcs = 1.0-colTmp.a;
}


// -------------------------------
void DiaGraph::getGridCoordinates( double &xLeft, double &xRight, double &yTop, double &yBottom)
// -------------------------------
{
    if( editor != NULL )
    {
    	editor->getDiagram()->getGridCoordinates(xLeft, xRight, yTop, yBottom);
    }
}


// -- visualization -------------------------------------------------


// -------------------------------------------
void DiaGraph::handlePaintEvent( GLCanvas* c )
// -------------------------------------------
{
    if ( critSect != true )
    {
        if ( mode == MODE_EDIT )
        {
            // draw in render mode
            if ( c == canvasEdit && editor != NULL )
                editor->visualize( false );
            else if ( c == canvasColChooser && colChooser != NULL )
                colChooser->visualize( false );
            else if ( c == canvasOpaChooser && opaChooser != NULL )
                opaChooser->visualize( false );
            else if ( c != NULL)
                c->clear();
        }
        else if ( mode == MODE_ANALYSIS )
        {
            // draw in render mode
            if ( c == canvasArcD && arcDgrm != NULL )
                arcDgrm->visualize( false );
            else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
                simulator->visualize( false );
            else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
                timeSeries->visualize( false );
            else if ( c == canvasExnr && examiner != NULL )
                examiner->visualize( false );
            else if ( c == canvasDistr && distrPlot != NULL )
                distrPlot->visualize( false );
            else if ( c == canvasCorrl && corrlPlot != NULL )
                corrlPlot->visualize( false );
            else if ( c == canvasCombn && combnPlot != NULL )
                combnPlot->visualize( false );
            else if ( c!= NULL )
                c->clear();
        }
    }
}


// ------------------------------------------
void DiaGraph::handleSizeEvent( GLCanvas* c )
// ------------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        // draw in render mode
        if ( c == canvasEdit && editor != NULL )
            editor->handleSizeEvent();
    }
    else if ( mode == MODE_ANALYSIS )
    {
        // draw in render mode
        if ( c == canvasArcD && arcDgrm != NULL )
            arcDgrm->handleSizeEvent();
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
            simulator->handleSizeEvent();
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleSizeEvent();
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleSizeEvent();
        else if ( c == canvasDistr && distrPlot != NULL )
            distrPlot->handleSizeEvent();
        else if ( c == canvasCorrl && corrlPlot != NULL )
            corrlPlot->handleSizeEvent();
        else if ( c == canvasCombn && combnPlot != NULL )
            combnPlot->handleSizeEvent();
    }
}


// ---------------------------------------------
void DiaGraph::updateDependancies( GLCanvas* c )
// ---------------------------------------------
{
    if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasSiml )
            canvasArcD->Refresh();
    }
}


// -- input event handlers --------------------------------------


// ----------------------------
void DiaGraph::handleDragDrop(
    const int &srcWindowId,
    const int &tgtWindowId,
    const int &tgtX,
    const int &tgtY,
    const vector< int > &data )
// ----------------------------
{
    frame->handleDragDrop(
        srcWindowId,
        tgtWindowId,
        tgtX,
        tgtY,
        data );
}


// ------------------------------------
void DiaGraph::handleMouseLftDownEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// ------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseLftDownEvent( x, y );
        else if ( c == canvasColChooser && colChooser != NULL )
            colChooser->handleMouseLftDownEvent( x, y );
        else if ( c == canvasOpaChooser && opaChooser != NULL )
            opaChooser->handleMouseLftDownEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
        {
            arcDgrm->handleMouseLftDownEvent( x, y );
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
        {
            simulator->handleMouseLftDownEvent( x, y );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
        {
            timeSeries->handleMouseLftDownEvent( x, y );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
        else if ( c == canvasExnr && examiner != NULL )
        {
            examiner->handleMouseLftDownEvent( x, y );
            canvasArcD->Refresh();
        }
    }
}


// ----------------------------------
void DiaGraph::handleMouseLftUpEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// ----------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
        {
            editor->handleMouseLftUpEvent( x, y );

            if ( editor->getEditMode() != DiagramEditor::EDIT_MODE_DOF )
            {
                editor->setEditModeSelect();
                frame->setEditModeSelect();
            }
        }
        else if ( c == canvasColChooser && colChooser != NULL )
            colChooser->handleMouseLftUpEvent( x, y );
        else if ( c == canvasOpaChooser && opaChooser != NULL )
            opaChooser->handleMouseLftUpEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
            arcDgrm->handleMouseLftUpEvent( x, y );
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
            simulator->handleMouseLftUpEvent( x, y );
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseLftUpEvent( x, y );
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleMouseLftUpEvent( x, y );
    }
}


// --------------------------------------
void DiaGraph::handleMouseLftDClickEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// --------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseLftDClickEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
        {
            arcDgrm->handleMouseLftDClickEvent( x, y );
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
            simulator->handleMouseLftDClickEvent( x, y );
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseLftDClickEvent( x, y );
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleMouseLftDClickEvent( x, y );
    }
}


// ------------------------------------
void DiaGraph::handleMouseRgtDownEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// ------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseRgtDownEvent( x, y );
        else if ( c == canvasColChooser && colChooser != NULL )
            colChooser->handleMouseRgtDownEvent( x, y );
        else if ( c == canvasOpaChooser && opaChooser != NULL )
            opaChooser->handleMouseRgtDownEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
            arcDgrm->handleMouseRgtDownEvent( x, y );
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL  ) )
            simulator->handleMouseRgtDownEvent( x, y );
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL  ) )
            timeSeries->handleMouseRgtDownEvent( x, y );
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleMouseRgtDownEvent( x, y );
    }
}


// ----------------------------------
void DiaGraph::handleMouseRgtUpEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// ----------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseRgtUpEvent( x, y );
        else if ( c == canvasColChooser && colChooser != NULL )
            colChooser->handleMouseRgtUpEvent( x, y );
        else if ( c == canvasOpaChooser && opaChooser != NULL )
            opaChooser->handleMouseRgtUpEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
            arcDgrm->handleMouseRgtUpEvent( x, y );
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
            simulator->handleMouseRgtUpEvent( x, y );
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseRgtUpEvent( x, y );
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleMouseRgtUpEvent( x, y );
    }
}


// --------------------------------------
void DiaGraph::handleMouseRgtDClickEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// --------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseRgtDClickEvent( x, y );
    }
}


// -----------------------------------
void DiaGraph::handleMouseMotionEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// -----------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
        {
            editor->handleMouseMotionEvent( x, y );
        }
        else if ( c == canvasColChooser && colChooser != NULL )
            colChooser->handleMouseMotionEvent( x, y );
        else if ( c == canvasOpaChooser && opaChooser != NULL )
            opaChooser->handleMouseMotionEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
        {
            arcDgrm->handleMouseMotionEvent( x, y );
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
        {
            simulator->handleMouseMotionEvent( x, y );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
        {
            timeSeries->handleMouseMotionEvent( x, y );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
    }

    if ( c == canvasDistr && distrPlot != NULL )
        distrPlot->handleMouseMotionEvent( x, y );
    else if ( c == canvasCorrl && corrlPlot != NULL )
        corrlPlot->handleMouseMotionEvent( x, y );
    else if ( c == canvasCombn && combnPlot != NULL )
        combnPlot->handleMouseMotionEvent( x, y );
}


// -------------------------------------
void DiaGraph::handleMouseWheelIncEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// -------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseWheelIncEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseWheelIncEvent( x, y );
    }
}


// -------------------------------------
void DiaGraph::handleMouseWheelDecEvent(
    GLCanvas* c,
    const int &x,
    const int &y )
// -------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseWheelDecEvent( x, y );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseWheelDecEvent( x, y );
    }
}


// ------------------------------------------------
void DiaGraph::handleMouseEnterEvent( GLCanvas* c )
// ------------------------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseEnterEvent();
    }
}


// ------------------------------------------------
void DiaGraph::handleMouseLeaveEvent( GLCanvas* c )
// ------------------------------------------------
{
    if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasArcD && arcDgrm != NULL )
            arcDgrm->handleMouseLeaveEvent();
        else if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
            simulator->handleMouseLeaveEvent();
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
            timeSeries->handleMouseLeaveEvent();
        else if ( c == canvasExnr && examiner != NULL )
            examiner->handleMouseLeaveEvent();
    }
    else if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleMouseLeaveEvent();
    }
}


// -------------------------------
void DiaGraph::handleKeyDownEvent(
    GLCanvas* c,
    const int &keyCode,
    const int &specialKey )
// -------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleKeyDownEvent( keyCode, specialKey );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( view == VIEW_SIM && ( c == canvasSiml && simulator != NULL ) )
        {
            simulator->handleKeyDownEvent( keyCode );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
        else if ( view == VIEW_TRACE && ( c == canvasTrace && timeSeries != NULL ) )
        {
            timeSeries->handleKeyDownEvent( keyCode );
            canvasArcD->Refresh();
            canvasExnr->Refresh();
        }
        else if ( c == canvasExnr && examiner != NULL )
        {
            examiner->handleKeyDownEvent( keyCode );
            canvasArcD->Refresh();
        }
    }
}


// -------------------------------
void DiaGraph::handleKeyUpEvent(
    GLCanvas* c,
    const int &keyCode,
    const int &specialKey )
// -------------------------------
{
    if ( mode == MODE_EDIT )
    {
        if ( c == canvasEdit && editor != NULL )
            editor->handleKeyUpEvent( keyCode, specialKey );
    }
    else if ( mode == MODE_ANALYSIS )
    {
        if ( c == canvasTrace && timeSeries != NULL )
            timeSeries->handleKeyUpEvent( keyCode );
    }
}


// -- overloaded operators --------------------------------------


// -------------------------------------------
void DiaGraph::operator<<( const std::string &msg )
// -------------------------------------------
{
    this->appOutputText( msg );
}


// ----------------------------------------
void DiaGraph::operator<<( const int &msg )
// ----------------------------------------
{
    this->appOutputText( msg );
}


// -- protected functions inhereted from Mediator -------------------


// ----------------------------
void DiaGraph::initColleagues()
// ----------------------------
{
    // init graph
    graph = NULL;

    // init frame
    frame = new Frame(
        this,
        wxT("DiaGraphica") );
    // show frame
    frame->Show( TRUE );
    this->SetTopWindow( frame );

    *frame << "Welcome to DiaGraphica.\n";

    // init progress dialog
    progressDialog = NULL;

    // init visualizations
    canvasArcD  = frame->getCanvasArcD();
    arcDgrm     = NULL;

    canvasSiml  = frame->getCanvasSiml();
    simulator   = NULL;

    canvasTrace = frame->getCanvasTrace();
    timeSeries  = NULL;

    canvasExnr  = frame->getCanvasExnr();
    examiner    = NULL;

    canvasEdit  = frame->getCanvasEdit();
    editor      = NULL;

    canvasDistr = NULL;
    distrPlot   = NULL;
    canvasCorrl = NULL;
    corrlPlot   = NULL;
    canvasCombn = NULL;
    combnPlot   = NULL;

    canvasColChooser = NULL;
    colChooser       = NULL;
    canvasOpaChooser = NULL;
    opaChooser       = NULL;

    dgrmSender = NULL;

    tempAttr = NULL;
}


// -----------------------------
void DiaGraph::clearColleagues()
// -----------------------------
{
    // composition
    if ( graph != NULL )
    {
        delete graph;
        graph = NULL;
    }

    // composition
    if ( progressDialog != NULL )
    {
        delete progressDialog;
        progressDialog = NULL;
    }

    // association
    canvasArcD = NULL;
    // composition
    if ( arcDgrm != NULL )
    {
        delete arcDgrm;
        arcDgrm = NULL;
    }

    // association
    canvasSiml = NULL;
    // composition
    if ( simulator != NULL )
    {
        delete simulator;
        simulator = NULL;
    }

    // association
    canvasTrace = NULL;
    // composition
    if ( timeSeries != NULL )
    {
        delete timeSeries;
        timeSeries = NULL;
    }

    // associatioin
    canvasExnr = NULL;
    // composition
    if ( examiner != NULL )
    {
        delete examiner;
        examiner = NULL;
    }

    // association
    canvasEdit = NULL;
    // composition
    if ( editor != NULL )
    {
        delete editor;
        editor = NULL;
    }

    // association
    canvasDistr = NULL;
    // composition
    if ( distrPlot != NULL )
    {
        delete distrPlot;
        distrPlot = NULL;
    }

    // association
    canvasCorrl = NULL;
    // composition
    if ( corrlPlot != NULL )
    {
        delete corrlPlot;
        corrlPlot = NULL;
    }

    // association
    canvasCombn = NULL;
    // composition
    if ( combnPlot != NULL )
    {
        delete combnPlot;
        combnPlot = NULL;
    }
}


// -----------------------------
void DiaGraph::displAttributes()
// -----------------------------
{
    Attribute*       attr;
    vector< int >    indcs;
    vector< std::string > names;
    vector< std::string > types;
    vector< int >    cards;
    vector< std::string > range;
    for ( int i = 0; i < graph->getSizeAttributes(); ++i )
    {
        attr = graph->getAttribute( i );

        indcs.push_back( attr->getIndex() );
        names.push_back( attr->getName() );
        types.push_back( attr->getType() );
        cards.push_back( attr->getSizeCurValues() );
        /* -*-
        if ( attr->getSizeCurValues() != 0 )
            range.push_back( "" );
        else
        {
        std::string rge = "[";
            rge += Utils::dblToStr( attr->getLowerBound() );
            rge += ", ";
            rge += Utils::dblToStr( attr->getUpperBound() );
            rge += "]";

            range.push_back( rge );
        }
        */
        if ( attr->getAttrType() == Attribute::ATTR_TYPE_DISCR )
            range.push_back( "" );
        else
        {
          std::string rge = "[";
            rge += Utils::dblToStr( attr->getLowerBound() );
            rge += ", ";
            rge += Utils::dblToStr( attr->getUpperBound() );
            rge += "]";

            range.push_back( rge );
        }
    }
    frame->displAttrInfo( indcs, names, types, cards, range );

    attr = NULL;
}


// ----------------------------------------------------
void DiaGraph::displAttributes( const int &selAttrIdx )
// ----------------------------------------------------
{
    Attribute*       attr;
    vector< int >    indcs;
    vector< std::string > names;
    vector< std::string > types;
    vector< int >    cards;
    vector< std::string > range;
    for ( int i = 0; i < graph->getSizeAttributes(); ++i )
    {
        attr = graph->getAttribute( i );

        indcs.push_back( attr->getIndex() );
        names.push_back( attr->getName() );
        types.push_back( attr->getType() );
        cards.push_back( attr->getSizeCurValues() );
        /* -*-
        if ( attr->getSizeCurValues() != 0 )
            range.push_back( "" );
        else
        {
        std::string rge = "[";
            rge += Utils::dblToStr( attr->getLowerBound() );
            rge += ", ";
            rge += Utils::dblToStr( attr->getUpperBound() );
            rge += "]";

            range.push_back( rge );
        }
        */
        if ( attr->getAttrType() == Attribute::ATTR_TYPE_DISCR )
            range.push_back( "" );
        else
        {
          std::string rge = "[";
            rge += Utils::dblToStr( attr->getLowerBound() );
            rge += ", ";
            rge += Utils::dblToStr( attr->getUpperBound() );
            rge += "]";

            range.push_back( rge );
        }
    }
    frame->displAttrInfo( selAttrIdx, indcs, names, types, cards, range );

    attr = NULL;
}


// -------------------------------------------------
void DiaGraph::displAttrDomain( const int &attrIdx )
// -------------------------------------------------
{
    Attribute* attribute;
    int        numValues;
    int        numNodes;
    vector< int >    indices;
    vector< std::string > values;
    vector< int >    number;
    vector< double>  perc;

    attribute = graph->getAttribute( attrIdx );
    numValues = attribute->getSizeCurValues();
    numNodes  = graph->getSizeNodes();

    // update indices and values
    for ( int i = 0; i < numValues; ++i )
    {
         indices.push_back( attribute->getCurValue(i)->getIndex() );
         values.push_back( attribute->getCurValue(i)->getValue() );
    }

    // get number of nodes
    graph->calcAttrDistr(
        attrIdx,
        number );

    // calc perc
    {
    for ( int i = 0; i < numValues; ++i )
        perc.push_back( Utils::perc( number[i], numNodes ) );
    }

    // display domain
    frame->displDomainInfo( indices, values, number, perc );

    // reset ptr
    attribute = NULL;
}


// -----------------------------
void DiaGraph::clearAttrDomain()
// -----------------------------
{}


// -- end -----------------------------------------------------------
