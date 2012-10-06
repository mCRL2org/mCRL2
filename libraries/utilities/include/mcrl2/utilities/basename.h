// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/basename.h
/// \brief functions to get the executable's basename.

#ifndef MCRL2_BASENAME_H
#define MCRL2_BASENAME_H


#include <stdio.h>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#ifdef __APPLE__
#include <limits.h>		/* PATH_MAX */
#include <mach-o/dyld.h>/* _NSGetExecutablePath */
#endif

namespace mcrl2
{

namespace utilities
{

    /// \brief Returns the basename of a tool.
    /// \return A string
    static std::string get_executable_basename()
    {
      std::string path;
#ifdef __linux
      path = "";
      pid_t pid = getpid();
      char buf[10];
      sprintf(buf,"%d",pid);
      std::string _link = "/proc/";
      _link.append(buf);
      _link.append("/exe");
      char proc[512];
      int ch = readlink(_link.c_str(),proc,512);
      if (ch != -1)
      {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
      }
#endif // __linux

#ifdef __APPLE__
	  char pathbuf[PATH_MAX];
	  uint32_t  bufsize = sizeof(pathbuf);
	  _NSGetExecutablePath( pathbuf, &bufsize);
 
      path = pathbuf;
      std::string::size_type t = path.find_last_of("/");
      path = path.substr(0,t);

#endif //__APPLE__

#ifdef _WIN32
      char buffer[MAX_PATH];//always use MAX_PATH for filepaths
      GetModuleFileName(NULL,buffer,sizeof(buffer));
      path = buffer;
      std::string::size_type t = path.find_last_of("\\");
      path = path.substr(0,t);
#endif // _WIN32

      return path;
    };
};

}


#endif //MCRL2_BASENAME_H
