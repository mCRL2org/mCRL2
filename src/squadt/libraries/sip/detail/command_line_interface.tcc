#ifndef SIP_COMMAND_LINE_INTERFACE_TCC
#define SIP_COMMAND_LINE_INTERFACE_TCC

#include <cstring>
#include <cstdlib>

#include <sip/detail/exception.h>

#include <sip/detail/command_line_interface.h>

namespace sip {

  namespace cli {

    const size_t command_line_argument_extractor::known_option_number = 2;
    const size_t command_line_argument_extractor::known_scheme_number = 2;

    const char*  command_line_argument_extractor::known_options[known_option_number] = { "--si-connect", "--si-identifier" };
    const char*  command_line_argument_extractor::known_schemes[known_scheme_number] = { "traditional", "socket" };

    /**
     * \returns The arguments for a selected scheme (e.g. hostname and port for the socket scheme), or 0
     **/
    inline scheme_ptr command_line_argument_extractor::get_scheme() const {
      return (selected_scheme);
    }

    /**
     * \returns The identifier extracted from one of the command line arguments, or the default identifier
     **/
    inline long command_line_argument_extractor::get_identifier() const {
      return (identifier);
    }

    /**
     * \returns A pointer within argv to the point upto which the option was matched.
     **/
    inline char* command_line_argument_extractor::parse_option(char* const option) {
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
     * \returns A pointer within argv to the point upto which the option was matched.
     **/
    inline char* command_line_argument_extractor::parse_scheme(char* const option) throw () {
      unsigned int i = 0;

      while (i < known_scheme_number) {
        size_t l = strlen(known_schemes[i]);

        if (strncmp(option, known_schemes[i], l) == 0) {
          last_matched = i;

          char* s = option + l;

          if (last_matched != 0 && strncmp(s, "://", 3) != 0) {
            throw (exception(exception_identifier::cli_parse_error_expected, "://", s));
          }

          s += 3;
         
          switch (last_matched) {
            case 1: /* Socket scheme (host:port) */
              selected_scheme = scheme_ptr(new socket_scheme);

              /* Search for host port separator */
              char* t = strchr(s, ':');
             
              if (t != 0) {
                /* Take everything between s and t as hostname */
                std::string& host_name = dynamic_cast < socket_scheme* > (selected_scheme.get())->host_name;

                host_name.resize(t - s);

                memcpy(s, &host_name, t - s);
              }

              /* The remaining string should be a port number */
              dynamic_cast < socket_scheme* > (selected_scheme.get())->port = atol(t);
              break;
            default: /* Traditional scheme */
              selected_scheme = scheme_ptr(new traditional_scheme);
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
     *
     * @param argc the number of command line arguments
     * @param argv a pointer to the list of command line arguments
     * \attention the specific command line options are removed, so and argc and argv are modified
     * \return whether options were found and whether a connection is being opened with a controller
     **/
    inline void command_line_argument_extractor::process(int& argc, char** const argv) throw () {
      size_t i = 0;
      size_t j = 0;

      while (static_cast < int> (i) < argc) {
        char* s = parse_option(argv[i]);

        if (s != argv[i]) {
          /* Ignore anything before the `=' character */
          s = strchr(s, '=');

          if (s == 0) {
            throw (exception(exception_identifier::cli_parse_error_expected, '=', s));
          }

          ++s;

          switch (last_matched) {
            case 0: /* Connect option */
              /* Now s should point to a known scheme identifier */
              char* t = parse_scheme(s);

              if (s == t) {
                throw (exception(exception_identifier::cli_parse_error_expected, "valid scheme", s));
              }
              break;
            case 1: /* Identifier option */
              identifier = atol(s);
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
