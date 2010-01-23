// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/squadt_interface.h
/// \brief Interface for simplified connection and communication with SQuADT

#ifndef MCRL2_SQUADT_INTERFACE_H_
#define MCRL2_SQUADT_INTERFACE_H_

#include <boost/cstdint.hpp>

#include "tipi/report.hpp"
#include "tipi/tool_display.hpp"
#include "tipi/configuration.hpp"
#include "tipi/tool/capabilities.hpp"

#include "mcrl2/utilities/tipi_ext.h"

/// \cond INTERNAL_DOCS
namespace tipi {
  class configuration;

  namespace tool {

    class communicator;
    class capabilities;
  }
}
/// \endcond

namespace mcrl2 {
  namespace utilities {

    /// SQuADT specific utility functionality
    namespace squadt {

      /**
       * \brief A convenient interface for tool developers to make use of the
       * functionality provided by the tipi tool-side library.
       *
       * \note this interface is focussed on tools in the mCRL2 toolset most
       * notably the gs message layer relay mechanism presented above is activated.
       **/
      class tool_interface {

        private:

          /** \brief whether or not the communicator is active */
          bool active;

        protected:

          /** \brief communicator object through which all communication with SQuADT will take place */
          tipi::tool::communicator* m_communicator;

        protected:

          /** \brief send notification message */
          void send_notification(std::string const&) const;

          /** \brief send status message */
          void send_report(const tipi::report::type, std::string const&);

          /** \brief send warning message */
          void send_warning(std::string const&) const;

          /** \brief send error message */
          void send_error(std::string const&) const;

          /** \brief sends a layout for the tool display */
          void send_display_layout(tipi::tool_display&);

          /** \brief sends a clear display layout signal */
          void send_clear_display();

          /** \brief sends a hide display layout signal */
          void send_hide_display();

        protected:

          /** \brief initialisation after connection to SQuADt */
          virtual void initialise() {};

          /** \brief configures tool capabilities */
          virtual void set_capabilities(tipi::tool::capabilities&) const = 0;

          /** \brief queries the user via SQuADT if needed to obtain configuration information */
          virtual void user_interactive_configuration(tipi::configuration&) = 0;

          /** \brief check an existing configuration object to see if it is usable */
          virtual bool check_configuration(tipi::configuration const&) const = 0;

          /** \brief performs the task specified by a configuration */
          virtual bool perform_task(tipi::configuration&) = 0;

          /** \brief finalisation after termination signal has been received */
          virtual void finalise() {};

          /** \brief virtual destructor */
          virtual ~tool_interface();

          /** \brief checks for a connection and if so starts the event loop */
          bool try_run();

        public:

          /** \brief default constructor */
          tool_interface();

          /** \brief builds a connection with SQuADT */
          bool try_interaction(int&, char** const);

          /** \brief builds a connection with SQuADT */
          bool try_interaction(char*);

          /**  \brief whether or not the communicator is active (connected to SQuADT) */
          bool is_active() const;
      };
    }
  }
}

#endif
