// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file outputpanel.h
//
// Implements the output class used to display log messages.

#include "wx.hpp" // precompiled headers

#include <memory>
#include <streambuf>
#include <iostream>

#include "outputpanel.h"
#include "mcrl2/utilities/logger.h"
using namespace mcrl2::log;

class message_relay;

static void relay_message(const mcrl2::log::function_pointer_output<formatter>::message_t t, const char* data);

class text_control_buf : public std::streambuf
{

  private:
    wxTextCtrl&    m_control;
  public:

    text_control_buf(wxTextCtrl& control) : std::streambuf(), m_control(control)
    {
    }

    int overflow(int c)
    {
      m_control.AppendText(wxString(static_cast< wxChar >(c)));

      return 1;
    }

    std::streamsize xsputn(const char* s, std::streamsize n)
    {
      m_control.AppendText(wxString(s, wxConvLocal, n));

      pbump(n);

      return n;
    }
};

class message_relay;

std::auto_ptr < message_relay > communicator;

class message_relay
{
    friend void relay_message(const mcrl2::log::function_pointer_output<formatter>::message_t, const char* data);

  private:

    wxTextCtrl&      m_control;
    std::streambuf*  m_error_stream;
    mcrl2::log::function_pointer_output<formatter> m_output_policy;

  private:

    static bool initialise_once(wxTextCtrl& control)
    {
      communicator.reset(new message_relay(control));

      return true;
    }

    message_relay(wxTextCtrl& control) : m_control(control)
    {
      m_error_stream = std::cerr.rdbuf(new text_control_buf(m_control));

      m_output_policy = mcrl2::log::function_pointer_output<formatter>(relay_message);
      mcrl2::log::mcrl2_logger::register_output_policy(m_output_policy);
    }

    void message(const char* data)
    {
      m_control.AppendText(wxString(data, wxConvLocal));
    }

  public:

    static bool initialise(wxTextCtrl& control)
    {
      static bool initialised = initialise_once(control);

      return initialised;
    }

    ~message_relay()
    {
      mcrl2::log::mcrl2_logger::register_output_policy(m_output_policy);

      delete std::cerr.rdbuf(m_error_stream);
    }
};

static void relay_message(const mcrl2::log::function_pointer_output<formatter>::message_t /*t*/, const char* data)
{
  communicator->message(data);
}

outputpanel::outputpanel(wxWindow* p_parent)
  : wxTextCtrl(p_parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP)
{
  message_relay::initialise(*this);
}

//std::string outputpanel::PrintTime()
//{
//  wxDateTime now = wxDateTime::Now();
//  return std::string(now.FormatTime().mb_str()) + " ** ";
//};


