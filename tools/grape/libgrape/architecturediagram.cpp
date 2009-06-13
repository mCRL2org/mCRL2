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
  m_channel_communications.Empty();
  m_channels.Empty();
  m_architecture_references.Empty();
  m_process_references.Empty();
}

architecture_diagram::architecture_diagram( const architecture_diagram &p_arch_diagram )
: diagram( p_arch_diagram )
{
  m_channel_communications = p_arch_diagram.m_channel_communications;
  m_channels = p_arch_diagram.m_channels;
  m_architecture_references = p_arch_diagram.m_architecture_references;
  m_process_references = p_arch_diagram.m_process_references;
}

architecture_diagram::~architecture_diagram(void)
{
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

    // Disconnect the channel from channel communications.
    arr_channel_communication_ptr* chan_comm_ptr = channel_ptr->get_channel_communications();
    for ( unsigned int j = 0; j < chan_comm_ptr->GetCount(); ++j )
    {
      channel_communication* c_comm_ptr = chan_comm_ptr->Item( j );
      if ( c_comm_ptr != 0 )
      {
        channel_ptr->detach_channel_communication( c_comm_ptr );
        c_comm_ptr->detach_channel( channel_ptr );

        // If the channel communication consists of less than two channels, remove it.
        int count = c_comm_ptr->count_channel();
        if ( count < 2 )
        {
          int n = m_channel_communications.Index( *c_comm_ptr );
          if ( n != wxNOT_FOUND )
          {
            m_channel_communications.Detach( n );
            delete c_comm_ptr;
          }
        } // end if
      } // end if
    } // end for

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

    // Disconnect the channel from channel communications.
    arr_channel_communication_ptr* chan_comm_ptr = channel_ptr->get_channel_communications();
    for ( unsigned int j = 0; j < chan_comm_ptr->GetCount(); ++j )
    {
      channel_communication* c_comm_ptr = chan_comm_ptr->Item( j );

      if ( c_comm_ptr != 0 )
      {
        channel_ptr->detach_channel_communication( c_comm_ptr);
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
    } // end for

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

  // Disconnect the channel from channel communications.
  arr_channel_communication_ptr* chan_comm_ptr = p_channel->get_channel_communications();
  for ( unsigned int j = 0; j < chan_comm_ptr->GetCount(); ++j )
  {
    channel_communication* c_comm_ptr = chan_comm_ptr->Item( j );

    if ( c_comm_ptr != 0 )
    {
      p_channel->detach_channel_communication( c_comm_ptr );
      c_comm_ptr->detach_channel( p_channel );

      // If the channel communication consists of less than two channels, remove it.
      int count = c_comm_ptr->count_channel();
      if ( count < 2 )
      {
        int n = m_channel_communications.Index( *c_comm_ptr );
        if (n != wxNOT_FOUND)
        {
          m_channel_communications.Detach( n );
          delete c_comm_ptr;
        }
      } // end if
    } // end if
  }

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
  // Establish relationships
  p_c_comm->attach_channel( p_chan );
  p_chan->attach_channel_communication( p_c_comm );
}

void architecture_diagram::detach_channel_from_channel_communication( channel* p_chan, channel_communication* p_c_comm )
{
  // Remove relationships
  p_c_comm->detach_channel( p_chan );
  p_chan->detach_channel_communication( p_c_comm );
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

  return 0;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_architecture_diagram )
