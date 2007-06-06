#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <boost/weak_ptr.hpp>

#include "task_monitor.h"
#include "tool.h"

namespace squadt {

  class tool;

  /**
   * \brief Simple processor that queries a tool's capabilities
   *
   * Extracts the tool information that is important for operation of the tool
   * manager.
   **/
  class extractor : public execution::task_monitor {

    public:

      /** \brief Convenience type for hiding shared pointer implementation */
      typedef boost::shared_ptr < extractor > ptr;

    private:

      /** \brief handler that accomplishes the actual task */
      void handle_store_tool_capabilities(const sip::message_ptr& m, tool& t);

      /** \brief terminates the associated process after a timeout period, if not already finished */
      void terminate_after_timeout(const boost::weak_ptr < void >);

    public:

      /** \brief Constructor */
      extractor(tool&);

      /** \brief Starts the extraction */
      void extract();
  };
}

#endif
