// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_channelcommunication.h
//
// Declares GraPE events for channel communications

#ifndef GRAPE_EVENT_CHANCOMM_H
#define GRAPE_EVENT_CHANCOMM_H

#include "event_blocked.h"
#include "event_visible.h"

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    class grape_event_detach_channel_communication;
    class grape_event_detach_property;

    /**
     * \short Represents the add channel communication event.
     */
    class grape_event_add_channel_communication : public grape_event_base
    {
      private:
        unsigned int                  m_c_comm; /**< Identifier of the to be created channel communication. */
        coordinate            m_coord; /**< The coordinate that was clicked. */
        unsigned int                  m_chan_1; /**< Identifier of the first clicked channel. */
        unsigned int                  m_chan_2; /**< Identifier of the second clicked channel. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram the channel communication is added to. */
        grape_event_remove_visible *m_removed_visible_channel_1; /**< Detach event for the property the connection already had, if applicable. */
        grape_event_remove_blocked *m_removed_blocked_channel_1; /**< Detach event for the property the connection already had, if applicable. */
        grape_event_remove_visible *m_removed_visible_channel_2; /**< Detach event for the property the connection already had, if applicable. */
        grape_event_remove_blocked *m_removed_blocked_channel_2; /**< Detach event for the property the connection already had, if applicable. */
        grape_event_detach_channel_communication *m_detached_comm_1; /**< Detach event for the channel communication the channel was in, if applicable. */
        grape_event_detach_channel_communication *m_detached_comm_2; /**< Detach event for the channel communication the channel was in, if applicable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the channel communication will be added.
         * @param p_chan_1 The first channel that is involved in the channel communication.
         * @param p_chan_2 The second channel that is involved in the channel communication.
         */
        grape_event_add_channel_communication(  grape_frame *p_main_frame, coordinate &p_coord, channel* p_chan_1, channel* p_chan_2  );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_channel_communication(  void  );

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
     * \short Represents the remove channel communication event.
     */
    class grape_event_remove_channel_communication : public grape_event_base
    {
      private:
        unsigned int                            m_c_comm; /**< Identifier of the to be deleted channel communication. */
        coordinate                      m_coordinate;   /**< Backup of the coordinate of the channel communication. */
        float                           m_width;        /**< Backup of the width of the channel communication. */
        float                           m_height;       /**< Backup of the height of the channel communicatione. */
        wxArrayLong                     m_comments;     /**< Identifiers of the list of comments attached to the channel communication. */
        unsigned int                            m_in_diagram;  /**< Identifier of the diagram from which the channel was deleted. */
        wxArrayLong                     m_channels;     /**< Identifiers of the channels the channel communication was attached to. */
        // Below implemented with arrays instead of pointers, to prevent annoying pointer crashes.
        arr_event_remove_visible     m_visible;  /**< Backup of the visible that was associated with the channel, if applicable. */
        arr_event_remove_blocked     m_blocked;  /**< Backup of the blocked that was associated with the channel, if applicable. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_c_comm Pointer to the to be deleted channel communication.
         * @param p_arch_dia_ptr Pointer to the diagram the channel communication is to be removed from.
         * @param p_normal A flag indicating whether the remove is a normal remove that should destroy other properties attached to the channel communication as well.
         */
        grape_event_remove_channel_communication(  grape_frame *p_main_frame, channel_communication *p_c_comm, architecture_diagram* p_arch_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_channel_communication(  void  );

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
     * \short Represents the attach channel communication event.
     */
    class grape_event_attach_channel_communication : public grape_event_base
    {
      private:
        unsigned int      m_channel_communication; /**< The channel communication to which a channel is added. */
        unsigned int      m_channel; /**< The channel that is added to the channel communication. */
        unsigned int      m_diagram; /**< Identifier of the diagram in which the change was made. */
        grape_event_remove_visible *m_remove_channel_visible; /**< The visible of the channel that is removed, if applicable. */
        grape_event_remove_blocked *m_remove_channel_blocked; /**< The visible of the channel that is removed, if applicable. */
        grape_event_detach_channel_communication *m_detach_channel_communication; /**< The channel communication of the channel that is detached, if applicable */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_channel_communication The channel communication that is attached.
         * @param p_channel The channel that is attached.
         */
        grape_event_attach_channel_communication(  grape_frame *p_main_frame, channel_communication* p_channel_communication, channel* p_channel );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_channel_communication(  void  );

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
     * \short Represents the detach channel communication event.
     */
    class grape_event_detach_channel_communication : public grape_event_base
    {
      private:
        unsigned int      m_channel_communication; /**< The channel communication from which a channel is detached. */
        unsigned int      m_channel; /**< The channel that is detached from the channel communication. */
        unsigned int      m_diagram; /**< Identifier of the diagram in which the change was made. */
        grape_event_remove_channel_communication *m_remove_event; /**< The remove channel communication event that was performed, if applicable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_channel_communication The channel communication that is to be detached.
         * @param p_channel The channel that is to be detached.
         */
        grape_event_detach_channel_communication(  grape_frame *p_main_frame, channel_communication* p_channel_communication, channel* p_channel );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_channel_communication(  void  );

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
    WX_DECLARE_OBJARRAY( grape_event_remove_channel_communication, arr_event_remove_channel_communication );

    /**
     * Array of remove channel events.
     */
    WX_DECLARE_OBJARRAY( grape_event_detach_channel_communication, arr_event_detach_channel_communication );
  }
}
#endif // GRAPE_EVENT_CHANCOMM_H
