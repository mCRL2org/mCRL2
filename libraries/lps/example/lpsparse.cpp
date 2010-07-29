// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparse.cpp
/// \brief Tool for testing the mCRL2 parser.

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "mcrl2/core/detail/algorithms.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/aterm_appl.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_tool;

// defined in mcrl2lexer.cpp
ATerm parse_streams_new(std::vector<std::istream*> &streams, bool print_parse_errors);

// copy some code from core library
ATerm parse_tagged_stream_new(const std::string &tag, std::istream &stream) {
  std::vector<std::istream*> *streams = new std::vector<std::istream*>();
  std::istringstream *tag_stream = new std::istringstream(tag);
  streams->push_back(tag_stream);
  streams->push_back(&stream);
  ATerm result = parse_streams_new(*streams, true);
  delete tag_stream;
  delete streams;
  return result;
}

ATermAppl parse_proc_spec_new(std::istream &ps_stream) {                    
  return (ATermAppl) parse_tagged_stream_new("proc_spec", ps_stream); 
}                                                                  

/// \brief     Parses a process specification.
/// \param[in] ps_stream An input stream from which can be read.
/// \post      The content of ps_stream is parsed as an mCRL2 process
///            specification.
/// \return    The parsed mCRL2 process specification in the internal ATerm
///            format after parsing (before type checking).
/// \exception mcrl2::runtime_error Parsing failed.
inline
ATermAppl parse_process_specification_new(std::istream& ps_stream)
{
  ATermAppl result = parse_proc_spec_new(ps_stream);
  if (result == NULL)
    throw mcrl2::runtime_error("parse error");
  return result;
}

class lps_parse_tool: public input_tool
{
  protected:
    typedef input_tool super;

  public:
    lps_parse_tool()
      : super(
          "lpsparse",
          "Wieger Wesselink",
          "test parsing of an mCRL2 specification",
          "Parse an mCRL2 specification in two different ways and compare the results."
          "If INFILE is not present, standard input is used"
        )
    {}

    atermpp::aterm_appl parse_old(std::string filename)
    {
    	std::ifstream from(filename.c_str());
      return core::detail::parse_process_specification(from);
    }

    atermpp::aterm_appl parse_new(std::string filename)
    {
    	std::ifstream from(filename.c_str());
      return parse_process_specification_new(from);
    }

    bool run()
    {
    	atermpp::aterm_appl t1 = parse_old(input_filename());
    	atermpp::aterm_appl t2 = parse_new(input_filename());
    	std::cout << (t1 == t2 ? "EQUAL" : "NOT EQUAL") << std::endl;
      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps_parse_tool().execute(argc, argv);
}

