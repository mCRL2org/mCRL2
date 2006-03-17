#ifndef TOOL_H
#define TOOL_H

#include <string>

#include <xml2pp/text_reader.h>
#include <sip/detail/tool_capabilities.h>

#include "exception.h"

namespace squadt {

  /**
   * \brief Container for information about a single tool
   **/
  class tool {
    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool >  ptr;

    private:

      /** \brief A name for the tool */
      std::string name;

      /** \brief The location where the tool can be found */
      std::string location;

    public:

      /** Constructor */
      inline tool(std::string, std::string);

      /** \brief Write configuration to stream */
      void write(std::ostream& = std::cout) const;

      /** \brief Read configuration from file */
      inline static tool::ptr read(xml2pp::text_reader&) throw ();

      /** \brief Get the location to for this tool */
      inline std::string& get_location();

      /** \brief Get the location to for this tool */
      inline std::string& get_name();
  };

  /**
   * @param l a full path to the executable
   * @param n a name for the tool
   **/
  inline tool::tool(std::string n, std::string l) : name(n), location(l) {
  }

  /**
   * @param s the stream to write to
   **/
  inline void tool::write(std::ostream& s) const {
    s << "<tool name=\"" << name
      << "\" location=\"" << location << "\"/>";
  }

  /**
   * \pre the reader points to a tool element
   *
   * @param r the XML text reader to read the data from
   **/
  inline tool::ptr tool::read(xml2pp::text_reader& r) throw () {
    std::string name;
    std::string location;

    if (!(r.get_attribute(&name, "name") && r.get_attribute(&name, "location"))) {
      throw (exception(exception_identifier::required_attributes_missing, "tool"));
    }

    r.read();
    r.skip_end_element("tool");

    return (tool::ptr(new tool(name, location)));
  }

  inline std::string& tool::get_location() {
    return (location);
  }

  inline std::string& tool::get_name() {
    return (name);
  }
}

#endif
