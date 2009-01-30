// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool_manager.ipp
/// \brief Add your file description here.

#ifndef TOOL_MANAGER_TCC__
#define TOOL_MANAGER_TCC__

#include "tool_manager.hpp"
#include "task_monitor.hpp"
#include "executor.hpp"

namespace squadt {
  /// \cond INTERNAL_DOCS

  class tool_manager_impl : private tipi::controller::communicator, public utility::visitable {
    friend class tool_manager;
    friend class processor_impl;

    template < typename R, typename S >
    friend class utility::visitor;

    private:

      /** \brief Numeric type for instance identification */
      typedef long int                                                                       instance_identifier;

      /** \brief Maps an instance identifier to its associated task */
      typedef std::map < instance_identifier, boost::shared_ptr< execution::task_monitor > > instance_list;

      /** \brief Maps a task to its associated process */
      typedef std::list < boost::shared_ptr< execution::task_monitor > >                     validated_instance_list;

      /** \brief Convenient type alias from interface class */
      typedef tool_manager::tool_list                                                        tool_list;

      /** \brief Convenient type alias from interface class */
      typedef tool_manager::const_tool_sequence                                              const_tool_sequence;

    private:

      /** \brief List of known tools */
      tool_list                   tools;

      /** \brief Assigns a unique instance identifier to a task */
      instance_list               instances;

      /** \brief Maps an instance identifier to its associated processor */
      validated_instance_list     validated_instances;

      /** \brief TCP port on which to listen for incoming connections */
      tipi::tcp_port              tcp_port_number;

    private:

      /** \brief Start a tool */
      void execute(tool const&, boost::filesystem::path const&, boost::shared_ptr< execution::task_monitor >, bool);

      /** \brief Execute a command */
      void execute(execution::command const*, boost::shared_ptr< execution::task_monitor >, bool);

    public:

      /** \brief Default constructor */
      tool_manager_impl();

      /** \brief Disconnect and terminate running tools */
      void activate(tipi::tcp_port port);

      /** \brief Establishes whether the named tool is among the known tools or not */
      bool exists(std::string const&) const;

      /** \brief Returns a tool by its name */
      boost::shared_ptr< const tool > find(std::string const&) const;

      /** \brief Returns a tool by its name */
      boost::shared_ptr< const tool > get_tool_by_name(std::string const&) const;

      /** \brief Add a new tool to the catalog */
      bool add_tool(std::string const&, boost::filesystem::path const&);

      /** \brief Add a new tool to the catalog */
      bool add_tool(tool const&);

      /** \brief Get the list of known tools */
      const_tool_sequence get_tools() const;

      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools();

      /** \brief Get the tool_capabilities object for a tool */
      bool query_tool(boost::shared_ptr < tool > const&);

      /** \brief Have the tool executor terminate all running tools */
      void terminate();

      /** \brief Disconnect and terminate running tools */
      void shutdown();
  };

  /// \endcond
}

#endif

