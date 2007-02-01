#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H

#include <algorithm>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include <sip/visitors.h>

namespace transport {
  namespace transceiver {
    class basic_transceiver;
  }
}

namespace sip {
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
    class message : public utility::visitable < message< M, D, A > > {
      friend class basic_messenger_impl < message < M, D, A > >;
      friend class sip::restore_visitor_impl;
      friend class sip::store_visitor_impl;

      public:

        /** \brief Type for message identification */
        typedef M                                                type_identifier_t;

        /** \brief Type for message originator identification */
        typedef const transport::transceiver::basic_transceiver* end_point;

        /** \brief The type identifier for messages of which the type is not known */
        static const M                                           message_unknown;

        /** \brief The type identifier for messages of any type */
        static const M                                           message_any;

      private:

        /** \brief The message type */
        type_identifier_t type;

        /** \brief Identifier for the origin of this message */
        end_point         originator;

        /** \brief The content of a message */
        std::string       content;

      private:

        /** \brief Default constructor */
        inline message();

        /** \brief Constructor used by messenger implementations */
        inline message(end_point&);

      public:

        /** \brief Generates an XML text string for the message */
        inline message(type_identifier_t t, end_point o = 0);

        /** \brief Generates an XML text string for the message */
        template < typename T >
        inline message(T, type_identifier_t t, end_point o = 0);

        /** \brief Copy constructor */
        inline message(message&);

        /** \brief Returns the message type */
        inline type_identifier_t get_type() const;
 
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
 
    /** \brief The type identifier for messages of which the type is not known */
//    template < class M, M D, M A >
//    const M message< M, D, A >::message_unknown = D;

    /** \brief The type identifier for messages of any type */
//    template < class M, M D, M A >
//    const M message< M, D, A >::message_any     = A;

    /**
     * \param o message originator identifier
     * \param t a message type identifier
     **/
    template < class M, M D, M A >
    inline message< M, D, A >::message(type_identifier_t t, end_point o) : type(t), originator(o) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message(message& m) : type(m.type), content(m.content) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message() : type(D) {
    }

    template < class M, M D, M A >
    inline message< M, D, A >::message(end_point& e) : type(D), originator(e) {
    }

    /**
     * \param c the initial content of the message
     * \param t a message type identifier
     * \param o the local endpoint
     **/
    template < class M, M D, M A >
    template < typename T >
    inline message< M, D, A >::message(T c, type_identifier_t t, end_point o) : type(t), originator(o) {
      set_content(c);
    }

    template < class M, M D, M A >
    inline M message< M, D, A >::get_type() const {
      return (type);
    }

    /** Returns the remote end point, or 0 if the message contains no originator information */
    template < class M, M D, M A >
    inline typename message< M, D, A >::end_point message< M, D, A >::get_originator() const {
      return (originator);
    }
 
    template < class M, M D, M A >
    inline std::string message< M, D, A >::to_string() const {
      return (content);
    }

    template < class M, M D, M A >
    inline bool message< M, D, A >::is_empty() const {
      return (content.empty());
    }

    /**
     * \param c contains the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(std::istream& c) {
      std::ostringstream temporary;
 
      temporary << c.rdbuf();
 
      content = temporary.str();
    }

    /**
     * \param c the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(const std::string& c) {
      content = c;
    }

    /**
     * \param c the initial content of the message
     **/
    template < class M, M D, M A >
    inline void message< M, D, A >::set_content(const char* c) {
      content = c;
    }
  }
}

#endif
