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
    public:
      /** \brief Datatype for a uri */
      typedef std::string                                  uri;

      /** \brief Datatype for a storage format (file format) */
      typedef std::string                                  storage_format;

    private:
      /** \brief Convenience data type to hide the shared pointer wrapping */
      typedef boost::shared_ptr < configuration > configuration_ptr;

      /** \brief Convenience data type */
      typedef std::map  < uri, storage_format >   output_list;

      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string   error;

      /** \brief Room for comments about anything at all */
      std::string   comment;

      /** \brief The list of outputs */
      output_list   outputs;

      /** \brief The configuration that can be used to rerun the tool and refresh its outputs */
      configuration_ptr _configuration;

    public:
      /** \brief Constructor */
      inline report();

      /** \brief An error description (implies that tool execution was unsuccessful) */
      inline void set_error(std::string);

      /** \brief Add an output object */
      inline void add_output(const uri URI, const std::string format);

      /** \brief Remove an output object */
      inline void remove_output(const uri URI);

      /** \brief Set the configuration that was used */
      void set_configuration(configuration* o);

      /** \brief Report comment (arbitrary text) */
      inline void set_comment(std::string);

      /** \brief Generates an XML representation for this report */
      void to_xml(std::ostream&) const;

      /** \brief Reconstructs a report from XML representation */
      static report* from_xml(xml2pp::text_reader&);
  };

  inline report::report() {
  }

  inline void report::set_error(std::string e) {
    error = e;
  }

  inline void report::add_output(const std::string u, const std::string f) {
    assert(outputs.find(u) == outputs.end());

    outputs[u] = f;
  }

  inline void report::remove_output(const std::string u) {
    assert(outputs.find(u) != outputs.end());

    outputs.erase(u);
  }

  /** \pre{configuration must have been allocated on the heap} */
  inline void report::set_configuration(configuration* c) {
    _configuration = configuration_ptr (c);
  }

  inline void report::set_comment(std::string c) {
    comment = c;
  }
}

#endif
