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
#include <cstdio>
#include <list>
#include <string>
#include <sstream>
#include <stdexcept>
#include "dynamiclibrary.h"
#include "mcrl2/utilities/file_utility.h"
#include "mcrl2/utilities/logger.h"

class uncompiled_library : public dynamic_library
{
private:
    std::list<std::string> m_tempfiles;
    std::string m_compile_script;

public:
    uncompiled_library(const std::string& script) : m_compile_script(script) {}

    void compile(const std::string& filename) throw(std::runtime_error)
    {
      std::stringstream commandline;
      commandline << '"' << m_compile_script << "\" " << filename << " " << " 2>&1";
      
      // Execute script.
      FILE* stream = popen(commandline.str().c_str(), "r");
      if (stream == NULL)
      {
        throw std::runtime_error("Could not execute compile script.");
      }
      
      // Explicitly lock the output stream. It seems that on MacOSX some mixing
      // of output streams happens, especially during testing with multiple threads.
      // This results in ferror returning a non-zero value further down.
      flockfile(stream);

      // Script produces one file per line. Last file is the shared library,
      // preceding files are temporary files that should be removed when the
      // library is unloaded.
      std::string files;
      char buf[1024];
      while(fgets(buf, 1024, stream) != NULL)
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
          while (fgets(buf, 1024, stream) != NULL)
          {
            mCRL2log(mcrl2::log::error) << std::string(buf);
          }
          funlockfile(stream);
          pclose(stream);
          throw std::runtime_error("Compile script failed.");
        }
        else
        {
          mCRL2log(mcrl2::log::debug, "uncompiled_library") << "Temporary file '" << line << "' generated." << std::endl;
        }
        m_tempfiles.push_back(line);
      }
      
      if (ferror(stream))
      {
        funlockfile(stream);
        pclose(stream);
        throw std::runtime_error("There was a problem reading the output of the compile script.");
      }
      
      funlockfile(stream);
      pclose(stream);

      m_filename = m_tempfiles.back();
    }

    void leave_files()
    {
      m_tempfiles.clear();
    }

    void cleanup() throw(std::runtime_error)
    {
      for(std::list<std::string>::iterator f = m_tempfiles.begin(); f != m_tempfiles.end(); ++f)
      {
        if (unlink((*f).c_str()))
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
      try
      {
        cleanup();
      }
      catch (std::runtime_error &error)
      {
        mCRL2log(mcrl2::log::error) << "Could not cleanup temporary files: " << error.what() << std::endl;
      }
    }

};

#endif // __UNCOMPILED_LIBRARY_H
