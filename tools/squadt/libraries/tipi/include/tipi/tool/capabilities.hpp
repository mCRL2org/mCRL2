// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/tool/capabilities.hpp
/// \brief Type for representing a tools's capabilities (protocol concept)

#ifndef _TIPI_TOOL_CAPABILITIES_HPP__
#define _TIPI_TOOL_CAPABILITIES_HPP__

#include <set>
#include <iosfwd>

#include <boost/shared_ptr.hpp>
#include <boost/range/iterator_range.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"

#include "tipi/configuration.hpp"
#include "tipi/common.hpp"
#include "tipi/mime_type.hpp"
#include "tipi/tool/category.hpp"

namespace tipi {
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
    class capabilities : public ::utility::visitable {
      friend class tipi::tool::communicator;
      friend class tipi::controller::communicator;

      template < typename R, typename S >
      friend class ::utility::visitor;

      public:

        /** \brief Description for a tool's main input object */
        class input_configuration : public ::utility::visitable {
          template < typename R, typename S >
          friend class ::utility::visitor;

          friend bool operator< (const input_configuration&, const input_configuration&);

          private:

            typedef std::map< tipi::configuration::parameter::identifier, tipi::mime_type > object_map;

          public:

            /// \brief type that specifies a sequence of {input, output} objects
            typedef boost::iterator_range < object_map::const_iterator >                   object_sequence;

          private:

            tipi::tool::category const&               m_category;           ///< tool category
            object_map                                m_object_map;         ///< storage format

          private:

            inline input_configuration(tool::category const& c) : m_category(c) {
            }

          public:

            /** \brief Constructor
             * \param[in] c the category that best matches the functionality for this configuration
             * \param[in] m the type of the input file
             * \param[in] id the identifier of the input file in a future tipi::configuration object
             **/
            inline input_configuration(tool::category const& c,
                        mime_type const& m, tipi::configuration::parameter::identifier const& id) : m_category(c) {

              m_object_map.insert(std::make_pair(id, m));
            }

            /** \brief Gets the category */
            inline tipi::tool::category const& get_category() const {
              return m_category;
            }

            /** \brief Gets the primary input object descriptor */
            inline object_map::value_type get_primary_object_descriptor() const {
              return *m_object_map.begin();
            }

            /** \brief Gets the sequence of objects descriptors */
            inline boost::iterator_range < object_map::const_iterator > object_range() const {
              return boost::make_iterator_range(m_object_map.begin(), m_object_map.end());
            }

            /** \brief Compares two input combinations for equality */
            inline static bool equal(const input_configuration& p, const input_configuration& q) {
              object_map::const_iterator i = p.m_object_map.begin();
              object_map::const_iterator j = q.m_object_map.begin();

              while (i != p.m_object_map.end() && j != q.m_object_map.end()) {
                if (i->first != j->first || i->second.sub_type() != j->second.sub_type()) {
                  return false;
                }

                ++i;
                ++j;
              }

              return i == p.m_object_map.end() && j == q.m_object_map.end() && p.m_category == q.m_category;
            }
        };

        /** \brief Description for a tool's output object */
        class output_configuration : public ::utility::visitable {
          template < typename R, typename S >
          friend class ::utility::visitor;

          friend bool operator< (const output_configuration&, const output_configuration&);

          private:

            mime_type                                  m_mime_type;  ///< storage format
            tipi::configuration::parameter::identifier m_identifier; ///< identifier of primary input

          public:

            /** \brief Constructor
             * \param[in] m the type of the output object
             * \param[in] id the identifier of the input file in a future tipi::configuration object
             **/
            inline output_configuration(mime_type const& m, tipi::configuration::parameter::identifier id) : m_mime_type(m), m_identifier(id) {
            }

            /** \brief Gets the format */
            inline tipi::mime_type get_format() const {
              return m_mime_type;
            }

            /** \brief Compares two input combinations for equality */
            inline static bool equal(const output_configuration& p, const output_configuration& q) {
              return (p.m_mime_type.sub_type() == q.m_mime_type.sub_type());
            }
        };

        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < boost::shared_ptr < const input_configuration > >       input_configuration_list;

        /** \brief Convenience type for a list of input configurations */
        typedef std::set  < boost::shared_ptr < const output_configuration > >      output_configuration_list;

        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < input_configuration_list::const_iterator >  input_configuration_range;

        /** \brief Convenience type for use in interface */
        typedef boost::iterator_range < output_configuration_list::const_iterator >   output_configuration_range;

      private:

        /** \brief The protocol version */
        version                  m_protocol_version;

        /** \brief The available input configurations */
        input_configuration_list m_input_configurations;

        /** \brief The available input configurations */
        output_configuration_list  m_output_configurations;

      public:

        /** \brief Constructor */
        capabilities(const version = default_protocol_version);

        /** \brief Add an input configuration */
        void add_input_configuration(std::string const&, mime_type const&, tool::category::standard_category_type const = category::unknown);

        /** \brief Add an input configuration */
        void add_input_configuration(std::string const&, mime_type const&, tool::category const&);

        /** \brief Add an output configuration */
        void add_output_configuration(std::string const&, mime_type const&);

        /** \brief Get the protocol version */
        version get_version() const;

        /** \brief Returns a reference to the list of input combinations */
        input_configuration_range get_input_configurations() const;

        /** \brief Returns a reference to the list of output combinations */
        output_configuration_range get_output_configurations() const;

        /** \brief Find a specific input combination of this tool, if it exists */
        inline boost::shared_ptr< const input_configuration > find_input_configuration(const mime_type& m, const tool::category::standard_category_type c) const {
          return find_input_configuration(m, tool::category::standard_categories()[c]);
        }

        /** \brief Find a specific input combination of this tool, if it exists */
        boost::shared_ptr< const input_configuration > find_input_configuration(const mime_type&, const tool::category&) const;
    };

    /// \cond INTERNAL
    /** \brief Smaller, performs simple lexicographic comparison (included for use with standard data structures) */
    inline bool operator< (const capabilities::input_configuration& p, const capabilities::input_configuration& q) {
      capabilities::input_configuration::object_map::const_iterator i = p.m_object_map.begin();
      capabilities::input_configuration::object_map::const_iterator j = q.m_object_map.begin();

      while (i != p.m_object_map.end() && j != q.m_object_map.end()) {
        if (i->first < j->first || i->second < j->second) {
          return true;
        }

        ++i;
        ++j;
      }

      return (i == p.m_object_map.end() && j != q.m_object_map.end()) && p.m_category < q.m_category;
    }

    /** \brief Smaller, performs simple lexicographic comparison (included for use with standard data structures) */
    inline bool operator< (const capabilities::output_configuration& a, const capabilities::output_configuration& b) {
      return (a.m_mime_type < b.m_mime_type || a.m_mime_type == b.m_mime_type);
    }
    /// \endcond
  }
}

#endif

