// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_channel.h
//
// Declares GraPE events for channels

#ifndef GRAPE_EVENT_CHANNEL_H
#define GRAPE_EVENT_CHANNEL_H

#include "event_base.h"

#include "event_blocked.h"
#include "event_visible.h"
#include "event_channelcommunication.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add channel event.
     */
    class grape_event_add_channel : public grape_event_base
    {
      private:
        unsigned int                  m_chan; /**< Identifier of the to be created channel. */
        coordinate            m_coord; /**< The coordinate that was clicked. */
        unsigned int                  m_ref; /**< Identifier of the compound reference that was clicked. */
        float                 m_def_chan_width;  /**< The default width of a channel. */
        float                 m_def_chan_height; /**< The default height of a channel. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram in which the channel will be added. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the channel should appear.
         * @param p_ref The compound reference the channel is to be added to.
         */
        grape_event_add_channel(  grape_frame *p_main_frame, coordinate &p_coord, compound_reference* p_ref  );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_channel(  void  );

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
     * \short Represents the remove channel event.
     */
    class grape_event_remove_channel : public grape_event_base
    {
      private:
        unsigned int                            m_chan; /**< Identifier of the to be deleted channel. */
        bool                            m_normal;       /**< Boolean value to indicate whether the channel should do anything to the channel communications it is attached to; a normal remove */
        wxString                        m_name; /**< Name of the channel. */
        coordinate                      m_coordinate;   /**< Backup of the coordinate of the channel. */
        unsigned int                            m_ref; /**< Identifier of the compound reference that the channel belonged to. */
        float                           m_width;        /**< Backup of the width of the channel. */
        float                           m_height;       /**< Backup of the height of the channel. */
        wxArrayLong                     m_comments;     /**< Backup of the list of comments attached to the channel. */
        unsigned int                            m_in_diagram; /**< Identifier of the diagram from which the channel was deleted. */

        // Below implemented with arrays instead of pointers, to prevent annoying pointer crashes.
        arr_event_remove_visible     m_visible;  /**< Backup of the visible that was associated with the channel, if applicable. */
        arr_event_remove_blocked     m_blocked;  /**< Backup of the blocked that was associated with the channel, if applicable. */
        arr_event_detach_channel_communication m_channel_communication; /**< Backup of the removed channel communication that the channel was attached to, if applicable. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_chan Pointer to the to be deleted channel.
         * @param p_normal A flag indicating whether it is a normal remove operation (true) and whether all objects need to be disconnected or removed.
         */
        grape_event_remove_channel(  grape_frame *p_main_frame, channel *p_chan, diagram* p_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_channel(  void  );

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
     * \short Represents the event to change a channel name.
     */
    class grape_event_change_channel : public grape_event_base
    {
      private:
        unsigned int        m_channel; /**< A pointer to the channel of which the name is to be changed. */
        wxString    m_old_text; /**< A string containing the previous name of the channel. */
        wxString    m_new_text; /**< A string containing the new name of the channel. */
        bool        m_pressed_ok; /**< A boolean indicating whether the user confirmed the rename action. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_channel The channel whose text is to be changed.
         */
        grape_event_change_channel( grape_frame *p_main_frame, channel* p_channel );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_channel( void );

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
     * \short Represents the event to detach a channel.
     */
    class grape_event_detach_channel : public grape_event_base
    {
      private:
        channel*   m_channel; /**< The channel that is to be detached. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_channel that is to be detached.
         */
        grape_event_detach_channel( grape_frame *p_main_frame, channel* p_channel );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_channel( void );

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
    WX_DECLARE_OBJARRAY( grape_event_remove_channel, arr_event_remove_channel );


  }
}
#endif // GRAPE_EVENT_CHANNEL_H
