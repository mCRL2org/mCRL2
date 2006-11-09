#ifndef SIP_TOOL_CAPABILITIES_H
#define SIP_TOOL_CAPABILITIES_H

#include <set>
#include <iosfwd>

#include <boost/shared_ptr.hpp>
#include <boost/range/iterator_range.hpp>

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
          storage_format     format;     ///< storage format
          object::identifier identifier; ///< identifier for the main input object

          /** \brief Compares two input combinations for equality */
          inline static bool equal(const input_combination&, const input_combination&);
        };
 
        /** \brief Description for a tool's output object */
        struct output_combination {
          storage_format     format;     ///< storage format
          object::identifier identifier; ///< identifier for the output object

          /** \brief Compares two input combinations for equality */
          inline static bool equal(const output_combination&, const output_combination&);
        };
 
        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < input_combination >                                   input_combination_list;
 
        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < output_combination >                                  output_combination_list;
 
        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < input_combination_list::const_iterator >  input_combination_range;

        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < output_combination_list::const_iterator > output_combination_range;

        /** \brief Convenience type that hides the shared pointer implementation */
        typedef boost::shared_ptr < capabilities >                                sptr;
 
      private:
 
        /** \brief The protocol version */
        version                  protocol_version;
 
        /** \brief The available input configurations */
        input_combination_list   input_combinations;

        /** \brief The available input configurations */
        output_combination_list  output_combinations;
 
        /** \brief Whether the configuration can be changed through user interaction, after the start signal */
        bool                     interactive;

      public:
 
        /** \brief Constructor */
        capabilities(const version = default_protocol_version);
 
        /** \brief Add an input configuration */
        void add_input_combination(object::identifier, storage_format, tool_category);
 
        /** \brief Add an output configuration */
        void add_output_combination(object::identifier, storage_format);
 
        /** \brief Get the protocol version */
        version get_version() const;
 
        /** \brief Set or reset flag that the tool is interactive (configuration may change through user interaction) */
        void set_interactive(bool);
 
        static capabilities::sptr read(const std::string&);
 
        /** \brief Read from XML stream */
        static capabilities::sptr read(xml2pp::text_reader& reader) throw ();
 
        /** \brief Write to XML string */
        std::string write() const;
 
        /** \brief Write to XML stream */
        void write(std::ostream&) const;

        /** \brief Returns a reference to the list of input combinations */
        input_combination_range get_input_combinations() const;

        /** \brief Returns a reference to the list of output combinations */
        output_combination_range get_output_combinations() const;

        /** \brief Find a specific input combination of this tool, if it exists */
        input_combination const* find_input_combination(const storage_format&, const tool_category&) const;
    };
 
    inline bool capabilities::input_combination::equal(const input_combination& p, const input_combination& q) {
      return (p.format == q.format && p.category == q.category);
    }

    inline bool capabilities::output_combination::equal(const output_combination& p, const output_combination& q) {
      return (p.format == q.format);
    }
  }
}

#endif

