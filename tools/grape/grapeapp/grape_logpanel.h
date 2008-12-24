// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_logpanel.h
//
// Declares the logpanel class used to display log messages.

#ifndef GRAPEAPP_GRAPE_LOGPANEL_H
#define GRAPEAPP_GRAPE_LOGPANEL_H

#include <wx/wx.h>
#include <wx/textctrl.h>

namespace grape
{
  namespace grapeapp
  {

    /**
     * \short The logpanel which contains the log.
     * Due to limitations in mcrl2 core message relaying output is always sent to the first created log panel.
     */
    class grape_logpanel : public wxTextCtrl
    {
      public:
        /**
         * Parental constructor.
         * Sets parent and initializes grape_logpanel.
         * @param p_parent Pointer to parent window.
         */
        grape_logpanel(wxWindow *p_parent);
    };

  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_LOGPANEL_H
