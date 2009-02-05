// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualvisible.h
//
// Declares the visualvisible class.

#ifndef VISUALS_VISUALVISIBLE_H
#define VISUALS_VISUALVISIBLE_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::visible;

  namespace grapeapp
  {
    /**
     * \short Represents a visualvisible.
     */
    class visualvisible : public visual_object
    {
      private:
        coordinate m_coord;  /**< p_coord coordinate of the visibility frame */
        float      m_width;  /**< p_width width visibility frame */
        float      m_height; /**< p_hegiht height visibility frame */
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_visible
         * @pre: p_visible should be pointed to an existing visible
         * @param p_visible The visible this visualvisible is to be associated with.
         * @param p_coord coordinate of the visibility frame.
         * @param p_width width visibility frame.
         * @param p_height height visibility frame.
         */
        visualvisible( visible* p_visible, coordinate &p_coord, float &p_width, float &p_height);

        /**
         * Copy constructor.
         * Creates a new visual_visible based on an existing visual_visible
         * @param p_visible The visual_visible to be copied.
         */
        visualvisible( const visualvisible &p_visible );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualvisible( void );

        /**
         * Draw visible
         */
        void draw( void );

        /**
         * Test if coordinate is inside the object
         * @param p_coord test coordinate
         * @return Returns whether the specified coordinate is inside the drawn object.
         */
        bool is_inside( libgrape::coordinate &p_coord );

        /**
         * Test if coordinate is on the border of the object
         * @param p_coord test coordinate
         * @return Returns whether the specified coordinate on the border of a drawn object.
         */
        grape_direction is_on_border( libgrape::coordinate &p_coord );
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_VISUALVISIBLE_H
