// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_frame.cpp
//
// Implements the main frame of the application class.

#include <wx/cmdproc.h>
#include <wx/filesys.h>
#include <wx/fs_arc.h>
#include <wx/html/helpctrl.h>
#include <wx/statusbr.h>
#include <wx/tglbtn.h>
#include <wx/timer.h>

#include "grape_frame.h"
#include "grape_events.h"
#include "grape_menubar.h"
#include "grape_toolbar.h"
#include "grape_glcanvas.h"
#include "grape_logpanel.h"
#include "grape_clipboard.h"
#include "grape_listbox.h"
#include "grape_ids.h"

// window icon
#include "pics/grape.xpm"

using namespace grape::grapeapp;
using namespace grape::libgrape;

grape_frame::grape_frame( const wxString &p_filename )
: wxFrame( NULL, wxID_ANY, _T("GraPE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("GraPE") )
, m_modified( false )
, m_specification( 0 )
, m_mode( GRAPE_MODE_NONE )
{
  // initialize widgets
  m_dataspecbutton = new wxToggleButton( this, GRAPE_DATATYPE_SPEC_BUTTON, _T("Datatype specification") );
  m_process_diagram_list = new grape_listbox(this, GRAPE_PROCESS_DIAGRAM_LIST, this);
  m_architecture_diagram_list = new grape_listbox(this, GRAPE_ARCHITECTURE_DIAGRAM_LIST, this);
  m_splitter = new wxSplitterWindow( this, GRAPE_SPLITTER );
  m_splitter->SetMinimumPaneSize( 1 );
  m_clipboard = new grape_clipboard( this );
  m_timer = new wxTimer( this, GRAPE_TIMER );
  m_timer->Start( 2000, true );


  int gl_args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
  m_datatext = new wxTextCtrl( m_splitter, GRAPE_DATASPEC_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_PROCESS_TAB | wxTE_PROCESS_ENTER );
  m_glcanvas = new grape_glcanvas(m_splitter, gl_args, this);
  m_logpanel = new grape_logpanel(m_splitter);
  m_splitter->SetSplitMode(wxSPLIT_HORIZONTAL);
  m_splitter->SplitHorizontally(m_glcanvas, m_logpanel);
  show_log_panel();
  m_statusbar = new wxStatusBar(this);
  m_menubar = new grape_menubar();
  
  SetBackgroundColour(m_menubar->GetBackgroundColour());
  
  // place widgets in sizers
  wxBoxSizer *main_box = new wxBoxSizer(wxHORIZONTAL);
  main_box->Add(m_splitter, 1, wxEXPAND | wxBOTTOM, 5);

  wxBoxSizer *process_box = new wxBoxSizer(wxVERTICAL);
  wxStaticText *proc_text = new wxStaticText( this, -1, _T("Processes"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
  process_box->Add( proc_text, 0, wxALIGN_CENTRE | wxEXPAND );
  process_box->Add(m_process_diagram_list, 1, wxEXPAND | wxALL | wxALIGN_CENTER, 5);

  wxBoxSizer *architecture_box = new wxBoxSizer(wxVERTICAL);
  wxStaticText *arch_text = new wxStaticText( this, -1, _T("Architectures"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
  architecture_box->Add( arch_text, 0, wxALIGN_CENTRE | wxEXPAND );
  architecture_box->Add(m_architecture_diagram_list, 1, wxEXPAND | wxALL | wxALIGN_CENTER, 5);

  wxBoxSizer *right_box = new wxBoxSizer(wxVERTICAL);
  right_box->Add(m_dataspecbutton, 0, wxEXPAND);
  right_box->Add(architecture_box, 1, wxEXPAND);
  right_box->Add(process_box, 1, wxEXPAND);

  main_box->Add(right_box, 0, wxEXPAND | wxALIGN_RIGHT);

  // initialize frame
  SetSizer(main_box);
  SetSize(wxSize(1000, 800));

  // set window icon
  SetIcon( grape_xpm );

  // attach statusbar, menubar and set the frame status (which sets the correct toolbar)
  SetStatusBar(m_statusbar);
  SetMenuBar(m_menubar);
  set_mode( GRAPE_MODE_NONE );

  // init counter
  m_counter = 0;
  // attach event handler to edit menu
  m_event_handler = new wxCommandProcessor;
  m_event_handler->SetEditMenu(m_menubar->get_menu( grape_menubar::GRAPE_MENU_EDIT ));
  m_event_handler->Initialize();

  // initialize variables
  bool show_messagebox = false;
  if ( p_filename.IsEmpty() ) // no filename supplied, use default one
  {
    wxCommandEvent event;
    event_menu_new( event );
  }
  else  // filename supplied
  {
    wxFileName filename = wxFileName( p_filename );
    // check filename
    if ( filename.FileExists() && ( filename.GetExt().Lower() == _T("gra") ) ) // filename ok
    {
      grape_event_open *event = new grape_event_open( this, p_filename );
      m_event_handler->Submit( event, false );
      update_bars();
    }
    else // filename not ok, open new
    {
      show_messagebox = true;
      wxCommandEvent event;
      event_menu_new( event );
    }
  }

  set_is_modified( false ); // implicitely calls set_title

  wxFileSystem fs;
  wxFileSystem::AddHandler(new wxArchiveFSHandler);
  m_help_controller = new wxHtmlHelpController(wxHF_DEFAULT_STYLE, this);
  wxString filename = wxEmptyString;
  if ( fs.FindFileInPath( &filename, _T( GRAPE_HELP_DIR ), _T("grapehelp.zip") ) )
  {
    // file found
    m_help_controller->AddBook( wxFileName( filename ) );
  }

  // show frame;
  Show();

  if ( show_messagebox )
  {
    wxMessageBox( _T("The file \"") + p_filename + _T("\" could not be opened."), _T("Warning"), wxOK | wxICON_EXCLAMATION, this );
  }
}

grape_frame::~grape_frame( void )
{
  delete m_process_diagram_list;
  m_process_diagram_list = 0;
  delete m_architecture_diagram_list;
  m_architecture_diagram_list = 0;
  delete m_glcanvas;
  m_glcanvas = 0;
  delete m_logpanel;
  m_logpanel = 0;
  delete m_splitter;
  m_splitter = 0;
  delete m_specification;
  m_specification = 0;
  delete m_event_handler;
  m_event_handler = 0;
  delete m_timer;
  m_timer = 0;
  delete m_help_controller;
  m_help_controller = 0;
  delete m_clipboard;
  m_clipboard = 0;
}

grape_menubar * grape_frame::get_menubar( void )
{
  return m_menubar;
}

grape_listbox * grape_frame::get_process_diagram_listbox( void )
{
  return m_process_diagram_list;
}

grape_listbox * grape_frame::get_architecture_diagram_listbox( void )
{
  return m_architecture_diagram_list;
}

grape_logpanel * grape_frame::get_logpanel( void )
{
  return m_logpanel;
}

wxStatusBar * grape_frame::get_statusbar( void )
{
  return m_statusbar;
}

grape_glcanvas * grape_frame::get_glcanvas( void )
{
  return m_glcanvas;
}

wxCommandProcessor * grape_frame::get_event_handler( void )
{
  return m_event_handler;
}

grape_clipboard * grape_frame::get_clipboard( void )
{
  return m_clipboard;
}

void grape_frame::show_log_panel()
{
  int height_w1, height_w2;
  m_splitter->GetWindow1()->GetClientSize( 0, &height_w1 );
  m_splitter->GetWindow2()->GetClientSize( 0, &height_w2 );
  m_splitter->SetSashPosition( height_w1 + height_w2 - 100 );
}

void grape_frame::hide_log_panel()
{
  // be sure to hit the bottom of the window
  m_splitter->SetSashPosition( 5000 );
}

wxFileName grape_frame::get_filename( void ) const
{
  return m_filename;
}

void grape_frame::set_filename( const wxFileName &p_filename )
{
  m_filename = p_filename;
}

bool grape_frame::get_is_modified( void ) const
{
  return m_modified;
}

void grape_frame::set_is_modified( bool p_modified )
{
  // set modified
  m_modified = p_modified;
  // set title
  set_title();
}


grape_specification* grape_frame::get_grape_specification( void ) const
{
  return m_specification;
}

void grape_frame::set_grape_specification( grape_specification* p_grape_spec )
{
  m_specification = p_grape_spec;
}

void grape_frame::set_toolbar( grape_mode p_mode )
{
  wxToolBar *current_toolbar = GetToolBar();
  delete current_toolbar;

  grape_toolbar *toolbar = 0;

  switch ( p_mode )
  {
    case GRAPE_MODE_NONE:
      toolbar = new grape_toolbar(this);
      toolbar->set_mode( grape_toolbar::GRAPE_TOOLMODE_NOSPEC );
      m_glcanvas->set_canvas_state( IDLE );
      break;
    case GRAPE_MODE_DATASPEC: // fall through
    case GRAPE_MODE_SPEC:
      toolbar = new grape_toolbar(this);
      toolbar->set_mode( grape_toolbar::GRAPE_TOOLMODE_SPEC );
      m_glcanvas->set_canvas_state( IDLE );
      break;
    case GRAPE_MODE_ARCH:
      toolbar = new grape_arch_toolbar(this);
      toolbar->set_mode( grape_toolbar::GRAPE_TOOLMODE_SPEC | grape_toolbar::GRAPE_TOOLMODE_ARCH );
      m_glcanvas->set_canvas_state( SELECT );
      toolbar->ToggleTool( GRAPE_TOOL_SELECT, true );
      break;
    case GRAPE_MODE_PROC:
      toolbar = new grape_proc_toolbar(this);
      toolbar->set_mode( grape_toolbar::GRAPE_TOOLMODE_SPEC | grape_toolbar::GRAPE_TOOLMODE_PROC );
      m_glcanvas->set_canvas_state( SELECT );
      toolbar->ToggleTool( GRAPE_TOOL_SELECT, true );
      break;
    default: toolbar = 0;
  }

  SetToolBar( toolbar );
}

void grape_frame::set_id_counter( long p_counter )
{
  m_counter = p_counter;
}

unsigned int grape_frame::get_new_id( void )
{
  return m_counter++;
}

BEGIN_EVENT_TABLE(grape_frame, wxFrame)
  // file menu
  EVT_MENU(wxID_NEW, grape_frame::event_menu_new)
  EVT_MENU(wxID_OPEN, grape_frame::event_menu_open)
  EVT_MENU(wxID_CLOSE, grape_frame::event_menu_close)
  EVT_MENU(wxID_SAVE, grape_frame::event_menu_save)
  EVT_MENU(wxID_SAVEAS, grape_frame::event_menu_saveas)
  EVT_MENU(GRAPE_MENU_EXPORTMCRL2, grape_frame::event_menu_exportmcrl2)
  EVT_MENU(GRAPE_MENU_EXPORTIMAGE, grape_frame::event_menu_exportimage)
  EVT_MENU(GRAPE_MENU_EXPORTTEXT, grape_frame::event_menu_exporttext)
  EVT_MENU(GRAPE_MENU_VALIDATE, grape_frame::event_menu_validate_specification)
  EVT_MENU(wxID_PRINT, grape_frame::event_menu_print)
  EVT_MENU(wxID_EXIT, grape_frame::event_menu_quit)

  // edit menu
  EVT_MENU(wxID_UNDO, grape_frame::event_menu_undo)
  EVT_MENU(wxID_REDO, grape_frame::event_menu_redo)
  EVT_MENU(wxID_CUT, grape_frame::event_menu_cut)
  EVT_MENU(wxID_COPY, grape_frame::event_menu_copy)
  EVT_MENU(wxID_PASTE, grape_frame::event_menu_paste)
  EVT_MENU(wxID_DELETE, grape_frame::event_menu_delete)
  EVT_MENU(GRAPE_MENU_PROPERTIES, grape_frame::event_menu_properties)
  EVT_MENU(GRAPE_MENU_SELECT_ALL, grape_frame::event_menu_select_all)
  EVT_MENU(GRAPE_MENU_DESELECT_ALL, grape_frame::event_menu_deselect_all)
  EVT_MENU(GRAPE_MENU_DATATYPESPEC, grape_frame::event_datatype_spec)

  // diagram menu
  EVT_MENU(GRAPE_MENU_ADD_ARCHITECTURE_DIAGRAM, grape_frame::event_menu_add_architecture_diagram)
  EVT_MENU(GRAPE_MENU_ADD_PROCESS_DIAGRAM, grape_frame::event_menu_add_process_diagram)
  EVT_MENU(GRAPE_MENU_RENAME_DIAGRAM, grape_frame::event_menu_rename_diagram)
  EVT_MENU(GRAPE_MENU_REMOVE_DIAGRAM, grape_frame::event_menu_remove_diagram)
  EVT_MENU(GRAPE_MENU_VALIDATE_DIAGRAM, grape_frame::event_menu_validate_diagram)

  // help menu
  EVT_MENU(wxID_HELP, grape_frame::event_menu_help)
  EVT_MENU(wxID_ABOUT, grape_frame::event_menu_about)

  // toolbar + tools menu
  EVT_TOOL(GRAPE_TOOL_SELECT, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ATTACH, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_DETACH, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_INITIAL_DESIGNATOR, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_REFERENCE_STATE, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_STATE, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_TERMINATING_TRANSITION, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_PROCESS_REFERENCE, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_CHANNEL, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_VISIBLE, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_BLOCKED, grape_frame::event_tool_selected)
  EVT_TOOL(GRAPE_TOOL_ADD_COMMENT, grape_frame::event_tool_selected)
  EVT_TOOL_ENTER(wxID_ANY, grape_frame::update_statusbar )

  // listbox
  EVT_TOGGLEBUTTON(GRAPE_DATATYPE_SPEC_BUTTON, grape_frame::event_datatype_spec)
  EVT_LISTBOX(GRAPE_ARCHITECTURE_DIAGRAM_LIST, grape_frame::event_select_diagram)
  EVT_LISTBOX(GRAPE_PROCESS_DIAGRAM_LIST, grape_frame::event_select_diagram)
  EVT_LISTBOX_DCLICK(GRAPE_ARCHITECTURE_DIAGRAM_LIST, grape_frame::event_menu_rename_diagram)
  EVT_LISTBOX_DCLICK(GRAPE_PROCESS_DIAGRAM_LIST, grape_frame::event_menu_rename_diagram)

  // frame
  EVT_SPLITTER_DCLICK(GRAPE_SPLITTER, grape_frame::event_splitter_dclick)
  EVT_CLOSE( grape_frame::event_window_close )
  EVT_TIMER(GRAPE_TIMER, grape_frame::grape_event_timer)

  // datatype specification
  EVT_TEXT( GRAPE_DATASPEC_TEXT, grape_frame::dataspec_modified )

END_EVENT_TABLE()

wxHtmlHelpController* grape_frame::get_help_controller( void )
{
  return m_help_controller;
}

void grape_frame::toggle_view( grape_mode p_mode )
{
  if ( p_mode == GRAPE_MODE_DATASPEC ) // switching to dts
  {
    load_datatype_specification();
    m_dataspecbutton->SetValue( true );
    m_splitter->ReplaceWindow( m_splitter->GetWindow1(), m_datatext );
    m_datatext->Show();
    m_datatext->SetFocus();
    m_glcanvas->Hide();

    // update statusbar
    m_statusbar->PushStatusText( _T("In the text field you can enter a datatype specification") );
  }
  else // switching back to canvas
  {
    save_datatype_specification();
    m_splitter->ReplaceWindow( m_splitter->GetWindow1(), m_glcanvas );
    m_datatext->Hide();
    m_glcanvas->Show();
    if ( m_glcanvas->get_diagram() )
    {
      grape_event_select_diagram *event = new grape_event_select_diagram( this, m_glcanvas->get_diagram()->get_name() );
      m_event_handler->Submit( event, false );

      if ( !m_architecture_diagram_list->SetStringSelection( m_glcanvas->get_diagram()->get_name() ) )
      {
        m_process_diagram_list->SetStringSelection( m_glcanvas->get_diagram()->get_name() );
      }
    }
    // update statusbar
    if ( m_statusbar->GetStatusText() == _T("In the text field you can enter a datatype specification") )
    {
      m_statusbar->PopStatusText();
      if ( m_statusbar->GetStatusText() == _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") ) 
      {
        m_statusbar->PopStatusText();
      }
      else if ( m_statusbar->GetStatusText() == wxEmptyString )
      {
        if ( m_mode != GRAPE_MODE_SPEC )
        {
          m_statusbar->PushStatusText( _T("Click -> select object. Drag -> move object. Drag border -> resize object. Double click -> edit object properties.") );
        }
      }
      else
      {
        m_statusbar->PopStatusText();
        m_statusbar->PushStatusText( _T("Click -> select object. Drag -> move object. Drag border -> resize object. Double click -> edit object properties.") );
      }
    }
  }
}

void grape_frame::load_datatype_specification()
{
  if ( m_specification && !m_datatext->IsModified() )
  {
    datatype_specification *dts = m_specification->get_datatype_specification();

    if ( dts )
    {
      m_datatext->ChangeValue( dts->get_declarations() );
    }
  }
}

void grape_frame::save_datatype_specification()
{
  if ( m_specification )
  {
    datatype_specification *dts = m_specification->get_datatype_specification();

    if ( dts && m_datatext->IsModified() )
    {
      dts->set_declarations( m_datatext->GetValue() );
      m_datatext->SetModified( false );
    }
  }
}

void grape_frame::set_title()
{
  wxString title = m_filename.GetFullName();

  if ( !title.IsEmpty() )
  {
    if ( m_modified )
    {
      title += _T(" [modified]");
    }

    title += _T(" - ");
  }

  title += _T("GraPE");

  SetTitle( title );
}

void grape_frame::set_mode( grape_mode p_mode )
{
  if ( ( p_mode == m_mode ) && ( p_mode != GRAPE_MODE_NONE ) )
  {
    return;
  }

  m_mode = p_mode;

  switch( p_mode )
  {
    case GRAPE_MODE_NONE:
    {
      m_process_diagram_list->Select( wxNOT_FOUND );
      m_architecture_diagram_list->Select( wxNOT_FOUND );
      m_menubar->set_mode( grape_menubar::GRAPE_MENUMODE_NOSPEC );
      m_dataspecbutton->Enable( false );
      m_dataspecbutton->SetValue( false );
      break;
    }
    case GRAPE_MODE_SPEC:
    {
      m_process_diagram_list->Select( wxNOT_FOUND );
      m_architecture_diagram_list->Select( wxNOT_FOUND );
      m_menubar->set_mode( grape_menubar::GRAPE_MENUMODE_SPEC );
      m_dataspecbutton->Enable();
      m_dataspecbutton->SetValue( false );
      break;
    }
    case GRAPE_MODE_ARCH:
    {
      m_process_diagram_list->Select( wxNOT_FOUND );
      m_menubar->set_mode( grape_menubar::GRAPE_MENUMODE_SPEC | grape_menubar::GRAPE_MENUMODE_ARCH );
      m_dataspecbutton->Enable();
      m_dataspecbutton->SetValue( false );
      break;
    }
    case GRAPE_MODE_PROC:
    {
      m_architecture_diagram_list->Select( wxNOT_FOUND );
      m_menubar->set_mode( grape_menubar::GRAPE_MENUMODE_SPEC | grape_menubar::GRAPE_MENUMODE_PROC );
      m_dataspecbutton->Enable();
      m_dataspecbutton->SetValue( false );
      break;
    }
    case GRAPE_MODE_DATASPEC:
    {
      m_process_diagram_list->Select( wxNOT_FOUND );
      m_architecture_diagram_list->Select( wxNOT_FOUND );
      m_menubar->set_mode( grape_menubar::GRAPE_MENUMODE_DATASPEC );
      m_dataspecbutton->Enable();
      m_dataspecbutton->SetValue( true );
      break;
    }
    default:
    {
	  // cannot be the case
      // assert( false );
    }
  }

  set_toolbar( p_mode );
  toggle_view( p_mode );
}

grape_mode grape_frame::get_mode( void )
{
  return m_mode;
}

void grape_frame::update_bars( void )
{
  update_menubar();
  update_toolbar();
  wxCommandEvent event;
  update_statusbar( event );
}

void grape_frame::update_toolbar( void )
{
  // update UNDO and REDO right for the toolbar
  GetToolBar()->EnableTool( wxID_UNDO, m_menubar->IsEnabled( wxID_UNDO ) );
  GetToolBar()->SetToolShortHelp( wxID_UNDO, m_menubar->FindItem( wxID_UNDO )->GetLabelFromText( m_menubar->FindItem( wxID_UNDO )->GetLabel() ) );
  GetToolBar()->EnableTool( wxID_REDO, m_menubar->IsEnabled( wxID_REDO ) );
  GetToolBar()->SetToolShortHelp( wxID_REDO, m_menubar->FindItem( wxID_REDO )->GetLabelFromText( m_menubar->FindItem( wxID_REDO )->GetLabel() ) );

  // update DELETE and PROPERTIES right for the toolbar
  GetToolBar()->EnableTool( wxID_DELETE, m_menubar->IsEnabled( wxID_DELETE ) );
  GetToolBar()->EnableTool( GRAPE_MENU_PROPERTIES, m_menubar->IsEnabled( GRAPE_MENU_PROPERTIES ) );
}
  
void grape_frame::update_menubar( void )
{
  diagram *dia_ptr = m_glcanvas->get_diagram();
  if ( dia_ptr && !(m_mode & GRAPE_MODE_DATASPEC) )
  {
    bool object_selected = dia_ptr->count_selected_objects() > 0;
    m_menubar->Enable( wxID_DELETE, object_selected );
    m_menubar->Enable( GRAPE_MENU_DESELECT_ALL, object_selected );
    // m_menubar->Enable( GRAPE_MENU_SELECT_ALL, m_glcanvas->count_visual_object() > 1 ); // 1 because the visibility frame or the preamble are also visual objects, wich can not be selected
    m_menubar->Enable( GRAPE_MENU_PROPERTIES, dia_ptr->count_selected_objects() == 1 );
  }
  else
  {
    m_menubar->Enable( wxID_DELETE, false );
    m_menubar->Enable( GRAPE_MENU_DESELECT_ALL, false );
    m_menubar->Enable( GRAPE_MENU_SELECT_ALL, false );
    m_menubar->Enable( GRAPE_MENU_PROPERTIES, false );
  }
}

void grape_frame::update_statusbar( wxCommandEvent& p_event )
{
  if ( m_mode == GRAPE_MODE_DATASPEC )
  {
    if ( p_event.GetSelection() == -1 )
    {
      if ( m_statusbar->GetStatusText() != _T("In the text field you can enter a datatype specification") )
      {
        m_statusbar->PushStatusText( _T("In the text field you can enter a datatype specification") );
      }
    }
  }
  else
  {
    if ( p_event.GetSelection() == -1 )
    {
      if ( ( m_mode == GRAPE_MODE_ARCH ) || ( m_mode == GRAPE_MODE_PROC ) )
      {
        canvas_state state = m_glcanvas->get_canvas_state();
        wxString status_text;
        switch( state )
        {
          case SELECT:
            status_text = _T("Click -> select object. Drag -> move object. Drag border -> resize object. Double click -> edit object properties."); break;
          case ATTACH:
            {
              if ( m_architecture_diagram_list->GetSelection() != wxNOT_FOUND )
              {
                status_text = _T("Drag from visible / blocked to channel (communication), channel communication to channel, comment to an object or vice versa.");
              }
              else
              {
                status_text = _T("Drag from (terminating) transition / initial designator to state / process reference, comment to an object or vice versa.");
              }
              break;
            }
          case DETACH:
            status_text = _T("Click one of the attached objects to detach it."); break;
          case ADD_COMMENT:
            status_text = _T("Click to add a comment"); break;
          case ADD_TERMINATING_TRANSITION:
            status_text = _T("Drag from a state to add a terminating transition"); break;
          case ADD_NONTERMINATING_TRANSITION:
            status_text = _T("Drag from a beginstate to an endstate to add a transition"); break;
          case ADD_INITIAL_DESIGNATOR:
            status_text = _T("Click a state to add an initial designator"); break;
          case ADD_STATE:
            status_text = _T("Click to add a state"); break;
          case ADD_REFERENCE_STATE:
          case ADD_PROCESS_REFERENCE:
            status_text = _T("Click to add a process reference"); break;
          case ADD_ARCHITECTURE_REFERENCE:
            status_text = _T("Click to add an architecture reference"); break;
          case ADD_CHANNEL:
            status_text = _T("Click on a reference to add a channel"); break;
          case ADD_CHANNEL_COMMUNICATION:
            status_text = _T("Drag from a channel to another channel to add a channel communication"); break;
          case ADD_VISIBLE:
            status_text = _T("Click on a channel or channel communication to make it visible"); break;
          case ADD_BLOCKED:
            status_text = _T("Click on a channel or channel communication to make it blocked"); break;
          case IDLE:
          default: status_text = wxEmptyString; break;
        }
        if ( ( m_statusbar->GetStatusText() != _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") ) || ( m_glcanvas == FindFocus() ) )
        {
	      if (!m_statusbar->GetStatusText().IsEmpty()) {
		    m_statusbar->PopStatusText();
	      }
          if ( m_statusbar->GetStatusText() != wxEmptyString ) {
            m_statusbar->PopStatusText();
            m_statusbar->PushStatusText( status_text );
          }
          else
          {
            m_statusbar->PushStatusText( status_text );
          }
        }
      }
      else
      {
        m_statusbar->PushStatusText(wxEmptyString);
      }
    }
    else
    {
      if ( ( m_statusbar->GetStatusText() == _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") ) && ( m_glcanvas == FindFocus() ) )
      {
        m_statusbar->PopStatusText();
      }
    }
  }
}
