#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H

#include <string>
#include <istream>
#include <sstream>

namespace sip {

  /** Base class for messages */
  class message {
    friend class sip_communicator;
    private:
      std::string content;

    public:
      /** Generates an XML text string for the message */
      inline message(std::string data);
      inline message(const char* data);
      inline message();

      inline std::string to_xml() const;

      inline std::string to_string() const;

      inline void set_content(std::string);
      inline void set_content(std::istream&);
      inline void set_content(const char*);
  };

  inline message::message() {
  }

  inline message::message(std::string data) : content(data) {
  }

  inline message::message(const char * data) : content(data) {
  }

  inline void message::set_content(std::string c) {
    content = c;
  }

  inline void message::set_content(std::istream& c) {
    std::ostringstream temporary;

    temporary << c.rdbuf();

    content = temporary.str();
  }

  inline void message::set_content(const char* c) {
    content = std::string(c);
  }

  inline std::string message::to_xml() const {
    std::string message("<message>");

    message.append(content);
    message.append("</message>");

    return (message);
  }

  inline std::string message::to_string() const {
    return (content);
  }
}

#endif
