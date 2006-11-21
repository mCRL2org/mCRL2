#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>

#include "sip/detail/controller.tcc"
#include "settings_manager.h"
#include "tool_manager.h"
#include "task_monitor.h"
#include "executor.h"
#include "command.h"
#include "extractor.h"

#include "setup.h"

namespace squadt {

  using execution::command;

  namespace bf = boost::filesystem;

  /** \brief Socket connection option scheme for easy command generation */
  const char* socket_connect_pattern = "--si-connect=socket://%s:%s";

  /** \brief Identifier option scheme for easy command generation */
  const char* identifier_pattern     = "--si-identifier=%s";

  const long tool_manager::default_tcp_port = 10947;

  const sip::tool::capabilities::sptr tool::no_capabilities(new sip::tool::capabilities());

  char const* tool_manager::default_tools[] = {"lpeconstelm", "lpedecluster", "lpeinfo", "lpeparelm", "lpeuntime", "lpe2lts", "lpesumelm",
                                               "ltsconvert", "ltsinfo", "ltsgraph", "ltsview", "mcrl22lpe", "pnml2mcrl2", "xsim", 0};

  tool_manager::tool_manager() : sip::controller::communicator(), free_identifier(0) {
    /* Listen for incoming socket connections on the loopback interface with the default port */
    impl->add_listener();

    /* Set handler for incoming instance identification messages */
    add_handler(sip::message_instance_identification, boost::bind(&tool_manager::handle_relay_connection, this, _1));
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

    global_build_system.get_executor()->execute(c, p, b);
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

    tool_list retry_list;

    BOOST_FOREACH(tool_list::value_type t, tools) {
      h(t->get_name());

      if (!query_tool(*t)) {
        retry_list.push_back(t);
      }
    }

    /* Retry initialisation of failed tools */
    BOOST_FOREACH(tool_list::value_type t, retry_list) {
      h(t->get_name());

      if (!query_tool(*t)) {
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

      global_build_system.get_executor()->terminate(p);

      return (true);
    }

    return (false);
  }

  void tool_manager::terminate() {
    using namespace execution;

    /* Request the local tool executor to terminate the running processes known to this tool manager */
    for (validated_instance_list::const_iterator i = validated_instances.begin(); i != validated_instances.end(); ++i) {
      global_build_system.get_executor()->terminate((*i)->get_process());
    }
  }

  /**
   * \param[in] n the name of the tool
   **/
  tool::sptr tool_manager::find(const std::string& n) const {
    using namespace boost;

    tool::sptr t;

    tool_list::const_iterator i = std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::sptr::get, _1))));

    if (i != tools.end()) {
      t = *i;
    }

    return (t);
  }

  /**
   * \param[in] n the name of the tool
   **/
  bool tool_manager::exists(std::string const& n) const {
    using namespace boost;

    return (tools.end() != std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::sptr::get, _1)))));
  }

  /**
   * \param[in] n the name of the tool
   * \param[in] l the location of the tool
   *
   * \return whether the tool was added or not
   **/
  bool tool_manager::add(const std::string& n, const std::string& l) {
    bool b = exists(n);

    if (!b) {
      tools.push_back(tool::sptr(new tool(n, l)));
    }

    return (b);
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

  /**
   * @param n the name of the tool
   *
   * \pre a tool with this name must be among the known tools
   **/
  boost::shared_ptr < tool > tool_manager::get_tool_by_name(std::string const& n) const {
    tool::sptr t = find(n);

    /* Check tool existence */
    if (!t) {
      throw (exception::exception(exception::requested_tool_unavailable, n));
    }

    return (t);
  }
}

