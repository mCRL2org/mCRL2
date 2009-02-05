// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file architecturediagram.cpp
//
// Implements the architecture_diagram class.

#include "architecturediagram.h"

using namespace grape::libgrape;

// private
architecture_diagram::architecture_diagram(void)
: diagram()
{
  m_blocked.Empty();
  m_visibles.Empty();
  m_channel_communications.Empty();
  m_channels.Empty();
  m_architecture_references.Empty();
  m_process_references.Empty();
}

architecture_diagram::architecture_diagram( const architecture_diagram &p_arch_diagram )
: diagram( p_arch_diagram )
{
  m_blocked = p_arch_diagram.m_blocked;
  m_visibles = p_arch_diagram.m_visibles;
  m_channel_communications = p_arch_diagram.m_channel_communications;
  m_channels = p_arch_diagram.m_channels;
  m_architecture_references = p_arch_diagram.m_architecture_references;
  m_process_references = p_arch_diagram.m_process_references;
}

architecture_diagram::~architecture_diagram(void)
{
  m_blocked.Clear();
  m_visibles.Clear();
  m_channel_communications.Clear();
  m_channels.Clear();
  m_architecture_references.Clear();
  m_process_references.Clear();
}

architecture_reference* architecture_diagram::add_architecture_reference( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height )
{
  // deselect all objects
  deselect_all_objects();

  // Create new architecture reference
  architecture_reference* new_arch_ref = new architecture_reference;
  new_arch_ref->set_id( p_id );
  new_arch_ref->set_coordinate( p_coord );
  new_arch_ref->set_width( p_def_width );
  new_arch_ref->set_height( p_def_height );
  new_arch_ref->set_diagram( this );
  select_object( new_arch_ref );

  // Establish relationships
  m_architecture_references.Add( new_arch_ref );
  return new_arch_ref;
}

void architecture_diagram::remove_architecture_reference( architecture_reference* p_arch_ref )
{
  deselect_object( p_arch_ref );

  // For each channel on the reference
  for ( unsigned int i = 0; i < p_arch_ref->count_channel(); ++i )
  {
    channel* channel_ptr = p_arch_ref->get_channel( i );

    // Remove all properties of the channel
    connection_property* property_ptr = channel_ptr->get_property();
    delete_property( property_ptr );

    // Disconnect the channel from channel communications.
    channel_communication* c_comm_ptr = channel_ptr->get_channel_communication();
    if ( c_comm_ptr != 0 )
    {
      channel_ptr->detach_channel_communication();
      c_comm_ptr->detach_channel( channel_ptr );

      // If the channel communication consists of less than two channels, remove it.
      int count = c_comm_ptr->count_channel();
      if ( count < 2 )
      {
        int n = m_channel_communications.Index( *c_comm_ptr );
        if ( n != wxNOT_FOUND )
        {
          m_channel_communications.Detach( n );
        }
        delete c_comm_ptr;
      } // end if
    } // end if

    // Remove the channel
    int m = m_channels.Index( *channel_ptr );
    if ( m != wxNOT_FOUND )
    {
      m_channels.Detach( m );
    }
    delete channel_ptr;
  } // end for

  // Remove the reference
  int n = m_architecture_references.Index( *p_arch_ref );
  if ( n != wxNOT_FOUND )
  {
    architecture_reference* del_arch_ref = m_architecture_references.Detach( n );
    delete del_arch_ref;
  }
}

unsigned int architecture_diagram::count_architecture_reference( void )
{
  return m_architecture_references.GetCount();
}

architecture_reference* architecture_diagram::get_architecture_reference( int p_i )
{
  return &( m_architecture_references.Item( p_i ) );
}

arr_architecture_reference* architecture_diagram::get_architecture_reference_list( void )
{
  return &m_architecture_references;
}

process_reference* architecture_diagram::add_process_reference( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height )
{
  // deselect all objects
  deselect_all_objects();

  // Create new process reference
  process_reference* new_proc_ref = new process_reference;
  new_proc_ref->set_id( p_id );
  new_proc_ref->set_coordinate( p_coord );
  new_proc_ref->set_width( p_def_width );
  new_proc_ref->set_height( p_def_height );
  new_proc_ref->set_diagram( this );
  select_object( new_proc_ref );

  // Establish relationships
  m_process_references.Add( new_proc_ref );
  return new_proc_ref;
}

