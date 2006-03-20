#include "processor.h"
#include "exception.h"

#include <boost/filesystem/operations.hpp>

#include <xml2pp/detail/text_reader.tcc>

#include "ui_core.h"

namespace squadt {

  void processor::dummy_visualiser(output_status) {
  }

  /**
   * @param s the stream to write to
   **/
  void processor::write(std::ostream& s) const {
    s << "<processor tool-name=\"" << program.get_name() << "\">";

    /* The last received configuration from the tool */
    if (configuration.get() != 0) {
      configuration->write(s);
    }

    /* The inputs */
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      s << "<input id=\"" << reinterpret_cast < unsigned long > ((*i).lock().get()) << "\"/>";
    }

    /* The outputs */
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      s << "<output id=\"" << reinterpret_cast < unsigned long > ((*i).get())
        << "\" format=\"" << (*i)->storage_format
        << "\" location=\"" << (*i)->location
        << "\" digest=\"" << (*i)->checksum
        << "\" timestamp\"" << (*i)->timestamp << "\"/>";
    }

    s << "</processor>";
  }

  /**
   * @param r an XML text reader object to read from
   * @param m a map that is used to associate shared pointers to processors with identifiers
   *
   * \pre must point to a processor element
   * \attention the same map m must be used to read back all processor instances that were written with write()
   **/
  processor::ptr processor::read(id_conversion_map& m, xml2pp::text_reader& r) throw () {
    std::string temporary;
    
    if (!r.get_attribute(&temporary, "tool-name")) {
      throw (exception(exception_identifier::required_attributes_missing, "processor"));
    }

    r.read();

    /* Check tool existence */
    if (!global_tool_manager->exists(temporary)) {
      throw (exception(exception_identifier::requested_tool_unavailable, temporary));
    }

    /* Get tool object for tool name */
    tool& t = global_tool_manager->find(temporary);

    processor::ptr c(new processor(t));

    c->configuration = sip::configuration::read(r);

    /* Read inputs */
    while (r.is_element("input")) {
      unsigned long id;

      if (!r.get_attribute(&id, "id")) {
        throw (exception(exception_identifier::required_attributes_missing, "processor->input"));
      }
      else {
        assert(m.find(id) != m.end());

        c->inputs.push_back(object_descriptor::wptr(m[id]));
      }

      r.read();

      r.skip_end_element("output");
    }

    /* Read outputs */
    while (r.is_element("output")) {
      unsigned long id;
      bool          b = r.get_attribute(&id, "id");

      if (b) {
        assert(m.find(id) == m.end());

        m[id] = object_descriptor::sptr(new object_descriptor);

        c->outputs.push_back(m[id]);
      }

      object_descriptor* n = m[id].get();

      if (!(b && r.get_attribute(&n->storage_format, "format")
              && r.get_attribute(&n->location, "location")
              && r.get_attribute(&temporary, "digest")
              && r.get_attribute(&n->timestamp, "timestamp"))) {

        throw (exception(exception_identifier::required_attributes_missing, "processor->output"));
      }

      n->checksum.read(temporary.c_str());

      r.read();

      r.skip_end_element("output");
    }

    return (c);
  }

  void processor::flush_outputs() {
    using namespace boost::filesystem;

    set_output_status(non_existent);

    /* Make sure any output objects are removed from storage */
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      path p((*i)->location);

      if (exists(p)) {
        remove(p);
      }
    }
  }
}

