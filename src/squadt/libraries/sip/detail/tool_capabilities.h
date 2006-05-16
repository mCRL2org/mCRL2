#ifndef SIP_TOOL_CAPABILITIES_H
#define SIP_TOOL_CAPABILITIES_H

#include <set>
#include <ostream>
#include <sstream>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <xml2pp/text_reader.h>
#include <sip/detail/object.h>
#include <sip/detail/configuration.h>
#include <sip/detail/common.h>

namespace sip {
  namespace tool {

    class capabilities;

    /**
     * \brief Describes some tool capabilities (e.g. supported protocol version)
     *
     * Objects of this type contain information about the capabilities of a tool:
     *
     *  - what version of the protocol the controller uses
     *  - a list of input configurations
     *  - ...
     *
     * As well as any information about the controller that might be interesting
     * for a tool developer.
     **/
    class capabilities {
      friend class tool::communicator;
      friend class controller::communicator;
 
      public:
 
        /** \brief Until there is something better this is the type for a storage format */
        typedef object::storage_format                       storage_format;
 
        /** \brief Until there is something better this is the type for a tool category */
        typedef configuration::tool_category                 tool_category;
 
        /** \brief Description for a tool's main input object */
        struct input_combination {
          tool_category      category;   ///< tool category
          storage_format     format;     ///< storage format of the main input
          object::identifier identifier; ///< identifier for the main input object

          /** \brief Compares two input combinations for equality */
          inline static bool equal(const input_combination&, const input_combination&);
        };
 
        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < input_combination >              input_combination_list;
 
        /** \brief Convenience type that hides the shared pointer implementation */
        typedef boost::shared_ptr < capabilities >           ptr;
 
      private:
 
        /** \brief The protocol version */
        version                  protocol_version;
 
        /** \brief The available input configurations */
        input_combination_list   input_combinations;
 
        /** \brief Whether the configuration can be changed through user interaction, after the start signal */
        bool                     interactive;

      public:
 
        /** \brief Constructor */
        inline capabilities(const version = default_protocol_version);
 
        /** \brief Add an input configuration */
        inline void add_input_combination(object::identifier, tool_category, storage_format);
 
        /** \brief Get the protocol version */
        inline version get_version() const;
 
        /** \brief Set or reset flag that the tool is interactive (configuration may change through user interaction) */
        inline void set_interactive(bool);
 
        inline static capabilities::ptr read(const std::string&);
 
        /** \brief Read from XML stream */
        inline static capabilities::ptr read(xml2pp::text_reader& reader) throw ();
 
        /** \brief Write to XML string */
        inline std::string write() const;
 
        /** \brief Write to XML stream */
        inline void write(std::ostream&) const;
 
        /** \brief Returns a reference to the list of input combinations */
        inline const input_combination_list& get_input_combinations() const;

        /** \brief Find a specific input combination of this tool, if it exists */
        inline input_combination const* find_input_combination(const storage_format&, const tool_category&) const;
    };
 
    inline bool capabilities::input_combination::equal(const input_combination& p, const input_combination& q) {
      return (p.format == q.format && p.category == q.category);
    }

    /**
     * \brief Operator for writing to stream
     *
     * @param s stream to write to
     * @param t the capabilities object to write out
     **/
    inline std::ostream& operator << (std::ostream& s, const capabilities& t) {
      t.write(s);
 
      return (s);
    }

    /** \brief Smaller, performs simple lexicographic comparison (included for use with standard data structures) */
    inline bool operator < (const capabilities::input_combination& a, const capabilities::input_combination& b) {
      return (a.format < b.format || ((a.format == b.format) && a.category < b.category));
    }
 
    inline capabilities::capabilities(const version v) : protocol_version(v), interactive(false) {
    }
 
    inline void capabilities::add_input_combination(object::identifier id, tool_category c, storage_format f) {
      input_combination ic = {c, f, id};
 
      input_combinations.insert(ic);
    }
 
    inline version capabilities::get_version() const {
      return (protocol_version);
    }
 
    inline std::string capabilities::write() const {
      std::ostringstream output;
 
      write(output);
 
      return (output.str());
    }
 
    inline const capabilities::input_combination_list& capabilities::get_input_combinations() const {
      return (input_combinations);
    }
 
    inline void capabilities::write(std::ostream& output) const {
      output << "<capabilities>"
             << "<protocol-version major=\"" << (unsigned short) protocol_version.major
             << "\" minor=\"" << (unsigned short) protocol_version.minor << "\"/>";
 
      if (interactive) {
        // Tool is interactive
        output << "<interactivity level=\"1\"/>";
      }
      
      for (input_combination_list::const_iterator i = input_combinations.begin(); i != input_combinations.end(); ++i) {
        output << "<input-configuration category=\"" << (*i).category
               << "\" format=\"" << (*i).format
               << "\" identifier=\"" << (*i).identifier << "\"/>";
      }
 
      output << "</capabilities>";
    }
 
    inline void capabilities::set_interactive(bool b) {
      interactive = b;
    }
 
    /**
     * @param s the string to read from
     **/
    inline capabilities::ptr capabilities::read(const std::string& s) {
      xml2pp::text_reader r(s);

      return (read(r));
    }

    /**
     * @param r the XML text reader to read from
     *
     * \attention if the reader does not point at a capabilities element nothing is read
     **/
    inline capabilities::ptr capabilities::read(xml2pp::text_reader& r) throw () {
      if (r.is_element("capabilities")) {
        version v = {0,0};
 
        r.read();
 
        assert (r.is_element("protocol-version"));
 
        r.get_attribute(&v.major, "major");
        r.get_attribute(&v.minor, "minor");
 
        capabilities::ptr c(new capabilities(v));
 
        r.read();
        r.skip_end_element("protocol-version");
 
        if (r.is_element("interactivity")) {
          c->interactive = r.get_attribute("level");
 
          r.read();
          r.skip_end_element("interactivity");
        }
 
        assert (r.is_element("input-configuration"));
 
        while (r.is_element("input-configuration")) {
          input_combination ic;
 
          r.get_attribute(&ic.category, "category");
          r.get_attribute(&ic.format, "format");
          r.get_attribute(&ic.identifier, "identifier");
 
          c->input_combinations.insert(ic);
 
          r.read();
          r.skip_end_element("input-configuration");
        }
 
        return (c);
      }
 
      return capabilities::ptr();
    }

    /**
     * @param f the storage format
     * @param t the category in which the tool operates
     **/
    inline capabilities::input_combination const*
              capabilities::find_input_combination(const storage_format& f, const tool_category& t) const {
 
      input_combination p = {t, f, 0};

      input_combination_list::const_iterator i = std::find_if(input_combinations.begin(),
                      input_combinations.end(), boost::bind(&input_combination::equal, _1, p));

      if (i == input_combinations.end()) {
        return (0);
      }
      else {
        return (&(*i));
      }
    }
  }
}

#endif

