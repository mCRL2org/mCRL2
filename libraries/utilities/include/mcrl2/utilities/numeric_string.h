#ifndef __NUMERIC_STRING_FUNCTIONS_H__
#define __NUMERIC_STRING_FUNCTIONS_H__

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

//String representations of numbers
//---------------------------------

char *gsStringDiv2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the smallest string representation of n div 2
//     Note that the result is created with malloc, so it has to be freed
     
int gsStringMod2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the value of n mod 2

char *gsStringDub(const char *n, const int inc);
//Pre: n is of the form "0 | [1-9][0-9]*"
//     0 <= inc <= 1
//Ret: the smallest string representation of 2*n + inc,
//     Note that the result is created with malloc, so it has to be freed

int NrOfChars(int n);
//Ret: the number of characters of the decimal representation of n

#ifdef __cplusplus
    }
  }
}
#endif

#endif
