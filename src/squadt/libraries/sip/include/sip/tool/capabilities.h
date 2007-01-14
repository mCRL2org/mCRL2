#ifndef SIP_TOOL_CAPABILITIES_H
#define SIP_TOOL_CAPABILITIES_H

#include <set>
#include <iosfwd>

#include <boost/shared_ptr.hpp>
#include <boost/range/iterator_range.hpp>

#include <utility/visitor.h>

#include <sip/configuration.h>
#include <sip/mime_type.h>
#include <sip/tool/category.h>

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
    class capabilities : public utility::visitable < tool::capabilities > {
      friend class sip::tool::communicator;
      friend class sip::controller::communicator;
      friend class sip::store_visitor_impl;
      friend class sip::restore_visitor_impl;
  
      public:
  
        /** \brief Description for a tool's main input object */
        class input_combination {
 
          public:
 
            tool::category const& m_category;   ///< tool category
            mime_type const       m_mime_type;  ///< storage format
            std::string           m_identifier; ///< identifier for the main input object
 
          public:
 
            /** \brief Constructor */
            input_combination(tool::category const&, mime_type const&, std::string const&);
 
            /** \brief Compares two input combinations for equality */
            static bool equal(const input_combination&, const input_combination&);
        };
  
        /** \brief Description for a tool's output object */
        class output_combination {
 
          public:
 
            mime_type          m_mime_type;  ///< storage format
            std::string        m_identifier; ///< identifier for the output object
 
          public:
 
            /** \brief Constructor */
            output_combination(mime_type const&, std::string const&);
 
            /** \brief Compares two input combinations for equality */
            static bool equal(const output_combination&, const output_combination&);
        };
  
        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < input_combination >                                   input_combination_list;
  
        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < output_combination >                                  output_combination_list;
  
        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < input_combination_list::const_iterator >  input_combination_range;
 
        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < output_combination_list::const_iterator > output_combination_range;
 
      private:
  
        /** \brief The protocol version */
        version                  m_protocol_version;
  
        /** \brief The available input configurations */
        input_combination_list   m_input_combinations;
 
        /** \brief The available input configurations */
        output_combination_list  m_output_combinations;
  
      public:
  
        /** \brief Constructor */
        capabilities(const version = default_protocol_version);
  
        /** \brief Add an input configuration */
        void add_input_combination(std::string const&, mime_type const&, tool::category const& = category::unknown);
  
        /** \brief Add an output configuration */
        void add_output_combination(std::string const&, mime_type const&);
  
        /** \brief Get the protocol version */
        version get_version() const;
  
        /** \brief Returns a reference to the list of input combinations */
        input_combination_range get_input_combinations() const;
 
        /** \brief Returns a reference to the list of output combinations */
        output_combination_range get_output_combinations() const;
 
        /** \brief Find a specific input combination of this tool, if it exists */
        input_combination const* find_input_combination(const mime_type&, const tool::category&) const;
    };
  
    /** \brief Smaller, performs simple lexicographic comparison (included for use with standard data structures) */
    inline bool operator < (const capabilities::input_combination& a, const capabilities::input_combination& b) {
      return (a.m_mime_type < b.m_mime_type || ((a.m_mime_type == b.m_mime_type) && a.m_category < b.m_category));
    }
 
    /** \brief Smaller, performs simple lexicographic comparison (included for use with standard data structures) */
    inline bool operator < (const capabilities::output_combination& a, const capabilities::output_combination& b) {
      return (a.m_mime_type < b.m_mime_type || a.m_mime_type == b.m_mime_type);
    }
 
    inline capabilities::input_combination::input_combination(tool::category const& c,
                        mime_type const& m, std::string const& id) : m_category(c), m_mime_type(m), m_identifier(id) {
    }
 
    inline capabilities::output_combination::output_combination(mime_type const& f, std::string const& id) :
                                                                                m_mime_type(f), m_identifier(id) {
    }
 
    inline bool capabilities::input_combination::equal(input_combination const& p, input_combination const& q) {
      return (p.m_mime_type.get_sub_type() == q.m_mime_type.get_sub_type() && p.m_category == q.m_category);
    }
 
    inline bool capabilities::output_combination::equal(output_combination const& p, output_combination const& q) {
      return (p.m_mime_type.get_sub_type() == q.m_mime_type.get_sub_type());
    }
  }
}

#endif

