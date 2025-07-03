// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file child_process.h

#ifndef MCRL2_SMT_CHILD_PROCESS_H
#define MCRL2_SMT_CHILD_PROCESS_H

#include <chrono>
#include <string>
#include <memory>

namespace mcrl2::smt
{

class child_process
{
protected:
  struct platform_impl;

  std::string m_name;
  // The declaration of the pipes requires expensive headers on Windows, so
  // we use the pimpl idiom to hide platform dependent implementation details.
  std::shared_ptr<platform_impl> m_pimpl;

  void initialize();

  /**
   * \brief Send the SIGINT signal to the child
   */
  void send_sigint() const;

public:
  child_process(const std::string& name)
  : m_name(name)
  {
    initialize();
  }

  ~child_process();

  void write(const std::string& s) const;

  /**
   * \brief Read output from the child process. This is a blocking call.
   */
  std::string read() const;

  /**
   * \brief Read output from the child process. If no output is available before
   * the timeout happens, a SIGINT signal is sent to the child process and the
   * resulting output is read and returned. NOTE: this function is not
   * on Windows, and behaves like read()
   */
  std::string read(const std::chrono::microseconds& timeout) const;
};

} // namespace mcrl2::smt

#endif // MCRL2_SMT_CHILD_PROCESS_H
