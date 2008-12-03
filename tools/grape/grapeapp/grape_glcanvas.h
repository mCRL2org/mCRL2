// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_glcanvas.h
//
// Declares the OpenGL canvas used to draw objects.

#ifndef GRAPEAPP_GRAPE_GLCANVAS_H
#define GRAPEAPP_GRAPE_GLCANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "mcrl2/utilities/font_renderer.h"
#include "visuals/visualobject.h"

namespace grape
{
  namespace grapeapp
  {

    using namespace mcrl2::utilities;

    class grape_frame;

    enum canvas_state
    {
      IDLE = 0,
      SELECT,
      ATTACH,
      DETACH,
      ADD_TERMINATING_TRANSITION,
      ADD_NONTERMINATING_TRANSITION,
      ADD_INITIAL_DESIGNATOR,
      ADD_STATE,
      ADD_REFERENCE_STATE,
      ADD_PROCESS_REFERENCE,
      ADD_ARCHITECTURE_REFERENCE,
      ADD_CHANNEL,
      ADD_CHANNEL_COMMUNICATION,
      ADD_VISIBLE,
      ADD_BLOCKED,
      ADD_COMMENT,
    };

    /**
     * \short An OpenGL drawing canvas, used when drawing diagrams.
     */
    class grape_glcanvas : public wxGLCanvas
    {
      private:
        int   m_viewport_width;       /**< Width of the viewport. */
        int   m_viewport_height;      /**< Height of the viewport. */
        bool  m_initialized;          /**< Flag raised when the GL statemachine is initialized. */
        float m_scroll_x;             /**< Amount scrolled in horizontal position. */
        float m_scroll_y;             /**< Amount scrolled in vertical position. */
        canvas_state   m_canvas_state;  /**< State in which the canvas is. */

        float m_min_size_x;           /**< The minimum horizontal size of the displayed diagram. */
        float m_max_size_x;           /**< The maximum horizontal size of the displayed diagram. */
        float m_min_size_y;           /**< The minimum vertical size of the displayed diagram. */
        float m_max_size_y;           /**< The maximum vertical size of the displayed diagram. */
        int   m_h_scroll_range;       /**< The current horizontal scrolling range. */
        int   m_h_scroll_pos;         /**< The current horizontal scrolling position. */
        int   m_v_scroll_range;       /**< The current vertical scrolling range. */
        int   m_v_scroll_pos;         /**< The current vertical scrolling position. */

        grape_frame*    m_main_frame;              /**< A pointer to the main grape frame. */
        diagram*        m_diagram;                 /**< A pointer to the diagram that is currently being viewed. */
        arr_visual_object_ptr m_visual_objects; /**< The current list of pointers to visual objects that are drawn on the canvas. */

        coordinate      m_lmouse_down_coordinate;    /**< The last coordinate at which the left mouse button was pressed down. */
        visual_object*  m_touched_visual_object; /**< The object that has been clicked last. */
        int             m_touched_visual_object_id; /**< The ID of the object that has been clicked. */
        grape_direction m_touched_click_location; /**< When a border of a visual is clicked, it is stored which border was clicked. This is needed for grape_event_drag to determine to which direction to resize. */
        bool            m_dragging;              /**< Whether the touched object was dragged. */
        bool            m_mousedown;             /**< Shall contain @c true when the left mouse button is pressed. */

        coordinate m_visibility_frame_coordinate; /**< The center coordinate of the visibility frame. */
        float m_visibility_frame_width;   /**< The width of the visibility frame of an architecture diagram. */
        float m_visibility_frame_height;  /**< The height of the visibility frame of an architecture diagram. */

        DECLARE_EVENT_TABLE();        /**< The event table of this canvas. */


        /**
         * OpenGL Initialization function.
         * Initializes OpenGL and this canvas.
         */
        void init_gl(void);

        /**
         * Drawing function.
         * Draws the visual objects on the canvas.
         */
        void draw_visual_objects(void);

        /**
         * Canvas painting function.
         * Paints the canvas.
         * @param p_dc Current device context.
         */
        void paint(wxDC &p_dc);

        /**
         * Scrollbar update function.
         * Updates the dimensions of the scrollbars. Called when a window is resized.
         * @pre The window is resized.
         * @post Scrollbar dimensions are updated according to the new window size.
         */
        void update_scrollbars(void);

        /**
         * Scrollbar info storage function.
         * Stores the current scrollbar ranges, along with the position of the thumbs.
         * @pre True.
         * @post m_h_scroll_range, m_v_scroll_range, m_h_scroll_pos and m_v_scroll_pos store the scrolling ranges and thumb positions.
         */
        void store_scroll_info(void);

        /**
         * Canvas coordinate retrieval function.
         * Takes the specified logical (window) coordinates and transforms them into glcanvas coordinates.
         * @param p_event_x The x-coordinate of the generated mouse event.
         * @param p_event_y The y-coordinate of the generated mouse event.
         * @return Returns the library / glcanvas coordinate.
         */
        coordinate get_canvas_coordinate( int p_event_x, int p_event_y );

      public:
        /**
         * Initialization constructor.
         * Sets parent and initializes grape_glcanvas with specified arguments.
         * @param p_parent Pointer to parent window.
         * @param p_args List of wx OpenGL attributes.
         * @param p_main_frame The main frame containing this canvas.
         */
        grape_glcanvas(wxWindow *p_parent, int* p_args, grape_frame* p_main_frame);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_glcanvas(void);

		/**
		 * Reset function
		 * Resets the scrollbars
		 */
		void reset();
        /**
         * Drawing function.
         * (Re-)draws the canvas.
         */
        void draw(void);

