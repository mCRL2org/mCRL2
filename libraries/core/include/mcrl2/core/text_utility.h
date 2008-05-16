// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/text_utility.h
/// \brief .

#ifndef MCRL2_CORE_TEXT_UTILITY_H
#define MCRL2_CORE_TEXT_UTILITY_H

#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive.hpp>

namespace mcrl2 {

namespace core {

  /// Splits a string into paragraphs.
  std::vector<std::string> split_paragraphs(const std::string& text)
  {
    using namespace boost::xpressive;
      
    std::vector<std::string> result;

    // find multiple line endings
    sregex paragraph_split = sregex::compile( "\\n\\s*\\n" );
  
    // the -1 below directs the token iterator to display the parts of
    // the string that did NOT match the regular expression.
    sregex_token_iterator cur( text.begin(), text.end(), paragraph_split, -1 );
    sregex_token_iterator end;

    for( ; cur != end; ++cur )
    {
      std::string paragraph = *cur;
      boost::trim(paragraph);
      if (paragraph.size() > 0)
      {
        result.push_back(paragraph);
      }
    }
    return result;
  }

  /// read text from a file
  inline
  std::string read_text(const std::string& filename, bool warn=false)
  {
    std::ifstream in(filename.c_str());
    if (!in)
    {
      if (warn)
        std::cerr << "Could not open input file: " << filename << std::endl;
      return "";
    }
    in.unsetf(std::ios::skipws); //  Turn of white space skipping on the stream
  
    std::string s;
    std::copy(
      std::istream_iterator<char>(in),
      std::istream_iterator<char>(),
      std::back_inserter(s)
    );
  
    return s;
  }

  /// Remove comments from a text (everything from '%' until end of line)
  inline
  std::string remove_comments(const std::string& text)
  {
    using namespace boost::xpressive;

    // matches everything from '%' until end of line
    sregex src = sregex::compile( "%[^\\n]*\\n" );

    std::string dest( "\n" );
    return regex_replace(text, src, dest);
  }

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_TEXT_UTILITY_H
