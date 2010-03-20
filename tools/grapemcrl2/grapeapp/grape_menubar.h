// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_menubar.h
//
// Declares the menubar class used by the main frame.

#ifndef GRAPEAPP_GRAPE_MENUBAR_H
#define GRAPEAPP_GRAPE_MENUBAR_H

#include <wx/wx.h>

namespace grape
{
  namespace grapeapp
  {

    /**
     * \short Represents the menubar as used by the main frame.
     */
    class grape_menubar : public wxMenuBar
    {
      private:
        wxMenu  *m_menu_file;           /**< The File menu shown on the menubar. */
        wxMenu  *m_menu_edit;           /**< The Edit menu shown on the menubar. */
        wxMenu  *m_menu_specification;  /**< The Specification menu shown on the menubar. */
        wxMenu  *m_menu_tools;          /**< The Tools menu shown on the menubar. */
        wxMenu  *m_menu_help;           /**< The Help menu shown on the menubar. */

      public:
        /**
         * Default constructor.
         * Initializes grape_menubar.
         */
        grape_menubar(void);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_menubar(void);

        /**
         * Several menu modes.
         */
        enum grape_menu_mode
        {
          GRAPE_MENUMODE_NOSPEC = 0, /**< No specification loaded. */
          GRAPE_MENUMODE_SPEC = 1, /**< Specification loaded, but not necessarily a diagram. */
          GRAPE_MENUMODE_ARCH = 2, /**< GraPE is in architecture diagram edit mode. */
          GRAPE_MENUMODE_PROC = 4, /**< GraPE is in process diagram edit mode. */
          GRAPE_MENUMODE_DATASPEC = 8 /**< GraPE is in datatype specification mode. */
        };

        /**
         * Each menu in the menubar should have its own entry, it is used
         * as a parameter to the get_menu function to retrieve a pointer
         * to the associated menu.
         */
        enum grape_main_menu {
          GRAPE_MENU_FILE = 0,
          GRAPE_MENU_EDIT,
          GRAPE_MENU_TOOLS,
          GRAPE_MENU_SPECIFICATION,
          GRAPE_MENU_HELP
        };

        /**
         * Retrieve a pointer to a menu.
         * @param p_which Should be a value defined in the grape_main_menu enumeration.
         * @return A pointer to the requested menu.
         */
        wxMenu *get_menu( grape_main_menu p_which );

        /**
         * Enables menu items based on the specified mode.
         * Please don't use this function from anywhere than grape_frame::set_mode().
         * @param p_mode The mode, OR'ed with values from the enumeration grape_menu_status.
        */
        void set_mode( int p_mode );
    };

  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_MENUBAR_H
