// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/basename.h
/// \brief functions to get the executable's path.

#ifndef MCRL2_BASENAME_H
#define MCRL2_BASENAME_H

#include "mcrl2/utilities/platform.h"

#include <string>
#include <vector>
#include "mcrl2/utilities/exception.h"

#ifdef MCRL2_PLATFORM_LINUX
#include <unistd.h>
#endif

#ifndef MCRL2_PLATFORM_WINDOWS
#include <cstdlib>
#endif

#ifdef MCRL2_PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef MCRL2_PLATFORM_MAC
#include <mach-o/dyld.h>
#endif

namespace mcrl2::utilities
{

  /// \brief Returns the absolute path to the running executable, resolved
  ///        against symbolic links where possible.
  /// \return The path, or an empty string when it could not be determined.
  inline std::string get_executable_path()
  {
    std::string path;
#ifdef MCRL2_PLATFORM_LINUX
    std::vector<char> buffer(4096);
    ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length <= 0)
    {
      return std::string();
    }
    buffer[static_cast<std::size_t>(length)] = '\0';
    path = buffer.data();
#elif defined(MCRL2_PLATFORM_MAC)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0)
    {
      return std::string();
    }
    path = buffer.data();
#elif defined(MCRL2_PLATFORM_WINDOWS)
    std::vector<char> buffer(4096);
    DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length == 0)
    {
      return std::string();
    }
    path = std::string(buffer.data(), length);
#endif

#ifndef MCRL2_PLATFORM_WINDOWS
    // Resolve symbolic links so that the returned path is independent of how
    // the executable was reached (a symlink or a name found on PATH).
    std::vector<char> resolved(4096);
    if (realpath(path.c_str(), resolved.data()) != nullptr)
    {
      return std::string(resolved.data());
    }
#endif
    return path;
  }

  /// \brief Returns the directory containing the running executable.
  /// \return A string
  inline std::string get_executable_basename()
  {
    const std::string path = get_executable_path();
    if (path.empty())
    {
      return path;
    }
#ifdef MCRL2_PLATFORM_WINDOWS
    const std::string::size_type t = path.find_last_of('\\');
#else
    const std::string::size_type t = path.find_last_of('/');
#endif
    return (t == std::string::npos) ? std::string() : path.substr(0, t);
  }
}

#endif // MCRL2_BASENAME_H
