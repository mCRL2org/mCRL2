// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_processreference.h
//
// Declares GraPE events for process references

#ifndef GRAPE_EVENT_PROCREF_H
#define GRAPE_EVENT_PROCREF_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add process reference event.
     */
    class grape_event_add_process_reference : public grape_event_base
    {
      private:
        unsigned int                 m_proc_ref; /**< A pointer to the to be created reference state. */
        coordinate           m_coord; /**< The coordinate that was clicked. */
        float                m_def_proc_ref_width;  /**< The default width of a reference state. */
        float                m_def_proc_ref_height; /**< The default height of a reference state. */
        unsigned int                 m_in_diagram; /**< De diagram the process reference will have been added to.*/
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the process reference will be added.
         */
        grape_event_add_process_reference( grape_frame *p_main_frame, coordinate &p_coord );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_process_reference( void );

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
     * \short Represents the remove process reference event.
     */
    class grape_event_remove_process_reference : public grape_event_base
    {
      private:
        unsigned int                        m_proc_ref; /**< A pointer to the to be deleted process reference. */
        wxString                    m_name;         /**< Backup of the name of the reference state. */
        unsigned int                m_property_of;  /**< Backup of the id of the process diagram the process reference refers to. */
        list_of_varupdate           m_parameter_assignments;  /**< Backup of the initial parameter values of the reference state. */
        coordinate                  m_coordinate;   /**< Backup of the coordinate of the reference state. */
        float                       m_width;        /**< Backup of the width of the reference state. */
        float                       m_height;       /**< Backup of the height of the reference state. */
        wxArrayLong                 m_comments;     /**< Backup of the list of comments attached to the reference state. */
        unsigned int                        m_in_diagram;  /**< Backup of the pointer to the diagram the reference state is in. */
        arr_event_remove_channel                 m_channels; /**< Backup of the channels that the process_reference had. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_proc_ref A pointer to the to be deleted process reference.
         * @param p_arch_dia_ptr A pointer to the diagram the reference is to be deleted from.
         * @param p_normal A flag indicating whether it is a normal remove (true) and all channels should be removed (also with the flag p_normal set to true)
         */
        grape_event_remove_process_reference( grape_frame *p_main_frame, process_reference* p_proc_ref, architecture_diagram* p_arch_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_process_reference( void );

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
     * \short Represents the event to change a process reference.
     */
    class grape_event_change_procref : public grape_event_base
    {
      private:
        int m_proc_ref_id; /** The ID of the process reference which is being editted. */
        int m_old_proc_id; /**< The ID of the process diagram the reference is pointed to before the event. */
        wxString m_old_proc_name; /**< The name of the process diagram the reference is pointed to before the event. */
        int m_new_proc_id; /** The ID of the process diagram the reference will point to after the event. */
        wxString m_new_proc_name; /** The name of the process diagram the reference will point to after the event. */
        wxString m_new_text; /** The new parameter initializations. */
        wxString m_old_text; /** The old parameter initializations. */
        bool m_ok_pressed; /** @c true if the user pressed OK in the dialog shown by the constructor. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_proc_ref The comment whose text is to be changed.
         */
        grape_event_change_procref( grape_frame *p_main_frame, process_reference* p_proc_ref );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_procref( void );

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
     * Array of remove process reference events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_process_reference, arr_event_remove_proc_ref );
  }
}
#endif // GRAPE_EVENT_PROCREF_H
