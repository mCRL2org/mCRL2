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

#ifdef __WXWINDOWS__
  /** 
   * \brief Convenience class for connecting wxWidgets applications to the environment (platform dependent wrapper around wxEntry)
   **/
  class entry_wrapper {
#ifdef __WINDOWS__
    private:

      HINSTANCE        hInstance;
      HINSTANCE        hPrevInstance;
      wxCmdLineArgType lpCmdLine;
      int              nCmdShow;

    public:

      entry_wrapper(HINSTANCE hc, HINSTANCE hp, wxCmdLineArgType lp, int ns) {
        hInstance     = hc;
        hPrevInstance = hp;
        lpCmdLine     = lp;
        nCmdShow      = ns;
      }

      bool perform_entry() {
        return (wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow) == 0);
      }
#else
    private:

      int&    argc;
      char**& argv;

    public:

      entry_wrapper(int& ac, char**& av) : argc(ac), argv(av) {
      }

      bool perform_entry() {
        return (wxEntry(argc, argv) == 0);
      }
#endif
  };
#endif

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
      template < typename M >
      radio_button_helper(M const&, T const&, radio_button*);

      /** \brief constructor */
      template < typename M >
      radio_button_helper(M const&, T const&, std::string const&);

      /** \brief associate a radio button with a layout manager and a value */
      template < typename M >
      void associate(M const&, T const&, radio_button*);

      /** \brief associate a radio button with a layout manager and a value */
      template < typename M >
      radio_button* associate(M const&, T const&, std::string const&, bool = false);

      /** \brief get the value for the selected radio button */
      T get_selection();

      /** \brief gets the label of the selected radio button */
      std::string get_selection_label() const;
  };

  /**
   * \param[in] l the layout manager to which the button should be attached
   * \param[in] r pointer to the radio button to attach
   * \param[in] v the value to associate the button with
   **/
  template < typename T >
  template < typename M >
  inline radio_button_helper< T >::radio_button_helper(M const& l, T const& v, radio_button* r) {
    first = r;

    associate(l, v, r);
  }

  /**
   * \param[in] l the layout manager to which the button should be attached
   * \param[in] v the value to associate the button with
   **/
  template < typename T >
  template < typename M >
  inline radio_button_helper< T >::radio_button_helper(M const& l, T const& v, std::string const& s) {
    first = new radio_button(s);

    associate(l, v, first);
  }

  /**
   * \param[in] l the layout manager to which the button should be attached
   * \param[in] r pointer to the radio button to attach
   * \param[in] v the value to associate the button with
   **/
  template < typename T >
  template < typename M >
  inline void radio_button_helper< T >::associate(M const& l, T const& v, radio_button* r) {
    l->add(r);

    selector[r] = v;
  }

  /**
   * \param[in] l the layout manager to which the button should be attached
   * \param[in] v the value to associate the button with
   **/
  template < typename T >
  template < typename M >
  inline sip::layout::elements::radio_button* radio_button_helper< T >::associate(M const& l, T const& v, std::string const& s, bool b) {
    radio_button* button = new radio_button(s, first, b);

    l->add(button);

    selector[button] = v;

    return (button);
  }

  template < typename T >
  inline T radio_button_helper< T >::get_selection() {
    return (selector[first->get_selected()]);
  }

  template < typename T >
  std::string radio_button_helper< T >::get_selection_label() const {
    return (first->get_selected()->get_label());
  }
}
 
#endif
   
#endif
