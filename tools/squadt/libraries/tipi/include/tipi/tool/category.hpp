// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/tool/category.hpp
/// \brief Type for representing a tool functionality category (protocol concept)

#ifndef __TIPI_CATEGORY_H__
#define __TIPI_CATEGORY_H__

#include <string>
#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/array.hpp>

namespace tipi {

  class configuration;

  namespace tool {

    class capabilities;

    /**
     * \brief Category used to classify functionality of tools
     **/
    class category {
      friend class tipi::tool::capabilities;
      friend class tipi::configuration;
      friend std::ostream& operator <<(std::ostream&, category const&);

      private:

        /** \brief Name of the category */
        std::string m_name;

      public:

        /// \brief Standard categories
        enum standard_category_type {
          editing,        ///< show edit
          reporting,      ///< show properties of objects
          conversion,     ///< transformations of objects between different storage formats
          transformation, ///< changing objects but retaining the storage format: e.g. optimisation, editing
          visualisation,  ///< visualisation of objects
          simulation,     ///< simulation
          unknown         ///< for internal use only
        };

      private:

        static boost::array< const category, 7 > const& standard_categories() {
          static const boost::array< const category, 7 > categories = { {
            category("editing"),
            category("reporting"),
            category("conversion"),
            category("transformation"),
            category("visualisation"),
            category("simulation"),
            category("unknown"),
          } };

          return categories;
        }

      public:

        /** \brief Constructor
         * \param[in] name a name for the category
         **/
        inline category(std::string const& name) : m_name(name) {
        }

        /** \brief Chooses the best matching category for a string that is interpreted as category name
         * \param[in] n a name for the category
         * \return the best matching category object
         **/
        inline static category const& match(std::string const& n) {

          for (boost::array < const category, 7 >::const_iterator i =
                 standard_categories().begin(); i != standard_categories().end(); ++i) {

            if (i->get_name() == n) {
              return (*i);
            }
          }

          return standard_categories()[unknown];
        }

        /** \brief Gets the name of the category */
        inline std::string get_name() const {
          return (m_name);
        }

        /** \brief Whether or not the category is unknown */
        inline bool is_unknown() const {
          return (&standard_categories()[unknown] == this);
        }

        /** \brief Compare for smaller */
        inline bool operator <(const standard_category_type c) const {
          return m_name < standard_categories()[c].m_name;
        }

        /** \brief Compare for smaller */
        inline bool operator <(category const& c) const {
          return (m_name < c.m_name);
        }

        /** \brief Compare for equality */
        inline bool operator ==(const standard_category_type c) const {
          return m_name == standard_categories()[c].m_name;
        }

        /** \brief Compare for equality */
        inline bool operator ==(category const& c) const {
          return (m_name == c.m_name);
        }

        /** \brief Conversion to STL string */
        inline operator std::string() const {
          return m_name;
        }
    };

    /** \brief Conversion to print category to a standard stream */
    inline std::ostream& operator <<(std::ostream& s, category const& c) {
      return (s << c.m_name);
    }
  }
}

#endif
