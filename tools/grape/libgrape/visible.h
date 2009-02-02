// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visible.h
//
// Declares the visible class.

#ifndef LIBGRAPE_VISIBLE_H
#define LIBGRAPE_VISIBLE_H

#include "connectionproperty.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a visible connection property.
     */
    class visible : public connection_property
    {
      protected:
        wxString  m_name; /**< name of the visible on visibility frame. */

      public:
        /**
         * Default constructor.
         * Initializes visible.
         */
        visible(void);

        /**
         * Copy constructor.
         * Creates a new visibile connection property based on an existing one.
         * @param p_visible The visible property to copy.
         */
        visible( const visible &p_visible );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~visible(void);

        /**
         * Name retrieval function.
         * @return The name of the visible property.
         */
        wxString get_name( void ) const;

        /**
         * Name assignment function.
         * @pre The visible is selected.
         * @param p_name The new name of the visible
         */
        void set_name( const wxString &p_name );

    };

    /**
     * Array of visible.
     */
    WX_DECLARE_OBJARRAY( visible, arr_visible );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_VISIBLE_H