void architecture_diagram::remove_process_reference( process_reference* p_proc_ref )
{
  deselect_object( p_proc_ref );

  // For each channel on the reference
  for ( unsigned int i = 0; i < p_proc_ref->count_channel(); ++i )
  {
    channel* channel_ptr = p_proc_ref->get_channel( i );

    // Remove all properties of the channel
    connection_property* property_ptr = channel_ptr->get_property();
    delete_property( property_ptr );

    // Disconnect the channel from channel communications.
    channel_communication* c_comm_ptr = channel_ptr->get_channel_communication();

    if ( c_comm_ptr != 0 )
    {
      channel_ptr->detach_channel_communication();
      c_comm_ptr->detach_channel( channel_ptr );

      // If the channel communication consists of less than two channels, remove it.
      if ( c_comm_ptr->count_channel() < 2 )
      {
        int n = m_channel_communications.Index( *c_comm_ptr );
        if ( n != wxNOT_FOUND )
        {
          m_channel_communications.Detach( n );
        }
        delete c_comm_ptr;
      } // end if
    } // end if

    // Remove the channel
    int k = m_channels.Index( *channel_ptr );
    if ( k != wxNOT_FOUND )
    {
      m_channels.Detach( k );
    }
    delete channel_ptr;
  } // end for

  // Remove the reference
  int n = m_process_references.Index( *p_proc_ref );
  if ( n != wxNOT_FOUND )
  {
    process_reference* del_proc_ref = m_process_references.Detach( n );
    delete del_proc_ref;
  }
}

unsigned int architecture_diagram::count_process_reference( void )
{
  return m_process_references.GetCount();
}

process_reference* architecture_diagram::get_process_reference( int p_i )
{
  return &( m_process_references.Item( p_i ) );
}

arr_process_reference* architecture_diagram::get_process_reference_list( void )
{
  return &m_process_references;
}

channel* architecture_diagram::add_channel( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height, compound_reference* p_ref  )
{
  // deselect all objects
  deselect_all_objects();

  // Create new channel
  channel* new_channel = new channel;

  new_channel->set_id( p_id );
  new_channel->set_coordinate( p_coord );
  new_channel->set_width( p_def_width );
  new_channel->set_height( p_def_height );
  new_channel->set_diagram( this );
  wxString name;
  name.Printf( _T("Channel%d" ), p_id );
  new_channel->set_name( name );
  select_object( new_channel );

  // Establish relationships
  p_ref->attach_channel( new_channel );
  new_channel->attach_reference( p_ref );
  m_channels.Add( new_channel );
  return new_channel;
}

void architecture_diagram::remove_channel( channel* p_channel )
{
  deselect_object( p_channel );

    // Remove all properties of the channel
    connection_property* property_ptr = p_channel->get_property();
    delete_property( property_ptr );

    // Disconnect the channel from channel communications.
    channel_communication* c_comm_ptr = p_channel->get_channel_communication();

    if ( c_comm_ptr != 0 )
    {
      p_channel->detach_channel_communication();
      c_comm_ptr->detach_channel( p_channel );

      // If the channel communication consists of less than two channels, remove it.
      int count = c_comm_ptr->count_channel();
      if ( count < 2 )
      {
        int n = m_channel_communications.Index( *c_comm_ptr );
        m_channel_communications.Detach( n );
        delete c_comm_ptr;
      } // end if
    } // end if

    // Remove the channel (relationships are properly removed in the destructor of channel)
    int n = m_channels.Index( *p_channel );
    if ( n != wxNOT_FOUND )
    {
      channel* del_channel = m_channels.Detach( n );
      delete del_channel;
    }
}

unsigned int architecture_diagram::count_channel( void )
{
  return m_channels.GetCount();
}

channel* architecture_diagram::get_channel( int p_i )
{
  return &( m_channels.Item( p_i ) );
}

arr_channel* architecture_diagram::get_channel_list( void )
{
  return &m_channels;
}

