#ifndef SIP_REPORT_H
#define SIP_REPORT_H

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <iosfwd>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <utility/visitor.h>

#include <sip/configuration.h>

namespace sip {

  /** \brief Describes a report of tool operation */
  class report : public utility::visitable < report > {

    public:

      /** \brief Type alias for convenience */
      typedef boost::shared_ptr < report > sptr;

      /** \brief The message class */
      enum type {
        notice,  //< like a warning but even less urgent
        warning, //< warning of some kind
        error    //< fatal error occurred
      };

    private:

      /** \brief The type of the report */
      type        report_type;

      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string description;

    public:

      /** \brief Constructor */
      inline report(type const&, std::string const&);

      /** \brief Gets the description */
      inline std::string get_description();

      /** \brief Output XML representation to string */
      inline std::string write() const;

      /** \brief Generates an XML representation for this report */
      void write(std::ostream&) const;

      /** \brief Reconstructs a report from XML representation */
      static sptr read(xml2pp::text_reader&) throw ();
  };

  /**
   * \brief Operator for writing to stream
   *
   * @param s stream to write to
   * @param r the report object to write out
   **/
  inline std::ostream& operator << (std::ostream& s, const report& r) {
    r.write(s);

    return (s);
  }

  /**
   * @param[in] t the type of the report
   * @param[in] d a description
   **/
  inline report::report(type const& t, std::string const& d) : report_type(t), description(d) {
  }

  inline std::string report::get_description() {
    return (description);
  }

  inline std::string report::write() const {
    std::ostringstream output;

    write(output);

    return (output.str());
  }
}

#endif