        /**
         * Canvas painting function.
         * Paints the translate canvas.
         * @param translation_coordinate translation coordinates
         */
        void paint_coordinate( coordinate translation_coordinate );

        /**
         * Maximum size setting function
         * Sets the maximum size of the canvas.
         * @param p_x The x coordinate of the maximum size;
         * @param p_y The y coordinate of the maximum size;
         */
        void set_max_size( float p_x, float p_y);

        /**
         * Maximum size retrieval function
         * Retrieves the x coordinate of the maximum size of the canvas.
         * @return The x coordinate of the maximum size;
         */
        float get_max_x( void );

        /**
         * Maximum size retrieval function
         * Retrieves the y coordinate of the maximum size of the canvas.
         * @return The y coordinate of the maximum size;
         */
        float get_max_y( void );

        /**
         * Paint event handler
         * Processes the canvas paint event.
         * @param p_event The generated event.
         */
        void event_paint(wxPaintEvent &p_event);

        /**
         * Erase background event handler.
         * Processes the erase background event.
         * @param p_event The generated event.
         */
        void event_erase_background(wxEraseEvent &p_event);

        /**
         * On idle event handler.
         * Processes the on idle event. Canvas is not updated when idle, only on paint events.
         * @param p_event The generated event.
         */
        void event_idle(wxIdleEvent &p_event);

        /**
         * Resize event handler.
         * Processes the size change events.
         * @param p_event The generated event.
         */
        void event_size(wxSizeEvent &p_event);

        /**
         * Scroll lineup event handler.
         * Processes the scroll lineup events.
         * @param p_event The generated event.
         */
        void event_scroll_lineup(wxScrollWinEvent &p_event);

        /**
         * Scroll linedown event handler.
         * Processes the scroll linedown events.
         * @param p_event The generated event.
         */
        void event_scroll_linedown(wxScrollWinEvent &p_event);

        /**
         * Scroll thumbtrack event handler.
         * Processes the thumb track events.
         * @param p_event The generated event.
         */
        void event_scroll_thumbtrack(wxScrollWinEvent &p_event);
        void event_scroll_thumbrelease(wxScrollWinEvent &p_event);
        /**
         * Scroll pageup event handler.
         * Processes the scroll pageup events.
         * @param p_event The generated event.
         */
        void event_scroll_pageup(wxScrollWinEvent &p_event);

        /**
         * Scroll pagedown event handler.
         * Processes the scroll pagedown events.
         * @param p_event The generated event.
         */
        void event_scroll_pagedown(wxScrollWinEvent &p_event);

        /**
         * Mouse motion event handler.
         * Processes the mouse motion events.
         * @param p_event The generated event.
         */
        void event_mouse_move(wxMouseEvent &p_event);

        /**
         * Left mouse down event handler.
         * Processes the left mouse down events.
         * @param p_event The generated event.
         */
        void event_lmouse_down(wxMouseEvent &p_event);

        /**
         * Left mouse up event handler.
         * Processes the left mouse up events.
         * @param p_event The generated event.
         */
        void event_lmouse_up(wxMouseEvent &p_event);

        /**
         * Left double click event handler.
         * @param p_event The generated event.
         */
        void event_lmouse_doubleclick( wxMouseEvent &p_event );

        /**
         * Get canvas status.
         * @return Returns the status of the canvas.
         */
        canvas_state get_canvas_state( void ) const;

        /**
         * Set canvas status.
         * @param p_canvas_state The new canvas status.
         */
        void set_canvas_state( const canvas_state &p_canvas_state );

        /**
         * Visual object count function.
         * @return Returns the number of visual objects on the canvas.
         */
        unsigned int count_visual_object( void );

        /**
         * Visual object retrieval function.
         * @return Returns the visual object at the specified index in the list of visual objects.
         */
        visual_object* get_visual_object( int p_i );

        /**
         * Finds the visual object representing an object.
         * @param p_obj The object to find the corresponding visual object for.
         * @return A pointer to the visual object. Or 0 if the visual object couldn't be found.
         */
        visual_object *get_visual_object( object *p_obj );

        /**
         * Visual object retrieval function.
         * @return Returns a pointer to the selectable visual object at the specified coordinate on the canvas, if such an object exists at that coordinate.
         * Note that a diagram is not a selectable object.
         */
        visual_object* get_selectable_visual_object( coordinate &p_coord );

        /**
         * Visual object addition function.
         * Adds the visual object to the list of objects on the canvas.
         */
        void add_visual_object( visual_object* p_obj );

        /**
         * Visual object clear function.
         * Removes all visual objects from the list of objects on the canvas.
         */
        void clear_visual_objects( void );

        /**
         * Selected visual objects count function.
         * @return Returns the number of selected (visual) objects.
         */
        int count_selected_objects( void );

        /**
         * Visual object reload function.
         * Removes all visual objects from the canvas and reloads them anew.
         */
        void reload_visual_objects( void );

        /**
         * Sets the diagram used by the canvas.
         */
        void set_diagram( diagram* p_diagram );

        /**
         * Diagram retrieval function.
         * @return Returns a pointer to the currently viewed diagram.
         */
        diagram* get_diagram( void );

        /**
         * Image retrieval function.
         * Returns an image containing what is on the canvas right now.
         */
        wxImage get_image( void );

        /**
         * Visibility frame test function.
         * @param p_coord The coordinate that is to be tested.
         * @return Returns whether the coordinate is inside the visibility frame.
         */
        bool is_inside_visibility_frame( coordinate &p_coord );

        /**
         * Font renderer retrieval function.
         * Returns the font renderer.
         */
        static font_renderer* get_font_renderer( void );


    };

  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_GLCANVAS_H
