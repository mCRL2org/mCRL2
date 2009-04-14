// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_architecturereference.h
//
// Declares GraPE events for architecture references.

#ifndef GRAPE_EVENT_ARCHREF_H
#define GRAPE_EVENT_ARCHREF_H

#include "event_base.h"

#include "event_channel.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add architecture reference event.
     */
    class grape_event_add_architecture_reference : public grape_event_base
    {
      private:
        unsigned int        m_arch_ref; /**< A pointer to the to be created architecture reference. */
        coordinate  m_coord; /**< The coordinate that was clicked. */
        float       m_def_arch_ref_width;  /**< The default width of a reference state. */
        float       m_def_arch_ref_height; /**< The default height of a reference state. */
        unsigned int        m_in_diagram; /**< A pointer to the diagram the architecture reference will be added to. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the architecture reference will be added.
         */
        grape_event_add_architecture_reference(  grape_frame *p_main_frame, coordinate &p_coord  );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_architecture_reference(  void  );

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
     * \short Represents the remove architecture reference event.
     */
    class grape_event_remove_architecture_reference : public grape_event_base
    {
      private:
        unsigned int                            m_arch_ref; /**< Identifier of the to be deleted architecture reference. */
        wxString                        m_name;         /**< Backup of the name of the reference state. */
        unsigned int                    m_property_of;  /**< Backup of the id of the architecture reference the architecture reference refers to. */
        coordinate                      m_coordinate;   /**< Backup of the coordinate of the reference state. */
        float                           m_width;        /**< Backup of the width of the reference state. */
        float                           m_height;       /**< Backup of the height of the reference state. */
        wxArrayLong                     m_comments;     /**< Identifiers of comments attached to the reference state. */
        unsigned int                            m_in_diagram;  /**< Backup of the pointer to the diagram the reference state is in. */
        arr_event_remove_channel                     m_channels; /**< Backup of the channels that the process_reference had. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_arch_ref A pointer to the to be deleted architecture reference.
         * @param p_arch_dia_ptr A pointer to the architecture diagram the reference is to be removed from.
         * @param p_normal A flag indicating whether it is a normal remove (remove channels as well) or not.
         */
        grape_event_remove_architecture_reference(  grape_frame *p_main_frame, architecture_reference* p_arch_ref, architecture_diagram* p_arch_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_architecture_reference(  void  );

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
     * \short Represents the event to change an architecture reference.
     */
    class grape_event_change_archref : public grape_event_base
    {
      private:
        int m_arch_ref_id;      /**< The ID of the architecture reference which is being editted. */
        wxString m_new_arch_name;    /**< The name of the architecture diagram the reference will point to after the event. */
        int m_new_arch_id;      /**< The ID of the architecture diagram the reference will point to after the event.*/
        wxString m_old_arch_name;/**< The name of the architecture diagram the reference pointed to before the event.*/
        int m_old_arch_id;      /**< The ID of the architecture diagram the reference pointed to before the event.*/
        bool m_ok_pressed; /** @c true if the user pressed OK in the dialog shown by the constructor. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_arch_ref The comment whose text is to be changed.
         */
        grape_event_change_archref( grape_frame *p_main_frame, architecture_reference* p_arch_ref );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_archref( void );

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
     * Array of remove architecture reference events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_architecture_reference, arr_event_remove_arch_ref );
  }
}
#endif // GRAPE_EVENT_ARCHREF_H
