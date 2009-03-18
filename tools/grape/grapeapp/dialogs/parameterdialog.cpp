// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parameterdialog.cpp
//
// Defines a parameter initialisation dialog.

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "parameterdialog.h"
#include "grape_ids.h"

using namespace grape::grapeapp;

grape_parameter_dialog::grape_parameter_dialog(list_of_decl &p_parameter_declarations)
: wxDialog( 0, wxID_ANY, _T("Set parameter initialisation"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
  wxPanel *panel = new wxPanel(this);

  wxGridSizer *grid = new wxFlexGridSizer(2, 3, 0);

  // refers to
  wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );
  wxStaticText *text = new wxStaticText( panel, wxID_ANY, _T( "Parameter:" ) );
  grid->Add( text, 0 );
  vsizer->Add(grid);

  // create grid
  m_grid = new wxGrid( panel, GRAPE_GRID_PARAM, wxDefaultPosition, wxSize(400, 300));
  m_grid->CreateGrid( p_parameter_declarations.GetCount(), 2 );
  for ( unsigned int i = 0; i < p_parameter_declarations.GetCount(); ++i )
  {
    // fill cells
    m_grid->SetCellValue(i, 0, p_parameter_declarations[i].get_name());
    m_grid->SetCellValue(i, 1, _T(""));
  }

  m_grid->SetColSize( 0, 170 );
  m_grid->SetColSize( 1, 100 );
  m_grid->SetColLabelValue(0, _T("Name"));
  m_grid->SetColLabelValue(1, _T("Value"));
  m_grid->SetRowLabelSize(30);

  vsizer->Add(m_grid, 1, wxEXPAND );
 
  panel->SetSizer( vsizer );

  wxBoxSizer *wnd_sizer = new wxBoxSizer(wxVERTICAL);

  wnd_sizer->AddSpacer( 3 );
  wnd_sizer->Add(panel, 1, wxEXPAND, 0 );
  wnd_sizer->AddSpacer( 3 );

  // create buttons
  wxSizer *sizer = CreateButtonSizer(wxOK | wxCANCEL);
  sizer->Layout();
  wnd_sizer->Add(sizer, 0, wxALIGN_RIGHT, 0);

  // realize sizers
  SetSizer(wnd_sizer);
  wnd_sizer->Fit(this);
  wnd_sizer->SetSizeHints(this);

  m_grid->SetFocus();
  
  check_parameters();
}

grape_parameter_dialog::grape_parameter_dialog()
: wxDialog()
{
  // shouldn't be called
}

grape_parameter_dialog::~grape_parameter_dialog()
{
}

bool grape_parameter_dialog::show_modal()
{
  return ShowModal() != wxID_CANCEL;
}

list_of_decl_init grape_parameter_dialog::get_initialisations() const
{
  return m_init;
}

void grape_parameter_dialog::check_parameters()
{
  bool valid = true;
  
  for ( unsigned int i = 0; i < m_grid->GetNumberRows(); ++i )
  {
    valid = valid && ( !m_grid->GetCellValue(i, 1).IsEmpty() );
  }
    
  FindWindow(GetAffirmativeId())->Enable(valid);
}

void grape_parameter_dialog::event_change_text( wxGridEvent &p_event )
{
  check_parameters();
}


void grape_parameter_dialog::event_ok(wxCommandEvent &WXUNUSED(p_event))
{
  // save initialisation
  for ( unsigned int i = 0; i < m_grid->GetNumberRows(); ++i )
  {
    m_init[i].set_value(m_grid->GetCellValue(i, 1));
  }
  
  EndModal(wxID_OK);
}

BEGIN_EVENT_TABLE(grape_parameter_dialog, wxDialog)
  EVT_BUTTON(wxID_OK, grape_parameter_dialog::event_ok)
  EVT_GRID_CMD_CELL_CHANGE(GRAPE_GRID_PARAM, grape_parameter_dialog::event_change_text)
END_EVENT_TABLE()
