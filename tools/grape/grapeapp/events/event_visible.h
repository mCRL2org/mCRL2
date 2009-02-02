// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_visible.h
//
// Declares GraPE events for visible property

#ifndef GRAPE_EVENT_VISIBLE_H
#define GRAPE_EVENT_VISIBLE_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    class grape_event_detach_channel_communication;
    class grape_event_remove_blocked;
    class grape_event_remove_visible;

    /**
     * \short Represents the add visible event.
     */
    class grape_event_add_visible : public grape_event_base
    {
      private:
        unsigned int                  m_vis; /**< Identifier of the to be created visible. */
        coordinate            m_coord; /**< the coordinate that was clicked. */
        unsigned int                  m_conn; /**< Identifier of the clicked connection. */
        float                 m_def_vis_width;  /**< The default width of a visible. */
        float                 m_def_vis_height; /**< The default height of a visible. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram the visible is added to. */
        grape_event_remove_visible *m_removed_visible; /**< Remove event for the visible the connection already had, if applicable. */
        grape_event_remove_blocked *m_removed_blocked; /**< Remove event for the blocked the connection already had, if applicable. */
        grape_event_detach_channel_communication *m_detached_chan_comm; /**< Detach event for the property the connection already had, if applicable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the visible property will be added.
         * @param p_conn The connection where the visible property belongs to.
         */
        grape_event_add_visible( grape_frame *p_main_frame, coordinate &p_coord, connection* p_conn );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_visible(  void  );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(  void  );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(  void  );
    };

    /**
     * \short Represents the remove visible event.
     */
    class grape_event_remove_visible : public grape_event_base
    {
      private:
        unsigned int                  m_vis;       /**< Identifier of the to be deleted visible. */
        wxString              m_name;       /**< The name of the to be deleted visible. */
        int                   m_connection;       /**< Identifier of the connection the visible was attached to. */
        coordinate            m_coordinate;      /**< the coordinate that was clicked. */
        float                 m_width;      /**< The default width of a visible. */
        float                 m_height;     /**< The default height of a visible. */
        wxArrayLong           m_comments;     /**< Identifiers of comments attached to the visible. */
        unsigned int                  m_in_diagram;  /**< Identifier of the diagram the visible is removed from. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_vis The visible that is to be deleted.
         * @param p_arch_dia_ptr The diagram that the visible is to be deleted from.
         */
        grape_event_remove_visible( grape_frame *p_main_frame, visible* p_vis, architecture_diagram* p_arch_dia_ptr );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_visible(  void  );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(  void  );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(  void  );
    };

    /**
     * \short Represents the event to change a visible name.
     */
    class grape_event_change_visible : public grape_event_base
    {
      private:
        unsigned int        m_visible; /**< A pointer to the visible of which the name is to be changed. */
        wxString    m_old_text; /**< A string containing the previous name of the visible. */
        wxString    m_new_text; /**< A string containing the new name of the visible. */
        bool        m_pressed_ok; /**< A boolean indicating whether the user confirmed the rename action. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_visible The visible whose name is to be changed.
         */
        grape_event_change_visible( grape_frame *p_main_frame, visible* p_visible );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_visible( void );

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
     * Array of remove channel events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_visible, arr_event_remove_visible );
  }
}
#endif // GRAPE_EVENT_VISIBLE_H
