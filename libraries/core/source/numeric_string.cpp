// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#include "mcrl2/core/aterm_ext.h"
#include "workarounds.h"

namespace mcrl2 {
  namespace core {

      //String representations of numbers
      //---------------------------------

      /**
       * \pre: '0' <= c < '9'
       * \param[in] c a character that represents a decimal digit
       * \return integer value corresponding to c
       **/
      static inline int gsChar2Int(const char c) {
        assert(c >= '0' && c <= '9');
        return c - '0';
      }

      /**
       * \pre: '0' <= n < '9'
       * \param[in] n an integer value
       * \return character value corresponding to n
       **/
      static inline char gsInt2Char(const int n) {
        assert(n >= 0 && n <= 9);
        return n + '0';
      }

      char *gsStringDiv2(const char *n)
      {
        assert(strlen(n) > 0);
        int l = strlen(n); //length of n
        char *r = (char *) malloc((l+1) * sizeof(char)); //result char*
        //calculate r[0]
        r[0] = gsInt2Char(gsChar2Int(n[0])/2);
        //declare counter for the elements of r
        int j = (r[0] == '0')?0:1;
        //calculate remaining indices of r
        for (int i=1; i<l; ++i)
        {
          //r[j] = 5*(n[i-1] mod 2) + n[i] div 2
          r[j] = gsInt2Char(5*(gsChar2Int(n[i-1])%2) + gsChar2Int(n[i])/2);
          //update j
          j = j+1;
        }
        //terminate string
        r[j] = '\0';
        return r;
      }

      int gsStringMod2(const char *n)
      {
        assert(strlen(n) > 0);
        return gsChar2Int(n[strlen(n)-1]) % 2;
      }

      char *gsStringDub(const char *n, const int inc)
      {
        assert(strlen(n) > 0);
        int l = strlen(n); //length of n
        char *r = (char *) malloc((l+2) * sizeof(char)); //result char*
        //calculate r[0]: n[0] div 5
        r[0] = gsInt2Char(gsChar2Int(n[0])/5);
        //declare counter for the elements of r
        int j = (r[0] == '0')?0:1;
        //calculate remaining indices of r
        for (int i=0; i<l-1; ++i)
        {
          //r[j] = 2*(n[i-1] mod 5) + n[i] div 5
          r[j] = gsInt2Char(2*(gsChar2Int(n[i])%5) + gsChar2Int(n[i+1])/5);
          //update j
          j = j+1;
        }
        //calculate last index of r
        r[j] = gsInt2Char(2*(gsChar2Int(n[l-1])%5) + inc);
        j = j+1;
        //terminate string
        r[j] = '\0';
        return r;
      }

      int NrOfChars(const int n)
      {
        if (n > 0)
          return (int) floor(log10((double) n)) + 1;
        else if (n == 0)
          return 1;
        else //n < 0
          return (int) floor(log10((double) abs(n))) + 2;
      }

  }
}
