// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualreference_state.h
//
// Declares the visualreference_state class.

#ifndef VISUALS_VISUALREFERENCE_STATE_H
#define VISUALS_VISUALREFERENCE_STATE_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::reference_state;

  namespace grapeapp
  {
    /**
     * \short Represents a visualreference_state.
     */
    class visualreference_state : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_reference_state.
         * @pre: p_reference_state should be pointed to an existing reference state
         */
        visualreference_state( reference_state* p_reference_state = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_reference_state based on an existing visual_reference_state
         * @param p_reference_state The visual_reference_state to be copied.
         */
        visualreference_state( const visualreference_state &p_reference_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualreference_state( void );

        /**
         * Draw reference state
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

#endif // VISUALS_VISUALREFERENCE_STATE_H
