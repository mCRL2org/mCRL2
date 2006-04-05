#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/ref.hpp>

#include <xml2pp/detail/text_reader.tcc>
#include <sip/detail/basic_messenger.tcc>

#include "executor.h"
#include "tool_manager.h"
#include "processor.tcc"
#include "task_monitor.h"
#include "extractor.h"
#include "settings_manager.tcc"
#include "core.h"

#include "setup.h"

namespace squadt {

  /** \brief Socket connection option scheme for easy command generation */
  const char* socket_connect_pattern = "--si-connect=socket://%s:%s";

  /** \brief Identifier option scheme for easy command generation */
  const char* identifier_pattern     = "--si-identifier=%s";

  const long tool_manager::default_tcp_port = 10946;

  tool_manager::tool_manager() : sip::controller::communicator(), free_identifier(0) {
    /* Listen for incoming socket connections on the loopback interface with the default port */
    add_listener();

    /* Set handler for incoming instance identification messages */
    set_handler(boost::bind(&tool_manager::handle_relay_connection, this, _1, _2), sip::send_instance_identifier);
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
  template < typename T >
  void tool_manager::execute(tool& t, T p) {
    instance_identifier id = free_identifier++;

    execution::command c(t.get_location());

    c.append_argument(boost::str(boost::format(socket_connect_pattern)
                            % get_local_host().name() % default_tcp_port));
    c.append_argument(boost::str(boost::format(identifier_pattern)
                            % id));

    instances[id] = p;

    local_executor.execute(c, p);
  }

  void tool_manager::query_tools() {
    using namespace boost;

    std::for_each(tools.begin(), tools.end(),
                    bind(&tool_manager::query_tool, this, 
                                    bind(&tool::ptr::operator*, _1)));
  }

  /**
   * @param h a function that is called with the name of a tool before it is queried
   **/
  void tool_manager::query_tools(boost::function < void (const std::string&) > h) {
    using namespace boost;

    for (tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
      h((*i)->get_name());

      query_tool(**i);
    }
  }

  /**
   * @param t the tool that is to be run
   *
   * \attention This function blocks.
   **/
  bool tool_manager::query_tool(tool& t) {
    /* Sanity check: establish tool existence */
    if (t.get_location().empty() || !boost::filesystem::exists(boost::filesystem::path(t.get_location()))) {
      throw (exception(exception_identifier::requested_tool_unavailable, t.get_name()));
    }

    /* Create extractor object, that will retrieve the data from the running tool process */
    boost::shared_ptr < extractor > e(new extractor(t));

    execute(t, boost::dynamic_pointer_cast < execution::task_monitor, extractor > (e));

    execution::process::ptr p(e->get_process(true));

    /* Wait until the process has been identified */
    if (p.get() != 0) {
      /* Start extracting */
      e->start();

      /* Wait for extraction process to complete */
      e->await_completion();

      /* Disconnect any connection to the process */
      e->disconnect(p.get());

      local_executor.terminate(p);

      return (true);
    }

    return (false);
  }

  void tool_manager::terminate() {
    using namespace execution;

    /* Request the local tool executor to terminate the running processes known to this tool manager */
    for (validated_instance_list::const_iterator i = validated_instances.begin(); i != validated_instances.end(); ++i) {
      local_executor.terminate((*i)->get_process());
    }
  }

  /**
   * @param m the message that was just delivered
   * @param o the local end point through which the message was received
   **/
  void tool_manager::handle_relay_connection(const sip::message_ptr& m, const sip::end_point o) {
    instance_identifier id = atol(m->to_string().c_str());

    if (instances.find(id) == instances.end()) {
      throw (exception(exception_identifier::unexpected_instance_identifier));
    }

    execution::task_monitor::ptr p = instances[id];

    relay_connection(p.get(), const_cast < transport::transceiver::basic_transceiver* > (o));

    /* Signal the listener that a connection has been established */
    p->signal_connection(o);

    instances.erase(id);
  }
}

