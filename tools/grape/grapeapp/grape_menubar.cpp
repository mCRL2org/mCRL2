// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_menubar.cpp
//
// Implements the menubar class used by the main frame.

#include "grape_menubar.h"
#include "grape_ids.h"
#include "grape_icons.h"

using namespace grape::grapeapp;

extern IconMap g_icons;

grape_menubar::grape_menubar(void) : wxMenuBar()
{
  wxMenuItem *item = 0;

  // initialize icons
  init_icons();

  // construct the menubar here
  m_menu_file = new wxMenu;

  // file menu
  item = new wxMenuItem( m_menu_file, wxID_NEW );
  item->SetBitmap( g_icons[ _T("new") ] );
  item->SetHelp( _T("New specification") );
  m_menu_file->Append( item );

  item = new wxMenuItem( m_menu_file, wxID_OPEN );
  item->SetBitmap( g_icons[ _T("open") ] );
  item->SetHelp( _T("Open specification") );
  m_menu_file->Append( item );

  item = new wxMenuItem( m_menu_file, wxID_CLOSE );
  item->SetBitmap( g_icons[ _T("close") ] );
  item->SetHelp( _T("Close current specification") );
  m_menu_file->Append( item );

  m_menu_file->AppendSeparator();

  item = new wxMenuItem( m_menu_file, wxID_SAVE );
  item->SetBitmap( g_icons[ _T("save") ] );
  item->SetHelp( _T("Save current specification") );
  m_menu_file->Append( item );

  item = new wxMenuItem( m_menu_file, wxID_SAVEAS );
  item->SetBitmap( g_icons[ _T("saveas") ] );
  item->SetHelp( _T("Save current specification with a different filename") );
  m_menu_file->Append( item );

  m_menu_file->AppendSeparator();

  item = new wxMenuItem( m_menu_file, GRAPE_MENU_VALIDATE, _T("&Validate specification\tShift+F5"), _T("Validates current specification") );
  item->SetBitmap( g_icons[ _T("validate") ] );
  m_menu_file->Append(item);

  m_menu_file->AppendSeparator();

  item = new wxMenuItem( m_menu_file, GRAPE_MENU_EXPORTTEXT, _T("&Export datatype specification to text"), _T("Export current datatype specification to text") );
  item->SetBitmap( g_icons[ _T("text") ] );
  m_menu_file->Append( item );

  item = new wxMenuItem( m_menu_file, wxID_PRINT );
  m_menu_file->Append( item );
  m_menu_file->Enable(wxID_PRINT, false);
  
  m_menu_file->AppendSeparator();

  item = new wxMenuItem( m_menu_file, wxID_EXIT );
  item->SetBitmap( g_icons[ _T("exit") ] );
  m_menu_file->Append( item );

  Append(m_menu_file, _T("&File"));

  // edit menu
  m_menu_edit = new wxMenu;

  item = new wxMenuItem( m_menu_edit, wxID_UNDO );
  item->SetBitmap( g_icons[ _T("undo") ] );
  m_menu_edit->Append( item );

  item = new wxMenuItem( m_menu_edit, wxID_REDO );
  item->SetBitmap( g_icons[ _T("redo") ] );
  m_menu_edit->Append( item );

  m_menu_edit->AppendSeparator();

  item = new wxMenuItem( m_menu_edit, wxID_CUT );
  item->SetBitmap( g_icons[ _T("cut") ] );
  m_menu_edit->Append( item );

  item = new wxMenuItem( m_menu_edit, wxID_COPY );
  item->SetBitmap( g_icons[ _T("copy") ] );
  m_menu_edit->Append( item );

  item = new wxMenuItem( m_menu_edit, wxID_PASTE );
  item->SetBitmap( g_icons[ _T("paste") ] );
  m_menu_edit->Append( item );

  item = new wxMenuItem( m_menu_edit, wxID_DELETE, _T("&Remove\tDel"), _T("Remove selected objects") );
  item->SetBitmap( g_icons[ _T("del") ] );
  m_menu_edit->Append( item );
  m_menu_edit->Enable(wxID_DELETE, false);

  // add properties
  item = new wxMenuItem( m_menu_edit, GRAPE_MENU_PROPERTIES, _T("&Properties...\tF2"), _T("Edit properties of selected object") );
  item->SetBitmap( g_icons[ _T("properties") ] );
  m_menu_edit->Append( item );
  m_menu_edit->Enable(GRAPE_MENU_PROPERTIES, false);

  m_menu_edit->AppendSeparator();

  item = new wxMenuItem( m_menu_edit, GRAPE_MENU_SELECT_ALL, _T("Select &All\tCtrl-A"), _T("Select all objects in current diagram") );
  m_menu_edit->Append( item );
  m_menu_edit->Enable(GRAPE_MENU_SELECT_ALL, false);

  item = new wxMenuItem( m_menu_edit, GRAPE_MENU_DESELECT_ALL, _T("&Deselect \tShift-Ctrl-A"), _T("Deselect all objects in current diagram") );
  m_menu_edit->Append( item );
  m_menu_edit->Enable(GRAPE_MENU_DESELECT_ALL, false);

  m_menu_edit->AppendSeparator();

  item = new wxMenuItem( m_menu_edit, GRAPE_MENU_DATATYPESPEC, _T("Edit Datatype specification"), _T("Edit Datatype specification"), wxITEM_CHECK );
  m_menu_edit->Append( item );

  Append(m_menu_edit, _T("&Edit"));

  // diagram menu
  m_menu_diagram = new wxMenu;
  
  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM, _T("Add &Architecture diagram"), _T("Add Architecture diagram") );
  item->SetBitmap( g_icons[ _T("newarch") ] );
  m_menu_diagram->Append( item );

  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_ADD_PROCESS_DIAGRAM, _T("Add &Process diagram"), _T("Add Process diagram") );
  item->SetBitmap( g_icons[ _T("newproc") ] );
  m_menu_diagram->Append( item );

  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_RENAME_DIAGRAM, _T("Re&name"), _T("Rename current diagram...") );
  m_menu_diagram->Append( item );

  item = new wxMenuItem(m_menu_diagram, GRAPE_MENU_VALIDATE_DIAGRAM, _T("&Validate\tF5"), _T("Validate current diagram"));
  item->SetBitmap( g_icons[ _T("validate") ] );
  m_menu_diagram->Append(item);

  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_EXPORTMCRL2, _T("Export to &mCRL2...\tCtrl-E"), _T("Export to mCRL2") );
  item->SetBitmap( g_icons[ _T("export") ] );
  m_menu_diagram->Append( item );

  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_EXPORTIMAGE, _T("Export to &image...\tCtrl-I"), _T("Export current diagram to image") );
  item->SetBitmap( g_icons[ _T("image") ] );
  m_menu_diagram->Append( item );

  m_menu_diagram->AppendSeparator();

  item = new wxMenuItem( m_menu_diagram, GRAPE_MENU_REMOVE_DIAGRAM, _T("&Remove"), _T("Remove current diagram") );
  item->SetBitmap( g_icons[ _T("del") ] );
  m_menu_diagram->Append( item );

  Append(m_menu_diagram, _T("&Diagram"));

  // tools menu
  m_menu_tools = new wxMenu;
  
  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_SELECT, _T("Selection"), _T("Select objects in current diagram") );
  item->SetBitmap( g_icons[ _T("toolselect") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ATTACH, _T("Attach objects"), _T("Attach objects to eachother") );
  item->SetBitmap( g_icons[ _T("attach") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_DETACH, _T("Detach objects"), _T("Detach objects from eachother") );
  item->SetBitmap( g_icons[ _T("detach") ] );
  m_menu_tools->Append( item );

  m_menu_tools->AppendSeparator();

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE, _T("Add Architecture reference"), _T("Add Architecture reference") );
  item->SetBitmap( g_icons[ _T("archref") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_PROCESS_REFERENCE, _T("Add Process reference"), _T("Add Process reference") );
  item->SetBitmap( g_icons[ _T("procref") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_CHANNEL, _T("Add Channel"), _T("Add Channel") );
  item->SetBitmap( g_icons[ _T("channel") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION, _T("Add Channel communication"), _T("Add Channel communication") );
  item->SetBitmap( g_icons[ _T("chancomm") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_BLOCKED, _T("Add Blocked property"), _T("Add Blocked property") );
  item->SetBitmap( g_icons[ _T("blocked") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_VISIBLE, _T("Add Visible property"), _T("Add Visible property") );
  item->SetBitmap( g_icons[ _T("visible") ] );
  m_menu_tools->Append( item );

  m_menu_tools->AppendSeparator();

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_STATE, _T("Add State"), _T("Add State") );
  item->SetBitmap( g_icons[ _T("state") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_REFERENCE_STATE, _T("Add Process reference"), _T("Add Process reference") );
  item->SetBitmap( g_icons[ _T("procref" ) ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION, _T("Add Transition"), _T("Add Transition") );
  item->SetBitmap( g_icons[ _T("transition") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_INITIAL_DESIGNATOR, _T("Add Initial designator"), _T("Add Initial designator") );
  item->SetBitmap( g_icons[ _T("initdes") ] );
  m_menu_tools->Append( item );

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_TERMINATING_TRANSITION, _T("Add Terminating transition"), _T("Add Terminating transition") );
  item->SetBitmap( g_icons[ _T("termtransition") ] );
  m_menu_tools->Append( item );

  m_menu_tools->AppendSeparator();

  item = new wxMenuItem( m_menu_tools, GRAPE_TOOL_ADD_COMMENT, _T("Add Comment"), _T("Add Comment") );
  item->SetBitmap( g_icons[ _T("comment") ] );
  m_menu_tools->Append( item );

  Append( m_menu_tools, _T("&Tools") );

  // help menu
  m_menu_help = new wxMenu;
  
  item = new wxMenuItem( m_menu_help, wxID_HELP, _T("&Contents\tF1"), _T("Show help contents") );
  item->SetBitmap( g_icons[ _T("helpcontents") ] );
  m_menu_help->Append( item );

  m_menu_help->AppendSeparator();

  item = new wxMenuItem( m_menu_help, wxID_ABOUT );
  item->SetBitmap( g_icons[ _T("helpabout") ] );
  m_menu_help->Append( item );

  Append(m_menu_help, _T("&Help"));
}

grape_menubar::~grape_menubar()
{
}

wxMenu* grape_menubar::get_menu( grape_main_menu p_which )
{
  switch( p_which )
  {
    case GRAPE_MENU_FILE: return m_menu_file;
    case GRAPE_MENU_EDIT: return m_menu_edit;
    case GRAPE_MENU_DIAGRAM: return m_menu_diagram;
    case GRAPE_MENU_TOOLS: return m_menu_tools;
    case GRAPE_MENU_HELP: return m_menu_help;
    default: break; // should never happen!
  }

  return 0; // should never happen
}

void grape_menubar::set_mode( int p_mode )
{
  bool in_spec = (p_mode & ( GRAPE_MENUMODE_SPEC + GRAPE_MENUMODE_DATASPEC )) != 0;
  bool in_diagram = (p_mode & ( GRAPE_MENUMODE_ARCH + GRAPE_MENUMODE_PROC )) != 0;

  // update menubar
  Enable(wxID_CLOSE, in_spec);
  Enable(wxID_SAVEAS, in_spec);
  Enable(GRAPE_MENU_VALIDATE, in_diagram);
  Enable(GRAPE_MENU_EXPORTTEXT, in_spec);
// Low prioritiy, not implemented; disabled
  Enable(wxID_PRINT, false );
  Enable(GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM, in_spec);
  Enable(GRAPE_MENU_ADD_PROCESS_DIAGRAM, in_spec);

// Low priority, not implemented; disabled
  Enable(wxID_CUT, false );
// Low priority, not implemented; disabled
  Enable(wxID_COPY, false );
// Low priority, not implemented; disabled
  Enable(wxID_PASTE, false );

  Enable(wxID_DELETE, ( p_mode & GRAPE_MENUMODE_DATASPEC ) == 0 );
  Enable(GRAPE_MENU_PROPERTIES, ( p_mode & GRAPE_MENUMODE_DATASPEC ) == 0 );
  // Enable(GRAPE_MENU_SELECT_ALL, !( p_mode & GRAPE_MENUMODE_DATASPEC ) );
  Enable(GRAPE_MENU_SELECT_ALL, false );
  Enable(GRAPE_MENU_DESELECT_ALL, ( p_mode & GRAPE_MENUMODE_DATASPEC ) == 0 );
  Enable(GRAPE_MENU_DATATYPESPEC, in_spec );

  Enable(GRAPE_TOOL_SELECT, in_diagram );
  Enable(GRAPE_TOOL_ATTACH, in_diagram );
  Enable(GRAPE_TOOL_DETACH, in_diagram );
  Enable(GRAPE_TOOL_ADD_COMMENT, in_diagram );

  Enable(GRAPE_MENU_RENAME_DIAGRAM, in_diagram );
  Enable(GRAPE_MENU_REMOVE_DIAGRAM, in_diagram );
  Enable(GRAPE_MENU_VALIDATE_DIAGRAM, in_diagram);
  Enable(GRAPE_MENU_EXPORTMCRL2, in_diagram);
  Enable(GRAPE_MENU_EXPORTIMAGE, in_diagram);

  // architecture diagram specific items
  Enable(GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );
  Enable(GRAPE_TOOL_ADD_PROCESS_REFERENCE, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );
  Enable(GRAPE_TOOL_ADD_CHANNEL, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );
  Enable(GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );
  Enable(GRAPE_TOOL_ADD_BLOCKED, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );
  Enable(GRAPE_TOOL_ADD_VISIBLE, (p_mode & GRAPE_MENUMODE_ARCH) != 0 );

  // process diagram specific items
  Enable(GRAPE_TOOL_ADD_STATE, (p_mode & GRAPE_MENUMODE_PROC) != 0 );
  Enable(GRAPE_TOOL_ADD_REFERENCE_STATE, (p_mode & GRAPE_MENUMODE_PROC) != 0 );
  Enable(GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION, (p_mode & GRAPE_MENUMODE_PROC) != 0 );
  Enable(GRAPE_TOOL_ADD_INITIAL_DESIGNATOR, (p_mode & GRAPE_MENUMODE_PROC) != 0 );
  Enable(GRAPE_TOOL_ADD_TERMINATING_TRANSITION, (p_mode & GRAPE_MENUMODE_PROC) != 0 );

  Check(GRAPE_MENU_DATATYPESPEC, (p_mode & GRAPE_MENUMODE_DATASPEC) != 0 );

  Refresh();
}
