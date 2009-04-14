// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool_declaration.cpp
/// \brief Example program for the documentation.

#include "boost.hpp" // precompiled headers

#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using utilities::interface_description;
using utilities::tools::input_tool;

//[tool_declaration
  class my_tool: public input_tool
  {
    public:
      my_tool()
        : input_tool(
            "my_tool",
            "John Doe",
            "A short description of my_tool",
            "A long description of my_tool"
          )
      {}
      
      bool run()
      {
        std::clog << "Executing my_tool" << std::endl;;
        return true;
      }
  };
//]

int main(int argc, char* argv[])
{
  my_tool tool;
  tool.execute(argc, argv);

  return 0;
}

