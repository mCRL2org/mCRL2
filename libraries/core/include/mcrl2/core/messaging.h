// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/messaging.h
/// \brief Controlling and printing of messages.

#ifndef __MCRL2_CORE_MESSAGING_H__
#define __MCRL2_CORE_MESSAGING_H__

#include <cstdarg>
#include <cassert>
#include "aterm2.h"
#include "boost/current_function.hpp"

namespace mcrl2 {
  /// \brief The main namespace for the Core library.
  namespace core {

      //Message printing options
      //------------------------

      /// \brief Printing of warnings, verbose information and extended debugging
      ///      information during program execution is disabled.
      void gsSetQuietMsg();

      /// \brief Printing of warnings during program execution is enabled. Printing of
      ///      verbose information and extended debugging information is disabled.
      void gsSetNormalMsg();

      /// \brief Printing of warnings and verbose information during program execution
      ///      is enabled. Printing of extended debugging information is disabled.
      void gsSetVerboseMsg();

      /// \brief Printing of warnings, verbose information and extended debugging
      ///      information during program execution is enabled.
      void gsSetDebugMsg();

      /// \brief If gsQuiet is set, printing of warnings, verbose information and extended debugging
      /// information during program execution is disabled.
      extern bool gsQuiet;

      /// \brief If gsWarning is set, printing of warnings during program execution is enabled. Printing of
      /// verbose information and extended debugging information is disabled.
      extern bool gsWarning;

      /// \brief If gsVerbose is set, printing of warnings and verbose information during program execution
      /// is enabled. Printing of extended debugging information is disabled.
      extern bool gsVerbose;

      /// \brief If gsDebug is set, printing of warnings, verbose information and extended debugging
      /// information during program execution is enabled.
      extern bool gsDebug;

      /// \brief Type for message distinction (by purpose).
      enum messageType {gs_notice, gs_warning, gs_error};

      /// \brief Function for printing regular messages  .
      void gsMessage(const char *Format, ...);
      /// \brief Function for printing error messages.
      void gsErrorMsg(const char *Format, ...);
      /// \brief Function for printing verbose messages.
      void gsVerboseMsg(const char *Format, ...);
      /// \brief Function for printing warning messages.
      void gsWarningMsg(const char *Format, ...);
      /// \brief Function for printing debug messages.
      void gsDebugMsg(const char *Format, ...);
      /// \brief Function for printing debug messages.
      void gsDebugMsgFunc(const char *FuncName, const char *Format, ...);

      /// \brief Function for printing debug messages.
#define gsDebugMsg(...)        gsDebugMsgFunc(BOOST_CURRENT_FUNCTION, __VA_ARGS__)

      /// \brief Replaces message_handler by the function pointer passed as argument.
      void gsSetCustomMessageHandler(void (*)(messageType, const char*));
  }
}

#endif
