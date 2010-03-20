// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualinitial_designator.h
//
// Declares the visualinitial_designator class.

#ifndef VISUALS_VISUALINITIAL_DESIGNATOR_H
#define VISUALS_VISUALINITIAL_DESIGNATOR_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::initial_designator;

  namespace grapeapp
  {
    /**
     * \short Represents a visualinitial_designator.
     */
    class visualinitial_designator : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_initial_designator.
         * @pre: p_initial_designator should be pointed to an existing initial designator
         */
        visualinitial_designator( initial_designator* p_initial_designator = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_initial_designator based on an existing visual_initial_designator
         * @param p_initial_designator The visual_initial_designator to be copied.
         */
        visualinitial_designator( const visualinitial_designator &p_initial_designator );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualinitial_designator( void );

        /**
         * Draw initial designator
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

#endif // VISUALS_VISUALINITIAL_DESIGNATOR_H
