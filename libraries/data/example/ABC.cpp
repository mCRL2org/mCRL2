#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "ABC.h"
#include "ABC_parser.h"

ATerm parse(const std::string& tag, std::istream& from)
{
  std::vector<std::istream*>* streams = new std::vector<std::istream*>();
  std::istringstream *tag_stream = new std::istringstream(tag);
  streams->push_back(tag_stream);
  streams->push_back(&from);
  ATerm result = ABC_parse(*streams, true);
  delete tag_stream;
  delete streams;
  return result;
}

atermpp::aterm_appl parse_identifier(std::istream& from)
{
  return (ATermAppl) parse("identifier", from);
}

int main()
{
  std::string s = "a";
  std::istringstream in(s);
  atermpp::aterm_appl a = parse_identifier(in);
  std::cout << "a = " << a << std::endl;

  return 0;
}
