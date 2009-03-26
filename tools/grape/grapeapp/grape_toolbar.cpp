// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_toolbar.cpp
//
// Implements the toolbar used by the main frame.

#include "grape_toolbar.h"
#include "grape_ids.h"
#include "grape_icons.h"

using namespace grape::grapeapp;

extern IconMap g_icons;

grape_toolbar::grape_toolbar(void) : wxToolBar()
{
  init_icons();
}

grape_toolbar::grape_toolbar(wxWindow *p_parent) : wxToolBar(p_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL)
{
  init_icons();
  AddTool(wxID_NEW, _T("New specification"), g_icons[ _T("new") ], wxNullBitmap, wxITEM_NORMAL, _T("New specification"), _T("New specification"));
  AddTool(wxID_OPEN, _T("Open specification"), g_icons[ _T("open") ], wxNullBitmap, wxITEM_NORMAL, _T("Open specification"), _T("Open specification"));
  AddTool(wxID_SAVE, _T("Save current specification"), g_icons[ _T("save") ], wxNullBitmap, wxITEM_NORMAL, _T("Save current specification"), _T("Save current specification"));
  AddSeparator();
  AddTool(wxID_UNDO, _T("Undo last action"), g_icons[ _T("undo") ], wxNullBitmap, wxITEM_NORMAL, _T("Undo last action"), _T("Undo last action"));
  AddTool(wxID_REDO, _T("Redo last action"), g_icons[ _T("redo") ], wxNullBitmap, wxITEM_NORMAL, _T("Redo last action"), _T("Redo last action"));
  AddTool(wxID_CUT, _T("Cut selection"), g_icons[ _T("cut") ], wxNullBitmap, wxITEM_NORMAL, _T("Cut selection"), _T("Cut selection"));
  AddTool(wxID_COPY, _T("Copy selection"), g_icons[ _T("copy") ], wxNullBitmap, wxITEM_NORMAL, _T("Copy selection"), _T("Copy selection"));
  AddTool(wxID_PASTE, _T("Paste selection"), g_icons[ _T("paste") ], wxNullBitmap, wxITEM_NORMAL, _T("Paste selection"), _T("Paste selection"));
  AddTool(wxID_DELETE, _T("Remove selected objects"), g_icons[ _T("del") ], wxNullBitmap, wxITEM_NORMAL, _T("Remove selected objects"), _T("Remove selected objects"));
  AddTool(GRAPE_MENU_PROPERTIES, _T("Edit properties"), g_icons[ _T("properties") ], wxNullBitmap, wxITEM_NORMAL, _T("Edit properties of selected objects"), _T("Edit properties of selected objects"));
  AddSeparator();
  AddTool(GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM, _T("Add Architecture diagram"), g_icons[ _T("newarch") ], wxNullBitmap, wxITEM_NORMAL, _T("Add Architecture diagram"), _T("Add Architecture diagram"));
  AddTool(GRAPE_MENU_ADD_PROCESS_DIAGRAM, _T("Add Process diagram"), g_icons[ _T("newproc") ], wxNullBitmap, wxITEM_NORMAL, _T("Add Process diagram"), _T("Add Process diagram"));
  Realize();
}

grape_toolbar::~grape_toolbar(void)
{
}

void grape_toolbar::set_mode( int p_mode )
{
  // update toolbar
  EnableTool(wxID_SAVE, p_mode & GRAPE_TOOLMODE_SPEC);

  EnableTool(GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM, p_mode & GRAPE_TOOLMODE_SPEC);
  EnableTool(GRAPE_MENU_ADD_PROCESS_DIAGRAM, p_mode & GRAPE_TOOLMODE_SPEC);

  // actions for both process and arch diagrams
// Needed for cut, copy paste. Low priority, cut copy paste not implemented; outcommented.
//  bool in_diagram = p_mode & ( GRAPE_TOOLMODE_ARCH + GRAPE_TOOLMODE_PROC );

// Low priority, not implemented; disabled.
  EnableTool(wxID_CUT, false );
// Low priority, not implemented; disabled.
  EnableTool(wxID_COPY, false );
// Low priority, not implemented; disabled.
  EnableTool(wxID_PASTE, false );
  Refresh();
}

grape_arch_toolbar::grape_arch_toolbar( void ) : grape_toolbar()
{
}

grape_arch_toolbar::~grape_arch_toolbar( void )
{
}

