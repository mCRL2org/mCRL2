#include <algorithm>
#include <ostream>

#include <xml2pp/text_reader.h>

#include <sip/detail/report.h>
#include <sip/detail/exception.h>

namespace sip {

  void report::to_xml(std::ostream& output) const {
    output << "<report>";

    /* Include error */
    if (!error.empty()) {
      const std::string pattern("</error>");

      /* Sanity check... */
      if (std::search(error.begin(), error.end(), pattern.begin(), pattern.end()) != error.end()) {
        throw (sip::exception(exception_identifier::forbidden_message_content));
      }

      output << "<error>" << error << "</error>";
    }

    /* Include comment */
    if (!comment.empty()) {
      const std::string pattern("</comment>");

      /* Sanity check... */
      if (std::search(comment.begin(), comment.end(), pattern.begin(), pattern.end()) != comment.end()) {
        throw (sip::exception(exception_identifier::forbidden_message_content));
      }

      output << "<comment>" << comment << "</comment>";
    }

    /* Include output specifications */
    if (0 < outputs.size()) {
            output_list::const_iterator i = outputs.begin();
      const output_list::const_iterator b = outputs.end();

      output << "<output>";

      while (i != b) {
        output << "<object uri=\"" << (*i).first << "\" format=\"" << (*i).second << "\"/>";

        ++i;
      }

      output << "</output>";
    }

    /* Include configuration specification */
    if (_configuration != 0 && !_configuration->is_empty()) {
      _configuration->to_xml(output);
    }

    output << "</report>";
  }

  /** \pre{the reader must point at a report element} */
  report* report::from_xml(xml2pp::text_reader& reader) {
    report* r = new report();

    reader.read();

    assert(reader.is_element("report"));

    reader.read();

    /* Next element is one of : error, comment, output or configuration (or the end tag of report) */
    while (!reader.is_end_element()) {
      if (reader.is_element("error")) {
        std::string temporary;

        reader.read();

        if (!reader.is_end_element()) {
          reader.get_value(&temporary);

          reader.read();
        }

        reader.read();

        r->set_error(temporary);
      }
      else if (reader.is_element("comment")) {
        std::string temporary;

        reader.read();

        if (!reader.is_end_element()) {
          reader.get_value(&temporary);

          reader.read();
        }

        reader.read();

        r->set_comment(temporary);
      }
      else if (reader.is_element("output")) {
        /* r.add_output() */
        reader.read();
        reader.read();
        reader.read();
      }
      else if (reader.is_element("configuration")) {
        r->set_configuration(configuration::from_xml(reader));
      }
    }

    return (r);
  }
}
