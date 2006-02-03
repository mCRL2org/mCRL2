#include "markstateruledialog.h"

BEGIN_EVENT_TABLE( MarkStateRuleDialog, wxDialog )
  EVT_CHOICE( myID_PARAMETER_CHOICE, MarkStateRuleDialog::onParameterChoice )
END_EVENT_TABLE()

MarkStateRuleDialog::MarkStateRuleDialog( wxWindow* parent, Mediator* owner,
    ATermList svspec )
 : wxDialog( parent, wxID_ANY, wxT("Add mark state rule"), wxDefaultPosition )

/*   : wxDialog( parent, wxID_ANY, wxT("Add mark state rule") )  oud  */
{
  mediator = owner;
  stateVectorSpec = svspec;

  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
  wxFlexGridSizer* choicesSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
    
  int numberOfParams = ATgetLength( stateVectorSpec );
  wxArrayString paramChoices;
  paramChoices.Alloc( numberOfParams );
  for ( int i = 0 ; i < numberOfParams ; ++i )
  {
    ATermAppl paramId = ATermAppl( ATelementAt( stateVectorSpec, i ) );
    wxString paramName = wxString( strdup( ATwriteToString( ATgetArgument(
	      paramId, 0 ) ) ) , wxConvLocal);
    paramChoices.Add( paramName );
    parameterTypes[ paramName ] = (ATermAppl) ATgetArgument( paramId, 1 );
  }
  paramChoices.Sort();
  wxString relChoices[2] = { wxT("is an element of"), wxT("is not an element of") };

  parameterChoice = new wxChoice( this, myID_PARAMETER_CHOICE,
      wxDefaultPosition, wxDefaultSize, paramChoices );
  parameterChoice->SetSelection( 0 );
  relationChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, 2, relChoices );
  relationChoice->SetSelection( 0 );
  valuesListBox = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition,
      wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_HSCROLL | wxLB_NEEDED_SB |
      wxLB_SORT );
  
  int flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int border = 5;
  
  choicesSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Parameter") ), 0, flags, border );
  choicesSizer->Add( parameterChoice, 0, flags, border );
  choicesSizer->Add( relationChoice, 0, flags, border );

  mainSizer->Add( choicesSizer, 0, wxEXPAND | wxALL, border );
  mainSizer->Add( valuesListBox, 1, wxEXPAND | wxALL, border );
  mainSizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxALL, border );
  mainSizer->Add( CreateButtonSizer( wxOK | wxCANCEL ), 0, wxEXPAND | wxALL, border );
  SetSizer( mainSizer );
  if ( paramChoices.Count() > 0 ) loadValues( paramChoices[ 0 ] );
}

MarkStateRuleDialog::~MarkStateRuleDialog()
{
}

void MarkStateRuleDialog::loadValues( wxString paramName )
{
  if ( parameterTypes.find( paramName ) != parameterTypes.end() )
  {
    wxArrayString values;
    ATermList valueList = (ATermList) ATgetArgument( parameterTypes[ paramName ], 1 );
    for ( int i = 0 ; i < ATgetLength( valueList ) ; ++i )
    {
      values.Add( wxString( strdup( ATwriteToString( ATelementAt( valueList, i ) ) ), wxConvLocal ) );
    }
    valuesListBox->Clear();
    valuesListBox->Append( values );
  }
}

void MarkStateRuleDialog::onParameterChoice( wxCommandEvent& event )
{
  loadValues( event.GetString() );
}
