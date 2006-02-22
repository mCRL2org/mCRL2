#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H

#include <algorithm>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include <xml2pp/xml_text_reader.h>

namespace sip {

  namespace messenger {

    template < class M, M D > class message;

    template < class M > class basic_messenger;

    /**
     * Base class for messages M is a type for message characterisation
     * 
     * \attention M should be an enumeration type for message identifiers with
     *   a value D that is used as the default message type identifier
     */
    template < class M, M D = static_cast < M > (0) >
    class message {
      friend class basic_messenger < message < M, D > >;

      public:
        /** Type for message identification */
        typedef M                  type_identifier_t;

      private:

        /** The message type */
        type_identifier_t          type;

        /** The content of a message */
        std::string                content;

        /** Tries to extract the message type, which is the first element */
        inline static type_identifier_t extract_type(std::string& content);
 
      public:
        /** Generates an XML text string for the message */
        inline message(type_identifier_t t = D);

        /** Returns the message type */
        inline type_identifier_t get_type() const;
 
        /** Generates an XML text string for the message */
        inline std::string to_xml() const;

        /** Generates an XML representation and writes to stream */
        inline void to_xml(std::ostream&) const;
 
        /** Returns the content without formatting */
        inline std::string to_string() const;
 
        /** Set content via string */
        inline void set_content(const std::string);

        /** Set content via stream */
        inline void set_content(std::istream&);

        /** Set content via null-terminated string */
        inline void set_content(const char*);
    };
 
    template < class M, M D >
    inline message< M, D >::message(type_identifier_t t) : type(t) {
    }

    template < class M, M D >
    inline M message< M, D >::get_type() const {
      return (type);
    }

    template < class M, M D >
    inline M message< M, D >::extract_type(std::string& content) {
      const std::string message_meta_tag("<message-meta");

      std::string::iterator i = content.begin();

      if (std::mismatch(message_meta_tag.begin(),message_meta_tag.end(),i).first == message_meta_tag.end()) {
        size_t identifier = 0;

        std::string::iterator e = std::find(content.begin(), content.end(), '>');

        xml2pp::text_reader reader(content, (++e - i));

        reader.read();

        reader.get_attribute(&identifier, "type");

        /* Remove message meta element */
        content.erase(content.begin(),e);

        return (std::max(static_cast < M > (identifier), D));
      }

      return (D);
    }
 
    template < class M, M D >
    inline void message< M, D >::set_content(const std::string c) {
      content = c;
    }
 
    template < class M, M D >
    inline void message< M, D >::set_content(std::istream& c) {
      std::ostringstream temporary;
 
      temporary << c.rdbuf();
 
      content = temporary.str();
    }
 
    template < class M, M D >
    inline void message< M, D >::set_content(const char* c) {
      content = std::string(c);
    }
 
    template < class M, M D >
    inline std::string message< M, D >::to_xml() const {
      std::ostringstream output;
      
      to_xml(output);
 
      return (output.str());
    }
 
    template < class M, M D >
    inline void message< M, D >::to_xml(std::ostream& output) const {
      output << "<message-meta type=\"" << type << "\"/>"
             << content;
    }

    template < class M, M D >
    inline std::string message< M, D >::to_string() const {
      return (content);
    }
  }
}

#endif
