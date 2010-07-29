// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file object.h
//
// Declares the abstract object class.

#ifndef LIBGRAPE_OBJECT_H
#define LIBGRAPE_OBJECT_H

#include <wx/string.h>

#include "coordinate.h"

namespace grape
{
  namespace libgrape
  {

    // forward declaration
    class comment;
    class diagram;

    WX_DEFINE_ARRAY_PTR(comment *, arr_comment_ptr);


    /**
      * Describes several types the object can be of
      */
    enum object_type
    {
      NONE = 0,
      COMMENT,
      INITIAL_DESIGNATOR,
      STATE,
      REFERENCE_STATE,
      NONTERMINATING_TRANSITION,
      TERMINATING_TRANSITION,
      CHANNEL,
      CHANNEL_COMMUNICATION,
      PROCESS_REFERENCE,
      ARCHITECTURE_REFERENCE,
      ANY
    };


    /**
     * \short Represents an abstract diagram object.
     * A generic entity that serves as a basis for objects, and is extended
     * by Comment, ConnectionProperty, Connection, CompoundReference, InitialDesignator,
     * CompoundState and Transition.
     */
    class object
    {
      private:
        /**
         * Default constructor. Shouldn't be called from GraPE code.
         * Initializes object.
         */
        object( void );

        /** Initializes the object. */
        void init();

        object_type     m_type;         /**< The type of the object. */

      protected:
        coordinate      m_coordinate;   /**< coordinate of object. */
        unsigned int            m_id;           /**< identifier of object. */
        bool            m_selected;     /**< selection flag of object. */
        float           m_width;        /**< width of object. */
        float           m_height;       /**< height of object. */
        arr_comment_ptr m_comments;      /**< comments attached to the object */
        diagram         *m_in_diagram;  /**< the diagram the object is in. */

      public:
        /**
         * Constructor.
         * Initializes object.
         * @param p_type The type of this object.
         */
        object( object_type p_type );

        /**
         * Copy constructor.
         * Creates a new object based on an existing one.
         * @param p_object The object to copy.
         */
        object( const object &p_object );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        virtual ~object( void );

        /**
         * Object type retrieval function.
         * @return Returns the type of the object.
         */
        object_type get_type( void );

        /**
         * Coordinate retrieval function.
         * @return The coordinate of the object.
         */
        coordinate get_coordinate( void ) const;

        /**
         * Coordinate assignment function.
         * Assigns the coordinate to the coordinate of the object.
         * @pre The object is selected.
         * @param p_coordinate The new coordinate.
         */
        void set_coordinate( coordinate &p_coordinate );

        /**
         * Id retrieval function
         * @return The ID of the object.
         */
        unsigned int get_id( void ) const;

        /**
         * Id assignment function.
         * Assigns the id to the id of the object.
         * @pre The object is selected.
         * @param p_id The new id.
         */
        void set_id( unsigned int p_id );

        /**
         * Select function.
         * Assigns true to the m_selected property of the object and deselects every other objec
         * @pre true
         * @post m_selected == true and for every other object m_selected == false
         */
        void select( void );

        /**
         * Deselect function.
         * Assigns false to the m_selected property of the object
         * @pre true
         * @post m_selected = false
         */
        void deselect( void );

        /**
         * Selected retrieval function.
         * @return @c true if the object is selected, otherwise @c false
         */
        bool get_selected( void ) const;


        /**
         * Width retrieval function
         * Returns
         * @return The width of the object
         */
        float get_width( void ) const;

        /**
         * Width assignment function.
         * Assigns the width to the width of the object.
         * @pre The object is selected.
         * @param p_width The new width.
         */
        void set_width( float p_width );

        /**
         * Height retrieval function
         * @return The height of the object.
         */
        float get_height( void ) const;

        /**
         * Height assignment function.
         * Assigns the height to the width of the object.
         * @pre The object is selected.
         * @param p_height The new height.
         */
        void set_height( float p_height );

        /**
         * Comment association function.
         * Associates the object with the specified comment.
         */
        void attach_comment( comment* p_comment );

        /**
         * Comment dissociation function.
         * Dissociates the object from the specified comment.
         */
        void detach_comment( comment *p_comment );

        /**
         * Comment count function.
         * @return Returns the number of comments attached to the object.
         */
        unsigned int count_comment( void );

        /**
         * Comment retrieval function.
         * @param p_i The index of the comment to be retrieved
         * @return Returns a pointer to the comment at the specified index in the list of comments.
         */
        comment* get_comment( int p_i );

        /**
         * Diagram retrieval function.
         * @return Returns a pointer to the diagram this object is in.
         */
        diagram* get_diagram( void );

        /**
         * Diagram assignment function.
         * @param p_diag The diagram the object is in.
         */
        void set_diagram( diagram* p_diag );
    };

    /**
     * Array of object pointers.
     */
    WX_DEFINE_ARRAY_PTR( object *, arr_object_ptr );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_OBJECT_H
