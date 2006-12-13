#include <utility/squadt_utility.h>

namespace squadt_utility {
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

  void finalise() {
    gsSetCustomMessageHandler(0);
  }
}