channel_communication* architecture_diagram::add_channel_communication( unsigned int p_id, coordinate &p_coord, channel* p_channel_1, channel* p_channel_2 )
{
  // deselect all objects
  deselect_all_objects();

  // Create new channel communication
  channel_communication* new_comm = new channel_communication( p_channel_1, p_channel_2 );
  p_channel_1->attach_channel_communication( new_comm );
  p_channel_2->attach_channel_communication( new_comm );
  new_comm->set_id( p_id );
  new_comm->set_coordinate( p_coord );
  new_comm->set_diagram( this );
  select_object( new_comm );

  // Establish relationships
  m_channel_communications.Add( new_comm );
  return new_comm;
}

void architecture_diagram::remove_channel_communication( channel_communication* p_c_comm )
{
  deselect_object( p_c_comm );

  // Remove the property of the channel communication, if it exists.
  connection_property* property_ptr = p_c_comm->get_property();

  if ( property_ptr )
  {
    // Try blocked
    blocked* del_blocked = dynamic_cast<blocked*> ( property_ptr );
    if ( del_blocked ) // Cast succesful
    {
      int m = m_blocked.Index( *del_blocked );
      if ( m != wxNOT_FOUND )
      {
        m_blocked.Detach( m );
        delete del_blocked;
      } // end if
    }
    else // Cast failed
    {
      // Try visible
      visible* del_visible = dynamic_cast<visible*> ( property_ptr );
      if ( del_visible != 0 ) // Cast succesful
      {
        int m = m_visibles.Index( *del_visible );
        if ( m != wxNOT_FOUND )
        {
          m_visibles.Detach( m );
          delete del_visible;
        } // end if
      } // end if del_visible
    } // end if del_blocked
  } // end if property_ptr


  // Remove the channel communication
  int n = m_channel_communications.Index( *p_c_comm );
  if ( n != wxNOT_FOUND )
  {
    channel_communication* del_c_comm = m_channel_communications.Detach( n );
    delete del_c_comm;
  }
}

unsigned int architecture_diagram::count_channel_communication( void )
{
  return m_channel_communications.GetCount();
}

channel_communication* architecture_diagram::get_channel_communication( int p_i )
{
  return &( m_channel_communications.Item( p_i ) );
}

arr_channel_communication* architecture_diagram::get_channel_communication_list( void )
{
  return &m_channel_communications;
}


void architecture_diagram::attach_channel_communication_to_channel( channel_communication* p_c_comm, channel* p_chan)
{
  // if the channel already has a property, detach it first
  connection_property* property_ptr = p_chan->get_property();
  if ( property_ptr != 0 )
  {
    p_chan->detach_property();
    property_ptr->detach();
  }
  // if the channel is already in a channel communication, detach it first
  channel_communication* comm_ptr = p_chan->get_channel_communication();
  if ( comm_ptr != 0 )
  {
    comm_ptr->detach_channel( p_chan );
    p_chan->detach_channel_communication();
  }

  // Establish relationships
  p_c_comm->attach_channel( p_chan );
  p_chan->attach_channel_communication( p_c_comm );
}

void architecture_diagram::detach_channel_from_channel_communication( channel* p_chan )
{
  // Remove relationships
  channel_communication* p_c_comm = p_chan->get_channel_communication();
  if ( p_c_comm != 0 )
  {
    p_c_comm->detach_channel( p_chan );
    p_chan->detach_channel_communication();
  }
}

blocked* architecture_diagram::add_blocked( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height, connection* p_conn )
{
  // deselect all objects
  deselect_all_objects();

  // Create new blocked
  blocked* new_blocked = new blocked;
  new_blocked->set_id( p_id );
  new_blocked->set_coordinate( p_coord );
  new_blocked->set_width( p_def_width );
  new_blocked->set_height( p_def_height );
  new_blocked->set_diagram( this );
  select_object( new_blocked );

  // Establish relationships
  new_blocked->attach( p_conn );
  if ( p_conn != 0 )
  {
    p_conn->attach_property( new_blocked );
  }
  m_blocked.Add( new_blocked );
  return new_blocked;
}

void architecture_diagram::remove_blocked( blocked* p_blocked )
{
  deselect_object( p_blocked );

  int n = m_blocked.Index( *p_blocked );
  if ( n != wxNOT_FOUND )
  {
    blocked*  del_blocked = m_blocked.Detach( n );
    delete del_blocked;
  }
}

