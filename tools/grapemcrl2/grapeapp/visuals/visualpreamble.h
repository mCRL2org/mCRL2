// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualpreamble.h
//
// Declares the visualpreamble class.

#ifndef VISUALS_VISUALPREAMBLE_H
#define VISUALS_VISUALPREAMBLE_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::preamble;

  namespace grapeapp
  {
    /**
     * \short Represents a visualpreamble.
     */
    class visualpreamble : public visual_object
    {
      private:
        preamble* m_preamble;
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_preamble.
         * @pre: p_preamble should be pointed to an existing preamble
         */
        visualpreamble( preamble* p_preamble = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_preamble based on an existing visual_preamble
         * @param p_preamble The visual_preamble to be copied.
         */
        visualpreamble( const visualpreamble &p_preamble );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualpreamble( void );

        /**
         * Draw preamble
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
         * Selected object retrieval function.
         * @return Returns @c false because a preamble cannot be selected, because it's not an object.
         */
        bool get_selected( void ) const;

        /**
         * @return @c NONE because preamble is not an object.
         */
        object_type get_type( void ) const { return NONE; }

        /**
         * @return A pointer to the preamble which this visual object represents.
         */
        preamble* get_preamble( void );
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_VISUALPREAMBLE_H
