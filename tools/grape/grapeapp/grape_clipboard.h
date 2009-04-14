// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_clipboard.h
//
// Declares the clipboard of the application.

#ifndef GRAPEAPP_GRAPE_CLIPBOARD_H
#define GRAPEAPP_GRAPE_CLIPBOARD_H

#include "terminatingtransition.h"
#include "state.h"
#include "referencestate.h"
#include "architecturereference.h"
#include "channelcommunication.h"

namespace grape
{
  namespace grapeapp
  {

    class grape_frame;

    /**
     * \short Represents the main frame of the application.
     */
    class grape_clipboard
    {
      private:
        grape_frame       *m_main_frame;       /**< A pointer to the main frame. */
        libgrape::arr_object_ptr     m_objects;          /**< The list of objects put on the clipboard. */
      protected:
      public:
        /**
         * Initialization constructor.
         * Initializes the clipboard.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_clipboard( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_clipboard( void );

        /**
         * Function to add an object to the clipboard
         * @param p_objects The objects to be added to the clipboard
         */
        void add( libgrape::arr_object_ptr p_objects );

        /**
         * Function to get an object from the clipboard
         * @return The object added to the clipboard
         */
        libgrape::arr_object_ptr get( void );
    };
  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_CLIPBOARD_H
