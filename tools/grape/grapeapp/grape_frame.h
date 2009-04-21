// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_frame.h
//
// Declares the main frame of the application.

#ifndef GRAPEAPP_GRAPE_FRAME_H
#define GRAPEAPP_GRAPE_FRAME_H

#include <wx/frame.h>
#include <wx/filename.h>
#include <wx/splitter.h>
#include "specification.h"
#include "visuals/visualobject.h"

class wxCommandProcessor;
class wxToggleButton;
class wxHtmlHelpController;

namespace grape
{
  namespace grapeapp
  {
    class grape_menubar;
    class grape_toolbar;
    class grape_glcanvas;
    class grape_logpanel;
    class grape_clipboard;
    class grape_listbox;

    /**
      * Describes several modes the window can have.
      */
    enum grape_mode
    {
      GRAPE_MODE_NONE = 0,  /**< The mode when not even a specification is loaded.*/
      GRAPE_MODE_SPEC,      /**< The mode when a specification is loaded, but no diagrams or dataspecs.*/
      GRAPE_MODE_ARCH,      /**< The mode when an architecture diagram is being edited. */
      GRAPE_MODE_PROC,      /**< The mode when a process diagram is being edited. */
      GRAPE_MODE_DATASPEC   /**< The mode when a datatype specification is being edited. */
    };

    /**
     * \short Represents the main frame of the application.
     */
    class grape_frame : public wxFrame
    {
      private:
        grape_menubar   *m_menubar;                   /**< The main menubar. */
        grape_listbox   *m_process_diagram_list;      /**< Listbox used to list process diagrams of the current specification. */
        grape_listbox   *m_architecture_diagram_list; /**< Listbox used to list architecture diagrams of the current specification. */
        grape_glcanvas  *m_glcanvas;                  /**< Canvas used for drawing diagrams. */
        wxSplitterWindow *m_splitter;                  /**< Splitter used to attach the canvas and the datatypespecification. */
        grape_logpanel  *m_logpanel;                  /**< Logpanel used to display messages. */
        wxStatusBar *m_statusbar;                 /**< Statusbar used to display information. */

        wxCommandProcessor  *m_event_handler;          /**< The main event handler and undo/redo stack. */

        grape_clipboard   *m_clipboard;             /**< The clipboard for the main frame */
        wxFileName          m_filename;                   /**< full path to the file containing this specification. */
        bool                m_modified;                   /**< The flag to point if the file is modified */
        libgrape::grape_specification *m_specification; /**< The GraPE specification that is being modified */
        wxHtmlHelpController    *m_help_controller;            /**< The help controller. */
        wxToggleButton      *m_dataspecbutton;             /**< The button to switch to datatype specification.*/
        wxTextCtrl          *m_datatext;                   /**< The text control for entering the datatype specification. */
        grape_mode            m_mode;                       /**< The frame's mode. */
        unsigned int                  m_counter;                    /**< The counter for new id's. */
        wxTimer             *m_timer;                       /**< Timer used to set the log panel right after some wall clock time. */

        DECLARE_EVENT_TABLE();                        /**< The event table of this frame. */

      protected:
        /**
         * Set the toolbar to one of the modes described in grape_toolbar_mode.
         * It basically operates as an encapsulation of wxFrame::SetToolbar().
         * @param p_mode Describes which toolbar to show.
         */
        void set_toolbar( grape_mode p_mode );

        /**
         * Loads the data specification from the specification in the library
         * and puts its contents in the text view.
         */
        void load_datatype_specification();

        /**
         * Toggles between datatype specification view and canvas view.
         * Also loads and saves specification if necessary.
         * @param p_mode The mode in which the frame is being set. Only
         * for a value of GRAPE_MODE_DATASPEC the frame will show the
         * data specification editor.
         */
        void toggle_view( grape_mode p_mode );

        /**
         * Sets the window title, with a filename and the modified status.
         */
        void set_title();

