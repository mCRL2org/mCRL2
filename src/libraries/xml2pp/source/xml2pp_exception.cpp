/*======================================================================
//
// Copyright (c) 2005 2006 TU/e
//
// author     : Jeroen van der Wulp <J.v.d.Wulp@tue.nl>
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// About :
// 
//  This is a C++ interface wrapper around the XMLTextReader functionality
//  provided by the gnome libxml2 library.
//
//=====================================================================*/

#include <xml2pp/detail/exception.h>

namespace xml2pp {
  namespace exception {

    /** Messages for exceptions */
    template < >
    const char* const exception::descriptions[] = {
      "Unable to read specified file: `%1%'!",
      "Unable to initialise reader.",
      "Attempted illegal operation after the first read.",
      "Parse error in XML document.",
      "End of stream was reached."
    };
  }
}

