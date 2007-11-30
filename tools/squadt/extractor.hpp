// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file extractor.h
/// \brief Add your file description here.

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <boost/weak_ptr.hpp>

#include "task_monitor.hpp"
#include "tool.hpp"

namespace squadt {

  class tool;

  /**
   * \brief Simple processor that queries a tool's capabilities
   *
   * Extracts the tool information that is important for operation of the tool
   * manager.
   **/
  class extractor : public execution::task_monitor {

    private:

      /** \brief handler that accomplishes the actual task */
      static void handle_store_tool_capabilities(boost::weak_ptr < extractor >, const tipi::message_ptr& m, boost::shared_ptr < tool > t);

    public:

      /** \brief Starts the extraction */
      bool extract(boost::weak_ptr < extractor > const&, boost::shared_ptr < tool > const&);
  };
}

#endif
