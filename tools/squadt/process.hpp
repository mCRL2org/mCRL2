// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file process.h
/// \brief Add your file description here.

#ifndef PROCESS_H
#define PROCESS_H

#include <cstdio>

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>

#include "workarounds.h" // pid_t

#include "command.hpp"

namespace squadt {
  namespace execution {

    class process_impl;

    /**
     * \brief Represents a system process with a status reporting facility
     **/
    class process {
      friend class executor;
      friend class process_impl;

      public:

        /** \brief state of the process */
        enum status {
          stopped,   ///< stopped or not yet started to run
          running,   ///< is running
          completed, ///< has terminated successfully
          aborted    ///< was aborted
        };

        /** \brief Convenience type for handlers */
        typedef boost::function < void (boost::shared_ptr < process >) >    termination_handler;

      private:

        /** \brief Pointer to implementation object (handle-body idiom) */
        boost::shared_ptr < process_impl >             impl;

      private:

        /** \brief Constructor */
        process();

      public:

        /** \brief Factory method */
        static boost::shared_ptr < process > create();

        /** \brief Start the process by executing a command */
        void execute(const command&);

        /** \brief Start the process by executing a command */
        void execute(const command&, termination_handler const& h);

        /** \brief Returns the process status */
        status get_status() const;

        /** \brief Returns the process id */
        pid_t get_identifier() const;

        /** \brief Returns the process id */
        std::string get_executable_name() const;

        /** \brief Terminates the process */
        bool terminate();

        /** \brief Returns the last command that is (or was) executing */
        command const& get_command() const;
    };
  }
}

#endif
