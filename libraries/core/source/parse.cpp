// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse.cpp

#include <istream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "mcrl2/core/detail/mcrl2lexer.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"

using namespace std;
using namespace mcrl2::log;

namespace mcrl2
{
namespace core
{

//Global precondition: the ATerm library has been initialised

ATerm parse_tagged_stream(const string& tag, istream& stream);
/*Pre: stream is opened for reading
  Post:the content of tag followed by stream is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/

ATermList parse_data_vars(istream& pe_stream)
{
  mCRL2log(debug) << "parsing data variables..." << std::endl;
  return (ATermList) parse_tagged_stream("data_vars", pe_stream);
}

ATerm parse_tagged_stream(const string& tag, istream& stream)
{
  vector<istream*> *streams = new vector<istream*>();
  istringstream* tag_stream = new istringstream(tag);
  streams->push_back(tag_stream);
  streams->push_back(&stream);
  ATerm result = parse_streams(*streams);
  delete tag_stream;
  delete streams;
  return result;
}

}
}

