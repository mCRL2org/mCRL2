// --- droptarget.cpp -----------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "droptarget.h"


// -- constructors and destructor -----------------------------------


// --------------------
DropTarget::DropTarget( 
    wxWindow* ownr, 
    Mediator* m )
    : Colleague( m )
// --------------------
{
    owner = ownr;
}


// ----------------------
DropTarget::~DropTarget()
// ----------------------
{
    owner = NULL;
}


// -- overridden functions from wxTextDropTarget --------------------
  

// -------------------------
bool DropTarget::OnDropText(
    wxCoord x,
    wxCoord y,
    const wxString &text )
// -------------------------
{
    vector< wxString > tokens;
    int    srcId;
    vector< int > data;

    wxStringTokenizer tkz( 
        text,         // string
        wxString( wxT(" ") ) ); // delimiters
    while( tkz.HasMoreTokens() )
        tokens.push_back( tkz.GetNextToken() );

    // get src id
    if ( tokens.size() > 0 )
        srcId = Utils::strToInt( string(tokens[0].fn_str()) );
    // get remaining data
    for ( size_t i = 1; i < tokens.size(); ++i )
        data.push_back( Utils::strToInt( string(tokens[i].fn_str()) ));
 
    mediator->handleDragDrop(
        srcId,
        owner->GetId(),
        x,
        y,
        data );
    return true;
}


// -- end -----------------------------------------------------------