        /**
         * Takes care of setting the appropiate undo and redo settings on the toolbar.
         * It only allowed to run after m_event_handler->Submit( event, bool ).
         */
        void update_toolbar( void );

        /**
         * Takes care of setting the appropiate delete settings on the menubar.
         * It only allowed to run after m_event_handler->Submit( event, bool ).
         */
        void update_menubar( void );

      public:
        /**
         * Default constructor.
         * Initializes grape_frame.
         * @param p_filename A filename to the specification which should be
         * opened by the frame. Usually this value is supplied through the
         * command line interface.
         */
        grape_frame( const wxString &p_filename = wxEmptyString );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_frame( void );

        /**
         * Get a pointer to the menubar.
         * @return Returns a pointer to the menubar
         */
        grape_menubar * get_menubar( void );

        /**
         * Get a pointer to the process diagram list.
         * @return Returns a pointer to the process diagram list
         */
        grape_listbox * get_process_diagram_listbox( void );

        /**
         * Get a pointer to the architecture diagram list.
         * @return Returns a pointer to the architecture diagram list
         */
        grape_listbox * get_architecture_diagram_listbox( void );

        /**
         * Get a pointer to the logpanel.
         * @return Returns a pointer to the logpanel
         */
        grape_logpanel * get_logpanel( void );

        /**
         * Get a pointer to the statusbar.
         * @return Returns a pointer to the statuspanel
         */
        wxStatusBar * get_statusbar( void );

        /**
         * Get a pointer to the canvas.
         * @return Returns a pointer to the canvas
         */
        grape_glcanvas * get_glcanvas( void );

        /**
         * Get a pointer to the event handler.
         * @return Returns a pointer to the eventhandler
         */
        wxCommandProcessor * get_event_handler( void );

        /**
         * Get a pointer to the clipboard
         * @return Returns a pointer to the clipboard
         */
        grape_clipboard * get_clipboard( void );

        /**
         * Selection tool event handler.
         * Event handler called when the user enables the selection tool on the toolbar.
         * @param p_event The generated event.
         */
        void event_tool_select(wxCommandEvent& p_event);

        /**
         * Selection event handler.
         * Event handler called when a click on the canvas is performed.
         * @param p_coord The clicked coordinate.
         * @param p_vis_obj The clicked visual object, if applicable.
         * @param p_shift_pressed A boolean to indicate if the shift key was held during the click.
         */
        void event_click( coordinate &p_coord, visual_object* p_vis_obj, bool p_shift_pressed );

        /**
         * Selection event handler.
         * Event handler called when a click on the canvas is performed.
         * @param p_coord The clicked coordinate, no object was clicked.
         */
        void event_click( coordinate &p_coord );

        /**
         * Doubleclick event handler.
         * Event handler called when a doubleclick on the canvas is performed.
         * @param p_vis_obj The doubleclicked object.
         */
        void event_doubleclick( visual_object* p_vis_obj );

        /**
         * Move event handler.
         * Event handler called when the select tool is enabled and the user clicks and drags an object in the workspace.
         * @param p_vis_obj_id The identifier of the dragged visual object.
         * @param p_down The original coordinate of the visual object.
         * @param p_up The coordinate at which the mouse button was released
         * @param p_click_location A direction (in terms wind directions) of on which side of the object the drag started.
         * @param p_mousedown A flag indicatin whether the mouse is still down at the moment of performing the event.
         */
        void event_drag( int p_vis_obj_id, coordinate &p_down, coordinate &p_up, grape_direction p_click_location, bool p_mousedown = false );

        /**
         * This event handler is called when a tool is selected
         * in the toolbar.
         * @param p_event The generated event.
         */
        void event_tool_selected(wxCommandEvent& p_event);

        /**
         * 'New' menu event handler.
         * Event handler called when the user wants to create a new specification through the menubar.
         * @param p_event The generated event.
         */
        void event_menu_new(wxCommandEvent& p_event);

