// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_object.h
//
// Declares GraPE events for objects

#ifndef GRAPE_EVENT_OBJECT_H
#define GRAPE_EVENT_OBJECT_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the move object event.
     */
    class grape_event_move : public grape_event_base
    {
      private:
        unsigned int        m_obj_id;     /**< The id of the object to be moved. */
        object_type m_obj_type;   /**< The type of the object. */
        coordinate  m_old_coord; /**< The original coordinate of the object. */
        coordinate  m_new_coord;  /**< The new coordinate of the object. */
        int         m_flag;  /**< Flag params. */
        unsigned int        m_diagram_id;    /**< The identifier of the diagram the object is in. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_obj A pointer to the clicked object.
         * @param p_old_coord The original coordinate of the object.
         * @param p_new_coord The new coordinate of the object.
         * @param p_undo Should be @c true (default) if the action should be undoable.
         * @param p_flag for passing flags.
         */
        grape_event_move( grape_frame *p_main_frame, object* p_obj, coordinate &p_old_coord, coordinate &p_new_coord, bool p_undo = true, int p_flag = -1 );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_move( void );

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
     * \short Represents the resize event.
     */
    class grape_event_resize: public grape_event_base
    {
      private:
        unsigned int    m_obj_id;         /**< The id of the object to be resized. */
        object_type m_obj_type;   /**< The type of the object. */
        grape_geometry m_old_geo; /**< The old geometry of the object. */
        grape_geometry m_new_geo; /**< The new geometry of the object. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_obj The object which is being editted.
         * @param p_old_geo The old geometry of the object.
         * @param p_new_geo The new geometry of the object.
         * @param p_undo A flag indicating whether the operation can be undone using the undo/redo functionality.
         */
        grape_event_resize( grape_frame *p_main_frame, object* p_obj, const grape_geometry &p_old_geo, const grape_geometry &p_new_geo, bool p_undo );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_resize( void );

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


    class grape_event_properties : public grape_event_base
    {
      private:
        visual_object *m_vis_obj; /**< A pointer to the selected visual object, if applicable. */
        object        *m_obj_ptr; /**< A pointer to the selected object, if applicable. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_properties( grape_frame *p_main_frame );

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_vis_obj The visual object on which a click happened.
         */
        grape_event_properties( grape_frame *p_main_frame, visual_object* p_vis_obj );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_properties( void );

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
#endif // GRAPE_EVENT_OBJECT_H
