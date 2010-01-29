// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/xpressive/xpressive_static.hpp"

#include "tipi/mime_type.hpp"

namespace tipi {
  /**
   * \param[in] s a string that represents a mime type
   **/
  mime_type::mime_type(std::string const& s) : m_main(application) {
    using namespace boost::xpressive;

    smatch matches;

    mark_tag main(1), sub(2);

    if (regex_match(s, matches, sregex((main= +(~(set = ' ','\n','(',')','<','>','@',',',';',':','\\','"','/','[',']','?','.','='))) >> !('/' >>
                                             (sub= +(~(set = ' ','\n','(',')','<','>','@',',',';',':','\\','"','/','[',']','?','.','='))))))) {

      if (matches.size() == 3 && (matches[2].length() != 0)) {
        m_sub = matches[2].str();

        const char* const* x = &main_type_as_string()[0];

        while (*x != 0) {
          if (*x == matches[1]) {
            m_main = static_cast < category_type > (x - &main_type_as_string()[0]);

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
      m_sub = "application";
    }
  }

  /**
   * \param[in] s the subtype string (must not contain white space characters)
   * \param[in] m the main type
   **/
  mime_type::mime_type(std::string const& s, category_type m) : m_main(m), m_sub(s) {
    if(!(!s.empty() && (s.find(' ') == std::string::npos) && (s.find('\t') == std::string::npos))){
      throw std::runtime_error("String \""+ s + "\" must not contain white space characters");
    }
  }
}

