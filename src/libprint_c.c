#ifdef __cplusplus
extern "C" {
#endif

#include "libprint_c.h"

#define PRINT_C
#include "libprint_impl.h"

void PrintPart_C(FILE *OutStream, const ATerm Part, t_pp_format pp_format)
{
  PrintPart__C(OutStream, Part, pp_format);
}

int gsprintf(const char *format, ...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = gsvfprintf(stdout, format, args);
  va_end(args);

  return result;
}

int gsfprintf(FILE *stream, const char *format, ...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = gsvfprintf(stream, format, args);
  va_end(args);

  return result;
}

int gsvfprintf(FILE *stream, const char *format, va_list args)
{
  const char     *p;
  char           *s;
  char            fmt[32];
  int             result = 0;

  for (p = format; *p; p++)
  {
    if (*p != '%')
    {
      fputc(*p, stream);
      continue;
    }

    s = fmt;
    while (!isalpha((int) *p))	/* parse formats %-20s, etc. */
      *s++ = *p++;
    while ( (*p) == 'l' || (*p) == 'h' || (*p) == 'j' || (*p) == 'L' || (*p) == 'j' || (*p) == 'z' || (*p) == 't' )
      *s++ = *p++;
    *s++ = *p;
    *s = '\0';

    switch (*p)
    {
      case 'c':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
	fprintf(stream, fmt, va_arg(args, int));
	break;

      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
	fprintf(stream, fmt, va_arg(args, double));
	break;

      case 'n':
      case 'p':
	fprintf(stream, fmt, va_arg(args, void *));
	break;

      case 'a':
      case 'A':
      case 's':
	fprintf(stream, fmt, va_arg(args, char *));
	break;

	/*
	 * MCRL2 specifics start here: "%P" to pretty print an ATerm using
         * the advanced method
	 */
      case 'P':
	PrintPart_C(stream, va_arg(args, ATerm), ppAdvanced);
	break;

	/*
	 * ATerm specifics start here: "%T" to print an ATerm; "%F" to
	 * print an AFun
	 *
	 * Commented out are:
	 * "%I" to print a list; "%N" to print a single ATerm node;
	 * "%H" to print the MD5 sum of a ATerm
	 */
      case 'T':
	fmt[strlen(fmt)-1] = 't';
	ATfprintf(stream, fmt, va_arg(args, ATerm));
	break;
/*      case 'I':
	fmt[strlen(fmt)-1] = 'l';
	ATfprintf(stream, fmt, va_arg(args, ATermList));
	break;*/
      case 'F':
	fmt[strlen(fmt)-1] = 'y';
	ATfprintf(stream, fmt, va_arg(args, AFun));
	break;
/*      case 'N':
	fmt[strlen(fmt)-1] = 'n';
	ATfprintf(stream, fmt, va_arg(args, ATerm));
	break;
      case 'H':
	fmt[strlen(fmt)-1] = 'h';
	ATfprintf(stream, fmt, va_arg(args, ATerm));
	break;*/

      default:
	fputc(*p, stream);
	break;
    }
  }
  return result;
}

#ifdef __cplusplus
}
#endif
