// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/numeric_string.h
/// \brief Collection of functions dealing with strings representing numbers

#ifndef __NUMERIC_STRING_FUNCTIONS_H__
#define __NUMERIC_STRING_FUNCTIONS_H__

#include <math.h>

namespace mcrl2 {
  namespace core {

    //String representations of numbers
    //---------------------------------

    /**
     * \brief Computes number of characters of the decimal representation
     * \param[in] n an integer number
     * \return the number of characters of the decimal representation of n
     **/
    inline
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

#endif
