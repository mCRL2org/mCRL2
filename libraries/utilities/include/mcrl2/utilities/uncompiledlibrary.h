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

#include <sstream>
#include <stdexcept>
#include "dynamiclibrary.h"

class uncompiled_library : public dynamic_library
{
private:
    std::string m_compile_script;
public:
    uncompiled_library(const std::string& script = "mcrl2compilerewriter") : m_compile_script(script) {};
    void compile(const std::string& filename) throw(std::runtime_error)
    {
      std::stringstream commandline;
      commandline << m_compile_script << " " << filename << " " << filename << ".bin";
      std::cout << commandline << std::endl;
      int r = system(commandline.str().c_str());
      if (r != 0)
      {
        std::stringstream s;
        s << "Executing compile script failed, return code was " << std::hex << r;
        throw std::runtime_error(s.str());
      }
      m_filename = std::string("./") + filename + ".bin";
    }
    virtual void unload() throw(std::runtime_error)
    {
      dynamic_library::unload();
      if (!m_filename.empty())
      {
        if (unlink(m_filename.c_str()))
        {
          std::stringstream s;
          s << "Could not remove file: " << m_filename;
          throw std::runtime_error(s.str());
        }
      }
    }
};

#endif // __UNCOMPILED_LIBRARY_H
