//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/mime_type.cpp

#include <boost/foreach.hpp>
#include <boost/xpressive/xpressive_static.hpp>

#include "tipi/mime_type.hpp"

namespace tipi {
  const char* const       mime_type::main_type_as_string[] = { "application", "audio", "image", "message", "multipart", "text", "video", "unknown", 0 };

  /**
   * \param[in] s a string that represents a mime type
   **/
  mime_type::mime_type(std::string const& s) : m_main(unknown) {
    using namespace boost::xpressive;

    smatch matches;

    //"([^ \\n\\(\\)<>@,;:\\\\\"/\\[\\]?.=]+)(?:/([^ \\n\\(\\)<>@,;:\\\\\"/\\[\\]?.=]+))?"))) {
    if (regex_match(s, matches, sregex((+(~(set = ' ','\n','(',')','<','>','@',',',';',':','\\','"','/','[',']','?','.','='))) >>
                                             (!(+(~(set = ' ','\n','(',')','<','>','@',',',';',':','\\','"','/','[',']','?','.','='))))))) {

      if (matches.size() == 3 && !matches[2].str().empty()) {
        m_sub = matches[2];

        const char* const* x = &main_type_as_string[0];

        while (*x != 0) {
          if (*x == matches[1]) {
            m_main = static_cast < main_type > (x - &main_type_as_string[0]);

            break;
          }

          ++x;
        }
      }
      else {
        m_sub = matches[1];
      }
    }
    else {
      m_sub = "unknown";
    }
  }

  /**
   * \param[in] s the subtype string (must not contain white space characters)
   * \param[in] m the main type
   **/
  mime_type::mime_type(std::string const& s, main_type m) : m_main(m), m_sub(s) {
    assert(!s.empty() && (s.find(' ') == std::string::npos) && (s.find('\t') == std::string::npos));
  }
}

