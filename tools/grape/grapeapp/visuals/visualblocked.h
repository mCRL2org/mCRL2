// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualblocked.h
//
// Declares the visualblocked class.

#ifndef VISUALS_VISUALBLOCKED_H
#define VISUALS_VISUALBLOCKED_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::blocked;

  namespace grapeapp
  {
    /**
     * \short Represents a visualblocked.
     */
    class visualblocked : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_blocked.
         * @pre: p_blocked should be pointed to an existing blocked
         */
        visualblocked( blocked* p_blocked = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_blocked based on an existing visual_blocked
         * @param p_blocked The visual_blocked to be copied.
         */
        visualblocked( const visualblocked &p_blocked );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualblocked( void );

        /**
         * Draw blocked
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

#endif // VISUALS_VISUALBLOCKED_H
