// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool_manager.cpp
/// \brief Add your file description here.

#include <algorithm>
#include <functional>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/foreach.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"
#include "tipi/visitors.hpp"

#include "tipi/detail/controller.ipp"

#include "settings_manager.hpp"
#include "tool_manager.ipp"
#include "task_monitor.hpp"
#include "executor.hpp"
#include "command.hpp"
#include "processor.hpp"
#include "executor.hpp"

namespace squadt {

  using execution::command;

  const boost::shared_ptr < tipi::tool::capabilities > tool::no_capabilities(new tipi::tool::capabilities());

  /// \cond INTERNAL_DOCS
  tool_manager_impl::tool_manager_impl()
                            : tipi::controller::communicator() {
    struct local {
      /**
       * \param[in] m the incoming message
       **/
      static void handle_relay_connection(tool_manager_impl& owner, boost::shared_ptr< const tipi::message >& m) {
        instance_identifier id = boost::lexical_cast< instance_identifier > (m->to_string());

        if (owner.instances.find(id) == owner.instances.end()) {
          static_cast< transporter& >(owner).disconnect(*(m->get_originator()));

          owner.get_logger().log(1, "connection terminated; peer provided invalid instance identifier");

          return;
        }

        boost::shared_ptr < execution::task_monitor > monitor(owner.instances[id]);

        if (monitor) {
          owner.relay_connection(monitor.get(), const_cast < transport::transceiver::basic_transceiver* > (m->get_originator()));

          owner.instances.erase(id);

          /* Signal the listener that a connection has been established */
          monitor->signal_connection(m->get_originator());
        }
      }
    };

    /* Set handler for incoming instance identification messages */
    add_handler(tipi::message_identification, boost::bind(&local::handle_relay_connection, boost::ref(*this), _1));
  }

  void tool_manager_impl::activate(tipi::tcp_port port) {
    tcp_port_number = port;

    /* Listen for incoming socket connections on the default interface with the default port */
    add_listener("", tcp_port_number);
  }

  /**
   * \param[in] c the command to run
   * \param[in] p pointer to the associated monitor
   * \param[in] b whether or not to circumvent the executor restriction mechanism
   **/
  void tool_manager_impl::execute(execution::command const* c, boost::shared_ptr< execution::task_monitor > p, bool b) {
    global_build_system.get_executor().execute(*c, p, b);
  }

  /**
   * \param[in] t the tool that is to be run
   * \param[in] p the monitor that should be passed the feedback of execution
   * \param[in] b whether or not to circumvent the executor restriction mechanism
   * \param[in] w the directory in which execution should take place
   **/
  void tool_manager_impl::execute(tool const& t, boost::filesystem::path const& w, boost::shared_ptr< execution::task_monitor > p, bool b) {
    static const char* socket_connect_pattern("--si-connect=tipi://%s:%s");
    static const char* identifier_pattern("--si-identifier=%s");
    static const char* log_filter_level_pattern("--si-log-filter-level=%s");

    // For generating unique identifiers
    static instance_identifier id = 0;

    ++id;

    execution::command c(t.get_location().string(), w);

    c.append_argument(boost::str(boost::format(socket_connect_pattern)
                            % boost::asio::ip::address_v4::loopback() % tcp_port_number));
    c.append_argument(boost::str(boost::format(identifier_pattern)
                            % id));
    c.append_argument(boost::str(boost::format(log_filter_level_pattern)
                            % boost::lexical_cast < std::string > (static_cast < unsigned int > (get_default_logger().get_default_filter_level()))));

    // Security note, should remove again if it is not matched to a process within some reasonable amount of time
    instances[id] = p;

    global_build_system.get_executor().execute(c, p, b);
  }

  /**
   * \brief Simple processor that queries a tool's capabilities
   *
   * Extracts the tool information that is important for operation of the tool
   * manager.
   **/
  class extractor : public execution::task_monitor {

    public:

      /** \brief Starts the extraction */
      bool operator()(boost::weak_ptr < extractor > const& e, boost::shared_ptr < tool > const& t) {
        struct local {
          static void store_capabilities(boost::weak_ptr < extractor > e, boost::shared_ptr< const tipi::message >& m, boost::shared_ptr < tool > t) {
            boost::shared_ptr < extractor > guard(e.lock());
       
            try {
              if (guard) {
                t->m_capabilities.reset(new tipi::tool::capabilities);
  
                tipi::visitors::restore(*t->m_capabilities, m->to_string());

                return;
              }
            }
            catch(...) {
            }

            t->m_capabilities.reset();
          }
        };
       
        boost::shared_ptr < extractor > guard(e.lock());
        
        if (guard) {
          try {
            add_handler(tipi::message_capabilities, bind(&local::store_capabilities, e, _1, t));
       
            /* Await connection */
            if (await_connection(5)) {
              request_tool_capabilities();
            }

            await_message(tipi::message_capabilities, 2);
        
            finish();
          }
          catch (...) {
            // ignore exceptions, the capabilities object is the only thing that matters
          }
        }
       
        return t->m_capabilities.get() != 0;
      }
  };

