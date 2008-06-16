// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_logpanel.cpp
//
// Implements the logpanel class used to display log messages.


#include "grape_logpanel.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>

using namespace grape::grapeapp;

grape_logpanel::grape_logpanel(void) : wxTextCtrl()
{
}

grape_logpanel::grape_logpanel(wxWindow *p_parent)
: wxTextCtrl(p_parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY)
{
#ifndef __APPLE__
// Currently the std_iostreams are not included in wxwidgets on apple by default.
// If this is the case, this ifndef can be removed.
  m_cerr_catcher = new wxStreamToTextRedirector( this, &std::cerr );
#endif
}

grape_logpanel::~grape_logpanel(void)
{
#ifndef __APPLE__
// Currently the std_iostreams are not included in wxwidgets on apple by default.
// If this is the case, this ifndef can be removed.
  delete m_cerr_catcher;
#endif
}

void grape_logpanel::enable_catch_cout(void)
{
  // TODO
}

void grape_logpanel::disable_catch_cout(void)
{
  // TODO
}
