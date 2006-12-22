#ifndef SIP_COMMAND_LINE_INTERFACE_TCC
#define SIP_COMMAND_LINE_INTERFACE_TCC

#include <boost/shared_array.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <sip/exception.h>

#include <sip/detail/command_line_interface.h>
#include <sip/detail/schemes.h>

namespace sip {

  namespace command_line_interface {

    using namespace messaging;

    const size_t argument_extractor::known_option_number = 2;
    const size_t argument_extractor::known_scheme_number = 2;

    const char*  argument_extractor::known_options[known_option_number] = { "--si-connect", "--si-identifier" };
    const char*  argument_extractor::known_schemes[known_scheme_number] = { "traditional", "socket" };

    /**
     * \param[in] cl the unparsed command line
     **/
    argument_extractor::argument_extractor(char* cl) {
      if (cl != 0) {
        char*  argv_store = new char[std::strlen(cl) + 1];
        int    argc       = 0;
       
        std::vector < char* > arguments;
       
        char* current = cl;
       
        arguments.push_back(argv_store);

        while (*current != '\0') {
          // skip initial white space
          while (*current == '\0' || *current == ' ') {
            ++current;
          }
       
          if (*current != '\0') {
            char* current_argument = current;
           
            while (*current != '\0' && *current != ' ') {
              if (*current == '\'') {
                while (*current != '\0' && *current != '\'') {
                  ++current;
                }

                ++current;
              }
              else if (*current == '\"') {
                while (*current != '\0' && *current != '\"') {
                  ++current;
                }

                ++current;
              }
              else {
                ++current;
              }
            }

            std::strncpy(arguments[argc], current_argument, current - current_argument);

            arguments[argc][current - current_argument] = '\0';

            arguments.push_back(arguments[argc++] + (current - current_argument + 1));
          }
        }

        boost::shared_array<char*> argv(new char*[argc]);

        // compile argv
        for (int i = 0; i < argc; ++i) {
          argv[i] = arguments[i];
        }

        process(argc, argv.get());

        current = cl;

        // convert back to the input string
        for (int i = 0; i < argc; ++i) {
          char* c = argv[i];
       
          while (c[0] != '\0') {
            *(current++) = *(c++);
          }
       
          if (1 < argc - i) {
            *(current++) = ' ';
          }
        }

        *current = '\0';

        delete argv_store;
      }
    }

    /**
     * \return The arguments for a selected scheme (e.g. hostname and port for the socket scheme), or 0
     **/
    scheme_ptr argument_extractor::get_scheme() const {
      return (selected_scheme);
    }

    /**
     * \return The identifier extracted from one of the command line arguments, or the default identifier
     **/
    long argument_extractor::get_identifier() const {
      return (identifier);
    }

    /**
     * \return A pointer within argv to the point upto which the option was matched.
     **/
    inline char* argument_extractor::parse_option(char* const option) {
      unsigned int i = 0;

      while (i < known_option_number) {
        size_t l = strlen(known_options[i]);

        if (strncmp(option, known_options[i], l) == 0) {
          last_matched = i;

          return (option + l);
        }

        ++i;
      }

      return (option);
    }

    /**
     * @param[in] option a pointer to where a scheme description starts
     *
     * \return A pointer within argv to the point upto which the option was matched.
     **/
    inline char* argument_extractor::parse_scheme(char* const option) {
      unsigned int i = 0;

      while (i < known_scheme_number) {
        size_t l = strlen(known_schemes[i]);

        if (strncmp(option, known_schemes[i], l) == 0) {
          last_matched = i;

          char* s = option + l;

          if (last_matched != 0 && strncmp(s, "://", 3) != 0) {
            throw (sip::exception(sip::cli_parse_error_expected, "://", s));
          }

          char* t = s + 3;

          switch (last_matched) {
            case 1: /* Socket scheme (host:port) */
              selected_scheme = scheme_ptr(new socket_scheme< sip::message >);

              /* Search for host/port separator */
              s = strchr(t, ':');
             
              if (t != 0) {
                /* Take everything between s and t as hostname */
                const size_t d = s - t;

                std::string& host_name = dynamic_cast < socket_scheme< sip::message >* > (selected_scheme.get())->host_name;

                host_name.reserve(d);
                host_name.assign(t, d);
                host_name.resize(d);

                /* The remaining string should be a port number */
                dynamic_cast < socket_scheme< sip::message >* > (selected_scheme.get())->port = atol(++t);
              }

              break;
            default: /* Traditional scheme */
              selected_scheme = scheme_ptr(new traditional_scheme< sip::message >);
              break;
          }

          return (s);
        }

        ++i;
      }

      return (option);
    }

    /**
     * The following connection options are recognised and extracted from the command line arguments:
     *
     *  - --si-connect=\<scheme\>, where \<scheme\> is one of
     *    - socket://\<host\>:\<port\> (for a socket connection)
     *    - traditional:// (for standard input/output communication)
     *  - --si-identifier=\<positive natural number\>
     *
     * @param argc the number of command line arguments
     * @param argv a pointer to the list of command line arguments
     * \attention the specific command line options are removed, so and argc and argv are modified
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    void argument_extractor::process(int& argc, char** const argv) {
      size_t i = 0;
      size_t j = 0;

      while (static_cast < int > (i) < argc) {
        char* s = parse_option(argv[i]);

        if (s != argv[i]) {
          /* Ignore anything before the `=' character */
          s = strchr(s, '=');

          if (s == 0) {
            throw (sip::exception(sip::cli_parse_error_expected, '=', s));
          }

          char* t = s + 1;

          switch (last_matched) {
            case 0: /* Connect option */
              /* Now s should point to a known scheme identifier */
              s = parse_scheme(t);

              if (s == t) {
                throw (sip::exception(sip::cli_parse_error_expected, "valid scheme", s));
              }
              break;
            case 1: /* Identifier option */
              identifier = atol(t);
              break;
            default: /* Unknown option */
              break;
          }
        }
        else {
          if (j < i) {
            argv[j] = s;
          }

          ++j;
        }
     
        ++i;
      }

      argc = j;
    }
  }
}
#endif
