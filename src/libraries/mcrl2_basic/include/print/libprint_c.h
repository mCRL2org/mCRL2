#include <stdio.h>
#include <aterm2.h>
#include <ctype.h>
#include <assert.h>
#include <malloc.h>
#include "libprint_types.h"
#include "liblowlevel.h"

#ifdef __cplusplus
extern "C" {
#endif

//This file contains C specific printing functions

//Global preconditions:
//- the ATerm library has been initialised
//- gsEnableConstructorFunctions() has been called

void PrintPart_C(FILE *OutStream, const ATerm Part, t_pp_format pp_format);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATerm containing a part of a mCRL2 specification
  Post:A textual (pretty printed) representation of Part is written to
       OutStream using method pp_format
*/

int gsprintf(const char *format, ...);
int gsfprintf(FILE *stream, const char *format, ...);
int gsvfprintf(FILE *stream, const char *format, va_list args);
//Extensions of the printf functions. The following new conversion formats are
//supported:
//- '%P' for the pretty printing ATerm's using the advanced method
//- '%T' for the normal printing of ATerm's 
//- '%F' for the printing of AFun's

// The default message handler (defined in lowlevel.cpp)
extern void (*custom_message_handler)(gsMessageType, char*);

// Helper function (wrapper around gsvfprintf) for printing to string
static void handler_wrapper(gsMessageType t, char *Format, ...) {
  FILE* stream = tmpfile();

  assert(stream);

  va_list Args;
  va_start(Args, Format);
  gsvfprintf(stream, Format, Args);
  va_end(Args);

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

inline static void gsErrorMsg(char *Format, ...)
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

inline static void gsWarningMsg(char *Format, ...)
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

inline static void gsVerboseMsg(char *Format, ...)
//Post: If the printing of verbose information is enabled, Format is printed to
//      stderr, where the remaining parameters are used as gsprintf arguments
//      to Format.
{
  if (gsVerbose) {
    va_list Args;
    va_start(Args, Format);

    if (custom_message_handler) {
      handler_wrapper(gs_info, Format, Args);
    }
    else {
      gsvfprintf(stderr, Format, Args);
    }

    va_end(Args);
  }
}

#define GS_DEBUG_MSG_FUNC(FuncName,Format) \
  if (gsDebug) { \
    fprintf(stderr, "(%s): ", FuncName); \
    va_list Args; \
    va_start(Args, Format); \
    gsvfprintf(stderr, Format, Args); \
    va_end(Args); \
  }

#ifdef _MSC_VER
inline static void gsDebugMsg(char *Format,...)
{
	GS_DEBUG_MSG_FUNC("unknown",Format)
}
#else
#define gsDebugMsg(...)        gsDebugMsgFunc(__func__, __VA_ARGS__)
#endif
//Post: If the printing of debug messages is enabled, the name of the current
//      function is printed to stderr, followed by the first parameter with the
//      remaining parameters as gsprintf arguments.

inline static void gsDebugMsgFunc(const char *FuncName, char *Format, ...)
//Post: If the printing of debug messages is enabled, the name of FuncName is
//      printed to stderr, followed by Format where  the remaining parameters
//      are used as gsprintf arguments to Format.
{
  GS_DEBUG_MSG_FUNC(FuncName,Format)
}

#ifdef __cplusplus
}
#endif
