// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_base.cpp
//
// Defines GraPE events for file operations.

#include "xml.h"
#include "xmlopen.h"

#include <wx/file.h>

#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"

#include "event_diagram.h"
#include "event_file.h"

using namespace grape::libgrape;
using namespace grape::grapeapp;

grape_event_new::grape_event_new( grape_frame *p_main_frame ) : grape_event_base( p_main_frame, false, _T( "new specification" ) )
{
}

grape_event_new::~grape_event_new( void )
{
}

bool grape_event_new::Do( void )
{
  if ( !close_specification() )
  {
    return false;
  }

  // set filename
  m_main_frame->set_filename( wxFileName( _T( "untitled" ) ) );

  // Create new grapespecification
  grape_specification* new_spec = new grape_specification;
  m_main_frame->set_grape_specification( new_spec );

  m_main_frame->set_is_modified( false );

  m_main_frame->set_mode( GRAPE_MODE_SPEC );

  // reset id counter
  m_main_frame->set_id_counter( 0 );

  return true;
}

bool grape_event_new::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_open::grape_event_open( grape_frame *p_main_frame, const wxString &p_filename ) : grape_event_base( p_main_frame, false, _T( "open specification" ) )
{
  m_filename.Assign( p_filename );
}

grape_event_open::~grape_event_open( void )
{
}

