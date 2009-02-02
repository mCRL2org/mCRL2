// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualvisibilityframe.h
//
// Declares the visualvisibilty_frame class.

#ifndef VISUALS_visualvisibilty_frame_H
#define VISUALS_visualvisibilty_frame_H

#include "../../libgrape/architecturediagram.h"
#include "../grape_glcanvas.h"
#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents a visualvisibilty_frame.
     */
    class visualvisibility_frame : public visual_object
    {
      private:
        coordinate m_coordinate;
        float m_width;
        float m_height;
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_visibility_frame.
         * @param p_coordinate A pointer to the coordinate of the bottom left corner
         * @param p_width The width of the frame
         * @param p_height The height of the frame
         */
        visualvisibility_frame( coordinate& p_coordinate, float p_width, float p_height );

        /**
         * Copy constructor.
         * Creates a new visual_visibility_frame based on an existing visual_visibility_frame
         * @param p_visibility_frame The visual_visibility_frame to be copied.
         */
        visualvisibility_frame( const visualvisibility_frame &p_visibility_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualvisibility_frame( void );

        /**
         * Draw visibility frame
         */
        void draw( void );

        /**
         * Determine whether the specified coordinate is inside the drawn figure.
         */
        bool is_inside( libgrape::coordinate &p_coord );

        /**
         * Coordinate retrieval function
         * returns the coordinate.
         */
        coordinate get_coordinate( void );

        /**
         * Width retrieval function
         * returns the width.
         */
        float get_width( void );

        /**
         * Height retrieval function
         * returns the height-coordinate.
         */
        float get_height( void );

        /** Is a noop for visibility frame. */
        grape_direction is_on_border( libgrape::coordinate & );

        /**
         * @return @c NONE because visibility frame is not an object.
         */
        object_type get_type( void ) const { return NONE; }
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_visualvisibilty_frame_H
