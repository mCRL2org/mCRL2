// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LYSALEXER_H
#define MCRL2_LYSALEXER_H

#include <istream>

boost::shared_ptr<lysa::Expression> parse_stream(std::istream &streams, lysa::lysa_options options);
/*Pre: streams contains at least one element
       each element of streams is opened for reading 
  Post:the contents of streams is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/ 



#endif // MCRL2_LYSALEXER_H
