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

        /** \brief unknown, for everything that does not map to one of the public categories */
        static const category unknown;

      public:

        static const category editing;        ///< show edit
        static const category reporting;      ///< show properties of objects
        static const category conversion;     ///< transformations of objects between different storage formats
        static const category transformation; ///< changing objects but retaining the storage format: e.g. optimisation, editing
        static const category visualisation;  ///< visualisation of objects
        static const category simulation;     ///< simulation

      private:

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
          static const boost::array < category const*, 7 > categories = { {
            &category::unknown,
            &category::editing,
            &category::reporting,
            &category::conversion,
            &category::transformation,
            &category::visualisation,
            &category::simulation,
          } };

          boost::array < category const*, 7 >::const_iterator i = std::find_if(categories.begin(), categories.end(),
                      boost::bind(std::equal_to< std::string const >(), n, boost::bind(&category::m_name, _1)));

          if (i != categories.end()) {
            return (**i);
          }

          return (unknown);
        }

        /** \brief Gets the name of the category */
        inline std::string get_name() const {
          return (m_name);
        }

        /** \brief Whether or not the category is unknown */
        inline bool is_unknown() const {
          return (&unknown == this);
        }

        /** \brief Compare for smaller */
        inline bool operator <(category const& c) const {
          return (m_name < c.m_name);
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

#ifdef TIPI_IMPORT_STATIC_DEFINITIONS
    const category category::unknown("unknown");
    const category category::editing("editing");
    const category category::reporting("reporting");
    const category category::conversion("conversion");
    const category category::transformation("transformation");
    const category category::visualisation("visualisation");
    const category category::simulation("simulation");
#endif

    /** \brief Conversion to print category to a standard stream */
    inline std::ostream& operator <<(std::ostream& s, category const& c) {
      return (s << c.m_name);
    }
  }
}

#endif
