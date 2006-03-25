#include <istream>
#include <string>
#include <sstream>
#include "mcrl2lexer.h"
#include "liblowlevel.h"

using namespace std;

//Global precondition: the ATerm library has been initialised

ATermAppl parse_specification(istream &spec_stream) {
  vector<istream*> *streams = new vector<istream*>();
  istringstream *start = new istringstream("@spec");
  streams->push_back(start);
  streams->push_back(&spec_stream);
  ATermAppl result = parse_streams(*streams);
  delete start;
  delete streams;
  return result;
}

ATermAppl parse_data_expression(std::istream &de_stream) {
  vector<istream*> *streams = new vector<istream*>();
  istringstream *start = new istringstream("@data_expr");
  streams->push_back(start);
  streams->push_back(&de_stream);
  ATermAppl result = parse_streams(*streams);
  delete start;
  delete streams;
  return result;
}
