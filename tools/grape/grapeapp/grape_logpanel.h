// Author(s): VitaminB100
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
#include <wx/wfstream.h>
#include <iostream>

namespace grape
{
  namespace grapeapp
  {

    /**
     * \short The logpanel which contains the log.
     * Once created, the log panel captures the cerr output and shows it instead of sending it to standard error. This is because the mCRL2 libraries report errors on stderr.
     */
    class grape_logpanel : public wxTextCtrl
    {
      private:
        /**
         * Default constructor.
         * Initializes grape_logpanel.
         */
        grape_logpanel(void);

        wxStreamToTextRedirector *m_cerr_catcher; /**Provides functionality to reroute cerr to this control.*/

      public:
        /**
         * Parental constructor.
         * Sets parent and initializes grape_logpanel.
         * @param p_parent Pointer to parent window.
         */
        grape_logpanel(wxWindow *p_parent);

        /**
         * Default constructor.
         * Frees allocated memory.
         */
        ~grape_logpanel(void);

        /**
         * Cout catching enabling function.
         * Enables catching cout to this logpanel.
         */
        void enable_catch_cout(void);

        /**
         * Cout cathching disabling function.
         * Disables catching cout to this logpanel.
         */
        void disable_catch_cout(void);
    };

  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_LOGPANEL_H
