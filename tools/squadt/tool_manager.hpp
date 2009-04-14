// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tool_manager.h
/// \brief Add your file description here.

#ifndef TOOL_MANAGER_H_
#define TOOL_MANAGER_H_

#include <list>
#include <map>
#include <string>

#include <boost/range/iterator_range.hpp>
#include <boost/filesystem/path.hpp>

#include "tool.hpp"

namespace squadt {

  namespace execution {
    class command;
  }

  class tool_manager_impl;

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
  class tool_manager : public utility::visitable {
    friend class build_system;
    friend class processor_impl;

    template < typename R, typename S >
    friend class utility::visitor;

    public:

      /** \brief Convenience type alias the list of tools, indexed by main input format */
      typedef std::multimap < build_system::storage_format, boost::shared_ptr < tool > > tool_map;

      /** \brief Convenience type alias the list of tools */
      typedef std::list < boost::shared_ptr < tool > >                                   tool_list;

      /** \brief Constant tool sequence type */
      typedef boost::iterator_range < tool_list::const_iterator >                        const_tool_sequence;

    private:

      /** \brief Pointer to implementation object (handle-body idiom) */
      boost::shared_ptr < tool_manager_impl > impl;

    public:

      /** \brief Constructor */
      tool_manager();

      /** \brief Constructor */
      tool_manager(tipi::tcp_port port);

      /** \brief Establishes whether the named tool is among the known tools or not */
      bool exists(std::string const&) const;

      /** \brief Returns a tool by its name */
      boost::shared_ptr < const tool > find(std::string const&) const;

      /** \brief Returns a tool by its name */
      boost::shared_ptr < const tool > get_tool_by_name(std::string const&) const;

      /** \brief Add a new tool to the catalog */
      bool add_tool(std::string const&, boost::filesystem::path const&);

      /** \brief Add a new tool to the catalog */
      bool add_tool(tool const&);

      /** \brief Get the tool_capabilities object for all known tools */
      void query_tools();

      /** \brief Get the tool_capabilities object for a tool */
      bool query_tool(boost::shared_ptr < tool > const&);

      /** \brief Get the list of known tools */
      const_tool_sequence get_tools() const;

      /** \brief Get the number of known tools */
      unsigned int number_of_tools() const;

      /** \brief Have the tool executor terminate all running tools */
      void terminate();

      /** \brief Disconnect and terminate running tools and shutdown listener */
      void shutdown();

      /** \brief Destructor */
      ~tool_manager();
  };

  inline tool_manager::~tool_manager() {
    shutdown();
  }
}

#endif
