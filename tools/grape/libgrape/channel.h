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

#include "connection.h"
#include "channeltype.h"

namespace grape
{
  namespace libgrape
  {

    // forward declaration
    class compound_reference;
    class communication;
    class channel_communication;

    /**
     * \short Represents a channel.
     * A Channel of a Process Reference or Architecture Reference is a
     * reference to an Action. It can be made visible, hidden or blocked
     * and can be used to communicate with other Channels.
     * @see action
     * @see process_reference
     * @see architecture_reference
     */
    class channel : public connection
    {
      private:
        /** Initialization function. */
        void init();

      protected:
        wxString                    m_name;           /**< name of the channel. */
        bool                        m_enabled_channel; /**< flag used during simulation. */
        channeltype                 m_channeltype;     /**< type of channel associated with this transition. */

        compound_reference          *m_has_channel;   /**< reference channel is connected to. */
        channel_communication       *m_channel_communication; /**< channel communication connected to this channel. */

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
         * Assigns the name to the name of the channel.
         * @pre The channel is selected.
         * @param p_name The new name of the channel.
         */
        void set_name( const wxString &p_name );

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
        void detach_channel_communication( void );

        /**
         * Channel communication retrieval function.
         * @return Returns a pointer to the channel communication the channel is associated with.
         */
        channel_communication* get_channel_communication( void );

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
        channeltype get_channeltype( void ) const;

        /**
         * Channel type assignment function
         * Takes a new channeltype and assigns it to the channeltype of the channel.
         * @param p_channeltype The new channeltype
         */
        void set_channeltype( const channeltype &p_channeltype );

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
