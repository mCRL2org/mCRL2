// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/mime_type.hpp

#ifndef MIME_TYPE_H__
#define MIME_TYPE_H__

#include <string>
#include <iostream>

namespace tipi {

  /**
   * \brief Class that provides a partial mime-type implementation
   **/
  class mime_type {

    public:

      /** \brief Recognised main types */
      enum main_type {
        application,
        audio,
        image,
        message,
        multipart,
        text,
        video,
        unknown
      };

      /** \brief Recognised sub types */
      typedef std::string sub_type;

    private:

      /** \brief Strings for conversion of main types */
      static const char* const main_type_as_string[];

    private:

      /** \brief The main type */
      main_type   m_main;

      /** \brief The sub type */
      sub_type    m_sub;

    public:

      /** \brief Constructor */
      mime_type(std::string const&);

      /** \brief Constructor */
      mime_type(std::string const&, main_type s);

      /** \brief Whether the main type is known */
      bool known_main_type() const;

      /** \brief Gets the main type */
      bool is_type(tipi::mime_type::main_type const) const;

      /** \brief Gets the main type */
      std::string get_main_type() const;

      /** \brief Gets the sub type */
      std::string get_sub_type() const;

      /** \brief Converts to string */
      std::string as_string() const;

      /** \brief Converts to string */
      std::string to_string() const;

      /** \brief Compare for equality */
      bool operator== (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator!= (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator< (mime_type const&) const;
  };

  /** \brief Output to stream as string */
  inline std::ostream& operator<<(std::ostream& o, mime_type const& t) {
    o << t.as_string();

    return (o);
  }

  inline bool mime_type::known_main_type() const {
    return (m_main != unknown);
  }

  inline bool mime_type::is_type(tipi::mime_type::main_type const m) const {
    return (m_main == m);
  }

  inline std::string mime_type::get_main_type() const {
    return (main_type_as_string[m_main]);
  }

  inline std::string mime_type::get_sub_type() const {
    return (m_sub);
  }

  inline std::string mime_type::as_string() const {
    return (std::string(main_type_as_string[m_main]) + "/" + m_sub);
  }

  inline std::string mime_type::to_string() const {
    return (std::string(main_type_as_string[m_main]) + "/" + m_sub);
  }

  inline bool mime_type::operator==(mime_type const& r) const {
    return (m_sub == r.m_sub);
  }

  inline bool mime_type::operator!=(mime_type const& r) const {
    return (m_main != r.m_main && m_sub != r.m_sub);
  }

  inline bool mime_type::operator<(mime_type const& r) const {
    return (m_sub < r.m_sub || (m_sub == r.m_sub && m_main < r.m_main));
  }
}

#endif
