// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file diagram.h
//
// Declares the diagram class.

#ifndef LIBGRAPE_DIAGRAM_H
#define LIBGRAPE_DIAGRAM_H

#include "comment.h"
#include "processreference.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a diagram.
     * A generic entity that serves as a basis for a diagram, and is extended
     * by Process Diagram and Architecture Diagram.
     * @see architecture_diagram
     * @see process_diagram
     */
    class diagram
    {
      protected:
        wxString               m_name;               /**< name of this diagram. */
        unsigned int                   m_id;                 /**< unique identifier of this diagram. */
        arr_comment            m_comments;           /**< comments in this diagram. */
        arr_object_ptr         m_selected_objects;   /**< selected objects in this diagram. */
      public:
        /*
         * Default constructor.
         * Initializes diagram.
         */
        diagram( void );

        /**
         * Copy constructor.
         * Creates a new diagram based on an existing one.
         * @param p_diagram The diagram to copy.
         */
        diagram( const diagram &p_diagram );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        virtual ~diagram( void );

        /**
         * Id retrieval function
         * @return The ID of the diagram.
         */
        unsigned int get_id( void ) const;

        /**
         * Id assignment function.
         * Assigns the id to the id of the diagram.
         * @param p_id The new id.
         */
        void set_id( unsigned int p_id );

        /**
         * Name retrieval function.
         * @return The name of the diagram.
         */
        wxString get_name( void ) const;

        /**
         * Name assignment function.
         * Assigns the name to the name of the diagram.
         * @param p_name The new name.
         */
        void set_name( const wxString &p_name );

        /**
         * Comment addition function.
         * Creates a new comment.
         * @param p_id The id of the comment
         * @param p_coord The coordinate at which the comment should be created.
         * @param p_def_width The default width of a comment.
         * @param p_def_height The default height of a comment
         * @return Returns a pointer to the newly created comment.
         */
        comment* add_comment( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height );

        /**
         * Comment removal function.
         * Removes the comment and all references to it.
         * @param p_comment The comment.
         * @pre: @p p_comment is stored in this diagram.
         */
        void remove_comment( comment* p_comment );

        /**
         * Comment count function.
         * @return Returns the number of comments in the diagram.
         */
        unsigned int count_comment( void );

        /**
         * Comment retrieval function.
         * @param p_i The index of the to be retrieved comment.
         * @return Returns a pointer to the comment at index p_i in the list of comments.
         */
        comment* get_comment( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of comments
         */
        arr_comment* get_comment_list( void );

        /**
         * Comment attachment function.
         * Attaches the specified comment to the specified object.
         * Detaches the comment from an existing attached object first, if necessary.
         * @pre The comment and object are both in this diagram.
         * @param p_comm The comment.
         * @param p_object The object.
         */
        void attach_comment_to_object( comment* p_comm, object* p_object );

        /**
         * Comment detachment function.
         * Detaches the specified comment from the object it is attached to.
         * @param p_comm The comment.
         */
        void detach_comment_from_object( comment* p_comm );

        /**
         * Object selection function.
         * Deselects all selected objects and then selects the specified object.
         * @pre The specified object exists in this diagram.
         * @param p_object A pointer to the to be selected object
         */
        void select_object( object* p_object );

        /**
         * Object plusselection function.
         * Selects the specified object.
         * @pre The specified object exists in this diagram.
         * @param p_object A pointer to the to be selected object
         */
        void plus_select_object( object* p_object );

        /**
         * Object deselection function.
         * Deselects the specified object.
         * @pre The specified object exists in this diagram.
         * @param p_object A pointer to the to be deselected object.
         */
        void deselect_object( object* p_object );

        /**
         * Object deselection function.
         * Inherited methods deselect all objects in the diagram.
         */
        virtual void deselect_all_objects() = 0;

        /**
         * Object selection function.
         * Inherited methods select all objects in the diagram.
         */
        virtual void select_all_objects() = 0;

        /**
         * Selected object count function.
         * @return Returns the number of selected objects in the diagram.
         */
        unsigned int count_selected_objects( void );

        /**
         * Selected object retrieval function.
         * @return Returns a pointer to the object in the list of selected objects.
         */
        object* get_selected_object( int p_index );
    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_DIAGRAM_H
