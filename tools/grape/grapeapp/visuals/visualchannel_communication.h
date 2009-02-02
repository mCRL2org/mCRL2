// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualchannel_communication.h
//
// Declares the visualchannel_communication class.

#ifndef VISUALS_VISUALCHANNEL_COMMUNICATION_H
#define VISUALS_VISUALCHANNEL_COMMUNICATION_H

#include "visualobject.h"

using namespace grape::libgrape;

namespace grape
{
  using libgrape::channel_communication;

  namespace grapeapp
  {
    /**
     * \short Represents a visualchannel_communication.
     */
    class visualchannel_communication : public visual_object
    {
      private:
      protected:
      public:

        /**
         * Default constructor.
         * Initializes visual_channel_communication.
         * @pre: p_channel_communication should be pointed to an existing channel communication
         */
        visualchannel_communication( channel_communication* p_channel_communication = 0 );

        /**
         * Copy constructor.
         * Creates a new visual_channel_communication based on an existing visual_channel_communication
         * @param p_channel_communication The visual_channel_communication to be copied.
         */
        visualchannel_communication( const visualchannel_communication &p_channel_communication );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~visualchannel_communication( void );

        /**
         * Draw channel communication
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

#endif // VISUALS_VISUALCHANNEL_COMMUNICATION_H
