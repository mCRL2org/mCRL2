#ifndef SIP_MESSAGE_H
#define SIP_MESSAGE_H

#include <algorithm>
#include <string>
#include <istream>
#include <ostream>
#include <sstream>

#include <xml2pp/xml_text_reader.h>

namespace sip {

  namespace communicator {

    /** Base class for messages */
    class message {
      friend class sip_communicator;

      public:
        /** Type for message identification */
        enum message_type {
          request_controller_capabilities    /// \brief{request the controller a description of its capabilities}
          ,reply_controller_capabilities     /// \brief{send a description of controller capabilities to the tool}
          ,request_tool_capabilities         /// \brief{request a tool a description of its capabilities}
          ,reply_tool_capabilities           /// \brief{send a description of tool capabilities to the controller}
          ,send_select_input_configuration   /// \brief{send the selected input configuration to a tool}
          ,signal_start                      /// \brief{tool can start operation}
          ,send_display_layout               /// \brief{send the controller a layout description for the display}
          ,send_display_data                 /// \brief{send the controller a data to be displayed using the current display layout}
          ,send_interaction_data             /// \brief{send a tool data from user interaction via the display associated with this tool}
          ,request_termination               /// \brief{send a tool the signal to end processing and terminate as soon as possible}
          ,signal_termination                /// \brief{send the controller the signal that the tool is about to terminate}
          ,send_report                       /// \brief{send the controller a report of a tools operation}
          ,unknown                           /// \brief{type is was specified or unknown (should be derived from content)}
        };

      private:

        /** The message type */
        message_type               type;

        /** The content of a message */
        std::string                content;

        /** The XML element name used for wrapping the content to XML representation */
        static const std::string   tag_open;
        static const std::string   tag_close;

        /** Tries to extract the message type, which is the first element */
        inline static message_type extract_type(std::string& content);
 
      public:
        /** Generates an XML text string for the message */
        inline message(message_type t = unknown);

        /** Returns the message type */
        inline message_type get_type() const;
 
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
 
    const std::string message::tag_open("<message>");
    const std::string message::tag_close("</message>");

    inline message::message(message_type t) : type(t) {
    }

    inline message::message_type message::get_type() const {
      return (type);
    }

    inline message::message_type message::extract_type(std::string& content) {
      const std::string message_type_tag("<message-meta");

      std::string::iterator i = content.begin();

      if (std::mismatch(message_type_tag.begin(),message_type_tag.end(),i).first == message_type_tag.end()) {
        size_t identifier = 0;

        std::string::iterator e = std::find(content.begin(), content.end(), '>');

        xml2pp::text_reader reader(content, (++e - i));

        reader.read();

        reader.get_attribute(&identifier, "type");

        /* Remove message meta element */
        content.erase(content.begin(),e);

        return (std::min(static_cast < message::message_type > (identifier), message::unknown));
      }

      return (message::unknown);
    }
 
    inline void message::set_content(const std::string c) {
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
      std::ostringstream output;
      
      to_xml(output);
 
      return (output.str());
    }
 
    inline void message::to_xml(std::ostream& output) const {
      output << "<message>"
             << "<message-meta type=\"" << type << "\"/>"
             << content
             << "</message>";
    }

    inline std::string message::to_string() const {
      return (content);
    }
  }
}

#endif
