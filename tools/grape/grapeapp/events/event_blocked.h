// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_blocked.h
//
// Declares GraPE events for blocked property.

#ifndef GRAPE_EVENT_BLOCKED_H
#define GRAPE_EVENT_BLOCKED_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    class grape_event_detach_channel_communication;
    class grape_event_remove_blocked;
    class grape_event_remove_visible;

    /**
     * \short Represents the add blocked event.
     */
    class grape_event_add_blocked : public grape_event_base
    {
      private:
        unsigned int                  m_block; /**< Identifier of the to be created blocked. */
        coordinate            m_coord; /**< The coordinate that was clicked. */
        unsigned int                  m_conn; /**< Identifier of the clicked connection. */
        float                 m_def_block_width;  /**< The default width of a blocked. */
        float                 m_def_block_height; /**< The default height of a blocked. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram the blocked is added to. */
        grape_event_remove_visible *m_removed_visible; /**< Remove event for the visible the connection already had, if applicable. */
        grape_event_remove_blocked *m_removed_blocked; /**< Remove event for the blocked the connection already had, if applicable. */
        grape_event_detach_channel_communication *m_detached_chan_comm; /**< Detach event for the property the connection already had, if applicable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the blocked property will be added.
         * @param p_conn The connection where the property belongs to.
         */
        grape_event_add_blocked( grape_frame *p_main_frame, coordinate &p_coord, connection* p_conn );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_blocked( void );

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
     * \short Represents the remove blocked event.
     */
    class grape_event_remove_blocked : public grape_event_base
    {
      private:
        unsigned int                  m_block;       /**< Identifier of the to be deleted blocked. */
        int                   m_connection;       /**< Identifier of the connection the blocked was attached to, if applicable. */
        coordinate            m_coordinate;   /**< the coordinate that was clicked. */
        float                 m_width;        /**< The width of the blocked. */
        float                 m_height;       /**< The height of the blocked. */
        wxArrayLong           m_comments;     /**< Identifiers of comments attached to the blocked. */
        unsigned int                  m_in_diagram;  /**< Identifier of the diagram the blocked is removed from. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_block The blocked that is to be deleted.
         * @param p_arch_dia_ptr The diagram the blocked is to be deleted from.
         */
        grape_event_remove_blocked( grape_frame *p_main_frame, blocked* p_block, architecture_diagram* p_arch_dia_ptr );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_blocked(  void  );

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
     * Array of remove channel events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_blocked, arr_event_remove_blocked );
  }
}
#endif // GRAPE_EVENT_BLOCKED_H