        /**
         * Open menu event handler.
         * Event handler called when the user opens a specification through the menubar.
         * @param p_event The generated event.
         */
        void event_menu_open(wxCommandEvent& p_event);

        /**
         * Close menu event handler.
         * Event handler called when the user closes a specification through the menubar.
         * @param p_event The generated event.
         */
        void event_menu_close(wxCommandEvent& p_event);

        /**
         * Save menu event handler.
         * Event handler called when the user saves a specification through the menubar.
         * @param p_event The generated event.
         */
        void event_menu_save(wxCommandEvent& p_event);

        /**
         * Save as menu event handler.
         * Event handler called when the user saves a specification as... through the menubar.
         * @param p_event The generated event.
         */
        void event_menu_saveas(wxCommandEvent& p_event);

        /**
         * Event handler for exporting to mCRL2.
         * @param p_event The generated event.
         */
        void event_menu_exportmcrl2(wxCommandEvent& p_event);

        /**
         * Event handler for specification validation.
         * @param p_event The generated event.
         */
        void event_menu_validate_specification(wxCommandEvent &p_event);

        /**
         * Event handler for diagram validation.
         * @param p_event The generated event.
         */
        void event_menu_validate_diagram(wxCommandEvent &p_event);

        /**
         * Event handler for exporting images.
         * @param p_event The generated event.
         */
        void event_menu_exportimage(wxCommandEvent& p_event );

        /**
         * Event handler for exporting the datatype specification to text.
         * @param p_event The generated event.
         */
        void event_menu_exporttext(wxCommandEvent& p_event );

        /**
         * Print event handler.
         * @param p_event The generated event.
         */
        void event_menu_print(wxCommandEvent& p_event);

        /**
         * Quit Grape menu event handler
         * @param p_event The generated event.
         */
        void event_menu_quit(wxCommandEvent& p_event);

        /**
         * Undo menu event handler.
         * Event handler called when the user undos an action.
         * @param p_event The generated event.
         */
        void event_menu_undo(wxCommandEvent& p_event);

        /**
         * Redo menu event handler
         * Event handler called when the user redoes an action.
         * @param p_event The generated event.
         */
        void event_menu_redo(wxCommandEvent& p_event);

        /**
         * Cut menu event handler
         * Event handler called when the user cuts an object.
         * @param p_event The generated event.
         */
        void event_menu_cut(wxCommandEvent& p_event);

        /**
         * Copy menu event handler
         * Event handler called when the user copies an object.
         * @param p_event The generated event.
         */
        void event_menu_copy(wxCommandEvent& p_event);

        /**
         * Paste menu event handler
         * Event handler called when the user pastes an object.
         * @param p_event The generated event.
         */
        void event_menu_paste(wxCommandEvent& p_event);

        /**
         * Select all menu event handler
         * Event handler called when the user wants to select all objects in the diagram.
         * @param p_event The generated event.
         */
        void event_menu_select_all(wxCommandEvent& p_event);

        /**
         * Delete menu event handler
         * Event handler called when user chooses to delete one or more objects.
         * @param p_event The generated event.
         */
        void event_menu_delete(wxCommandEvent& p_event);

        /**
         * Properties menu event handler
         * Event handler called when user chooses to edit the properties of an object.
         * @param p_event The generated event.
         */
        void event_menu_properties(wxCommandEvent& p_event);

        /**
         * Deselect all menu event handler
         * Event handler called when the user wants to deselect all objects in the diagram.
         * @param p_event The generated event.
         */
        void event_menu_deselect_all(wxCommandEvent& p_event);

        /**
         * Add architecture diagram menu event handler
         * Event handler called when the user adds an architecture diagram.
         * @param p_event The generated event.
         */
        void event_menu_add_architecture_diagram(wxCommandEvent& p_event);

        /**
         * Help menu event handler
         * Event handler called when the user wants to display help.
         * @param p_event The generated event.
         */
        void event_menu_help(wxCommandEvent& p_event);

