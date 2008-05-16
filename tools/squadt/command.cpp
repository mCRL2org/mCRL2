// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file command.cpp
/// \brief Add your file description here.

#include "command.hpp"

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
                do {
                  ++current;
                }
                while (current != c.end() && *current != '\'');

                if (*current == '\'') {
                  ++current;
                }
              }
              else if (*current == '\"') {
                do {
                  ++current;
                }
                while (current != c.end() && *current != '\"');

                if (*current == '\"') {
                  ++current;
                }
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

    std::string command::as_string(bool b) const {
      std::string s;

      if (b) {
        s = executable;
      }
  
      for (std::deque < std::string >::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
        s.append(" " + *i);
      }
  
      return (s);
    }
  
    command::const_argument_sequence command::get_arguments() const {
      return (boost::make_iterator_range(arguments.begin(), arguments.end()));
    }

    command::argument_sequence command::get_arguments() {
      return (boost::make_iterator_range(arguments.begin(), arguments.end()));
    }

    boost::shared_array < char const* > command::get_array(bool b) const {
      boost::shared_array < char const* > p(new char const*[arguments.size() + 2]);

      char const** d = p.get();

      if (b) {
        *(d++) = executable.c_str();
      }

      for (std::deque < std::string >::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
        if (((*i)[0] == '\'' || (*i)[0] == '\"') && 2 < i->size()) {
          *(d++) = (*i).substr(1, i->size() - 2).c_str();
        }
        else {
          *(d++) = (*i).c_str();
        }
      }

      *d = 0;

      return (p);
    }
  }
}
