// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualterminating_transition.h
//
// Declares the visualterminating_transition class.

#ifndef VISUALS_TERMINATING_TRANSITION_H
#define VISUALS_NTERMINATING_TRANSITION_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::terminating_transition;

  namespace grapeapp
  {
    /**
     * \short Represents a visualterminating_transition.
     */
    class visualterminating_transition : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_terminating_transition.
         * @pre: p_terminating_transition should be pointed to an existing terminating transition
         */
        visualterminating_transition( terminating_transition* p_terminating_transition = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_terminating_transition based on an existing visual_terminating_transition
         * @param p_terminating_transition The visual_terminating_transition to be copied.
         */
        visualterminating_transition( const visualterminating_transition &p_terminating_transition );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualterminating_transition( void );

        /**
         * Draw terminating transition
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

#endif // VISUALS_TERMINATING_TRANSITION_H