unsigned int architecture_diagram::count_blocked( void )
{
  return m_blocked.GetCount();
}

blocked* architecture_diagram::get_blocked( int p_i )
{
  return &( m_blocked.Item( p_i ) );
}

arr_blocked* architecture_diagram::get_blocked_list( void )
{
  return &m_blocked;
}

visible* architecture_diagram::add_visible( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height, connection* p_conn )
{
  // deselect all objects
  deselect_all_objects();

  // Create new visible
  visible* new_visible = new visible;
  new_visible->set_id( p_id );
  new_visible->set_coordinate( p_coord );
  new_visible->set_width( p_def_width );
  new_visible->set_height( p_def_height );
  new_visible->set_diagram( this );
  wxString name;
  name.Printf( _T("Visible%d" ), p_id );
  new_visible->set_name( name );
  select_object( new_visible );

  // Establish relationships
  new_visible->attach( p_conn );
  if ( p_conn != 0 )
  {
    p_conn->attach_property( new_visible );
  }
  m_visibles.Add( new_visible );
  return new_visible;
}

void architecture_diagram::remove_visible( visible* p_visible )
{
  deselect_object( p_visible );

  int n = m_visibles.Index( *p_visible );
  if ( n != wxNOT_FOUND )
  {
    visible* del_visible = m_visibles.Detach( n );
    delete del_visible;
  }
}

unsigned int architecture_diagram::count_visible( void )
{
  return m_visibles.GetCount();
}

visible* architecture_diagram::get_visible( int p_i )
{
  return &( m_visibles.Item( p_i ) );
}

arr_visible* architecture_diagram::get_visible_list( void )
{
  return &m_visibles;
}

void architecture_diagram::attach_property_to_connection( connection_property* p_prop, connection* p_connection )
{
  // if the property is already connected, detach it first
  connection* connection_ptr = p_prop->get_attached_connection();
  if ( connection_ptr != 0 )
  {
    p_prop->detach();
    connection_ptr->detach_property();
  }
  // if the connection already has a property, detach it first
  connection_property* property_ptr = p_connection->get_property();
  if ( property_ptr != 0 )
  {
    p_connection->detach_property();
    property_ptr->detach();
  }
  // if the connection is a channel and is in a channel communication, detach it first
  // Try channel
  channel* channel_ptr = dynamic_cast<channel*> ( p_connection );
  if ( channel_ptr != 0 ) // Cast succesful
  {
    channel_communication* comm_ptr = channel_ptr->get_channel_communication();
    if ( comm_ptr != 0 )
    {
      comm_ptr->detach_channel( channel_ptr );
      channel_ptr->detach_channel_communication();
    }
  }

  p_prop->attach( p_connection );
  p_connection->attach_property( p_prop );
}

void architecture_diagram::detach_property_from_connection( connection_property* p_prop )
{
  connection* p_connection = p_prop->get_attached_connection();
  if ( p_connection != 0 )
  {
    p_connection->detach_property();
    p_prop->detach();
  }
}

void architecture_diagram::select_all_objects( void )
{
  deselect_all_objects();
  int count = m_comments.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    comment* comment_ptr = &( m_comments.Item( i ) );
    plus_select_object( comment_ptr );
  }
  count = m_blocked.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    blocked* block_ptr = &( m_blocked.Item( i ) );
    plus_select_object( block_ptr );
  }
  count = m_visibles.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    visible* vis_ptr = &( m_visibles.Item( i ) );
    plus_select_object( vis_ptr );
  }
  count = m_channel_communications.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    channel_communication* comm_ptr = &( m_channel_communications.Item( i ) );
    plus_select_object( comm_ptr );
  }
  count = m_channels.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    channel* chan_ptr = &( m_channels.Item( i ) );
    plus_select_object( chan_ptr );
  }
  count = m_architecture_references.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    architecture_reference* arch_ref_ptr = &( m_architecture_references.Item( i ) );
    plus_select_object( arch_ref_ptr );
  }
  count = m_process_references.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    process_reference* proc_ref_ptr = &( m_process_references.Item( i ) );
    plus_select_object( proc_ref_ptr );
  }
}