        /**
         * About menu event handler
         * Event handler called when the user wants to display the about box.
         * @param p_event The generated event.
         */
        void event_menu_about(wxCommandEvent& p_event);

        /**
         * Add process diagram menu event handler
         * Event handler called when the user adds a process diagram.
         * @param p_event The generated event.
         */
        void event_menu_add_process_diagram(wxCommandEvent& p_event);

        /**
         * This event is called whenever the user clicks a diagram listbox
         * @param p_event The generated event.
         */
        void event_select_diagram( wxCommandEvent &p_event );

        /**
         * This event is called whenever the user wants to rename a diagram
         * @param p_event The generated event.
         */
        void event_menu_rename_diagram( wxCommandEvent &p_event );

        /**
         * This event is called whenever the user wants to remove a diagram with the menu
         * @param p_event The generated event.
         */
        void event_menu_remove_diagram( wxCommandEvent &p_event );

        /**
         * This event is called whenever the user wants to remove a diagram with the keyboard
         * @param p_diagram_type The diagram type the event is mentioned for.
         */
        void event_listbox_remove_diagram( int p_diagram_type );

        /**
         * This is called when the window is being closed (with the [X] or with File->Quit.
         */
        void event_window_close( wxCloseEvent &p_event );

        /**
         * This event is called when the datatype specification mode is toggled.
         * @param p_event The generated event.
         */
        void event_datatype_spec( wxCommandEvent &p_event );

        /**
         * This function is called when the timer in the constructor times out.
         * It sets the splitter's gravity after a certain amount of time.
         * @param p_event The timer event.
         */
        void grape_event_timer( wxTimerEvent &p_event );

        /**
         * Gives the current filename.
         */
        wxFileName get_filename( void ) const;

        /**
         * Sets the filename of the opened file.
         * @param p_filename The filename.
         */
        void set_filename( const wxFileName &p_filename );

        /**
         * @return Returns whether the current file is modified.
         */
        bool get_is_modified( void ) const;

        /**
         * Sets whether the file is modified.
         * @param p_modified True if the file is modified.
         */
        void set_is_modified( bool p_modified );

        /**
         * @return A pointer to the GraPE specification held by this frame.
         */
        libgrape::grape_specification* get_grape_specification( void ) const;

        /**
         * Assigns a GraPE specification to this frame.
         * @param p_grape_spec The specification.
         */
        void set_grape_specification( libgrape::grape_specification* p_grape_spec );

        /**
         * Id generator set function.
         * Sets the new value of the counter after loading a (new) grapespecification.
         * @param p_counter The new value of the counter.
         */
        void set_id_counter( long p_counter );

        /**
         * Id generator.
         * Increments it.
         * @return Returns an unsigned integer with the new object id.
         */
        unsigned int get_new_id( void );

        /**
         * Retrieve a pointer to the help controller.
         * @return A pointer to the help controller.
         */
        wxHtmlHelpController *get_help_controller( void );

        /**
         * Whenever the data specification was changed, grab the contents
         * and save them in the specification in the library.
         */
        void save_datatype_specification();

        /**
         * Takes care of setting the appropiate window settings for a certain mode.
         * It adjusts the menubar, toolbar and the listboxes on the right.
         * @param p_mode The mode as specified at grape_mode .
         */
        void set_mode( grape_mode p_mode );

        /**
         * Mode retrieval function.
         * @returns The current mode of the grape frame.
         */
        grape_mode get_mode( void );

        /**
         * Takes care of setting the appropiate settings on the toolbar, menubar and statusbar.
         */
        void update_bars( void );

        /**
         * Takes care of setting the appropiate text on the statusbar.
         */
        void update_statusbar( wxCommandEvent& p_event );

        /**
         * Is called when the data specification is modified.
         * @param p_event The generated event.
         */
        void dataspec_modified( wxCommandEvent &p_event );
    };
  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_FRAME_H
