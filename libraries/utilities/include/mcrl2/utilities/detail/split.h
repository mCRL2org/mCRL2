// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/split.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_DETAIL_SPLIT_H
#define MCRL2_UTILITIES_DETAIL_SPLIT_H

#include "mcrl2/utilities/detail/separate_keyword_section.h"
#include "mcrl2/utilities/text_utility.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace mcrl2 {

namespace utilities {

namespace detail {

  // Splits a text into non-empty sections. The sections are trimmed.
  inline
  std::vector<std::string> split_text(const std::string& text, const std::string& keyword)
  {
    std::vector<std::string> result;
    std::vector<std::string> sections = utilities::regex_split(text, keyword);
    for (std::string& section: sections)
    {
      boost::trim(section);
      if (!section.empty())
      {
        result.push_back(section);
      }
    }
    return result;
  }

  // Splits a text into sections based on keywords. The result maps keywords to the corresponding text.
  inline
  std::map<std::string, std::string> split_text_using_keywords(const std::string& text, const std::vector<std::string>& keywords)
  {
    std::map<std::string, std::string> result;
    std::string text1 = text;
    for (auto i = keywords.begin(); i != keywords.end(); ++i)
    {
      const std::string& keyword = *i;
      std::pair<std::string, std::string> p = separate_keyword_section(text1, keyword, keywords);
      result[keyword] = p.first;
      text1 = p.second;
    }
    return result;
  }

  // Splits a text into nonempty lines, and returns them. The lines are trimmed.
  inline
  std::vector<std::string> nonempty_lines(const std::string& text)
  {
    std::vector<std::string> lines = utilities::regex_split(text, "\\n");
    std::vector<std::string> result;
    for (std::string& line: lines)
    {
      boost::trim(line);
      if (!line.empty())
      {
        result.push_back(line);
      }
    }
    return result;
  }

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_SPLIT_H
