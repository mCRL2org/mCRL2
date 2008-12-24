// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file blocked.h
//
// Declares the blocked class.

#ifndef LIBGRAPE_BLOCKED_H
#define LIBGRAPE_BLOCKED_H

#include "connectionproperty.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a blocked connection property.
     */
    class blocked : public connection_property
    {
      protected:
      public:
        /**
         * Default constructor.
         * Initializes blocked.
         */
        blocked( void );

        /**
         * Copy constructor
         * Creates a new blocked property based on an existing one.
         * @param p_blocked The Blocked property to be copied.
         */
        blocked( const blocked &p_blocked );

        /**
         * Default destructor.
         * Frees allocated memory and removes all references to the object.
         */
        ~blocked( void );
    };

    /**
     * Array of Blocked.
     */
    WX_DECLARE_OBJARRAY( blocked, arr_blocked );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_BLOCKED_H
