#include <boost/filesystem/operations.hpp>

#include <xml2pp/detail/text_reader.tcc>
#include <sip/controller.h>

#include "task_monitor.h"
#include "processor.tcc"
#include "exception.h"
#include "core.h"

namespace squadt {

  void processor::reporter::dummy(output_status) {
  }

  /**
   * Currently it is only checked whether a status of up_to_date is still
   * correct with respect to the state of the inputs and ouputs on physical
   * storage. If the processor is in any other state the function will return
   * false without doing checks.
   *
   * Other checks simply do not look useful at this point given the way a
   * processor is used.
   *
   * \return whether the status was adjusted or not
   *
   * @param[in] r whether to check recursively or not
   **/
  inline bool processor::check_status(const bool r) {
    using namespace boost::filesystem;

    if (current_output_status == up_to_date) {
      output_status new_status = current_output_status;
     
      time_t maximum_input_timestamp  = 0;
      time_t minimum_output_timestamp = 0;
     
      /* Check whether outputs all exist and find the minimum timestamp of the inputs */
      for (output_list::const_iterator i; i != outputs.end(); ++i) {
        path l((*i)->location);
     
        if (exists(l)) {
          /* Output exists, get timestamp */ 
          minimum_output_timestamp = std::min(minimum_output_timestamp, last_write_time(l));
        }
        else {
          /* Output does not exist; consequently the output is not up-to-date */
          new_status = not_up_to_date;

          break;
        }
      }
     
      /* Find the maximum timestamp of the inputs */
      for (input_list::const_iterator i; i != inputs.end(); ++i) {
        object_descriptor::sptr d = (*i).lock();
     
        if (d.get() == 0) {
          throw (exception(exception_identifier::missing_object_descriptor));
        }
     
        path l(d->location);
     
        if (exists(l)) {
          /* Input exists, get timestamp */ 
          time_t stamp = last_write_time(l);
     
          maximum_input_timestamp = std::max(maximum_input_timestamp, stamp);
     
          if (d->timestamp < stamp) {
            /* Compare checksums and update recorded checksum */
            md5pp::compact_digest old = d->checksum;
     
            d->timestamp = stamp;
            d->checksum  = md5pp::MD5::MD5_sum(l);
     
            if (old != d->checksum) {
              new_status = not_up_to_date;
     
              break;
            }
          }
        }
      }
     
      if (minimum_output_timestamp <= maximum_input_timestamp) {
        new_status = not_up_to_date;
      }
      else if (r && current_output_status <= new_status) {
        /* Status can still be okay, check recursively */
        for (input_list::const_iterator i; i != inputs.end(); ++i) {
          object_descriptor::sptr d = (*i).lock();
     
          if (d.get() == 0) {
            throw (exception(exception_identifier::missing_object_descriptor));
          }
     
          if (d->generator->check_status(true)) {
            new_status = not_up_to_date;
     
            break;
          }
        }
      }
     
      if (new_status < current_output_status) {
        current_output_status = new_status;
     
        return (true);
      }
    }

    return (false);
  }

  /**
   * @param s the stream to write to
   **/
  void processor::write(std::ostream& s) const {
    s << "<processor";

    if (tool_descriptor.get() != 0) {
      s << " tool-name=\"" << tool_descriptor->get_name() << "\"";
    }

    s << ">\n";

    /* The last received configuration from the tool */
    sip::configuration::ptr c = monitor->get_configuration();

    if (c.get() != 0) {
      c->write(s);
    }

    /* The inputs */
    for (input_list::const_iterator i = inputs.begin(); i != inputs.end(); ++i) {
      s << "<input id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).lock().get()) << "\"/>";
    }

    /* The outputs */
    for (output_list::const_iterator i = outputs.begin(); i != outputs.end(); ++i) {
      s << "<output id=\"" << std::dec << reinterpret_cast < unsigned long > ((*i).get())
        << "\" format=\"" << (*i)->format
        << "\" location=\"" << (*i)->location
        << "\" digest=\"" << (*i)->checksum
        << "\" timestamp=\"" << std::dec << (*i)->timestamp << "\"/>";
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
    processor::ptr c(new processor());
    std::string    temporary;

    if (r.get_attribute(&temporary, "tool-name")) {
      c->tool_descriptor = global_tool_manager->find(temporary);

      /* Check tool existence */
      if (!global_tool_manager->exists(temporary)) {
        throw (exception(exception_identifier::requested_tool_unavailable, temporary));
      }
    }

    r.read();

    if (r.is_element("configuration")) {
      c->monitor->set_configuration(sip::configuration::read(r));
    }

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

      r.skip_end_element("input");
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

      if (!(b && r.get_attribute(&n->format, "format")
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

