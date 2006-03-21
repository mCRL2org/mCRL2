#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/ref.hpp>

#include <xml2pp/detail/text_reader.tcc>
#include <sip/detail/basic_messenger.tcc>

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "ui_core.h"

#include "setup.h"

namespace squadt {

  /** \brief Scheme for easy command generation */
  const char* default_argument_pattern = "%s --si-connect=socket://%s:%s --si-identifier=%s";

  const long tool_manager::default_tcp_port = 10946;

  tool_manager::tool_manager() : free_identifier(0) {
    /* Listen for incoming socket connections on the loopback interface with the default port */
    add_listener();

    /* Set handler for incoming instance identification messages */
    set_handler(boost::bind(&tool_manager::handle_relay_connection, this, _1), sip::send_instance_identifier);
  }

  void tool_manager::write(std::ostream& stream) const {
    const tool_list::const_iterator b = tools.end();
          tool_list::const_iterator i = tools.begin();
 
    /* Write header */
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
           << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
           << " xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";
 
    while (i != b) {
      (*i)->write(stream);
 
      ++i;
    }
 
    /* Write footer */
    stream << "</tool-catalog>\n";
  }

  tool_manager::ptr tool_manager::read() {
    return (read(global_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name)));
  }

  /**
   * @param n the name of the file to read from
   **/
  tool_manager::ptr tool_manager::read(const std::string& n) {
    xml2pp::text_reader::file_name< std::string > f(n);

    if (!bf::exists(bf::path(f.get()))) {
      throw (exception(exception_identifier::failed_loading_object, "squadt tool catalog", f.get()));
    }

    xml2pp::text_reader reader(f);

    reader.set_schema(xml2pp::text_reader::file_name< std::string >(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::tool_catalog_base_name))));

    return (read(reader));
  }

  /**
   * @param r an XML text reader to use to read data from
   **/
  tool_manager::ptr tool_manager::read(xml2pp::text_reader& r) {

    tool_manager::ptr new_tool_manager(new tool_manager());
 
    /* Read root element (tool-catalog) */
    r.read();
    r.read();
  
    while (!r.is_end_element()) {
      /* Add a new tool to the list of tools */
      new_tool_manager->tools.push_back(tool::read(r));
    }
 
    return (new_tool_manager);
  }

  /**
   * @param t the tool that is to be run
   * @param p the processor that should be passed the feedback of execution
   **/
  void tool_manager::execute(tool& t, processor* p) const {
    std::string command = t.get_location();
 
    /* TODO contact executor to execute */
  }

  void tool_manager::query_capabilities() throw () {
    using namespace boost;

    std::for_each(tools.begin(), tools.end(),
                    bind(&tool_manager::query_capabilities, this, 
                                    bind(&tool::ptr::operator*, _1)));
  }

  /**
   * @param t the tool that is to be run
   **/
  void tool_manager::query_capabilities(tool& t) throw () {
    boost::filesystem::path p(t.get_location());
 
    /* Sanity check: establish tool existence */
    if (!boost::filesystem::exists(p)) {
      throw (exception(exception_identifier::requested_tool_unavailable));
    }

    boost::format command(default_argument_pattern);

    command % p.native_file_string();
    command % get_local_host().name() % default_tcp_port;
    command % free_identifier++;

    local_executor.execute(boost::str(command), 0);

    /* TODO special processor?!? */
  }

  /**
   * @param m the message that was just delivered
   **/
  void tool_manager::handle_relay_connection(sip::message_ptr& m) {
    instance_identifier id = atol(m->to_string().c_str());

    if (instances.find(id) == instances.end()) {
      throw (exception(exception_identifier::unexpected_instance_identifier));
    }

//    instances[id].accept_instance_identifier(m);
    /* TODO finish?!? */
  }

  void tool_manager::terminate() {
    /* TODO signal executor to terminate the processes related to this tool manager */
  }
}

