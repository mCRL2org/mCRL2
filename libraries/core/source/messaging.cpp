// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file messaging.cpp

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/print.h"

#include "workarounds.h"

namespace mcrl2 {
  namespace core {

    //Message printing options
    //------------------------
    bool gsQuiet   = false;//indicates if quiet mode is enabled
    bool gsWarning = true; //indicates if warning messages should be printed
    bool gsVerbose = false;//indicates if verbose messages should be printed
    bool gsDebug   = false;//indicates if debug messages should be printed

    void gsSetQuietMsg(void)
    {
      gsQuiet   = true;
      gsWarning = false;
      gsVerbose = false;
      gsDebug   = false;
    }

    void gsSetNormalMsg(void)
    {
      gsQuiet   = false;
      gsWarning = true;
      gsVerbose = false;
      gsDebug   = false;
    }

    void gsSetVerboseMsg(void)
    {
      gsQuiet   = false;
      gsWarning = true;
      gsVerbose = true;
      gsDebug   = false;
    }

    void gsSetDebugMsg(void)
    {
      gsQuiet   = false;
      gsWarning = true;
      gsVerbose = true;
      gsDebug   = true;
    }

    // Function pointer for a custom message printing routine
    void (*custom_message_handler)(messageType, const char*) = 0;

    // Sets custom_message_handler a custom message printing routine
    void gsSetCustomMessageHandler(void (*h)(messageType, const char*)) {
      custom_message_handler = h;
    }

    // Helper function (wrapper around gsvfprintf) for printing to string
    static void handler_wrapper(messageType t, const char* Format, va_list args) {

      FILE* stream = tmpfile();

      assert(stream);

      gsvfprintf(stream, Format, args);

      size_t n = ftell(stream);

      fflush(stream);
      rewind(stream);

      char* output  = (char*) malloc((n + 1) * sizeof(char));
      char* current = output;

      while (0 < n--) {
        *current = (char) fgetc(stream);

        ++current;
      }

      *current = '\0';

      fclose(stream);

      custom_message_handler(t, output);

      free(output);
    }

    void gsMessage(const char *Format, ...)
    //Post: Format is printed to stderr where the remaining parameters are used as
    //      gsprintf arguments to Format.
    //
    //Note that this function is intended for printing explicitly requested
    //information (i.e. information that is not a error, warning or verbose message
    //nor part of the output going to stdout).
    //
    //Also note that the message handler does not receive these messages; they are
    //part of the output.
    {
      if ( !gsQuiet )
      {
        va_list Args;
        va_start(Args, Format);

        if (custom_message_handler) {
          handler_wrapper(gs_notice, Format, Args);
        }
        else {
          gsvfprintf(stderr, Format, Args);
        }

        va_end(Args);
      }
    }

    void gsErrorMsg(const char *Format, ...)
    //Post: "error: " is printed to stderr followed by Format, where the remaining
    //      parameters are used as gsprintf arguments to Format.
    {
      va_list Args;
      va_start(Args, Format);

      if (custom_message_handler) {
        handler_wrapper(gs_error, Format, Args);
      }
      else {
        fprintf(stderr, "error: ");
        gsvfprintf(stderr, Format, Args);
      }

      va_end(Args);
    }

    void gsWarningMsg(const char *Format, ...)
    //Post: If the printing of warning messages is enabled, "warning: " is printed
    //      to stderr followed by Format, where the remaining parameters are used
    //      as gsprintf arguments to Format.
    {
      if (gsWarning) {
        va_list Args;
        va_start(Args, Format);

        if (custom_message_handler) {
          handler_wrapper(gs_warning, Format, Args);
        }
        else {
          fprintf(stderr, "warning: ");
          gsvfprintf(stderr, Format, Args);
        }

        va_end(Args);
      }
    }

    void gsVerboseMsg(const char *Format, ...)
    //Post: If the printing of verbose information is enabled, Format is printed to
    //      stderr, where the remaining parameters are used as gsprintf arguments
    //      to Format.
    {
      if (gsVerbose) {
        va_list Args;
        va_start(Args, Format);

        if (custom_message_handler) {
          handler_wrapper(gs_notice, Format, Args);
        }
        else {
          gsvfprintf(stderr, Format, Args);
        }

        va_end(Args);
      }
    }

# define GS_DEBUG_MSG_FUNC(FuncName,Format) \
    if (gsDebug) { \
      fprintf(stderr, "(%s): ", FuncName); \
      va_list Args; \
      va_start(Args, Format); \
      gsvfprintf(stderr, Format, Args); \
      va_end(Args); \
    }

    //Post: If the printing of debug messages is enabled, the name of the current
    //      function is printed to stderr, followed by the first parameter with the
    //      remaining parameters as gsprintf arguments.

    void gsDebugMsgFunc(const char *FuncName, const char *Format, ...)
    //Post: If the printing of debug messages is enabled, the name of FuncName is
    //      printed to stderr, followed by Format where  the remaining parameters
    //      are used as gsprintf arguments to Format.
    {
      GS_DEBUG_MSG_FUNC(FuncName,Format)
    }

  }
}

