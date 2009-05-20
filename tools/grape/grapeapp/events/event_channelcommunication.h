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

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    class grape_event_detach_channel_communication;
    class grape_event_detach_property;

    /**
     * Array of detach channel events.
     */
    WX_DECLARE_OBJARRAY( grape_event_detach_channel_communication, arr_event_detach_channel_communication );

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
        channel_type                  m_channel_type_1; /**< Channel type of the first clicked channel. */
        channel_type                  m_channel_type_2; /**< Channel type of the second clicked channel. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram the channel communication is added to. */
        arr_event_detach_channel_communication  m_detached_comm_1; /**< Array of detached channel communications from channel 1. */
        arr_event_detach_channel_communication  m_detached_comm_2; /**< Array of detached channel communications from channel 2. */

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
        wxString                        m_rename;       /**< Rename of the channel communication. */
        wxArrayLong                     m_comments;     /**< Identifiers of the list of comments attached to the channel communication. */
        unsigned int                            m_in_diagram;  /**< Identifier of the diagram from which the channel was deleted. */
        wxArrayLong                     m_channels;     /**< Identifiers of the channels the channel communication was attached to. */
        channel_communication_type      m_channel_communication_type;   /**< Backup of the channel communication type. */
        int                             m_communication_selected; /**< Backup of the communication id of which the communication was selected. */
        int                             m_communication_channel; /**< Backup of the channel id of which the communication was selected. */
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
        arr_event_detach_channel_communication  m_detached_comm; /**< The channel communication array of the channels that are detached, if applicable */
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
     * \short Represents the event to change a channel communication name.
     */
    class grape_event_change_channel_communication : public grape_event_base
    {
      private:
        unsigned int        m_channel_communication; /**< A pointer to the channel communication of which the name is to be changed. */        
        channel_communication             m_old_channel_communication; /**< Containing the previous channel communication. */
        channel_communication             m_new_channel_communication; /**< Containing the new channel communication. */
        bool                m_pressed_ok; /**< A boolean indicating whether the user confirmed the rename action. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_channel The channel whose text is to be changed.
         */
        grape_event_change_channel_communication( grape_frame *p_main_frame, channel_communication* p_channel_communication );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_channel_communication( void );

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
    WX_DECLARE_OBJARRAY( grape_event_remove_channel_communication, arr_event_remove_channel_communication );

  }
}
#endif // GRAPE_EVENT_CHANCOMM_H
