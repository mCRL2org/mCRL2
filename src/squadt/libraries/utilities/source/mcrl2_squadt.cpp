#include <utilities/logger.h>
#include <utilities/mcrl2_squadt.h>

namespace mcrl2_squadt {
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

    utility::logger::log_level l = utility::logger::get_default_filter_level();

    if (1 < l) {
      gsSetVerboseMsg();

      if (2 < l) {
        gsSetDebugMsg();
      }
    }
  }

  void finalise() {
    gsSetCustomMessageHandler(0);
  }

  void tool_interface::initialise() {
  }

  void tool_interface::finalise() {
  }

  bool tool_interface::try_run() {
    if (active) {
      bool valid_configuration_present = false;
      bool termination_requested       = false;

      /* Initialise squadt utility pseudo-library */
      mcrl2_squadt::initialise(m_communicator);

      initialise();

      while (!termination_requested) {
        switch (m_communicator.await_message(sip::message_any)->get_type()) {
          case sip::message_offer_configuration: {
              sip::configuration& configuration = m_communicator.get_configuration();

              /* Insert configuration in tool communicator object */
              valid_configuration_present = check_configuration(configuration);

              if (configuration.is_fresh()) {
                do {
                  user_interactive_configuration(configuration);

                  /* Insert configuration in tool communicator object */
                  valid_configuration_present = check_configuration(configuration);
             
                } while (!valid_configuration_present);
              }

              /* Signal that the configuration is acceptable */
              m_communicator.send_accept_configuration();
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

      /* Unregister message relay */
      mcrl2_squadt::finalise();

      active = false;

      return (true);
    }

    return (false);
  }

  bool tool_interface::try_interaction(char* av) {
    set_capabilities(m_communicator.get_tool_capabilities());

    active = m_communicator.activate(av);

    return (try_run());
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
  bool tool_interface::try_interaction(int& ac, char** const av) {

    set_capabilities(m_communicator.get_tool_capabilities());

    active = m_communicator.activate(ac,av);

    return (try_run());
  }

  bool tool_interface::is_active() const {
    return (active);
  }

  void tool_interface::send_notification(std::string const& m) const {
    m_communicator.send_status_report(sip::report::notice, m);
  }

  void tool_interface::send_warning(std::string const& m) const {
    m_communicator.send_status_report(sip::report::warning, m);
  }

  void tool_interface::send_error(std::string const& m) const {
    m_communicator.send_status_report(sip::report::error, m);
  }

  void tool_interface::send_display_layout(std::auto_ptr < sip::layout::manager >& p) {
    m_communicator.send_display_layout(sip::layout::tool_display::create(p));
  }

  void tool_interface::send_clear_display() {
    m_communicator.send_clear_display();
  }

  void tool_interface::send_hide_display() {
    boost::shared_ptr < sip::layout::tool_display > p(new sip::layout::tool_display());

    p->show(false);

    m_communicator.send_display_layout(p);
  }
}
