// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualnonterminating_transition.h
//
// Declares the visualnonterminating_transition class.

#ifndef VISUALS_NONTERMINATING_TRANSITION_H
#define VISUALS_NONTERMINATING_TRANSITION_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::nonterminating_transition;

  namespace grapeapp
  {
    /**
     * \short Represents a visualnonterminating_transition.
     */
    class visualnonterminating_transition : public visual_object
    {
      private:
        coordinate m_arrow_base; /**< The begincoordinate of the arrow with respect to the coordinate of the transition. */
        coordinate m_arrow_head; /**< The coordinate of the arrowhead with respect to the coordinate of the transition. */
        bool       m_same_begin_end; /**< A flag indicating whether the transition has the same beginstate as endstate */
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_nonterminating_transition.
         * @pre: p_nonterminating_transition should be pointed to an existing nonterminating transition
         */
        visualnonterminating_transition( nonterminating_transition* p_nonterminating_transition = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_nonterminating_transition based on an existing visual_nonterminating_transition
         * @param p_nonterminating_transition The visual_nonterminating_transition to be copied.
         */
        visualnonterminating_transition( const visualnonterminating_transition &p_nonterminating_transition );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualnonterminating_transition( void );

        /**
         * Draw nonterminating transition
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

        /**
         * Test if coordinate is nearest to the head of the arrow or (if false) nearest the other end of the arrow.
         * @param p_coord The to be tested coordinate.
         * @return Returns whether the specified coordinate is nearest to the head of the arrow (true) or nearest to the other end of the arrow (false).
         */
        bool is_nearest_head( libgrape::coordinate &p_coord );        
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_NONTERMINATING_TRANSITION_H
