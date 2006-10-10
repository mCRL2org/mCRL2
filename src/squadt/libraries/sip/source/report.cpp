#include <algorithm>
#include <ostream>

#include <xml2pp/text_reader.h>

#include <sip/detail/report.h>
#include <sip/detail/exception.h>

namespace sip {

  void report::write(std::ostream& output) const {
    using sip::exception;

    output << "<report type=\"" << report_type << "\">";

    /* Include description */
    if (!description.empty()) {
      const std::string pattern("]]>");

      /* Sanity check... (todo better would be to use Base-64 or some other encoding) */
      if (std::search(description.begin(), description.end(), pattern.begin(), pattern.end()) != description.end()) {
        throw (new exception(sip::message_forbidden_content, pattern));
      }

      output << "<description><![CDATA[" << description << "]]></description>";
    }

    output << "</report>";
  }

  /** \pre the reader must point at a report element */
  report::sptr report::read(xml2pp::text_reader& r) throw () {
    int         t = notice;
    std::string d;

    assert(r.is_element("report"));

    r.get_attribute(&t, "type");

    r.next_element();

    if (r.is_element("description")) {
      r.next_element();

      r.get_value(&d);

      r.next_element();
    }

    r.skip_end_element("report");

    report::sptr l(new report(static_cast < type > (t), d));

    return (l);
  }
}
