// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file coordinate.h
//
// Declares the coordinate datatype.

#ifndef LIBGRAPE_COORDINATE_H
#define LIBGRAPE_COORDINATE_H

#include <wx/wx.h>
#include <wx/dynarray.h>

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a coordinate on a 2D plane.
     */
    class coordinate
    {
      public:
        float m_x; /**< x-coordinate. */
        float m_y; /**< y-coordinate. */

        /** Assigns the coordinate @p p_c to this instance of the coordinate. */
        void operator=( const coordinate &p_c );

        /** @return True if the given coordinate is the same as the object. */
        bool operator==( const coordinate &p_c );
        /** @return Return the coordinate that is the difference between the two coordinates. */
        coordinate operator-(const coordinate &p_c);
        /** @return Return the coordinate that is the sum of the two coordinates. */
        coordinate operator+(const coordinate &p_c);
    };

    /**
     * List of coordinate. Represents a list of coordinates.
     */
    WX_DECLARE_OBJARRAY( coordinate, list_of_coordinate );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_COORDINATE_H


