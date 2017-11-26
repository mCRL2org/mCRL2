// Author(s): Jeroen Keiren, Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/uncompiled_library.h
/// \brief Extends the dynamic_library from dynamiclibrary.h to be able to compile a
///        source file and load the resulting library.

#ifndef __UNCOMPILED_LIBRARY_H
#define __UNCOMPILED_LIBRARY_H

/*
 * Extends the dynamic_library from dynamiclibrary.h to be able to compile a
 * source file and load the resulting library.
 *
 * Usage:
 *
 *   uncompiled_library mylib;
 *   mylib.compile(source_filename);
 *   myfunc = mylib.proc_address("myfunc");
 *   myfunc(10);
 *
 * Remarks:
 *
 * The source is compiled using a script that must take two string arguments.
 * The first argument is the source file, the second is the destination file.
 * After (successful) termination, only the source and destination files must
 * remain on disk -- it is the responsibility of the script to remove any
 * temporary files.
 *
 */

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <list>
#include <string>
#include <sstream>
#include <stdexcept>
#include "mcrl2/utilities/dynamiclibrary.h"
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/utilities/logger.h"

class uncompiled_library : public dynamic_library
{
  private:
    std::list<std::string> m_tempfiles;
    std::string m_compile_script;

  public:
    uncompiled_library(const std::string& script) : m_compile_script(script) {}

    void compile(const std::string& filename) 
    {
      std::stringstream commandline;
      commandline << '"' << m_compile_script << "\" " << filename << " " << " 2>&1";
      
      // Execute script.
      FILE* stream = popen(commandline.str().c_str(), "r");
      if (stream == NULL)
      {
        throw std::runtime_error("Could not execute compile script.");
      }

      // Script produces one file per line. Last file is the shared library,
      // preceding files are temporary files that should be removed when the
      // library is unloaded.
      std::string files;
      char buf[1024];
      while(!feof(stream))
      {        
        if(fgets(buf, sizeof(buf), stream) != NULL)
        {
          std::string line(buf);
          assert(*line.rbegin() == '\n');
          line.erase(line.size() - 1);
          mCRL2log(mcrl2::log::debug, "uncompiled_library") << "  Read line: " << line;
          
          // Check that reported file exists. If not, produce error message and
          // flush script output to the log.
          if (!mcrl2::utilities::file_exists(line))
          {
            mCRL2log(mcrl2::log::error) << "Compile script " << m_compile_script << " produced unexpected output:\n";
            mCRL2log(mcrl2::log::error) << line << std::endl;
            while (fgets(buf, sizeof(buf), stream) != NULL)
            {
              mCRL2log(mcrl2::log::error) << std::string(buf);
            }
            pclose(stream);
            throw std::runtime_error("Compile script failed.");
          }
          m_tempfiles.push_back(line);
        }
        else if(ferror(stream) && errno == EINTR)
        {
            // On OSX, interrupts sometimes arrive during the call to read(), which
            // is called by fgets. If an interrupt arrives, we just ignore it
            // an clear the error status of the stream, and try again.
            mCRL2log(mcrl2::log::debug, "uncompiled_library") << "Reading was interrupted. Clearing error status and retrying" << std::endl;
            perror("Error according to errno");
            clearerr(stream);
        }
      }
      
      if (ferror(stream))
      {
        pclose(stream);
        throw std::runtime_error("There was a problem reading the output of the compile script.");
      }
      
      pclose(stream);

      m_filename = m_tempfiles.back();
    }

    void leave_files()
    {
      m_tempfiles.clear();
    }

    void cleanup() 
    {
      for(std::list<std::string>::iterator f = m_tempfiles.begin(); f != m_tempfiles.end(); ++f)
      {
        if (remove((*f).c_str()))
        {
          std::stringstream s;
          s << "Could not remove file: " << *f;
          throw std::runtime_error(s.str());
        }
        else
        {
           mCRL2log(mcrl2::log::debug, "uncompiled_library") << "Temporary file '" << *f << "' deleted." << std::endl;
        }
      }
    }

    virtual ~uncompiled_library()
    {
#ifndef NDEBUG // In debug mode, the compiled rewriter has not been removed directly after loading, 
               // and we still have to remove it.
      try
      {
        cleanup();
      }
      catch (std::runtime_error &error)
      {
        mCRL2log(mcrl2::log::error) << "Could not cleanup temporary files: " << error.what() << std::endl;
      }
#endif
    }

};

#endif // __UNCOMPILED_LIBRARY_H
