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
#include "mcrl2/utilities/logger.h"

#include "workarounds.h"

namespace mcrl2
{
namespace core
{

// Helper function (wrapper around gsvfprintf) for printing to string
static void logger_wrapper(mcrl2_log_level_t t, const char* Format, va_list args)
{

  FILE* stream = tmpfile();

  assert(stream);

  gsvfprintf(stream, Format, args);

  size_t n = ftell(stream);

  fflush(stream);
  rewind(stream);

  char* output  = (char*) malloc((n + 1) * sizeof(char));
  char* current = output;

  while (0 < n--)
  {
    *current = (char) fgetc(stream);

    ++current;
  }

  *current = '\0';

  fclose(stream);

  mCRL2logGS(t) << output;

  free(output);
}

void gsMessage(const char* Format, ...)
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
  va_list Args;
  va_start(Args, Format);
  logger_wrapper(log_info, Format, Args);
  va_end(Args);
}

void gsErrorMsg(const char* Format, ...)
//Post: "error: " is printed to stderr followed by Format, where the remaining
//      parameters are used as gsprintf arguments to Format.
{
  va_list Args;
  va_start(Args, Format);
  logger_wrapper(log_error, Format, Args);
  va_end(Args);
}

void gsWarningMsg(const char* Format, ...)
//Post: If the printing of warning messages is enabled, "warning: " is printed
//      to stderr followed by Format, where the remaining parameters are used
//      as gsprintf arguments to Format.
{
  va_list Args;
  va_start(Args, Format);
  logger_wrapper(log_warning, Format, Args);
  va_end(Args);
}

void gsVerboseMsg(const char* Format, ...)
//Post: If the printing of verbose information is enabled, Format is printed to
//      stderr, where the remaining parameters are used as gsprintf arguments
//      to Format.
{
  va_list Args;
  va_start(Args, Format);
  logger_wrapper(log_verbose, Format, Args);
  va_end(Args);
}

#define GS_DEBUG_MSG_FUNC(FuncName,Format) \
  if (log_debug <= mcrl2_logger::get_reporting_level()) { \
    va_list Args; \
    va_start(Args, Format); \
    mCRL2log(debug) << FuncName << " "; \
    logger_wrapper(log_debug, Format, Args); \
    va_end(Args); \
  }

//Post: If the printing of debug messages is enabled, the name of the current
//      function is printed to stderr, followed by the first parameter with the
//      remaining parameters as gsprintf arguments.

void gsDebugMsgFunc(const char* FuncName, const char* Format, ...)
//Post: If the printing of debug messages is enabled, the name of FuncName is
//      printed to stderr, followed by Format where  the remaining parameters
//      are used as gsprintf arguments to Format.
{
  GS_DEBUG_MSG_FUNC(FuncName,Format)
}

}
}

