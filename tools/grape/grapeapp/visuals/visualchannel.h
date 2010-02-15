// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualchannel.h
//
// Declares the visualchannel class.

#ifndef VISUALS_VISUALCHANNEL_H
#define VISUALS_VISUALCHANNEL_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::channel;

  namespace grapeapp
  {
    /**
     * \short Represents a visualchannel.
     */
    class visualchannel : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_channel.
         * @pre: p_channel should be pointed to an existing channel
         */
        visualchannel( channel* p_channel = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_channel based on an existing visual_channel
         * @param p_channel The visual_channel to be copied.
         */
        visualchannel( const visualchannel &p_channel );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualchannel( void );

        /**
         * Draw channel
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

#endif // VISUALS_VISUALCHANNEL_H
