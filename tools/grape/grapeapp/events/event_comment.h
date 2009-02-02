// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_comment.h
//
// Declares GraPE events for comments.

#ifndef GRAPE_EVENT_COMMENT_H
#define GRAPE_EVENT_COMMENT_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add comment event.
     */
    class grape_event_add_comment : public grape_event_base
    {
      private:
        unsigned int        m_comm;           /**< The id of the to be created comment. */
        coordinate  m_coord;           /**< the coordinate that was clicked. */
        float       m_def_comm_width;  /**< The default width of a comment. */
        float       m_def_comm_height; /**< The default height of a comment. */
        unsigned int        m_in_diagram;     /**< The id of the diagram the comment is added to. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the comment should be added.
         */
        grape_event_add_comment( grape_frame *p_main_frame, coordinate &p_coord );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_comment( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the remove comment event.
     */
    class grape_event_remove_comment : public grape_event_base
    {
      private:
        unsigned int                  m_comm;          /**< The identifier of the to be deleted comment. */
        wxString              m_text;           /**< A backup of the text of the comment. */
        int                   m_object;         /**< The identifier of the object the comment was attached to. -1 if none. */
        coordinate            m_coordinate;     /**< the coordinate of the comment. */
        float                 m_width;          /**< The width of the comment. */
        float                 m_height;         /**< The width of the comment. */
        wxArrayLong           m_comments;       /**< Identifiers of the comments attached to the comment. */
        unsigned int                  m_in_diagram;     /**< The id of the diagram the comment is removed from. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_comm The comment that is to be removed.
         * @param p_dia_ptr A pointer to the diagram the comment is to be removed from.
         */
        grape_event_remove_comment( grape_frame *p_main_frame, comment* p_comm, diagram* p_dia_ptr );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_comment( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the event to change a comment.
     */
    class grape_event_change_comment_text : public grape_event_base
    {
      private:
        unsigned int        m_comment_id; /**< A pointer to the comment of which the text is to be changed. */
        wxString    m_new_text; /**< A string containing the new contents of the comment. */
        wxString    m_old_text; /**< A string containing the previous contents of the comment. */
        bool        m_ok_pressed; /**< A boolean indicating whether the user pressed OK. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_comment The comment whose text is to be changed.
         */
        grape_event_change_comment_text( grape_frame *p_main_frame, comment* p_comment );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_comment_text( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * Array of remove comment events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_comment, arr_event_remove_comment );

    /**
     * \short Represents the event to attach a comment.
     */
    class grape_event_attach_comment : public grape_event_base
    {
      private:
        unsigned int    m_comment; /**< The identifier of the comment that is to be attached. */
        int     m_connected_to; /**< The identifier of the object that the comment is currently attached to */
        unsigned int    m_object; /**< The identifier of the object the comment is to be attached to. */
        unsigned int    m_diagram; /**< The identifier of the diagram in which the event occurs. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_comment The comment that is to be attached.
         * @param p_object The object the comment is to be attached to.
         */
        grape_event_attach_comment( grape_frame *p_main_frame, comment* p_comment, object* p_object );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_comment( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the event to detach a comment.
     */
    class grape_event_detach_comment : public grape_event_base
    {
      private:
        unsigned int    m_comment; /**< The identifier of the comment that is to be detached. */
        int     m_object; /**< The identifier of the object the comment is to be detached from. */
        unsigned int    m_diagram; /**< The identifier of the diagram in which the event occurs. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_comment The comment that is to be detached.
         */
        grape_event_detach_comment( grape_frame *p_main_frame, comment* p_comment );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_comment( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };
  }
}
#endif // GRAPE_EVENT_COMMENT_H
