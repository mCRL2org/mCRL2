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
      /** \brief Convenience data type to hide the shared pointer wrapping */
      typedef configuration::configuration_ptr configuration_ptr;

      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string       error;

      /** \brief Room for comments about anything at all */
      std::string       comment;

      /** \brief The configuration that can be used to rerun the tool and refresh its outputs */
      configuration_ptr _configuration;

    public:
      /** \brief Constructor */
      inline report();

      /** \brief An error description (implies that tool execution was unsuccessful) */
      inline void set_error(std::string);

      /** \brief Set the configuration that was used */
      void set_configuration(configuration_ptr o);

      /** \brief Report comment (arbitrary text) */
      inline void set_comment(std::string);

      /** \brief Generates an XML representation for this report */
      void to_xml(std::ostream&) const;

      /** \brief Reconstructs a report from XML representation */
      static report* from_xml(xml2pp::text_reader&) throw ();
  };

  inline report::report() {
  }

  inline void report::set_error(std::string e) {
    error = e;
  }

  /** \pre{configuration must have been allocated on the heap} */
  inline void report::set_configuration(configuration_ptr c) {
    _configuration = configuration_ptr (c);
  }

  inline void report::set_comment(std::string c) {
    comment = c;
  }
}

#endif
