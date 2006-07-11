#ifndef SQUADT_UTILITY_H_
#define SQUADT_UTILITY_H_

/**
 * This is a small collection of functions that make it easier to adapt mCRL2
 * tools to support communication with SQuADT.
 **/

#ifdef ENABLE_SQUADT_CONNECTIVITY

#include <memory>

#include <sip/tool.h>

#include "liblowlevel.h"

namespace squadt_utility {

  /* Helps relaying messages printed through the mcrl2_basic::print */
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

  /* Used to relay messages generated using mcrl2_basic::print */
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

  /* Replace standard messaging functions */
  void initialise(sip::tool::communicator& t) {
    postman = std::auto_ptr < printer_helper > (new printer_helper(t));

    gsSetCustomMessageHandler(relay_message);
  }

}  
   
#endif
   
#endif
   
   
   
   
   
