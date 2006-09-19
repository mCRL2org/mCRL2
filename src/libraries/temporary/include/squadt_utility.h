#ifndef SQUADT_UTILITY_H_
#define SQUADT_UTILITY_H_

/**
 * This is a small collection of functions that make it easier to adapt mCRL2
 * tools to support communication with SQuADT.
 **/

#ifdef ENABLE_SQUADT_CONNECTIVITY

#include <memory>
#include <string>

#include <sip/tool.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "liblowlevel.h"

namespace squadt_utility {

  /** \brief Helps relaying messages printed through the mcrl2_basic::print */
  class printer_helper {
    friend void relay_message(gsMessageType t, char* data);

    private:

      /* The communicator object to use */
      sip::tool::communicator& tc;

    public:

      printer_helper(sip::tool::communicator& t) : tc(t) {
      }
  };

  std::auto_ptr < printer_helper > postman;

  /** \brief Used to relay messages generated using mcrl2_basic::print */
  void relay_message(gsMessageType t, char* data) {
    sip::report::type report_type;

    assert(postman.get() != 0);
  
    switch (t) {
      case gs_notice:
        report_type = sip::report::notice;
        break;
      case gs_warning:
        report_type = sip::report::warning;
        break;
      case gs_error:
      default:
        report_type = sip::report::error;
        break;
    }
  
    postman->tc.send_status_report(report_type, std::string(data));
  }  

  /** \brief Replace standard messaging functions */
  void initialise(sip::tool::communicator& t) {
    postman = std::auto_ptr < printer_helper > (new printer_helper(t));

    gsSetCustomMessageHandler(relay_message);
  }

  /**
   * \brief A convenient interface for tool developers to make use of the
   * functionality provided by the sip tool-side library.
   *
   * \note this interface is focussed on tools in the mCRL2 toolset most
   * notably the gs message layer relay mechanism presented above is activated.
   **/
  class squadt_tool_interface {

    private:

      /** \brief whether or not the communicator is active */
      bool active;

    protected:

      /** \brief communicator object through which all communication with SQuADT will take place */
      sip::tool::communicator m_communicator;

    protected:

      /** \brief send notification message */
      void send_notification(std::string const&);

      /** \brief send warning message */
      void send_warning(std::string const&);

      /** \brief send error message */
      void send_error(std::string const&);

    protected:

      /** \brief initialisation after connection to SQuADt */
      virtual void initialise();

      /** \brief configures tool capabilities */
      virtual void set_capabilities(sip::tool::capabilities&) const = 0;

      /** \brief queries the user via SQuADT if needed to obtain configuration information */
      virtual void user_interactive_configuration(sip::configuration&) = 0;

      /** \brief check an existing configuration object to see if it is usable */
      virtual bool check_configuration(sip::configuration const&) const = 0;

      /** \brief performs the task specified by a configuration */
      virtual bool perform_task(sip::configuration&) = 0;

      /** \brief finalisation after termination signal has been received */
      virtual void finalise();

      /** \void pure virtual destructor */
      virtual ~squadt_tool_interface() = 0;

    public: 

      /** \brief default constructor */
      squadt_tool_interface();

      /** \brief builds a connection with SQuADT */
      bool try_interaction(int, char** const);

      /**  \brief whether or not the communicator is active (connected to SQuADT) */
      bool is_active() const;
  };

  inline squadt_tool_interface::squadt_tool_interface() {
  }

  inline void squadt_tool_interface::initialise() {
  }

  inline void squadt_tool_interface::finalise() {
  }

  /**
   * The connection is built using information such as a socket identifier when
   * found among the command line arguments. The command line arguments that
   * are SQuADT specific are filtered out.
   *
   * \param[in] ac the number of command line arguments
   * \param[in] av a pointer to an array of the actual command line arguments
   *
   * \return whether or not SQuADT interaction was successful
   **/
  inline bool squadt_tool_interface::try_interaction(int ac, char** const av) {

    set_capabilities(m_communicator.get_tool_capabilities());

    active = m_communicator.activate(ac,av);

    /* On purpose we do not catch exceptions */
    if (active) {
      bool valid_configuration_present = false;
      bool termination_requested       = false;

      /* Initialise squadt utility pseudo-library */
      squadt_utility::initialise(m_communicator);

      initialise();
 
      while (!termination_requested) {
        switch (m_communicator.await_message(sip::message_any)->get_type()) {
          case sip::message_offer_configuration: {
              sip::configuration& configuration = m_communicator.get_configuration();
 
              if (configuration.is_fresh()) {
                user_interactive_configuration(configuration);
              }
       
              /* Insert configuration in tool communicator object */
              valid_configuration_present = check_configuration(configuration);

              if (valid_configuration_present) {
                /* Signal that the configuration is acceptable */
                m_communicator.send_accept_configuration();
              }
            }
            break;
          case sip::message_signal_start:
            if (valid_configuration_present) {
              /* Signal that the job is finished */
              m_communicator.send_signal_done(perform_task(m_communicator.get_configuration()));
            }
            else {
              send_error("Start signal received without valid configuration!");
            }
            break;
          case sip::message_request_termination:
 
            termination_requested = true;
 
            break;
          default:
            /* Messages with a type that do not need to be handled */
            break;
        }
      }

      finalise();
 
      m_communicator.send_signal_termination();

      active = false;

      return (true);
    }

    return (false);
  }

  inline bool squadt_tool_interface::is_active() const {
    return (active);
  }

  inline void squadt_tool_interface::send_notification(std::string const& m) {
    m_communicator.send_status_report(sip::report::notice, m);
  }

  inline void squadt_tool_interface::send_warning(std::string const& m) {
    m_communicator.send_status_report(sip::report::warning, m);
  }

  inline void squadt_tool_interface::send_error(std::string const& m) {
    m_communicator.send_status_report(sip::report::error, m);
  }

  inline squadt_tool_interface::~squadt_tool_interface() {
  }
}  
   
#endif
   
#endif
   
   
   
   
   
