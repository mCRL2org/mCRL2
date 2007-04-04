#ifndef MCRL2_SQUADT_INTERFACE_H_
#define MCRL2_SQUADT_INTERFACE_H_

#include <sip/tool.h>

#include <utilities/squadt_utility.h>

#include "liblowlevel.h"

/** \brief Helps relaying messages printed through the mcrl2_basic::print */
namespace mcrl2_squadt {

  class printer_helper {
    friend void relay_message(gsMessageType t, char* data);
 
    private:
 
      /* The communicator object to use */
      sip::tool::communicator& tc;
 
    public:
 
      printer_helper(sip::tool::communicator& t) : tc(t) {
      }
  };

  void initialise(sip::tool::communicator& t);
  void finalise(sip::tool::communicator& t);

  /**
   * \brief A convenient interface for tool developers to make use of the
   * functionality provided by the sip tool-side library.
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
      sip::tool::communicator m_communicator;

    protected:

      /** \brief send notification message */
      void send_notification(std::string const&) const;

      /** \brief send warning message */
      void send_warning(std::string const&) const;

      /** \brief send error message */
      void send_error(std::string const&) const;

      /** \brief sends a layout for the tool display */
      void send_display_layout(std::auto_ptr < sip::layout::manager >&);

      /** \brief sends a clear display layout signal */
      void send_clear_display();

      /** \brief sends a hide display layout signal */
      void send_hide_display();

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

      /** \void virtual destructor */
      virtual ~tool_interface();

    private:

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

  template < typename T >
  class interactor {

    public:

      static bool free_activation(int&, char** const);

      static bool free_activation(char*);
  };

  /** \brief builds a connection with SQuADT */
  template < typename T >
  inline bool interactor< T >::free_activation(char* av) {
    T c;

    return (c.try_interaction(av));
  }

  /** \brief builds a connection with SQuADT */
  template < typename T >
  inline bool interactor< T >::free_activation(int& ac, char** const av) {
    T c;

    return (c.try_interaction(ac, av));
  }

  inline tool_interface::tool_interface() {
  }

  inline tool_interface::~tool_interface() {
  }
}

#endif
