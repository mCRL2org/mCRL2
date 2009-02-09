// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/detail/message.hpp

#ifndef TIPI_MESSAGE_H
#define TIPI_MESSAGE_H

#include <algorithm>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include "tipi/detail/visitors.hpp"

namespace transport {
  namespace transceiver {
    class basic_transceiver;
  }
}

namespace tipi {
  namespace messaging {

    template < class M, M D, M A > class message;

    template < class M > class basic_messenger_impl;

    /**
     * \brief Base class for messages M is a type for message characterisation
     *
     * \attention M should be an enumeration type for message identifiers with
     *   a value D that is used as the default message type identifier
     *   a value A that is used as any (the wildcard) message type
     */
    template < class M, M D, M A >
    class message : public ::utility::visitable {
      friend class basic_messenger_impl < message < M, D, A > >;

      template < typename R, typename S >
      friend class ::utility::visitor;

      public:

        /** \brief Type for message identification */
        typedef M                                                message_type;

        /** \brief Type for message originator identification */
        typedef const transport::transceiver::basic_transceiver* end_point;

        /** \brief The type identifier for messages of which the type is not known */
        inline static message_type unknown() {
          return D;
        }

        inline static message_type any() {
          return A;
        }

      private:

        /** \brief The message type */
        message_type      m_type;

        /** \brief Identifier for the origin of this message */
        end_point         m_originator;

        /** \brief The content of a message */
        std::string       m_content;

      private:

        /** \brief Default constructor */
        inline message();

        /** \brief Constructor used by messenger implementations */
        inline message(end_point&);

      public:

        /** \brief Generates an XML text string for the message */
        inline message(message_type t, end_point o = 0);

        /** \brief Generates an XML text string for the message */
        template < typename T >
        inline message(T const&, message_type t, end_point o = 0);

        /** \brief Copy constructor */
        inline message(message const&);

        /** \brief Returns the message type */
        inline message_type get_type() const;

        /** \brief Returns the message originator information */
        inline end_point get_originator() const;

        /** \brief Returns the content without formatting */
        inline std::string to_string() const;

        /** \brief Whether the message has content or not */
        inline bool is_empty() const;

        /** \brief Generates an XML text string for the message */
        inline void set_content(const std::string&);

        /** \brief Generates an XML text string for the message */
        inline void set_content(const char*);

        /** \brief Generates an XML text string for the message */
        inline void set_content(std::istream&);
    };

    /**
     * \param o message originator identifier
     * \param t a message type identifier
     **/
    template < class M, M D, M A >
    inline message< M, D, A >::message(const message_type t, end_point o) : m_type(t), m_originator(o) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message(message const& m) : m_type(m.m_type), m_content(m.m_content) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message() : m_type(D) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message(end_point& e) : m_type(D), m_originator(e) {
    }

    /**
     * \param c the initial content of the message
     * \param t a message type identifier
     * \param o the local endpoint
     **/
    template < class M, M D, M A >
    template < typename T >
    inline message< M, D, A >::message(T const& c, const message_type t, end_point o) : m_type(t), m_originator(o) {
      set_content(c);
    }

    template < class M, M D, M A >
    inline M message< M, D, A >::get_type() const {
      return (m_type);
    }

    /** Returns the remote end point, or 0 if the message contains no originator information */
    template < class M, M D, M A >
    inline typename message< M, D, A >::end_point message< M, D, A >::get_originator() const {
      return (m_originator);
    }

    template < class M, M D, M A >
    inline std::string message< M, D, A >::to_string() const {
      return (m_content);
    }

    template < class M, M D, M A >
    inline bool message< M, D, A >::is_empty() const {
      return (m_content.empty());
    }

    /**
     * \param c contains the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(std::istream& c) {
      std::ostringstream temporary;

      temporary << c.rdbuf();

      m_content = temporary.str();
    }

    /**
     * \param c the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(const std::string& c) {
      m_content = c;
    }

    /**
     * \param c the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(const char* c) {
      m_content = c;
    }
  }
}

#endif
