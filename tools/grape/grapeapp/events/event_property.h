// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_property.h
//
// Declares GraPE events for properties

#ifndef GRAPE_EVENT_PROPERTY_H
#define GRAPE_EVENT_PROPERTY_H

#include "event_base.h"
#include "event_channelcommunication.h"

namespace grape
{
  namespace grapeapp
  {
    class grape_event_detach_property;

    /**
     * \short Represents the attach property event.
     */
    class grape_event_attach_property : public grape_event_base
    {
      private:
        unsigned int                                m_property;       /**< Identifier of the to be attached visible. */
        unsigned int                                m_connection;       /**< Identifier of the connection the visible is attached to. */
        unsigned int                                 m_diagram; /**< Identifier of the diagram the channel communication is added to. */
        grape_event_detach_property          *m_detach_prop; /**< Detach events for blocked that was attached to the connection. */
        grape_event_detach_channel_communication  *m_channel_communication; /**< Detach events for channel communication that was attached to the connection, if the connection was a channel. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_prop The connection property that is to be attached.
         * @param p_conn The connection the property is to be attached to.
         */
        grape_event_attach_property( grape_frame *p_main_frame, connection_property* p_prop, connection* p_conn );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_property(  void  );

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
     * \short Represents the detach property event.
     */
    class grape_event_detach_property : public grape_event_base
    {
      private:
        unsigned int                  m_property;       /**< Identifier of the to be detached visible. */
        int                   m_connection;     /**< Identifier of the connection the visible was attached to. */
        unsigned int                  m_diagram;        /**< Identifier of the diagram in which the objects reside. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_prop The connection property that is to be detached.
         */
        grape_event_detach_property( grape_frame *p_main_frame, connection_property* p_prop );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_property(  void  );

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
  }
}
#endif // GRAPE_EVENT_PROPERTY_H
