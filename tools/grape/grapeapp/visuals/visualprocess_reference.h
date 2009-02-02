// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualprocess_reference.h
//
// Declares the visualprocess_reference class.

#ifndef VISUALS_VISUALPROCESS_REFERENCE_H
#define VISUALS_VISUALPROCESS_REFERENCE_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::process_reference;

  namespace grapeapp
  {
    /**
     * \short Represents a visualprocess_reference.
     */
    class visualprocess_reference : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_process_reference.
         * @pre: p_process_reference should be pointed to an existing process reference
         */
        visualprocess_reference( process_reference* p_process_reference = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_process_reference based on an existing visual_process_reference
         * @param p_process_reference The visual_process_reference to be copied.
         */
        visualprocess_reference( const visualprocess_reference &p_process_reference );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualprocess_reference( void );

        /**
         * Draw process reference
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
         * Test if coordinate is on the border of the object, if not a nearby coordinate that is on the border is returned.
         * @param p_coord test coordinate 
         * @return Returns a coordinate on the border of the visualobject
         */
        libgrape::coordinate move_to_border( libgrape::coordinate &p_coord );
    };
  } // namespace grapeapp
} // namespace grape

#endif // VISUALS_VISUALPROCESS_REFERENCE_H
