// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file messaging.h
/// \brief Controlling and printing of messages.  
///
/// Global precondition: the ATerm library has been initialised

#ifndef __MCRL2_MESSAGING_H__
#define __MCRL2_MESSAGING_H__

#include <stdarg.h>
#include <assert.h>
#include <aterm2.h>

#include <workarounds.h>

namespace mcrl2 {
  /// \brief The main namespace for the Core library.
  namespace core {

  
      //Message printing options
      //------------------------

      /// \brief Printing of warnings, verbose information and extended debugging
      ///      information during program execution is disabled.
      void gsSetQuietMsg(void);
  
      /// \brief Printing of warnings during program execution is enabled. Printing of
      ///      verbose information and extended debugging information is disabled.
      void gsSetNormalMsg(void);
  
      /// \brief Printing of warnings and verbose information during program execution
      ///      is enabled. Printing of extended debugging information is disabled.
      void gsSetVerboseMsg(void);
  
      /// \brief Printing of warnings, verbose information and extended debugging
      ///      information during program execution is enabled.
      void gsSetDebugMsg(void);
  
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
      extern void gsMessage(const char *Format, ...);
      /// \brief Function for printing error messages.
      extern void gsErrorMsg(const char *Format, ...);
      /// \brief Function for printing verbose messages.
      extern void gsVerboseMsg(const char *Format, ...);
      /// \brief Function for printing warning messages.
      extern void gsWarningMsg(const char *Format, ...);
      /// \brief Function for printing debug messages.
      extern void gsDebugMsg(const char *Format, ...);
  
      /// \brief Replaces message_handler by the function pointer passed as argument.
      void gsSetCustomMessageHandler(void (*)(messageType, const char*));

#  ifdef __func__
    void gsDebugMsgFunc(const char *FuncName, const char *Format, ...);

#   define gsDebugMsg(...)        gsDebugMsgFunc(__func__, __VA_ARGS__)
#  endif

  }
}

#endif
