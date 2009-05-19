// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualcomment.h
//
// Declares the visualcomment class.

#ifndef VISUALS_VISUALCOMMENT_H
#define VISUALS_VISUALCOMMENT_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::comment;

  namespace grapeapp
  {
    /**
     * \short Represents a visualcomment.
     */
    class visualcomment : public visual_object
    {
      private:
        bool m_reference_selected; /**< Selected reference of the comment. */
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_comment.
         * @pre: p_comment should be pointed to an existing comment
         */
        visualcomment( comment* p_comment = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_comment based on an existing visual_comment
         * @param p_visualcomment The visual_comment to be copied.
         */
        visualcomment( const visualcomment &p_visualcomment );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualcomment( void );

        /**
         * Draw comment
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
         * Test if coordinate is inside the reference
         * @param p_coord test coordinate
         * @return Returns whether the specified coordinate is inside the reference
         */
        bool is_inside_reference( libgrape::coordinate &p_coord );
                
        /**
         * Get reference coordinate
         * @return The coordinate
         */
        coordinate get_reference_coordinate( void );
        
        /** @return Returns whether the reference of the comment is selected. */
        bool get_reference_selected();
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_VISUALCOMMENT_H
