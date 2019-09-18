// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file child_process.h

#include <string>
#include <memory>

namespace mcrl2
{
namespace smt
{

class child_process
{
protected:
  struct pipes;

  std::string m_name;
  // The declaration of the pipes requires expensive headers on Windows, so
  // we use the pimpl idiom to hide those implementation details.
  std::shared_ptr<pipes> m_pipes;

  void initialize();

public:
  child_process(const std::string& name)
  : m_name(name)
  {
    initialize();
  }

  ~child_process();

  void write(const std::string& s) const;
  std::string read() const;
};

}
}