void architecture_diagram::deselect_all_objects( void )
{
  int count = m_comments.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    comment* comment_ptr = &( m_comments.Item( i ) );
    deselect_object( comment_ptr );
  }
  count = m_blocked.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    blocked* block_ptr = &( m_blocked.Item( i ) );
    deselect_object( block_ptr );
  }
  count = m_visibles.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    visible* vis_ptr = &( m_visibles.Item( i ) );
    deselect_object( vis_ptr );
  }
  count = m_channel_communications.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    channel_communication* comm_ptr = &( m_channel_communications.Item( i ) );
    deselect_object( comm_ptr );
  }
  count = m_channels.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    channel* chan_ptr = &( m_channels.Item( i ) );
    deselect_object( chan_ptr );
  }
  count = m_architecture_references.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    architecture_reference* arch_ref_ptr = &( m_architecture_references.Item( i ) );
    deselect_object( arch_ref_ptr );
  }
  count = m_process_references.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    process_reference* proc_ref_ptr = &( m_process_references.Item( i ) );
    deselect_object( proc_ref_ptr );
  }
}

// public
void architecture_diagram::delete_property( connection_property* p_prop )
{
  if ( p_prop != 0 )
  {
    // Try blocked
    blocked* del_blocked = dynamic_cast<blocked*> ( p_prop );
    if ( del_blocked != 0 ) // Cast succesful
    {
      int m = m_blocked.Index( *del_blocked );
      if ( m != wxNOT_FOUND )
      {
        m_blocked.Detach( m );
        delete del_blocked;
      } // end if
    }
    else // Cast failed
    {
      // Try visible
      visible* del_visible = dynamic_cast<visible*> ( p_prop );
      if ( del_visible != 0 ) // Cast succesful
      {
        int m = m_visibles.Index( *del_visible );
        if ( m != wxNOT_FOUND )
        {
          m_visibles.Detach( m );
          delete del_visible;
        } // end if
      } // end if del_visible
    } // end if del_blocked
  } // end if p_prop
}

object* architecture_diagram::find_object( architecture_diagram* p_arch_dia, unsigned int p_id, object_type p_type )
{
  bool b = p_type == ANY || p_type == COMMENT;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_comment(); ++i )
  {
    comment* comm_ptr = p_arch_dia->get_comment( i );
    if ( comm_ptr->get_id() == p_id )
    {
      return comm_ptr;
    }
  }

  b = p_type == ANY || p_type == ARCHITECTURE_REFERENCE;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_architecture_reference(); ++i )
  {
    architecture_reference* ref_ptr = p_arch_dia->get_architecture_reference( i );
    if ( ref_ptr->get_id() == p_id )
    {
      return ref_ptr;
    }
  }

  b = p_type == ANY || p_type == PROCESS_REFERENCE;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_process_reference(); ++i )
  {
    process_reference* ref_ptr = p_arch_dia->get_process_reference( i );
    if ( ref_ptr->get_id() == p_id )
    {
      return ref_ptr;
    }
  }

  b = p_type == ANY || p_type == CHANNEL;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_channel(); ++i )
  {
    channel* chan_ptr = p_arch_dia->get_channel( i );
    if ( chan_ptr->get_id() == p_id )
    {
      return chan_ptr;
    }
  }

  b = p_type == ANY || p_type == CHANNEL_COMMUNICATION;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_channel_communication(); ++i )
  {
    channel_communication* comm_ptr = p_arch_dia->get_channel_communication( i );
    if ( comm_ptr->get_id() == p_id )
    {
      return comm_ptr;
    }
  }

  b = p_type == ANY || p_type == VISIBLE;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_visible(); ++i )
  {
    visible* vis_ptr = p_arch_dia->get_visible( i );
    if ( vis_ptr->get_id() == p_id )
    {
      return vis_ptr;
    }
  }

  b = p_type == ANY || p_type == BLOCKED;
  for ( unsigned int i = 0; b && i < p_arch_dia->count_blocked(); ++i )
  {
    blocked* block_ptr = p_arch_dia->get_blocked( i );
    if ( block_ptr->get_id() == p_id )
    {
      return block_ptr;
    }
  }
  return 0;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_architecture_diagram );
