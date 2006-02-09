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

  /** A helper class for the generation of reports */
  class report {
    public:
      /** Datatype for a uri */
      typedef std::string                                  uri;

      /** Datatype for a storage format (file format) */
      typedef std::string                                  storage_format;

    private:
      typedef boost::shared_ptr < configuration > configuration_ptr;

      typedef std::map  < uri, storage_format >   output_list;

      /** Room for errors (any error here implies unsuccessful termination) */
      std::string   error;

      /** Room for comments about anything at all */
      std::string   comment;

      /** The list of outputs */
      output_list   outputs;

      /** The configuration that can be used to rerun the tool and refresh its outputs */
      configuration_ptr _configuration;

    public:
      /** Constructor */
      inline report();

      /** An error description (implies that tool execution was unsuccessful) */
      inline void set_error(std::string);

      /** Add an output object */
      inline void add_output(const uri URI, const std::string format);

      /** Remove an output object */
      inline void remove_output(const uri URI);

      /** Set the configuration that was used */
      void set_configuration(configuration* o);

      /** Report comment (arbitrary text) */
      inline void set_comment(std::string);

      /** Generates an XML representation for this report */
      void to_xml(std::ostream&) const;

      /** Reconstructs a report from XML representation */
      static report* from_xml(const std::string&);
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
