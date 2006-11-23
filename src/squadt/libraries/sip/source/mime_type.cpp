#include "sip/mime_type.h"

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

namespace sip {
  char* const       mime_type::main_type_as_string[] = { "application", "audio", "image", "message", "multipart", "text", "video", "unknown", "" };

  /**
   * \param[in] s a string that represents a mime type
   **/
  mime_type::mime_type(std::string const& s) : m_main(unknown) {
    static const boost::regex match_type_and_subtype("([^ \\n\\(\\)<>@,;:\\\\\"/\\[\\]?.=]+)(?:/([^ \\n\\(\\)<>@,;:\\\\\"/\\[\\]?.=]+))?");

    boost::smatch  matches;

    if (boost::regex_match(s, matches, match_type_and_subtype)) {
      if (matches.size() == 3 && !matches[2].str().empty()) {
        m_sub = matches[2];

        char* const* x = &main_type_as_string[0];

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
    assert(!s.empty() && !s.find(' ') && !(s.find('\t')));
  }
}

