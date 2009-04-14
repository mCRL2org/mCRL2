// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_CHI2LEXER_H
#define MCRL2_CHI2LEXER_H

#include <istream>
#include <aterm2.h>

//Global precondition: the ATerm library has been initialised

ATermAppl parse_stream(std::istream &streams);
/*Pre: streams contains at least one element
       each element of streams is opened for reading
  Post:the contents of streams is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/

#endif // MCRL2_CHI2LEXER_H
