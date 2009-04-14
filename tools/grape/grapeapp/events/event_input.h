// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_input.h
//
// Declares GraPE events for mouse actions.

#ifndef GRAPE_EVENT_INPUT_H
#define GRAPE_EVENT_INPUT_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    class visual_object;

    /**
     * \short Represents the click event, which is a click that creates a new object.
     */
    class grape_event_click : public grape_event_base
    {
      private:
        coordinate    m_coord; /**< The coordinate that was clicked. */
        visual_object *m_vis_obj; /**< A pointer to the clicked visual object, if applicable. If not, then a coordinate was clicked. */
        bool          m_shift_pressed; /**< A boolean indicated whether the Shift key was pressed. */
      public:
        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate that was clicked.
         * @param p_vis_obj The visual object on which a click happened, if applicable.
         * @param p_shift_pressed A boolean to indicate if the shift key was held during the click.
         */
        grape_event_click( grape_frame *p_main_frame, coordinate &p_coord, visual_object* p_vis_obj = 0, bool p_shift_pressed = false );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_click( void );

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
     * \short Represents the double click event, which is a click that creates a new object.
     */
    class grape_event_doubleclick : public grape_event_base
    {
      private:
        visual_object *m_vis_obj; /**< A pointer to the clicked visual object, if applicable. If not, then a coordinate was clicked. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_vis_obj The visual object on which a click happened.
         */
        grape_event_doubleclick( grape_frame *p_main_frame, visual_object* p_vis_obj );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_doubleclick( void );

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
     * \short Represents the drag event, which transfers control to either the move or the resize event.
     */
    class grape_event_drag : public grape_event_base
    {
      private:
        int         m_obj; /**< The identifier of the dragged object. */
        coordinate  m_down; /**< The coordinate where the mouse was clicked. */
        coordinate  m_up;   /**< The coordinate where the mouse was released. */
        grape_direction m_click_location; /**< Contains the border where the object was clicked, if it was clicked on a border.*/
        bool        m_mousedown; /**< Should be @c true (default) if the action should be undoable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_vis_obj_id The ID of the visual object being dragged.
         * @param p_down The coordinate at which the mouse button was pressed.
         * @param p_up The coordinate at which the mouse button was released, or during a drag, the position of the mouse cursor.
         * @param p_click_location The border where the object was clicked, if it was clicked on a border.
         * @param p_undo A flag indicatin whether the event can be undone.
         */
        grape_event_drag( grape_frame *p_main_frame, int p_vis_obj_id, coordinate &p_down, coordinate &p_up, grape_direction p_click_location, bool p_undo = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_drag( void );

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
  }
}

#endif // GRAPE_EVENT_INPUT_H
