// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file communication.h
//
// Declares the communication association class.

#ifndef LIBGRAPE_COMMUNICATION_H
#define LIBGRAPE_COMMUNICATION_H

#include "linetype.h"
#include "channel.h"

namespace grape
{
  namespace libgrape
  {

    // forward declarations
    class channel_communication;
    class channel;

    /**
     * \short Represents a communication.
     * Two or more connected Channels that synchronize the Actions
     * that the Channels correspond to.
     * @see channel
     */
    class communication
    {
      private:
        linetype               m_linetype;               /**< type of this line. */
        list_of_coordinate     m_breakpoints;            /**< breakpoints of this line. */
        channel                *m_channel;                /**< channel belonging to this communication. */


      public:
        // Note: A communication is _always_ associated with a channel, it cannot be disconnected. Therefore, a constructor without parameters does not exist, there always has to be a channel it belongs to.
        /**
         * Default constructor.
         * Initializes communication.
         */
        communication( channel* p_channel );

        /**
         * Copy constructor.
         * Creates a new communication based on an existing one.
         * @param p_communication The communication to be copied.
         */
        communication( const communication &p_communication );

        /**
         * Default destructor.
         * Frees allocated memory .
         */
        ~communication( void );

        /**
         * Channel retrieval function.
         * @return Returns a reference to the channel the communication is associated with.
         */
        channel* get_channel( void );

        /**
         * Add function
         * Add a breakpoint before the element at place p_place
         * @param p_coordinate The coordinate that has to be added
         * @param p_place The place in the array where it has to be added
         */

        void add_breakpoint ( coordinate &p_coordinate, int p_place );

        /**
         * Update function
         * Update the position of a breakpoint at place p_place
         * @param p_coordinate The coordinate that the breakpoint has moved to
         * @param p_place The place in the array (starting at 0) where the breakpoint is located
         */

        void move_breakpoint ( coordinate &p_coordinate, int p_place );

        /**
         * Remove function
         * Remove a breakpoint
         * @param p_place The place in the array (starting at 0) where the breakpoint is located
         */

        void remove_breakpoint ( int p_place );

        /**
         * Update function
         * Function to set a new linetype to the communication
         * @param p_linetype The new linetype
         */

        void set_linetype( linetype p_linetype );

        /**
         * Query function
         * Function to return the linetype of a communication
         * @return The linetype of the communication
         */

        linetype get_linetype( void ) const;

    };

    /**
     * Array of communication pointers.
     */
    WX_DEFINE_ARRAY_PTR( communication *, arr_communication_ptr );



    /**
     * Array of communication.
     */
    WX_DECLARE_OBJARRAY( communication, arr_communication );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_COMMUNICATION_H
