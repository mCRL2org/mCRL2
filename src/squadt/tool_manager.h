#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <algorithm>
#include <functional>
#include <list>
#include <iosfwd>
#include <map>
#include <ostream>
#include <string>

#include <boost/bind.hpp>

#include <sip/controller.h>

#include "tool.h"
#include "task_monitor.h"
#include "processor.h"
#include "executor.h"

namespace squadt {

  /**
   * \brief Basic component that provides information about a set of tools
   *
   * The tool manager is a special purpose database that is used to keep track
   * of a set of tools across system runs. To this end it can store and
   * retrieve tool information to/from persistent storage. Additionally the
   * tool manager provides functionality to help run, and once running, to
   * terminate tools.
   *
   * The tool manager sets the appropriate command line options and handles the
   * initial connection with the tool. A tool (instance) receives a unique
   * identifier, which is communicated as a command line argument (e.g. via
   * `--si-identifier=4' for the identifier 4). As a first act of a new
   * connection the tool communicates the identifier. Effectively it identifies
   * itself to the tool manager. The tool manager then passes through the
   * connection to the processor that requested the tool to be started in the
   * first place.
   *
   * Design choices:
   *  - a task is the unit of work it denotes the execution of a single command
   *  - the tool_manager and executor are separated such the that tool manager
   *    act as a proxy, and in the future do load balancing for a number
   *    executors that may run on different machines
   **/
  class tool_manager : public sip::controller::communicator {
    friend class processor_impl;
 
    public:
 
      /** \brief Convenience type alias for hiding shared pointer implementation */
      typedef boost::shared_ptr < tool_manager >                              ptr;

      /** \brief Convenience type alias the list of tools */
      typedef std::list < tool::sptr >                                        tool_list;
 
      /** \brief Convenience type alias the list of tools, indexed by main input format */
      typedef std::multimap < storage_format, tool::sptr >                    tool_map;

    private:
 
      /** \brief Numeric type for instance identification */
      typedef long int                                                        instance_identifier;

      /** \brief Maps an instance identifier to its associated task */
      typedef std::map < instance_identifier, execution::task_monitor::sptr > instance_list;

      /** \brief Maps a task to its associated process */
      typedef std::list < execution::task_monitor::sptr >                     validated_instance_list;

    private:
 
      /** \brief Type for output print format variants */
      enum output_variant {
        plain_text,          ///< Output is formatted plain text
        squadt_tool_catalog  ///< Output is squadt XML tool catalog format
      };

      /** \brief The default TCP port for a tool manager */
      static const long   default_tcp_port;

      /** \brief Set of tool names that are assumed to be available */
      static char const*  default_tools[];

    private:
 
      /** \brief List of known tools */
      tool_list                   tools;

      /** \brief Assigns a unique instance identifier to a task */
      instance_list               instances;

      /** \brief Maps an instance identifier to its associated processor */
      validated_instance_list     validated_instances;

      /** \brief Local executor for executing tools on the current machine */
      execution::executor         local_executor;

      /** \brief Used to obtain unused instance identifiers */
      mutable instance_identifier free_identifier;

    private:

      /** \brief Start a tool */
      template < typename T >
      void execute(tool&, std::string const&, T, bool);

      /** \brief This is the event handler for incoming identification messages */
      void handle_relay_connection(sip::message_ptr const&);

    public:
 
      /** \brief Constructor */
      tool_manager();
 
      /** \brief Destructor */
      inline ~tool_manager();
 
      /** \brief Write configuration to stream */
      void write(std::ostream& = std::cout) const;
 
      /** \brief Read configuration from the default location */
      static tool_manager::ptr read();

      /** \brief Read configuration from file */
      static tool_manager::ptr read(const std::string&);

      /** \brief Read configuration with an XML text reader */
      static tool_manager::ptr read(xml2pp::text_reader&);
 
      /** \brief Establishes whether the named tool is among the known tools or not */
      inline bool exists(const std::string&) const;

      /** \brief Returns a tool by its name */
      inline tool::sptr find(const std::string&) const;

      /** \brief Add a new tool to the catalog */
      inline bool add(const std::string&, const std::string&);
 
      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools();

      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools(boost::function < void (const std::string&) >);

      /** \brief Get the tool_capabilities object for a tool */
      bool query_tool(tool&);

      /** \brief Get the list of known tools */
      inline const tool_list& get_tools();
 
      /** \brief Get the number of known tools */
      inline const unsigned int number_of_tools() const;

      /** \brief Gets the maximum number of tool instances */
      size_t get_maximum_instance_count() const;

      /** \brief Sets the maximum number of tool instances */
      void set_maximum_instance_count(size_t);
 
      /** \brief Have the tool executor terminate all running tools */
      void terminate();
  };

  inline tool_manager::~tool_manager() {
    terminate();
  }

  inline const tool_manager::tool_list& tool_manager::get_tools() {
    return (tools);
  }

  inline const unsigned int tool_manager::number_of_tools() const {
    return (tools.size());
  }

  /**
   * @param n the name of the tool
   *
   * \pre a tool with this name must be among the known tools
   **/
  inline tool::sptr tool_manager::find(const std::string& n) const {
    using namespace boost;

    return (*std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::sptr::get, _1)))));
  }

  /**
   * @param n the name of the tool
   **/
  inline bool tool_manager::exists(const std::string& n) const {
    using namespace boost;

    return (tools.end() != std::find_if(tools.begin(), tools.end(), 
               bind(std::equal_to< std::string >(), n, 
                       bind(&tool::get_name,
                               bind(&tool::sptr::get, _1)))));
  }

  /**
   * @param n the name of the tool
   * @param l the location of the tool
   *
   * \return whether the tool was added or not
   **/
  inline bool tool_manager::add(const std::string& n, const std::string& l) {
    bool b = exists(n);

    if (!b) {
      tools.push_back(tool::sptr(new tool(n, l)));
    }

    return (b);
  }
}

#endif
