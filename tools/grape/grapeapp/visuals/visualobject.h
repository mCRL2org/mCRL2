// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualobject.h
//
// Declares the visualobject class.

#ifndef VISUALS_VISUALOBJECT_H
#define VISUALS_VISUALOBJECT_H

#include "geometric.h"

#include "object.h"

const float g_min_object_width = 0.2f;
const float g_min_object_height = 0.2f;

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents a visualobject.
     */
    class visual_object
    {
      private:
      protected:
        object *m_object;
      public:

        /**
         * Default constructor.
         * Initializes visual_object.
         * @pre: p_channel should be pointed to an existing channel
         */
        visual_object( void ) {
          m_object = NULL;
        };

        visual_object( const visual_object &p_vis_object ) { m_object = p_vis_object.m_object; }

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        virtual ~visual_object( void ) {};

        /**
         * Virtual draw function.
         * Inherited functions draw the object to the canvas.
         */
        virtual void draw( void ) = 0;

        /**
         * Virtual function.
         * Inherited functions determine whether the specified coordinate is inside the drawn figure.
         */
        virtual bool is_inside( libgrape::coordinate &p_coord ) = 0;

        /**
         * Virtual function.
         * Inherited functions return a pointer to the object the visible is associated with.
         */
        virtual object* get_selectable_object( void ) { return m_object; }

        /**
         * Virtual function.
         * Inherited functions return whether the associated object is selected (if selectable, otherwise false)
         */
        virtual bool get_selected( void ) const { return m_object->get_selected(); };

        /**
         * @return Returns the type of the object which is represented by this visual object.
         */
        virtual object_type get_type( void ) const { return m_object->get_type(); }

        /**
         * Abstract function to detect if a coordinate is on the border of the visual object.
         * @param p_coord This coordinate is checked if its on a border.
         * @return The direction on which border the coordinate was on. Or @c GRAPE_DIR_NONE if no border was clicked.
         */
        virtual grape_direction is_on_border( libgrape::coordinate &p_coord ) = 0;

        static float get_width_hint( const wxString &p_string ) { return grapeapp::get_width_hint( p_string ); }
    };

    /**
     * Array of visual_object.
     */
    WX_DEFINE_ARRAY_PTR( visual_object *, arr_visual_object_ptr );
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_VISUALOBJECT_H
