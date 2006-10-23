#include "command.h"

namespace squadt {
  namespace execution {

    std::auto_ptr < command > command::from_command_line(std::string const& c) {
      std::auto_ptr < command > new_command;

      if (!c.empty()) {
        std::string::const_iterator current = c.begin();
       
        // skip initial white space
        while (current != c.end() && *current == ' ') {
          ++current;
        }
        // read command
        while (current != c.end() && *current != ' ') {
          ++current;
        }

        new_command.reset(new command(std::string(c.begin(), current)));

        while (current != c.end()) {
          // skip white space
          while (current != c.end() && *current == ' ') {
            ++current;
          }
       
          if (current != c.end()) {
            std::string::const_iterator start = current;
           
            while (current != c.end() && *current != ' ') {
              if (*current == '\'') {
                while (current != c.end() && *current != '\'') {
                  ++current;
                }

                ++current;
              }
              else if (*current == '\"') {
                while (current != c.end() && *current != '\"') {
                  ++current;
                }

                ++current;
              }
              else {
                ++current;
              }
            }

            new_command->append_argument(std::string(start, current));
          }
        }
      }

      return (new_command);
    }

    std::string command::argument_string(bool b) const {
      std::string s;

      if (b) {
        s.append(executable).append(" ");
      }
  
      for (std::deque < std::string >::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
        s.append(*i).append(" ");
      }
  
      return (s);
    }
  
    command::const_argument_sequence command::get_arguments() const {
      return (boost::make_iterator_range(arguments.begin(), arguments.end()));
    }

    command::argument_sequence command::get_arguments() {
      return (boost::make_iterator_range(arguments.begin(), arguments.end()));
    }

    boost::shared_array < char const* > command::get_argument_array(bool b) const {
      boost::shared_array < char const* > p(new char const*[arguments.size() + 2]);

      char const** d = p.get();

      if (b) {
        *(d++) = executable.c_str();
      }

      for (std::deque < std::string >::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
        *(d++) = (*i).c_str();
      }

      *d = 0;

      return (p);
    }
  }
}