bool grape_event_open::Do( void )
{
  // display open dialog if there is no filename provided
  if ( m_filename.GetFullName() == wxEmptyString )
  {
    wxFileDialog open_dialog( m_main_frame, _T( "Open specification" ), _T( "" ), _T( "" ), _T( "GraPE Specification files ( *.gra )|*.gra" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );
    int result = open_dialog.ShowModal();

    // if a file is selected and Open is clicked
    if ( result == wxID_OK )
    {
      // get filename
      m_filename.Assign( open_dialog.GetPath() );
    }
    else
    {
      // when the user clicked cancel
      return false;
    }
  }

  // Create new grapespecification and load filecontent
  grape_specification* new_spec = new grape_specification;
  long max_id = 0;
  if ( !xml_open( new_spec, m_filename.GetFullPath(), max_id ) )
  {
    wxMessageBox( m_filename.GetName() + _T( ".gra is not loaded." ), _T( "Notification" ), wxOK | wxICON_EXCLAMATION, m_main_frame );

    // opening was unsuccessful
    return false;
  }

  if ( !close_specification() )
  {
    return false;
  }

  //set id counter
  m_main_frame->set_id_counter( max_id + 1 );

  // load architecture and process diagram lists
  arr_architecture_diagram *arr_arch_dia = new_spec->get_architecture_diagram_list();
  arr_process_diagram *arr_proc_dia = new_spec->get_process_diagram_list();
  for ( unsigned int i = 0; i < arr_arch_dia->GetCount(); ++i )
  {
    m_main_frame->get_architecture_diagram_listbox()->Append( arr_arch_dia->Item( i ).get_name() );
  }
  for ( unsigned int i = 0; i < arr_proc_dia->GetCount(); ++i )
  {
    m_main_frame->get_process_diagram_listbox()->Append( arr_proc_dia->Item( i ).get_name() );
  }

  // set grape specification
  m_main_frame->set_grape_specification( new_spec );

  // update process listbox
  grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
  if ( arch_listbox->GetCount() >= 1 )
  {
    arch_listbox->Select( 0 );
  }
  else
  {
    // if the architecture diagram listbox is empty, there will be no selection, try process diagram listbox
    m_main_frame->get_process_diagram_listbox()->Select( 0 );
  }

  // update grape mode
  wxString m_selected_diagram = arch_listbox->GetStringSelection();
  if ( m_selected_diagram == wxEmptyString )
  {
    // if this is also empty there will be no selection; grape_event_select_diagram will process according to that.
    m_selected_diagram = m_main_frame->get_process_diagram_listbox()->GetStringSelection();
  }

  // select new diagram
  grape_event_select_diagram *event = new grape_event_select_diagram(m_main_frame, m_selected_diagram);
  m_main_frame->get_event_handler()->Submit(event, false);

  // set filename
  m_main_frame->set_filename( m_filename );

  m_main_frame->set_is_modified( false );

  return true;
}

bool grape_event_open::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_close::grape_event_close( grape_frame *p_main_frame ) : grape_event_base( p_main_frame, false, _T( "close specification" ) )
{
}

grape_event_close::~grape_event_close( void )
{
}

bool grape_event_close::Do( void )
{
  return close_specification();
}

bool grape_event_close::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_save::grape_event_save( grape_frame *p_main_frame ) : grape_event_base( p_main_frame, false, _T( "save specification" ) )
{
}

grape_event_save::~grape_event_save( void )
{
}

bool grape_event_save::Do( void )
{
  // access the main frame through m_main_frame->
  // if not saved before
  if ( !m_main_frame->get_filename().HasExt() )
  {
    grape_event_saveas *event = new grape_event_saveas( m_main_frame );
    return m_main_frame->get_event_handler()->Submit( event, false );
  }
  else
  {
    m_main_frame->save_datatype_specification();
    // check if there are changes after last save ( as ) event
    if ( m_main_frame->get_is_modified() )
    {
      // save by converting the grape specification to xml
      wxString filename = m_main_frame->get_filename().GetFullPath();
      xml_convert( *m_main_frame->get_grape_specification(), filename, 2 );

      // mark as not modified ( saved )
      m_main_frame->set_is_modified( false );

      return true;
    }
    else
    {
      return true;
    }
  }
}

bool grape_event_save::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_saveas::grape_event_saveas( grape_frame *p_main_frame ) : grape_event_base( p_main_frame, false, _T( "save specification as" ) )
{
}

grape_event_saveas::~grape_event_saveas( void )
{
}

bool grape_event_saveas::Do( void )
{
  // access the main frame through m_main_frame->
  // display save dialog
  wxFileDialog save_dialog( m_main_frame, _T( "Save as..." ), m_main_frame->get_filename().GetPath(), m_main_frame->get_filename().GetFullName(), _T( "GraPE Specification files ( *.gra )|*.gra" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
  int result = save_dialog.ShowModal();
  // if a file is selected and Save is clicked
  if ( result == wxID_OK )
  {
    m_main_frame->save_datatype_specification();

    // get filename
    wxFileName filename( save_dialog.GetPath() );

    // set file extension
    if ( filename.GetExt().Lower() != _T("gra") )
    {
      wxString fullname = filename.GetFullName();
      filename.SetName( fullname );
      filename.SetExt( _T( "gra" ) );
      if ( filename.FileExists() )
      {
        wxString s = _T("The file ");
        s += filename.GetFullPath();
        s += _T(" already exists, do you wish to overwrite this file?" );
        int result = wxMessageBox( s, _T("Question"), wxYES_NO | wxICON_QUESTION, m_main_frame );
        if ( result == wxNO )
        {
          return false;
        }
      }
    }
    // set filename
    m_main_frame->set_filename( filename );

    // save by converting the grape specification to xml
    wxString file = m_main_frame->get_filename().GetFullPath();
    xml_convert( *m_main_frame->get_grape_specification(), file, 2 );

    // mark as not modified ( saved )
    m_main_frame->set_is_modified( false );

    return true;
  }

  return false;
}

bool grape_event_saveas::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_print::grape_event_print( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "print specification" ) )
{
}

grape_event_print::~grape_event_print( void )
{
}

bool grape_event_print::Do( void )
{
  // Low priority; not implemented
  return false;
}

bool grape_event_print::Undo( void )
{
  // cannot be undone
  return false;
}



grape_event_import::grape_event_import( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "import" ) )
{
}

grape_event_import::~grape_event_import( void )
{
}

bool grape_event_import::Do( void )
{
  return true;
}

bool grape_event_import::Undo( void )
{
  return true;
}



grape_event_export_datatype_specification_text::grape_event_export_datatype_specification_text( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "export datatype specification to text" ) )
{
}

grape_event_export_datatype_specification_text::~grape_event_export_datatype_specification_text( void )
{
}

bool grape_event_export_datatype_specification_text::Do( void )
{
  // display save dialog
  wxFileDialog save_dialog( m_main_frame, _T( "Export to text..." ),  m_main_frame->get_filename().GetPath(), _T( "" ), _T( "All files ( *.* )|*.*" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT ); 
  int result = save_dialog.ShowModal();
  if ( result == wxID_OK )
  {
    // get filename
    wxFileName filename( save_dialog.GetPath() );
    wxFile new_file( filename.GetFullPath(), wxFile::write );
    if ( new_file.IsOpened() )
    {
      m_main_frame->save_datatype_specification();
      grape_specification* spec = m_main_frame->get_grape_specification();
      datatype_specification* data_spec = spec->get_datatype_specification();
      bool succes = new_file.Write( data_spec->get_declarations() );
      new_file.Close();
      return succes;
    }
  }
  return false;
}

bool grape_event_export_datatype_specification_text::Undo( void )
{
  return true;
}
