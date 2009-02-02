// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file connection.h
//
// Declares the Connection class.

#ifndef LIBGRAPE_CONNECTION_H
#define LIBGRAPE_CONNECTION_H

#include "connectionproperty.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a connection.
     * A generic entity that serves as a basis for synchronization entities,
     * and is extended by Channel and Channel Communication.
     * @see channel
     * @see channel_communication
     */
    class connection : public object
    {
      private:
        /**
          * Default constructor.
          * Initializes Connection.
          */
        connection( void );

      protected:
        connection_property  *m_has_property;  /**< property associated with this connection. */

      public:
        /**
         * Constructor.
         * Initializes Connection.
         * @param p_type The type of this reference. Should be either @c CHANNEL or @c CHANNEL_COMMUNICATION .
         */
        connection( object_type p_type );

        /**
         * Copy constructor.
         * Creates a new connection based on en existing one.
         * @param p_connection The connection to copy.
         */
        connection( const connection &p_connection );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        virtual ~connection( void );

        /**
         * Connection property association function.
         * Associates the connection with the specified connection property
         */
        void attach_property( connection_property* p_prop );

        /**
         * Connection property dissociation function.
         * Dissociates the connection from the connection property
         */
        void detach_property( void );

        /**
         * Connection property retrieval function.
         * @return Returns a pointer to the conneciton property associated with the connection.
         */
        connection_property* get_property( void );

    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_CONNECTION_H
