#ifndef SIP_CATEGORY_H__
#define SIP_CATEGORY_H__

#include <algorithm>
#include <functional>
#include <boost/bind.hpp>

#include <boost/array.hpp>

namespace sip {
  namespace tool {

    class capabilities;

    class category {
      friend class sip::tool::capabilities;
      friend std::ostream& operator <<(std::ostream&, category const&);

      private:

        /** \brief Name of the category */
        std::string name;

      private:

        /** \brief unknown, for everything that does not map to one of the public categories */
        static const category unknown;

      public:

        static const category reporting;      ///< show properties of objects
        static const category conversion;     ///< transformations of objects between different storage formats
        static const category transformation; ///< changing objects but retaining the storage format: e.g. optimisation, editing
        static const category visualisation;  ///< visualisation of objects
        static const category simulation;     ///< simulation

        static const boost::array < category const*, 6 > categories;

      private:

        /** \brief Constructor */
        inline category(std::string const&);

      public:

        /** \brief Gets the name of the category */
        inline std::string get_name() const;

        /** \brief Whether or not the category is unknown */
        inline bool is_unknown();

        /** \brief Chooses the best matching category for a string that is interpreted as category name */
        inline static category const& fit(std::string const&);

        /** \brief Compare for smaller */
        inline bool operator <(category const&) const;

        /** \brief Compare for equality */
        inline bool operator ==(category const&) const;

        /** \brief Conversion to STL string */
        inline operator std::string() const;
    };

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
    const category category::unknown("unknown");
    const category category::reporting("reporting");
    const category category::conversion("conversion");
    const category category::transformation("transformation");
    const category category::visualisation("visualisation");
    const category category::simulation("simulation");

    const boost::array < category const*, 6 > category::categories = { {
      &category::unknown,
      &category::reporting,
      &category::conversion,
      &category::transformation,
      &category::visualisation,
      &category::simulation,
    } };
#endif

    inline category::category(std::string const& n) : name(n) {
    }

    inline std::string category::get_name() const {
      return (name);
    }

    inline bool category::is_unknown() {
      return (&unknown == this);
    }

    inline category const& category::fit(std::string const& n) {
      boost::array < category const*, 5 >::const_iterator i = std::find_if(categories.begin(), categories.end(),
                  boost::bind(std::equal_to< std::string const >(), n, boost::bind(&category::name, _1)));

      if (i != categories.end()) {
        return (**i);
      }

      return (unknown);
    }

    inline std::ostream& operator <<(std::ostream& s, category const& c) {
      return (s << c.name);
    }

    inline bool category::operator <(category const& c) const {
      return (c.name < name);
    }

    inline bool category::operator ==(category const& c) const {
      return (&c == this);
    }

    inline category::operator std::string() const {
      return (name);
    }
  }
}

#endif
