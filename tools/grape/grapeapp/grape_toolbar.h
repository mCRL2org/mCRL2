// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_toolbar.h
//
// Declares the toolbar used by the main frame.

#ifndef GRAPEAPP_GRAPE_TOOLBAR_H
#define GRAPEAPP_GRAPE_TOOLBAR_H

#include <wx/toolbar.h>
#include <wx/cmdproc.h>
#include <wx/bitmap.h>

namespace grape
{
  namespace grapeapp
  {
    class grape_frame;

    /**
     * \short The toolbar as used by the main frame.
     */
    class grape_toolbar : public wxToolBar
    {
      private:
      public:
        /**
         * Default constructor.
         * Initializes grape_toolbar.
         */
        grape_toolbar(void);

        /**
         * Initialization constructor.
         * Sets parent and initializes grape_toolbar.
         * @param p_parent Pointer to parent window.
         */
        grape_toolbar(wxWindow *p_parent);


        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_toolbar(void);

        /**
         * Several tool modes.
         */
        enum grape_tool_mode
        {
          GRAPE_TOOLMODE_NOSPEC = 0, /**< No specification loaded. */
          GRAPE_TOOLMODE_SPEC = 1, /**< Specification loaded, but not necessarily a diagram. */
          GRAPE_TOOLMODE_ARCH = 2, /**< GraPE is in architecture diagram edit mode. */
          GRAPE_TOOLMODE_PROC = 4 /**< GraPE is in process diagram edit mode. */
        };

        /**
         * Enables tool items based on the specified mode.
         * Please don't use this function from anywhere than grape_frame::set_toolbar().
         * @param p_mode The mode, OR'ed with values from the enumeration grape_tool_status.
        */
        void set_mode( int p_mode );
    };

    /**
     * \short The toolbar as used by the main frame.
     */
    class grape_arch_toolbar : public grape_toolbar
    {
      private:
      public:
        /**
         * Default constructor.
         * Initializes grape_toolbar.
         */
        grape_arch_toolbar(void);

        /**
         * Initialization constructor.
         * Sets parent and initializes grape_toolbar.
         * @param p_parent Pointer to parent window.
         */
        grape_arch_toolbar(wxWindow *p_parent);


        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_arch_toolbar(void);
    };

    /**
     * \short The toolbar as used by the main frame when a process diagram is selected
     */
    class grape_proc_toolbar : public grape_toolbar
    {
      private:
      public:
        /**
         * Default constructor.
         * Initializes grape_toolbar.
         */
        grape_proc_toolbar(void);

        /**
         * Initialization constructor.
         * Sets parent and initializes grape_toolbar.
         * @param p_parent Pointer to parent window.
         */
        grape_proc_toolbar(wxWindow *p_parent);


        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_proc_toolbar(void);
    };


  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_TOOLBAR_H
