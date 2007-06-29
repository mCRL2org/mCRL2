// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file messaging.h

#ifndef __MCRL2_MESSAGING_H__
#define __MCRL2_MESSAGING_H__

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdarg.h>
#include <assert.h>
#include <aterm2.h>

#include <workarounds.h>

#ifdef __cplusplus
namespace mcrl2 {
  namespace utilities {
    extern "C" {
#endif

      //Global precondition: the ATerm library has been initialised
  
      //Message printing options
      //------------------------
  
      void gsSetQuietMsg(void);
      //Post: Printing of warnings, verbose information and extended debugging
      //      information during program execution is disabled.
  
      void gsSetNormalMsg(void);
      //Post: Printing of warnings during program execution is enabled. Printing of
      //      verbose information and extended debugging information is disabled.
  
      void gsSetVerboseMsg(void);
      //Post: Printing of warnings and verbose information during program execution
      //      is enabled. Printing of extended debugging information is disabled.
  
      void gsSetDebugMsg(void);
      //Post: Printing of warnings, verbose information and extended debugging
      //      information during program execution is enabled.
  
      extern bool gsQuiet;
      extern bool gsWarning;
      extern bool gsVerbose;
      extern bool gsDebug;
  
      // Type for message distinction (by purpose)
      enum messageType {gs_notice, gs_warning, gs_error};
  
      extern void gsMessage(const char *Format, ...);
      extern void gsErrorMsg(const char *Format, ...);
      extern void gsVerboseMsg(const char *Format, ...);
      extern void gsWarningMsg(const char *Format, ...);
      extern void gsDebugMsg(const char *Format, ...);
  
      // Replaces message_handler by the function pointer passed as argument
      void gsSetCustomMessageHandler(void (*)(messageType, const char*));

#  ifdef __func__
    void gsDebugMsgFunc(const char *FuncName, const char *Format, ...);

#   define gsDebugMsg(...)        gsDebugMsgFunc(__func__, __VA_ARGS__)
#  endif

#ifdef __cplusplus
    }
  }
}
#endif

#endif
