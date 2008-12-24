// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file connectionproperty.h
//
// Declares the connection_property class.

#ifndef LIBGRAPE_CONNECTIONPROPERTY_H
#define LIBGRAPE_CONNECTIONPROPERTY_H

#include "object.h"
#include "linetype.h"

namespace grape
{
  namespace libgrape
  {

    // forward declarations
    class connection;

    /**
     * \short Represents an abstract property of a connection.
     * A generic entity that serves as a basis for blockedness and visibility,
     * and is extended by Blocked and Visible.
     * @see blocked
     * @see visible
     */
    class connection_property : public object
    {
      private:
        /**
         * Default constructor. Shouldn't be called from GraPE code.
         * Initializes connection_property.
         */
        connection_property( void );
      protected:
        linetype            m_linetype;       /**< type of line associated with property. */
        list_of_coordinate  m_breakpoints;    /**< breakpoints associated with line of property. */

        connection          *m_has_property;  /**< connection associated with this property. */

      public:
        /**
         * Complete constructor.
         * Initializes connection_property with a corresponding connection.
         * @param p_type The type of the object, either VISIBLE or BLOCKED.
         * @param p_connection The connection the property is associated with
         */
        connection_property( object_type p_type, connection *p_connection = 0 );

        /**
         * Copy constructor.
         * Creates a new connection property based on an existing one.
         * @param p_connection_property The connection property to copy.
         */
        connection_property( const connection_property &p_connection_property );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        virtual ~connection_property( void );

        /**
         * Connection attachment function.
         * Attaches the connection property to a connection.
         * @param p_conn The connection the connection property is to be associated with.
         */
        void attach( connection* p_conn );

        /**
         * Connection detachment function.
         * Detaches the connection property from a connection.
         */
        void detach( void );

        /**
         * Connection retrieval function.
         * @return Returns a pointer to the connection the connection property is associated with.
         */
        connection * get_attached_connection( void );

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

        linetype get_linetype( void );

        /**
         * Query function
         * Function to return if a property is unassociated with a connection (and thus movable)
         * @return True if a property is unassociated with a connection (and thus movable)
         */

        bool movable( void );

    };

    /**
     * Array of Connection property.
     */
    WX_DECLARE_OBJARRAY( connection_property, arr_connection_property );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_CONNECTIONPROPERTY_H
