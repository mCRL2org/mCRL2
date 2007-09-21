// Author(s): Jeroen van der Wulp
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/squadt_interface.h
/// \brief Add your file description here.

#ifndef MCRL2_SQUADT_INTERFACE_H_
#define MCRL2_SQUADT_INTERFACE_H_

#include <boost/cstdint.hpp>

#include <tipi/tool.hpp>

#include "mcrl2/utilities/tipi_ext.h"

#ifndef NO_MCRL2_TOOL_FACILITIES
#include "print/messaging.h"
#endif

/** \brief Helps relaying messages printed through the mcrl2_basic::print */
namespace mcrl2 {
  namespace utilities {
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
          tipi::tool::communicator m_communicator;
  
        protected:
  
          /** \brief send notification message */
          void send_notification(std::string const&) const;
  
          /** \brief send warning message */
          void send_warning(std::string const&) const;
  
          /** \brief send error message */
          void send_error(std::string const&) const;
  
          /** \brief sends a layout for the tool display */
          void send_display_layout(std::auto_ptr < tipi::layout::manager >&);
  
          /** \brief sends a clear display layout signal */
          void send_clear_display();
  
          /** \brief sends a hide display layout signal */
          void send_hide_display();
  
        protected:
  
          /** \brief initialisation after connection to SQuADt */
          virtual void initialise();
  
          /** \brief configures tool capabilities */
          virtual void set_capabilities(tipi::tool::capabilities&) const = 0;
  
          /** \brief queries the user via SQuADT if needed to obtain configuration information */
          virtual void user_interactive_configuration(tipi::configuration&) = 0;
  
          /** \brief check an existing configuration object to see if it is usable */
          virtual bool check_configuration(tipi::configuration const&) const = 0;
  
          /** \brief performs the task specified by a configuration */
          virtual bool perform_task(tipi::configuration&) = 0;
  
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

      class mcrl2_tool_interface : public tool_interface {
        template < typename T >
        friend void relay_message(const T, const char* data);

        protected:

          static tipi::tool::communicator* communicator;

        protected:

          /** \brief initialisation after connection to SQuADt */
          void initialise();
  
          /** \brief finalisation after termination signal has been received */
          void finalise();
      };
  
#ifndef NO_MCRL2_TOOL_FACILITIES
      using ::mcrl2::utilities::messageType;
  
      /** \brief Used to relay messages generated using mcrl2_basic::print */
      template < >
      inline void relay_message(const messageType t, const char* data) {
        tipi::report::type report_type;
  
        switch (t) {
          case gs_notice:
            report_type = tipi::report::notice;
            break;
          case gs_warning:
            report_type = tipi::report::warning;
            break;
          case gs_error:
          default:
            report_type = tipi::report::error;
            break;
        }
      
        mcrl2_tool_interface::communicator->send_status_report(report_type, std::string(data));
      }  

      inline void mcrl2_tool_interface::initialise() {
        /* Initialise squadt utility pseudo-library */
        gsSetCustomMessageHandler(relay_message<messageType>);
  
        tipi::utility::logger::log_level l = tipi::utility::logger::get_default_filter_level();
  
        gsSetNormalMsg();
  
        if (1 < l) {
          gsSetVerboseMsg();
  
          if (2 < l) {
            gsSetDebugMsg();
          }
        }

        mcrl2_tool_interface::communicator = &m_communicator;
      }
  
      inline void mcrl2_tool_interface::finalise() {
        /* Unregister message relay */
        gsSetCustomMessageHandler(0);
      }
#endif

      inline void tool_interface::initialise() {
      }
  
      inline void tool_interface::finalise() {
      }

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

      /* Standard type for input validation */
      extern boost::shared_ptr < tipi::datatype::enumeration > rewrite_strategy_enumeration;
  
      // Helper function for unsigned long to string conversion
      inline std::ostream& operator<<(std::ostream& o, unsigned long const& t) {
        char buf[21];
   
        sprintf(buf,"%lu",t);
   
        o << buf;
   
        return o;
      }
  
      // Helper function for unsigned long long to string conversion
      inline std::ostream& operator<<(std::ostream& o, unsigned long long const& t) {
        char buf[21];
   
        sprintf(buf,"%llu",t);
   
        o << buf;
   
        return o;
      }
    }
  }
}

#endif
