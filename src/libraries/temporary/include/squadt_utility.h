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

#include "squadt_tool_interface.h"

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

  /** Helper class to project the selected radio button in a group to instances of a type T */
  template < typename T >
  class radio_button_helper {

    typedef sip::layout::elements::radio_button radio_button;

    private:

      std::map < radio_button const*, T > selector;

    public:

      /** \brief The first button in the group */
      radio_button* first;

    public:

      /** \brief constructor */
      radio_button_helper(sip::layout::manager*, radio_button*, T const&);

      /** \brief associate a radio button with a layout manager and a value */
      void associate(sip::layout::manager*, radio_button*, T const&);

      /** \brief get the value for the selected radio button */
      T get_selection();
  };

  template < typename T >
  inline radio_button_helper< T >::radio_button_helper(sip::layout::manager* l, radio_button* r, T const& v) {
    first = r;

    associate(l, r, v);
  }

  template < typename T >
  inline void radio_button_helper< T >::associate(sip::layout::manager* l, radio_button* r, T const& v) {
    l->add(r);

    selector[r] = v;
  }

  template < typename T >
  inline T radio_button_helper< T >::get_selection() {
    return (selector[first->get_selected()]);
  }
}
 
#endif
   
#endif
   
   
   
   
   
