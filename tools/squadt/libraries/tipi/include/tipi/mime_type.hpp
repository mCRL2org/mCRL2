// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/mime_type.hpp
/// \brief Type used for representing MIME-types

#ifndef _MIME_TYPE_HPP__
#define _MIME_TYPE_HPP__

#include <string>
#include <iostream>

namespace tipi {

  /**
   * \brief Class that provides a partial mime-type implementation
   **/
  class mime_type {

    public:

      /** \brief Available main types */
      enum category_type {
        application,
        audio,
        image,
        message,
        multipart,
        text,
        video,
        unknown
      };

    private:

      /** \brief Strings for conversion of main types */
      static const char* const main_type_as_string[];

    private:

      /** \brief The main type */
      category_type m_main;

      /** \brief The sub type */
      std::string   m_sub;

    public:

      /** \brief Constructor */
      mime_type(std::string const&);

      /** \brief Constructor */
      mime_type(std::string const&, category_type s);

      /** \brief Gets the category */
      category_type category() const;

      /** \brief Gets the main type */
      std::string main_type() const;

      /** \brief Gets the sub type */
      std::string sub_type() const;

      /** \brief Converts to string */
      std::string string() const;

      /** \brief Compare for equality */
      bool operator== (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator!= (mime_type const&) const;

      /** \brief Compare for inequality */
      bool operator< (mime_type const&) const;
  };

  /** \brief Output to stream as string */
  inline std::ostream& operator<<(std::ostream& o, mime_type const& t) {
    o << t.string();

    return o;
  }

  inline mime_type::category_type mime_type::category() const {
    return m_main;
  }

  inline std::string mime_type::main_type() const {
    return (main_type_as_string[m_main]);
  }

  inline std::string mime_type::sub_type() const {
    return (m_sub);
  }

  inline std::string mime_type::string() const {
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