grape_arch_toolbar::grape_arch_toolbar(wxWindow *p_parent) : grape_toolbar(p_parent)
{
  AddSeparator();
  AddRadioTool(GRAPE_TOOL_SELECT, _T("Select objects in current diagram"), g_icons[ _T("toolselect") ], wxNullBitmap, _T("Select objects in current diagram"), _T("Select objects in current diagram"));
//  AddRadioTool(GRAPE_TOOL_ATTACH, _T("Attach objects"), g_icons[ _T("attach") ], wxNullBitmap, _T("Attach objects to eachother"), _T("Attach objects to eachother"));
  AddRadioTool(GRAPE_TOOL_DETACH, _T("Detach objects"), g_icons[ _T("detach") ], wxNullBitmap, _T("Detach objects from eachother"), _T("Detach objects from eachother"));
  AddRadioTool(GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE, _T("Add Architecture reference"), g_icons[ _T("archref") ], wxNullBitmap, _T("Add Architecture reference"), _T("Add Architecture reference"));
  AddRadioTool(GRAPE_TOOL_ADD_PROCESS_REFERENCE, _T("Add Process reference"), g_icons[ _T("procref") ], wxNullBitmap, _T("Add Process reference"), _T("Add Process reference"));
  AddRadioTool(GRAPE_TOOL_ADD_CHANNEL, _T("Add Channel"), g_icons[ _T("channel") ], wxNullBitmap, _T("Add Channel"), _T("Add Channel"));
  AddRadioTool(GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION, _T("Add Channel communication"), g_icons[ _T("chancomm") ], wxNullBitmap, _T("Add Channel communication"), _T("Add Channel communication"));
  AddRadioTool(GRAPE_TOOL_ADD_COMMENT, _T("Add Comment"), g_icons[ _T("comment") ], wxNullBitmap, _T("Add Comment"), _T("Add Comment"));
  AddTool(GRAPE_MENU_VALIDATE_DIAGRAM, _T("Validate current diagram"), g_icons[ _T("validate") ], wxNullBitmap, wxITEM_NORMAL, _T("Validate current diagram"), _T("Validate current diagram"));

  Realize();
}

grape_proc_toolbar::grape_proc_toolbar( void ) : grape_toolbar()
{
  init_icons();
}

grape_proc_toolbar::~grape_proc_toolbar( void )
{
}

grape_proc_toolbar::grape_proc_toolbar(wxWindow *p_parent) : grape_toolbar(p_parent)
{
  AddSeparator();
  AddRadioTool(GRAPE_TOOL_SELECT, _T("Select"), g_icons[ _T("toolselect") ], wxNullBitmap, _T("Select items"), _T("Select items"));
//  AddRadioTool(GRAPE_TOOL_ATTACH, _T("Attach objects"), g_icons[ _T("attach") ], wxNullBitmap, _T("Attach objects to eachother"), _T("Attach objects to eachother"));
//  AddRadioTool(GRAPE_TOOL_DETACH, _T("Detach objects"), g_icons[ _T("detach") ], wxNullBitmap, _T("Detach objects from eachother"), _T("Detach objects from eachother"));
  AddRadioTool(GRAPE_TOOL_ADD_STATE, _T("Add State"), g_icons[ _T("state") ], wxNullBitmap, _T("Add State"), _T("Add State"));
  AddRadioTool(GRAPE_TOOL_ADD_REFERENCE_STATE, _T("Add Process reference"), g_icons[ _T("procref") ], wxNullBitmap, _T("Add Process reference"), _T("Add Process reference"));
  AddRadioTool(GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION, _T("Add Transition"), g_icons[ _T("transition") ], wxNullBitmap, _T("Add Transition"), _T("Add Transition"));
  AddRadioTool(GRAPE_TOOL_ADD_INITIAL_DESIGNATOR, _T("Add Initial designator"), g_icons[ _T("initdes") ], wxNullBitmap, _T("Add Initial designator"), _T("Add Initial designator"));
  AddRadioTool(GRAPE_TOOL_ADD_TERMINATING_TRANSITION, _T("Add Terminating transition"), g_icons[ _T("termtransition") ], wxNullBitmap, _T("Add Terminating transition"), _T("Add Terminating transition"));
  AddRadioTool(GRAPE_TOOL_ADD_COMMENT, _T("Add Comment"), g_icons[ _T("comment") ], wxNullBitmap, _T("Add Comment"), _T("Add Comment"));
  AddTool(GRAPE_MENU_VALIDATE_DIAGRAM, _T("Validate current diagram"), g_icons[ _T("validate") ], wxNullBitmap, wxITEM_NORMAL, _T("Validate current diagram"), _T("Validate current diagram"));

  Realize();
}
