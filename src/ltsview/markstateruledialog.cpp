#include "markstateruledialog.h"

BEGIN_EVENT_TABLE( MarkStateRuleDialog, wxDialog )
  EVT_LISTBOX( myID_PARAMETER_CHOICE, MarkStateRuleDialog::onParameterChoice )
END_EVENT_TABLE()

MarkStateRuleDialog::MarkStateRuleDialog( wxWindow* parent, Mediator* owner,
    ATermList svspec )
 : wxDialog( parent, wxID_ANY, wxT("Add mark state rule"), wxDefaultPosition )
{
  mediator = owner;

  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );

  wxFlexGridSizer* controlSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
    
  int numberOfParams = ATgetLength( svspec );
  wxArrayString paramChoices;
  paramChoices.Alloc( numberOfParams );
  
  ATermAppl paramId;
  int i=0;
  ATermList stateVectorSpec = svspec;
  while ( !ATisEmpty( stateVectorSpec ) )
  {
    paramId = (ATermAppl)ATgetFirst( stateVectorSpec );
    wxString paramName = wxString( ATwriteToString( ATgetArgument( paramId, 0
	    )), wxConvLocal);
    paramChoices.Add( paramName );
    parameterTypes[ paramName ] = (ATermAppl)ATgetArgument( paramId, 1 );
    parameterIndices[ paramName ] = i++;
    stateVectorSpec = ATgetNext( stateVectorSpec );
  }
  paramChoices.Sort();
  wxString relChoices[2] = { wxT("is an element of"),
    wxT("is not an element of") };

  parameterListBox = new wxListBox( this, myID_PARAMETER_CHOICE,
      wxDefaultPosition, wxSize(150,150), paramChoices, wxLB_SINGLE |
      wxLB_HSCROLL | wxLB_NEEDED_SB );
  parameterListBox->SetSelection( 0 );
  relationListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition,
      wxSize(150,150), 2, relChoices );
  relationListBox->SetSelection( 0 );
  valuesListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition,
      wxSize(150, 150), 0, NULL, wxLB_MULTIPLE | wxLB_HSCROLL | wxLB_NEEDED_SB |
      wxLB_SORT );
  
  int flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL | wxALL;
  int border = 5;
  
  controlSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Parameter:") ), 0,
      flags, border );
  controlSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Relation:") ), 0,
      flags, border );
  controlSizer->Add( new wxStaticText( this, wxID_ANY, wxT("Values:") ), 0,
      flags, border );
  controlSizer->Add( parameterListBox, 0, flags, border );
  controlSizer->Add( relationListBox, 0, flags, border );
  controlSizer->Add( valuesListBox, 0, flags, border );

  mainSizer->Add( controlSizer, 0, wxEXPAND | wxALL, border );
  mainSizer->Add( new wxStaticLine( this, wxID_ANY ), 0, wxEXPAND | wxALL,
      border );
  mainSizer->Add( CreateButtonSizer( wxOK | wxCANCEL ), 0, wxEXPAND | wxALL,
      border );
  
  mainSizer->Fit( this );
  SetSizer( mainSizer );
  Layout();
  
  if ( paramChoices.Count() > 0 ) loadValues( paramChoices[ 0 ] );
}

MarkStateRuleDialog::~MarkStateRuleDialog()
{
}

void MarkStateRuleDialog::loadValues( wxString paramName )
{
  if ( parameterTypes.find( paramName ) != parameterTypes.end() )
  {
    ATermList valueList = (ATermList) ATgetArgument( parameterTypes[ paramName ], 1 );
    ATermAppl value;
    wxArrayString values;
    valueIndices.clear();
    while ( !ATisEmpty( valueList ) )
    {
      value = (ATermAppl)ATgetFirst( valueList );
      wxString valuestr = wxString( ATwriteToString( ATgetArgument( value, 0 )
	    ), wxConvLocal );
      values.Add( valuestr );
      valueIndices[ valuestr ] = ATgetInt( (ATermInt)ATgetArgument( value, 1 ) );
      valueList = ATgetNext( valueList );
    }
    valuesListBox->Set( values );
  }
}

void MarkStateRuleDialog::onParameterChoice( wxCommandEvent& event )
{
  loadValues( event.GetString() );
}

void MarkStateRuleDialog::setMarkRule( MarkRule* mr, ATermList svspec )
{
  ATermAppl paramId = (ATermAppl)ATelementAt( svspec, mr->paramIndex );
  ATermList values = (ATermList)ATgetArgument( (ATermAppl)ATgetArgument(
	paramId, 1 ), 1 );
  wxString paramName = wxString( ATwriteToString( ATgetArgument( paramId, 0 ) ),
      wxConvLocal);
  parameterListBox->SetStringSelection( paramName );
  loadValues( paramName );
  
  if ( !mr->isNegated )
  {
    relationListBox->SetSelection( 0 );
    ATermAppl value;
    while ( !ATisEmpty( values ) )
    {
      value = (ATermAppl)ATgetFirst( values );
      if ( mr->valueSet[ ATgetInt( (ATermInt)ATgetArgument( value, 1 ) ) ] )
      {
	valuesListBox->SetStringSelection( wxString( ATwriteToString(
		ATgetArgument( value, 0 ) ), wxConvLocal ) );
      }
      values = ATgetNext( values );
    }
  }
  else
  {
    relationListBox->SetSelection( 1 );
    ATermAppl value;
    while ( !ATisEmpty( values ) )
    {
      value = (ATermAppl)ATgetFirst( values );
      if ( !mr->valueSet[ ATgetInt( (ATermInt)ATgetArgument( value, 1 ) ) ] )
      {
	valuesListBox->SetStringSelection( wxString( ATwriteToString(
		ATgetArgument( value, 0 ) ), wxConvLocal ) );
      }
      values = ATgetNext( values );
    }
  }
}

MarkRule* MarkStateRuleDialog::getMarkRule()
{
  MarkRule* result = new MarkRule;
  result->paramIndex = parameterIndices[ parameterListBox->GetStringSelection() ];
  result->isNegated = ( relationListBox->GetSelection() == 1 );
  
  int N = valuesListBox->GetCount();
  wxArrayInt selections;
  int NS = valuesListBox->GetSelections( selections );
  if ( relationListBox->GetSelection() == 0 )
  {
    result->valueSet.assign( N, false );
    for ( int i = 0 ; i < NS ; ++i )
    {
      result->valueSet[ valueIndices[ valuesListBox->GetString( selections[i] )
	] ] = true;
    }
  }
  else
  {
    result->valueSet.assign( N, true );
    for ( int i = 0 ; i < NS ; ++i )
    {
      result->valueSet[ valueIndices[ valuesListBox->GetString( selections[i] )
	] ] = false;
    }
  }  
  return result;
}

wxString MarkStateRuleDialog::getMarkRuleString()
{
  wxString result = parameterListBox->GetStringSelection();
  result += ( relationListBox->GetSelection() == 0 ) ? wxT(" in { ") :
    wxT(" not in { ");
  wxArrayInt selections;
  int NS = valuesListBox->GetSelections( selections );
  for ( int i = 0 ; i < NS-1 ; ++i )
  {
    result += valuesListBox->GetString( selections[i] );
    result += wxT(", ");
  }
  result += valuesListBox->GetString( selections[NS-1] );
  result += wxT(" }");
  return result;
}
