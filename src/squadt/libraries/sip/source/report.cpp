#include <algorithm>
#include <ostream>

#include <xml2pp/text_reader.h>

#include <sip/detail/report.h>
#include <sip/detail/exception.h>

namespace sip {

  void report::write(std::ostream& output) const {
    output << "<report>";

    /* Include error */
    if (!error.empty()) {
      const std::string pattern("</error>");

      /* Sanity check... */
      if (std::search(error.begin(), error.end(), pattern.begin(), pattern.end()) != error.end()) {
        throw (sip::exception(exception_identifier::message_forbidden_content, pattern));
      }

      output << "<error>" << error << "</error>";
    }

    /* Include comment */
    if (!comment.empty()) {
      const std::string pattern("</comment>");

      /* Sanity check... */
      if (std::search(comment.begin(), comment.end(), pattern.begin(), pattern.end()) != comment.end()) {
        throw (sip::exception(exception_identifier::message_forbidden_content, pattern));
      }

      output << "<comment>" << comment << "</comment>";
    }

    /* Include configuration specification */
    if (_configuration != 0 && !_configuration->is_empty()) {
      _configuration->write(output);
    }

    output << "</report>";
  }

  /** \pre the reader must point at a report element */
  report* report::read(xml2pp::text_reader& r) throw () {
    report* l = new report();

    r.read();

    assert(r.is_element("report"));

    r.read();

    /* Next element is one of : error, comment, output or configuration (or the end tag of report) */
    while (!r.is_end_element()) {
      if (r.is_element("error")) {
        std::string temporary;

        r.read();

        if (!r.is_end_element()) {
          r.get_value(&temporary);

          r.read();
        }

        r.read();

        l->set_error(temporary);
      }
      else if (r.is_element("comment")) {
        std::string temporary;

        r.read();

        if (!r.is_end_element()) {
          r.get_value(&temporary);

          r.read();
        }

        r.read();

        l->set_comment(temporary);
      }
      else if (r.is_element("configuration")) {
        l->set_configuration(configuration::read(r));
      }
    }

    return (l);
  }
}
