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
  m_cerr_catcher = new wxStreamToTextRedirector( this, &std::cerr );
}

grape_logpanel::~grape_logpanel(void)
{
  delete m_cerr_catcher;
}

void grape_logpanel::enable_catch_cout(void)
{
  // TODO
}

void grape_logpanel::disable_catch_cout(void)
{
  // TODO
}