  /**
   * \param[in] t the tool to run
   *
   * \attention This function blocks.
   **/
  bool tool_manager_impl::query_tool(boost::shared_ptr < tool > const& t) {
    /* Sanity check: establish existence of program binary */
    if (!t->get_location().empty() && boost::filesystem::exists(t->get_location())) {

      /* Extractor object, for retrieving the data from the running tool process */
      boost::shared_ptr < extractor > e(new extractor());
 
      execute(*t, boost::filesystem::current_path().string(),
                 boost::dynamic_pointer_cast < execution::task_monitor > (e), true);
 
      /* Wait until the process has been identified */
      boost::shared_ptr < execution::process > p(e->get_process(true));
 
      if (p.get() != 0) {
        /* Start extracting */
        return (*e)(e, t);
      }
    }

    get_logger().log(1, "Request for interface failed (tool `" + t->get_name()+ "')");

    return false;
  }

  void tool_manager_impl::terminate() {
    /* Request the local tool executor to terminate the running processes known to this tool manager */
    for (validated_instance_list::const_iterator i = validated_instances.begin(); i != validated_instances.end(); ++i) {
      global_build_system.get_executor().terminate((*i)->get_process());
    }
  }

  void tool_manager_impl::shutdown() {
    using namespace execution;

    terminate();

    disconnect();
  }
  
  /**
   * \param[in] n the name of the tool
   **/
  boost::shared_ptr< const tool > tool_manager_impl::find(const std::string& n) const {
    using namespace boost;

    boost::shared_ptr< const tool > t;

    for (tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
      if ((*i)->get_name() == n) {
        t = *i;

        break;
      }
    }

    return (t);
  }

  /**
   * \param[in] n the name of the tool
   **/
  bool tool_manager_impl::exists(std::string const& n) const {
    using namespace boost;

    return (find(n).get() != 0);
  }

  /**
   * \param[in] n the name of the tool
   * \param[in] l the location of the tool
   *
   * \return whether the tool was added or not
   **/
  bool tool_manager_impl::add_tool(const std::string& n, const boost::filesystem::path& l) {
    bool b = exists(n);

    if (!b) {
      tools.push_back(boost::shared_ptr< tool >(new tool(n, l)));
    }

    return (b);
  }

  /**
   * \param[in] t the name of the tool
   **/
  bool tool_manager_impl::add_tool(tool const& t) {
    bool b = exists(t.get_name());

    if (!b) {
      tools.push_back(boost::shared_ptr< tool >(new tool(t)));
    }

    return (b);
  }

  /** \brief Get the list of known tools */
  tool_manager::const_tool_sequence tool_manager_impl::get_tools() const {
    return tool_manager::const_tool_sequence(tools);
  }
  /// \endcond

  tool_manager::tool_manager() :
                                        impl(new tool_manager_impl()) {
  }

  tool_manager::tool_manager(tipi::tcp_port port) :
                                        impl(new tool_manager_impl()) {
    impl->activate(port);
  }

  /**
   * \param[in] n the name of the tool
   **/
  bool tool_manager::exists(std::string const& n) const {
    return (impl->exists(n));
  }

  /**
   * \param[in] n the name of the tool
   **/
  boost::shared_ptr< const tool > tool_manager::find(const std::string& n) const {
    return impl->find(n);
  }

  /**
   * @param n the name of the tool
   *
   * \pre a tool with this name must be among the known tools
   **/
  boost::shared_ptr < const tool > tool_manager::get_tool_by_name(std::string const& n) const {
    boost::shared_ptr< const tool > t = impl->find(n);

    /* Check tool existence */
    if (!t) {
      throw std::runtime_error("No tool has been registered by the name " + n);
    }

    return (t);
  }

  /**
   * \param[in] n the name of the tool
   * \param[in] l the location of the tool
   *
   * \return whether the tool was added or not
   **/
  bool tool_manager::add_tool(const std::string& n, const boost::filesystem::path& l) {
    return (impl->add_tool(n, l));
  }

  /**
   * \param[in] t the name of the tool
   **/
  bool tool_manager::add_tool(tool const& t) {
    return (impl->add_tool(t));
  }
 
  void tool_manager::query_tools() {
    tool_manager::const_tool_sequence tools(get_tools());

    tool_list retry_list;

    BOOST_FOREACH(tool_list::value_type t, tools) {
      if (!impl->query_tool(t)) {
        retry_list.push_back(t);
      }
    }

    /* Retry initialisation of failed tools */
    BOOST_FOREACH(tool_list::value_type t, retry_list) {
      impl->query_tool(t);
    }
  }

  /**
   * \param[in] t the tool that is to be run
   *
   * \attention This function blocks.
   **/
  bool tool_manager::query_tool(boost::shared_ptr < tool > const& t) {
    return impl->query_tool(t);
  }

  /** \brief Get the list of known tools */
  tool_manager::const_tool_sequence tool_manager::get_tools() const {
    return boost::make_iterator_range(impl->tools);
  }
 
  /** \brief Get the number of known tools */
  unsigned int tool_manager::number_of_tools() const {
    return impl->tools.size();
  }

  /** \brief Have the tool executor terminate all running tools */
  void tool_manager::terminate() {
    impl->terminate();
  }

  /** \brief Have the tool executor terminate all running tools */
  void tool_manager::shutdown() {
    impl->shutdown();
  }
}

