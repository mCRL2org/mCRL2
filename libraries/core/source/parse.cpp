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
#include "mcrl2/core/detail/mcrl2lexer.h"
#include "mcrl2/core/messaging.h"

using namespace std;

namespace mcrl2 {
  namespace core {

//Global precondition: the ATerm library has been initialised

ATerm parse_tagged_stream(const string &tag, istream &stream);
/*Pre: stream is opened for reading
  Post:the content of tag followed by stream is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_identifier(istream &se_stream) {
  return (ATermAppl) parse_tagged_stream("identifier", se_stream);
}

ATermAppl parse_sort_expr(istream &se_stream) {
  return (ATermAppl) parse_tagged_stream("sort_expr", se_stream);
}

ATermAppl parse_data_expr(istream &de_stream) {
  return (ATermAppl) parse_tagged_stream("data_expr", de_stream);
}

ATermAppl parse_data_spec(istream &ps_stream) {
  return (ATermAppl) parse_tagged_stream("data_spec", ps_stream);
}

ATermAppl parse_mult_act(istream &pe_stream) {
  return (ATermAppl) parse_tagged_stream("mult_act", pe_stream);
}

ATermAppl parse_proc_expr(istream &pe_stream) {
  return (ATermAppl) parse_tagged_stream("proc_expr", pe_stream);
}

ATermAppl parse_proc_spec(istream &ps_stream) {
  return (ATermAppl) parse_tagged_stream("proc_spec", ps_stream);
}

ATermAppl parse_state_frm(istream &pe_stream) {
  return (ATermAppl) parse_tagged_stream("state_frm", pe_stream);
}

ATermAppl parse_action_rename_spec(istream &pe_stream) {
  return (ATermAppl) parse_tagged_stream("action_rename", pe_stream);
}

ATermAppl parse_pbes_spec(istream &pbes_spec_stream) {
  return (ATermAppl) parse_tagged_stream("pbes_spec", pbes_spec_stream);
}

ATermList parse_data_vars(istream &pe_stream) {
  return (ATermList) parse_tagged_stream("data_vars", pe_stream);
}

ATerm parse_tagged_stream(const string &tag, istream &stream) {
  vector<istream*> *streams = new vector<istream*>();
  istringstream *tag_stream = new istringstream(tag);
  streams->push_back(tag_stream);
  streams->push_back(&stream);
  ATerm result = parse_streams(*streams);
  delete tag_stream;
  delete streams;
  return result;
}

  }
}

