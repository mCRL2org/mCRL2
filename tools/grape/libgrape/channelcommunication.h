// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channelcommunication.h
//
// Declares the channel_communication class.

#ifndef LIBGRAPE_CHANNELCOMMUNICATION_H
#define LIBGRAPE_CHANNELCOMMUNICATION_H

#include "communication.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Channel communication type enumeration.
     * An enumeration that includes all possible channel communication types ({VISIBLE, HIDDEN, BLOCKED}).
     */
    enum channel_communication_type
    {
      VISIBLE_CHANNEL_COMMUNICATION = 0, 
      HIDDEN_CHANNEL_COMMUNICATION, 
      BLOCKED_CHANNEL_COMMUNICATION
    };

    /**
     * \short Represents a channel communication.
     * Two or more connected Channels that synchronize the Actions
     * that the Channels correspond to.
     * @see channel
     */
    class channel_communication : public object
    {
      protected:
        /** The communications involved in this channel communication. Each communication is associated with exactly one channel. */
        arr_communication           m_communication;
        channel_communication_type  m_channel_communication_type; /**< type of channel communication. */
        wxString                    m_rename_to;                  /**< rename of the channel communication. */

      public:

        /**
         * To be used exclusively during XML-conversion.
         * Empty constructor
         */
        channel_communication( void );

        /**
         * Default constructor.
         * Initializes channel_communication.
         * A channel always has exactly two channels between which it exits.
         */
        channel_communication( channel* p_channel_1, channel* p_channel_2 );

        /**
         * Copy constructor.
         * Creates a new ChannelCommunication based on an existing one.
         * @param p_channel_comm The channel communication to be copied.
         */
        channel_communication( const channel_communication &p_channel_comm );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~channel_communication( void );

        /**
         * Rename retrieval function.
         * @return The rename to of the channel communication.
         */
        wxString get_rename_to(void) const;

       /**
         * Rename assignment function.
         * Assigns the name to the rename of the channel communication.
         * @pre The channel communication is selected.
         * @param p_rename_to The new rename of the channel communication.
         */
        void set_rename_to( const wxString &p_rename_to );
        
        /**
         * Channel addition function.
         * Adds a new channel to the channel communication.
         * @param p_channel The new channel to be added
         */
        void attach_channel( channel* p_channel );

        /**
         * Channel deletion function.
         * Deletes a channel from the channel communication, if possible.
         * @pre m_communication.GetCount > 2
         * @post The channel is removed from the channel communication, if the a communication with the channel exists
         */
        void detach_channel( channel* p_channel );

        /**
         * Channel count function.
         * @return Returns the number of channels involved in this channel communication.
         */
        unsigned int count_channel( void );

        /**
         * Channel retrieval function.
         * @return Returns a pointer to the channel involved in this channel communication, at the specified index in the list.
         */
        channel* get_attached_channel( int p_i );

        /** communications retrieval function
         * @return Returns a pointer to the array with communications
         */
        arr_communication*  get_communications( void );
        
        /**
         * Channel communication type retrieval function
         * @return The channel communication type
         */
        channel_communication_type get_channel_communication_type( void ) const;

        /**
         * Channel communication type assignment function
         * Takes a new channeltype and assigns it to the channeltype of the channel.
         * @param p_channel type The new channeltype
         */
        void set_channel_communication_type( const channel_communication_type &p_channel_communication_type );
        
        /**
         * @param p_channel the channel
         * @return channel communication is attached on p_channel
         */
        bool has_channel( const channel* p_channel);
    };

    /**
     * Array of ChannelCommunication.
     */
    WX_DECLARE_OBJARRAY( channel_communication, arr_channel_communication );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_CHANNELCOMMUNICATION_H
