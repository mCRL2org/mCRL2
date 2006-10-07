#include <fstream>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>

#include <xml2pp/text_reader.h>

#include "sip/detail/controller.tcc"
#include "executor.h"
#include "tool_manager.h"
#include "task_monitor.h"
#include "extractor.h"
#include "settings_manager.tcc"

#include "setup.h"

namespace squadt {

  namespace bf = boost::filesystem;

  /** \brief Socket connection option scheme for easy command generation */
  const char* socket_connect_pattern = "--si-connect=socket://%s:%s";

  /** \brief Identifier option scheme for easy command generation */
  const char* identifier_pattern     = "--si-identifier=%s";

  const long tool_manager::default_tcp_port = 10946;

  const sip::tool::capabilities::ptr tool::no_capabilities(new sip::tool::capabilities());

  char const* tool_manager::default_tools[] = {"lpeconstelm", "lpeinfo", "lpeparelm", "lpeuntime", "lpe2lts", "lpesumelm",
                                               "ltsconvert", "ltsinfo", "ltsgraph", "ltsview", "mcrl22lpe", "xsim", 0};

  tool_manager::tool_manager() : sip::controller::communicator(), free_identifier(0) {
    /* Listen for incoming socket connections on the loopback interface with the default port */
    impl->add_listener();

    /* Set handler for incoming instance identification messages */
    add_handler(sip::message_instance_identifier, boost::bind(&tool_manager::handle_relay_connection, this, _1));
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
    boost::filesystem::path p(global_settings_manager->path_to_user_settings(settings_manager::tool_catalog_base_name));

    if (!boost::filesystem::exists(p)) {
      /* Write the default configuration */;
      boost::format f(" <tool name=\"%s\" location=\"%s\"/>\n");

      std::ofstream o(p.native_file_string().c_str());

      o << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << "<tool-catalog xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"tool_catalog.xsd\" version=\"1.0\">\n";

      bf::path default_path(global_settings_manager->path_to_default_binaries());

      for (char const** tool = default_tools; *tool != 0; ++tool) {
        o << (f % *tool % (default_path / bf::path(*tool)).native_file_string());
      }

      o << "</tool-catalog>\n";

      o.close();
    }

    return (read(p.native_file_string()));
  }

  /**
   * \param[in] n the name of the file to read from
   **/
  tool_manager::ptr tool_manager::read(const std::string& n) {
    bf::path p(n);

    if (!bf::exists(p)) {
      throw (exception::exception(exception::failed_loading_object, "squadt tool catalog", p.native_file_string()));
    }

    xml2pp::text_reader reader(p);

    reader.set_schema(bf::path(
                            global_settings_manager->path_to_schemas(
                                    settings_manager::append_schema_suffix(
                                            settings_manager::tool_catalog_base_name))));

    return (read(reader));
  }

  /**
   * \param[in] r an XML text reader to use to read data from
   **/
  tool_manager::ptr tool_manager::read(xml2pp::text_reader& r) {

    tool_manager::ptr new_tool_manager(new tool_manager());
 
    /* Read root element (tool-catalog) */
    r.next_element();

    while (!r.is_end_element("tool-catalog")) {
      /* Add a new tool to the list of tools */
      new_tool_manager->tools.push_back(tool::read(r));
    }

    return (new_tool_manager);
  }

  /**
   * \param[in] t the tool that is to be run
   * \param[in] p the monitor that should be passed the feedback of execution
   * \param[in] b whether or not to circumvent the executor restriction mechanism
   * \param[in] w the directory in which execution should take place
   **/
  template < typename T >
  void tool_manager::execute(tool& t, std::string const& w, T p, bool b) {
    instance_identifier id = free_identifier++;

    execution::command c(t.get_location(), w);

    c.append_argument(boost::str(boost::format(socket_connect_pattern)
                            % impl->get_local_host() % default_tcp_port));
    c.append_argument(boost::str(boost::format(identifier_pattern)
                            % id));

    instances[id] = p;

    local_executor.execute(c, p, b);
  }

  void tool_manager::query_tools() {
    using namespace boost;

    BOOST_FOREACH(tool_list::value_type t, tools) {
      query_tool(*t);
    }
  }

  /**
   * \param[in] h a function that is called with the name of a tool before it is queried
   **/
  void tool_manager::query_tools(boost::function < void (const std::string&) > h) {
    using namespace boost;

    tool_list failed_tools;

    for (tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
      h((*i)->get_name());

      if (!query_tool(*(*i))) {
        failed_tools.push_back(*i);
      }
    }

    /* Retry initialisation of failed tools */
    for (tool_list::const_iterator i = failed_tools.begin(); i != failed_tools.end(); ++i) {
      h((*i)->get_name());

      if (!query_tool(*(*i))) {
        /* TODO log failure */
      }
    }
  }

  /**
   * \param[in] t the tool that is to be run
   *
   * \attention This function blocks.
   **/
  bool tool_manager::query_tool(tool& t) {
    /* Sanity check: establish tool existence */
    if (t.get_location().empty() || !boost::filesystem::exists(boost::filesystem::path(t.get_location()))) {
      return (false);
    }

    /* Create extractor object, that will retrieve the data from the running tool process */
    boost::shared_ptr < extractor > e(new extractor(t));

    execute(t, boost::filesystem::current_path().native_file_string(),
               boost::dynamic_pointer_cast < execution::task_monitor > (e), false);

    /* Wait until the process has been identified */
    execution::process::ptr p(e->get_process(true));

    if (p.get() != 0) {
      /* Start extracting */
      e->extract();

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
   * \param[in] m the message that was just delivered
   **/
  void tool_manager::handle_relay_connection(sip::message_ptr const& m) {
    instance_identifier id = atol(m->to_string().c_str());

    if (instances.find(id) == instances.end()) {
      throw (exception::exception(exception::unexpected_instance_identifier));
    }

    execution::task_monitor::sptr p = instances[id];

    impl->relay_connection(p->impl.get(), const_cast < transport::transceiver::basic_transceiver* > (m->get_originator()));

    /* Signal the listener that a connection has been established */
    p->signal_connection(m->get_originator());

    instances.erase(id);
  }
}

