// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/file_utility.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_FILE_UTILITY_H
#define MCRL2_UTILITIES_FILE_UTILITY_H

#include <ctime>
#include <cstdio>
#include <sstream>
#include <vector>

namespace mcrl2
{

namespace utilities
{

class file_format
{
private:
  std::string m_shortname;
  std::string m_description;
  std::vector<std::string> m_extensions;
public:
  file_format(const std::string& shortname, const std::string& description)
    : m_shortname(shortname), m_description(description)
  { }
  void add_extension(const std::string& ext)
  {
    m_extensions.push_back(ext);
  }
  const std::string& shortname() const { return m_shortname; }
  const std::string& description() const { return m_description; }
  bool matches(const std::string& filename) const
  {
    for (auto it = m_extensions.begin(); it != m_extensions.end(); ++it)
    {
      if (filename.rfind(*it, filename.size() - it->size()) != std::string::npos)
      {
        return true;
      }
    }
    return false;
  }
  static const file_format* unknown()
  {
    static file_format unknown("unknown", "Unknown format");
    return &unknown;
  }
  bool operator==(const file_format& other)
  {
    return m_shortname == other.m_shortname;
  }
};

inline
std::ostream& operator<<(std::ostream& stream, const file_format& format)
{
  return stream << format.shortname();
}

inline
std::ostream& operator<<(std::ostream& stream, const file_format* format)
{
  return stream << format->shortname();
}

inline
bool file_exists(const std::string& filename)
{
  if (FILE * file = fopen(filename.c_str(), "r"))
  {
    fclose(file);
    return true;
  }
  return false;
}

inline
std::string create_filename(const std::string& prefix = "file", const std::string& extension = ".txt")
{
  std::stringstream ss;
  ss << prefix << time(nullptr) << extension;
  return ss.str();
}

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_FILE_UTILITY_H
