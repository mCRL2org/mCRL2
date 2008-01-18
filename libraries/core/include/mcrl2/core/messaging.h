// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file messaging.h
/// \brief Library for controlling and printing messages.  
///
/// Global precondition: the ATerm library has been initialised

#ifndef __MCRL2_MESSAGING_H__
#define __MCRL2_MESSAGING_H__

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdarg.h>
#include <assert.h>
#include <aterm2.h>

#include <workarounds.h>

namespace mcrl2 {
  namespace core {

  
      //Message printing options
      //------------------------

      ///\post Printing of warnings, verbose information and extended debugging
      ///      information during program execution is disabled.
      void gsSetQuietMsg(void);
  
      ///\post Printing of warnings during program execution is enabled. Printing of
      ///      verbose information and extended debugging information is disabled.
      void gsSetNormalMsg(void);
  
      ///\post Printing of warnings and verbose information during program execution
      ///      is enabled. Printing of extended debugging information is disabled.
      void gsSetVerboseMsg(void);
  
      ///\post Printing of warnings, verbose information and extended debugging
      ///      information during program execution is enabled.
      void gsSetDebugMsg(void);
  
      /// If gsQuiet is set, printing of warnings, verbose information and extended debugging
      /// information during program execution is disabled.
      extern bool gsQuiet;

      /// If gsWarning is set, printing of warnings during program execution is enabled. Printing of
      /// verbose information and extended debugging information is disabled.
      extern bool gsWarning;

      /// If gsVerbose is set, printing of warnings and verbose information during program execution
      /// is enabled. Printing of extended debugging information is disabled.
      extern bool gsVerbose;

      /// If gsDebug is set, printing of warnings, verbose information and extended debugging
      /// information during program execution is enabled.
      extern bool gsDebug;
  
      /// Type for message distinction (by purpose)
      enum messageType {gs_notice, gs_warning, gs_error};
 
      /// Function for printing regular messages  
      extern void gsMessage(const char *Format, ...);
      /// Function for printing error messages
      extern void gsErrorMsg(const char *Format, ...);
      /// Function for printing verbose messages
      extern void gsVerboseMsg(const char *Format, ...);
      /// Function for printing warning messages
      extern void gsWarningMsg(const char *Format, ...);
      /// Function for printing debug messages
      extern void gsDebugMsg(const char *Format, ...);
  
      /// Replaces message_handler by the function pointer passed as argument
      void gsSetCustomMessageHandler(void (*)(messageType, const char*));

#  ifdef __func__
    void gsDebugMsgFunc(const char *FuncName, const char *Format, ...);

#   define gsDebugMsg(...)        gsDebugMsgFunc(__func__, __VA_ARGS__)
#  endif

  }
}

#endif
