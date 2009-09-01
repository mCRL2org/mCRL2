// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channel.h
//
// Declares the channel class.

#ifndef LIBGRAPE_CHANNEL_H
#define LIBGRAPE_CHANNEL_H

#include "object.h"

namespace grape
{
  namespace libgrape
  {

    // forward declaration
    class compound_reference;
    class channel_communication;

    /**
     * Array of ChannelCommunication.
     */
    WX_DEFINE_ARRAY_PTR( channel_communication*, arr_channel_communication_ptr );

    /**
     * \short Channel type enumeration.
     * An enumeration that includes all possible channel types ({VISIBLE, HIDDEN, BLOCKED}).
     */
    enum channel_type
    {
      VISIBLE_CHANNEL = 0, 
      HIDDEN_CHANNEL, 
      BLOCKED_CHANNEL
    };

    /**
     * \short Represents a channel.
     * A Channel of a Process Reference or Architecture Reference is a
     * reference to an Action. It can be made visible, hidden or blocked
     * and can be used to communicate with other Channels.
     * @see action
     * @see process_reference
     * @see architecture_reference
     */
    class channel : public object
    {
      private:
        /** Initialization function. */
        void init();

      protected:
        wxString                      m_name;                   /**< name of the channel. */
        wxString                      m_rename_to;              /**< rename of the channel. */
        
        bool                          m_enabled_channel;        /**< flag used during simulation. */
        channel_type                  m_channel_type;           /**< type of channel associated with this transition. */

        compound_reference            *m_has_channel;           /**< reference channel is connected to. */
        arr_channel_communication_ptr m_channel_communication; /**< channel communications connected to this channel. */

      public:

        /**
         * Default constructor.
         * Initializes channel which is not on a compound reference.
         */
        channel( void );

        /**
         * Complete constructor.
         * Initializes channel.
         * @param p_has_channel The reference it is created on.
         */
        channel( compound_reference &p_has_channel );

        /**
         * Copy constructor.
         * Creates a new channel based on an existing one.
         * @param p_channel The channel to be copied.
         */
        channel( const channel &p_channel );

         /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~channel( void );

        /**
         * Checks if a channel is equal.
         * @param p_channel The channel to compare with.
         * @return @c true if the channel is equal, otherwise @c false
         */
        bool operator==( const channel &p_channel );

        /**
         * Name retrieval function.
         * @return The name of the channel.
         */
        wxString get_name(void) const;

       /**
         * Name assignment function.
         * Assigns the name the rename of the channel.
         * @pre The channel is selected.
         * @param p_name The new rename of the channel.
         */
        void set_name( const wxString &p_name );
        
        /**
         * Rename retrieval function.
         * @return The rename to of the channel.
         */
        wxString get_rename_to(void) const;

       /**
         * Rename assignment function.
         * Assigns the name to the rename of the channel.
         * @pre The channel is selected.
         * @param p_rename_to The new rename of the channel.
         */
        void set_rename_to( const wxString &p_rename_to );

       /**
         * Compound reference association function.
         * Associates the channel with the specified compound reference.
         * @param p_reference The new compound_reference the channel is to be associated with.
         */
        void attach_reference( compound_reference* p_reference );

        /**
         * Compound reference dissociation function.
         * Dissociates the channel from its compound reference.
         */
        void detach_reference( void );

        /**
         * Compound reference retrieval function.
         * @return Returns a pointer to the compound reference the channel is associated with.
         */
        compound_reference* get_reference( void );

        /**
         * Channel communication association function.
         * Associates the channel with the specified channel communication.
         */
        void attach_channel_communication( channel_communication* p_comm );

        /**
         * Channel communication dissociation function.
         * Dissociates the channel from its channel communcation.
         */
        void detach_channel_communication( channel_communication* p_comm );

        /**
         * Channel communication retrieval function.
         * @return Returns a pointer to the channel communication the channel is associated with.
         */
        arr_channel_communication_ptr* get_channel_communications( void );

        /**
         * Enabled retrieval function.
         * @return @c true if the channel is enabled and @c false otherwise.
         */
        bool get_enabled( void ) const;

        /**
         * Enabled set function.
         * @param p_enabled true if the channel has to be enabled and false otherwise.
         */
        void set_enabled( bool p_enabled );

        /**
         * Channel type retrieval function
         * @return The channel type
         */
        channel_type get_channel_type( void ) const;

        /**
         * Channel type assignment function
         * Takes a new channeltype and assigns it to the channeltype of the channel.
         * @param p_channeltype The new channeltype
         */
        void set_channel_type( const channel_type &p_channel_type );
        
    };

    /**
     * Array of Channel pointers.
     */
    WX_DEFINE_ARRAY_PTR( channel *, arr_channel_ptr );

    /**
     * Array of Channel.
     */
    WX_DECLARE_OBJARRAY( channel, arr_channel );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_CHANNEL_H
