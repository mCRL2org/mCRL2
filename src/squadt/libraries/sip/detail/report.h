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

#include <sip/detail/configuration.h>

namespace sip {

  /** \brief Describes a report of tool operation */
  class report {
    private:
      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string         error;

      /** \brief Room for comments about anything at all */
      std::string         comment;

      /** \brief The configuration that can be used to rerun the tool and refresh its outputs */
      configuration::sptr final_configuration;

    public:
      /** \brief Constructor */
      inline report();

      /** \brief An error description (implies that tool execution was unsuccessful) */
      inline void set_error(std::string);

      /** \brief Set the configuration that was used */
      void set_configuration(configuration::sptr o);

      /** \brief Report comment (arbitrary text) */
      inline void set_comment(std::string);

      /** \brief Output XML representation to string */
      inline std::string write() const;

      /** \brief Generates an XML representation for this report */
      void write(std::ostream&) const;

      /** \brief Reconstructs a report from XML representation */
      static report* read(xml2pp::text_reader&) throw ();
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

  inline report::report() {
  }

  inline void report::set_error(std::string e) {
    error = e;
  }

  inline std::string report::write() const {
    std::ostringstream output;

    write(output);

    return (output.str());
  }

  /** \pre{configuration must have been allocated on the heap} */
  inline void report::set_configuration(configuration::sptr c) {
    final_configuration = configuration::sptr (c);
  }

  inline void report::set_comment(std::string c) {
    comment = c;
  }
}

#endif
