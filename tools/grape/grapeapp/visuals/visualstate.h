// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualstate.h
//
// Declares the visualstate class.

#ifndef VISUALS_VISUALSTATE_H
#define VISUALS_VISUALSTATE_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::state;

  namespace grapeapp
  {
    /**
     * \short Represents a visualstate.
     */
    class visualstate : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_state.
         * @pre: p_sate should be pointed to an existing state
         */
        visualstate( state* p_state = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_state based on an existing visual_state
         * @param p_state The visual_state to be copied.
         */
        visualstate( const visualstate &p_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualstate( void );

        /**
         * Draw state
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

#endif // VISUALS_VISUALSTATE_H
