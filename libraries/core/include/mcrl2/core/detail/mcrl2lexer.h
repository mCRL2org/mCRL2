// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2lexer.h

#ifndef MCRL2_MCRL2LEXER_H
#define MCRL2_MCRL2LEXER_H

#include <istream>
#include <vector>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATerm parse_streams(std::vector<std::istream*> &streams, bool print_parse_errors = true);
/*Pre: streams contains at least one element
       each element of streams is opened for reading
  Post:the contents of streams is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/

#endif // MCRL2_MCRL2LEXER_H
