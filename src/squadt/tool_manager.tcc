#include "tool_manager.h"
#include "task_monitor.h"
#include "executor.h"

namespace squadt {
  /// \cond PRIVATE_PART

  class tool_manager_impl : public utility::visitable< tool_manager_impl >, public sip::controller::communicator {
    friend class tool_manager;
    friend class processor_impl;
    friend class preferences_read_visitor_impl;

    private:
 
      /** \brief Numeric type for instance identification */
      typedef long int                                                        instance_identifier;

      /** \brief Maps an instance identifier to its associated task */
      typedef std::map < instance_identifier, execution::task_monitor::sptr > instance_list;

      /** \brief Maps a task to its associated process */
      typedef std::list < execution::task_monitor::sptr >                     validated_instance_list;

      /** \brief Convenient type alias from interface class */
      typedef tool_manager::tool_list                                         tool_list;

      /** \brief Convenient type alias from interface class */
      typedef tool_manager::tool_const_sequence                               tool_const_sequence;

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

      /** \brief Used to obtain unused instance identifiers */
      mutable instance_identifier free_identifier;

    private:

      /** \brief Start a tool */
      void execute(tool&, std::string const&, execution::task_monitor::sptr, bool);

      /** \brief Execute a command */
      void execute(execution::command const*, execution::task_monitor::sptr, bool);

      /** \brief This is the event handler for incoming identification messages */
      void handle_relay_connection(sip::message_ptr const&);

    public:

      /** \brief Default constructor */
      tool_manager_impl();

      /** \brief Establishes whether the named tool is among the known tools or not */
      bool exists(std::string const&) const;

      /** \brief Returns a tool by its name */
      tool::sptr find(std::string const&) const;

      /** \brief Returns a tool by its name */
      tool::sptr get_tool_by_name(std::string const&) const;

      /** \brief Add a new tool to the catalog */
      bool add_tool(std::string const&, std::string const&);
 
      /** \brief Add a new tool to the catalog */
      bool add_tool(tool const&);

      /** \brief Get the list of known tools */
      tool_const_sequence get_tools() const;
 
      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools();

      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools(boost::function < void (std::string const&) >);

      /** \brief Get the tool_capabilities object for a tool */
      bool query_tool(tool&);

      /** \brief Get the number of known tools */
      const unsigned int number_of_tools() const;

      /** \brief Have the tool executor terminate all running tools */
      void terminate();
  };

  /// \endcond
}
