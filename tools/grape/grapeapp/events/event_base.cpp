// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_base.cpp
//
// Implements the base class for GraPE events

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"
#include "grape_logpanel.h"

#include "event_file.h"

#include "event_base.h"
#include "event_diagram.h"

using namespace grape::grapeapp;

       
diagram* grape::grapeapp::find_a_diagram( grape_frame *m_main_frame, wxString p_dia_name, grape_diagram_type p_dia_type )
{
  diagram* result = 0;
  grape_specification* spec = m_main_frame->get_grape_specification();
  if ( ( p_dia_type == GRAPE_NO_DIAGRAM ) || ( p_dia_type == GRAPE_PROCESS_DIAGRAM ) )
  {
    for ( unsigned int i = 0; i < spec->count_process_diagram(); ++i )
    {
      process_diagram* proc_dia_ptr = spec->get_process_diagram( i );
      if ( proc_dia_ptr->get_name() == p_dia_name )
      {
        result = proc_dia_ptr;
        break; // break from the for-loop; we've found something
      }
    }
  }
  if ( ( p_dia_type == GRAPE_NO_DIAGRAM ) || ( p_dia_type == GRAPE_ARCHITECTURE_DIAGRAM ) )
  {
    for ( unsigned int i = 0; ( result == 0 ) && ( i < spec->count_architecture_diagram() ); ++i )
    {
      architecture_diagram* arch_dia_ptr = spec->get_architecture_diagram( i );
      if ( arch_dia_ptr->get_name() == p_dia_name )
      {
        result = arch_dia_ptr;
        break; // break from the for-loop; we've found something
      }
    }
  }
  return result;
}

diagram* grape::grapeapp::find_a_diagram( grape_frame *m_main_frame, unsigned int p_dia_id, grape_diagram_type p_dia_type )
{
  diagram* result = 0;
  grape_specification* spec = m_main_frame->get_grape_specification();
  if ( ( p_dia_type == GRAPE_NO_DIAGRAM ) || ( p_dia_type == GRAPE_PROCESS_DIAGRAM ) )
  {
    for ( unsigned int i = 0; i < spec->count_process_diagram(); ++i )
    {
      process_diagram* proc_dia_ptr = spec->get_process_diagram( i );
      if ( proc_dia_ptr->get_id() == p_dia_id )
      {
        result = proc_dia_ptr;
        break; // break from the for-loop; we've found something
      }
    }
  }
  if ( ( p_dia_type == GRAPE_NO_DIAGRAM ) || ( p_dia_type == GRAPE_ARCHITECTURE_DIAGRAM ) )
  {
    for ( unsigned int i = 0; ( result == 0 ) && ( i < spec->count_architecture_diagram() ); ++i )
    {
      architecture_diagram* arch_dia_ptr = spec->get_architecture_diagram( i );
      if ( arch_dia_ptr->get_id() == p_dia_id )
      {
        result = arch_dia_ptr;
        break; // break from the for-loop; we've found something
      }
    }
  }
  return result;
}

grape_event_base::grape_event_base( grape_frame *p_main_frame, bool p_can_undo, const wxString &p_name ) : wxCommand( p_can_undo, p_name )
{
  // store pointer to main frame
  m_main_frame = p_main_frame;
}

grape_event_base::~grape_event_base( void )
{
}

void grape_event_base::finish_modification( void )
{
  // set modified
  m_main_frame->set_is_modified( true );
  // refresh visuals
  m_main_frame->get_glcanvas()->reload_visual_objects();
}

object* grape_event_base::find_object( unsigned int p_obj_id, object_type p_obj_type, int p_dia_id )
{
  if ( p_dia_id == -1 ) // no diagram id was supplied, search the whole spec
  {
    grape_specification* spec = m_main_frame->get_grape_specification();
    return grape_specification::find_object( spec, p_obj_id, p_obj_type );
  }

  object* result = 0;

  diagram* dia_ptr = find_diagram( p_dia_id );
  process_diagram* proc_dia_ptr = dynamic_cast<process_diagram*> ( dia_ptr );
  if ( proc_dia_ptr != 0 ) // Cast succesful
  {
    result = process_diagram::find_object( proc_dia_ptr, p_obj_id, p_obj_type );
  }
  else
  {
    architecture_diagram* arch_dia_ptr = dynamic_cast<architecture_diagram*> ( dia_ptr );
    assert( arch_dia_ptr != 0 );

    result = architecture_diagram::find_object( arch_dia_ptr, p_obj_id, p_obj_type );
  }
  return result;
}

diagram* grape_event_base::find_diagram( wxString p_name, grape_diagram_type p_dia_type )
{
  return find_a_diagram( m_main_frame, p_name, p_dia_type );
}
        
diagram* grape_event_base::find_diagram( unsigned int p_dia_id, grape_diagram_type p_dia_type )
{
   return find_a_diagram( m_main_frame, p_dia_id, p_dia_type );
}
 
bool grape_event_base::close_specification()
{
  // access the main frame through m_main_frame->
  if ( m_main_frame->get_is_modified() ) // Retrieve whether the document was modified and show dialog.
  {
    int answer = wxMessageBox( _T( "Do you wish to save changes to ") + m_main_frame->get_filename().GetFullName() + _T("?" ), _T( "Question" ), wxCANCEL | wxYES_NO | wxICON_QUESTION, m_main_frame );

    switch ( answer )
    {
      case wxYES:
      {
        grape_event_save *event = new grape_event_save( m_main_frame );
        if ( !m_main_frame->get_event_handler()->Submit( event, false ) ) // user pressed Cancel in save dialog
        {
          return false;
        }
        break;
      }
      case wxNO: break;
      case wxCANCEL:
      default:
        // user cancelled, don't throw away anything.
        return false;
        break;
    }
  }

  // get specification
  grape_specification* del_spec = m_main_frame->get_grape_specification();

  // reset listboxes
  m_main_frame->get_process_diagram_listbox()->Clear();
  m_main_frame->get_architecture_diagram_listbox()->Clear();

  // reset eventhandler
  m_main_frame->get_event_handler()->ClearCommands();
  m_main_frame->get_event_handler()->Initialize();

  // reset logpanel
  m_main_frame->get_logpanel()->Clear();

  // reset canvas
  m_main_frame->get_glcanvas()->set_diagram( 0 );
  m_main_frame->get_glcanvas()->reset();

  // reset frame (variables)
  m_main_frame->set_filename( wxFileName( wxEmptyString ) );
  m_main_frame->set_is_modified( false );
  m_main_frame->set_mode( GRAPE_MODE_NONE );
  m_main_frame->GetToolBar()->EnableTool( wxID_UNDO, false );
  m_main_frame->GetToolBar()->SetToolShortHelp( wxID_UNDO, _T("Undo") );
  m_main_frame->GetToolBar()->EnableTool( wxID_REDO, false );
  m_main_frame->GetToolBar()->SetToolShortHelp( wxID_REDO, _T("Redo") );

  //Destroy grapespecification
  delete del_spec;
  m_main_frame->set_grape_specification( 0 );

  // closing was successful
  return true;
}
