// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_diagram.h
//
// Declares GraPE events for diagrams

#ifndef GRAPE_EVENT_DIAGRAM_H
#define GRAPE_EVENT_DIAGRAM_H

#include <wx/glcanvas.h>

#include "event_base.h"

#include "event_architecturereference.h"
#include "event_architecturediagram.h"
#include "event_channel.h"
#include "event_channelcommunication.h"
#include "event_comment.h"
#include "event_processreference.h"
#include "event_referencestate.h"
#include "event_terminatingtransition.h"
#include "event_nonterminatingtransition.h"
#include "event_state.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the diagram select event.
     */
    class grape_event_select_diagram : public grape_event_base
    {
      private:
        wxString            m_diagram_name;           /**< String of the selection for identifiing the right listbox. */
        grape_diagram_type  m_diagram_type;           /**< Identifier of the diagram type. */
        unsigned int                m_diagram;                /**< Identifier of the the selected diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_diagram_name Name of the selected diagram.
         */
        grape_event_select_diagram(grape_frame *p_main_frame, const wxString &p_diagram_name);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_select_diagram(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

    /**
      * \short Represents the rename event.
      */
    class grape_event_rename_diagram : public grape_event_base
    {
      private:
        unsigned int        m_diagram;        /**< Identifier of the diagram to be renamed */
        int         m_diagram_type;   /**< Identifier of the diagram type. */
        wxString    m_orig_name;      /**< The old name of the diagram. Also for identifiing the right listbox. */
        wxString    m_new_name;       /**< The new name of the diagram. */
      public:
        /**
        * Initialization constructor.
        * Initializes the event.
        * @param p_main_frame Pointer to the main frame.
        * @param p_new_name The new name of the diagram.
        * @param p_dia A pointer to the to be renamed diagram.
        */
        grape_event_rename_diagram( grape_frame *p_main_frame, const wxString &p_new_name, diagram* p_dia );

        /**
        * Default destructor.
        * Frees allocated memory.
        */
        ~grape_event_rename_diagram( void );

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
     * \short Represents the diagram rename event.
     */
    class grape_event_dialog_rename_diagram : public grape_event_base
    {
      private:
        unsigned int        m_diagram;      /**< Identifier of the diagram. */
        wxString    m_old_name;   /**< The old name of the diagram. */
        wxString    m_new_name;    /**< The new name of the diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_dialog_rename_diagram(grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_dialog_rename_diagram(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

    /**
     * \short Represents the diagram remove event.
     */
    class grape_event_remove_diagram : public grape_event_base
    {
      private:
        unsigned int                        m_diagram;  /**< Identifier of the renamed diagram. */
        grape_diagram_type          m_type;          /**< Identifier of the diagram type. */
        wxString                    m_name;          /**< Backup of the name of the diagram. */

        preamble                    m_preamble; /**< The preamble is stored here in case of a process diagram. */

        arr_event_remove_comment                 m_comments; /**< Backup of the list of comments attached to the diagram. */

        arr_event_remove_ref_state               m_reference_states; /**< Backup of the list of refence states belonging to the diagram. */
        arr_event_remove_state                   m_states; /**< Backup of the list of states beloning to the diagram. */
        arr_event_remove_tt                      m_terminating_transitions;  /**< Backup of the list of terminating transitions belonging to the diagram. */
        arr_event_remove_ntt                     m_nonterminating_transitions; /**< Backup of the list of nonterminating transitions belonging to the diagram. */
        arr_event_remove_init       m_initial_designators;  /**< Backup of the list of initial designators belonging to the diagram. */
        arr_event_remove_channel_communication    m_channel_communications; /**< Backup of the list of channel communications belonging to the diagram. */
        arr_event_remove_channel    m_channels; /**< Backup of the list of channels belonging to the diagram. */
        arr_event_remove_arch_ref   m_architecture_references;  /**< Backup of the list of architecture references belonging to the diagram. */
        arr_event_remove_proc_ref   m_process_references; /**< Backup of the list of process references belonging to the diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_dia_ptr The current name of the diagram which is being removed.
         */
        grape_event_remove_diagram(grape_frame *p_main_frame, diagram* p_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_diagram(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

    /**
     * \short Represents the export to image event for the currently viewed diagram.
     */
    class grape_event_export_current_diagram_image : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_export_current_diagram_image( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_export_current_diagram_image( void );

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
     * \short Represents the export to mCRL2 event for the currently viewed diagram.
     */
    class grape_event_export_current_diagram_mcrl2 : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_export_current_diagram_mcrl2(grape_frame *p_main_frame);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_export_current_diagram_mcrl2(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

    /**
     * \short Represents the validation event for the currently viewed diagram.
     */
    class grape_event_validate_diagram : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_validate_diagram(grape_frame *p_main_frame);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_validate_diagram(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

    /**
     * \short Represents the delete selected objects event.
     */
    class grape_event_delete_selected_objects : public grape_event_base
    {
      private:
        arr_event_remove_comment                m_comment;   /**< The remove events for this type of object. */
        arr_event_remove_state                  m_state;   /**< The remove events for this type of object. */
        arr_event_remove_ref_state              m_ref_state;   /**< The remove events for this type of object. */
        arr_event_remove_ntt                    m_ntt;   /**< The remove events for this type of object. */
        arr_event_remove_tt                     m_tt;   /**< The remove events for this type of object. */
        arr_event_remove_init                   m_init;   /**< The remove events for this type of object. */
        arr_event_remove_channel                m_channel;   /**< The remove events for this type of object. */
        arr_event_remove_channel_communication  m_c_comm;   /**< The remove events for this type of object. */
        arr_event_remove_proc_ref               m_proc_ref;   /**< The remove events for this type of object. */
        arr_event_remove_arch_ref               m_arch_ref;   /**< The remove events for this type of object. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_delete_selected_objects( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_delete_selected_objects( void );

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
     * Display the logpanel text in a statusbar or messagebox
     */
    void display_message(grape_frame *p_main_frame, bool is_valid, int error);
  }
}
#endif // GRAPE_EVENT_DIAGRAM_H
