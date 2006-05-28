#ifndef TOOL_H
#define TOOL_H

#include <string>

#include <xml2pp/text_reader.h>
#include <sip/detail/tool_capabilities.h>

#include "exception.h"
#include "core.h"

namespace squadt {

  class extractor;

  /**
   * \brief Container for information about a single tool
   **/
  class tool {
    friend class extractor;
    friend class tool_manager;

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool >                 ptr;

      /** \brief Convenience type alias */
      typedef sip::tool::capabilities::input_combination input_combination;

    private:

      /** \brief A name for the tool */
      std::string                               name;

      /** \brief The location where the tool can be found */
      std::string                               location;

      /** \brief Stores the tool capabilities object obtained through protocol implementation */
      sip::tool::capabilities::ptr              capabilities;

      /** \brief The default capabilities descriptor; one that is empty */
      static const sip::tool::capabilities::ptr no_capabilities;

    private:

      /** \brief Constructor */
      inline tool(std::string, std::string, sip::tool::capabilities::ptr = tool::no_capabilities);

    public:

      /** \brief Write configuration to stream */
      void write(std::ostream& = std::cout) const;

      /** \brief Read configuration from file */
      inline static tool::ptr read(xml2pp::text_reader&) throw ();

      /** \brief Get the last received capabilities object for this tool */
      inline const sip::tool::capabilities::ptr get_capabilities() const;

      /** \brief Get the location to for this tool */
      inline std::string& get_location();

      /** \brief Get the name of this tool */
      inline std::string& get_name();

      /** \brief Find a specific input combination of this tool, if it exists */
      inline input_combination const* find_input_combination(const storage_format&, const tool_category&) const;
  };

  /**
   * @param l a full path to the executable
   * @param n a name for the tool
   * @param c a tool::capabilities object for the tool
   **/
  inline tool::tool(std::string n, std::string l, sip::tool::capabilities::ptr c) : name(n), location(l), capabilities(c) {
  }

  /**
   * @param s the stream to write to
   **/
  inline void tool::write(std::ostream& s) const {
    s << "<tool name=\"" << name
      << "\" location=\"" << location << "\"";

    if (capabilities.get() != 0) {
      s << capabilities << "</tool>";
    }
    else {
      s << "/>";
    }
  }

  /**
   * \pre the reader points to a tool element
   *
   * @param r the XML text reader to read the data from
   **/
  inline tool::ptr tool::read(xml2pp::text_reader& r) throw () {
    std::string name;
    std::string location;

    if (!(r.get_attribute(&name, "name") && r.get_attribute(&location, "location"))) {
      throw (exception::exception(exception::required_attributes_missing, "tool"));
    }

    if (!r.is_end_element()) {
      r.read();

      sip::tool::capabilities::ptr c = sip::tool::capabilities::read(r);
      
      if (c.get() != 0) {
        return (tool::ptr(new tool(name, location, c)));
      }
    }

    r.skip_end_element("tool");
      
    return (tool::ptr(new tool(name, location)));
  }

  inline const sip::tool::capabilities::ptr tool::get_capabilities() const {
    return (capabilities);
  }

  inline std::string& tool::get_location() {
    return (location);
  }

  inline std::string& tool::get_name() {
    return (name);
  }

  /**
   * @param f the storage format
   * @param t the category in which the tool operates
   **/
  inline tool::input_combination const* tool::find_input_combination(const storage_format& f, const tool_category& t) const {
    return (capabilities->find_input_combination(f, t));
  }
}

#endif
