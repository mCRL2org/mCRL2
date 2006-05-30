#include <istream>
#include <string>
#include <sstream>
#include "mcrl2lexer.h"
#include "liblowlevel.h"

using namespace std;

//Global precondition: the ATerm library has been initialised

ATermAppl parse_tagged_stream(const string &tag, istream &stream);
/*Pre: stream is opened for reading
  Post:the content of tag followed by stream is parsed
  Ret: the parsed content, if everything went ok
       NULL, otherwise
*/ 

ATermAppl parse_specification(istream &spec_stream) {
  return parse_tagged_stream("spec", spec_stream);
}

ATermAppl parse_sort_expression(istream &se_stream) {
  return parse_tagged_stream("sort_expr", se_stream);
}

ATermAppl parse_data_expression(istream &de_stream) {
  return parse_tagged_stream("data_expr", de_stream);
}

ATermAppl parse_proc_expression(istream &pe_stream) {
  return parse_tagged_stream("proc_expr", pe_stream);
}

ATermAppl parse_tagged_stream(const string &tag, istream &stream) {
  vector<istream*> *streams = new vector<istream*>();
  istringstream *tag_stream = new istringstream(tag);
  streams->push_back(tag_stream);
  streams->push_back(&stream);
  ATermAppl result = parse_streams(*streams);
  delete tag_stream;
  delete streams;
  return result;
}
