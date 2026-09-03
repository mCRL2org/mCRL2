// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/process_pool.h
/// \brief A pool of worker processes that process line-oriented requests.

#ifndef MCRL2_UTILITIES_PROCESS_POOL_H
#define MCRL2_UTILITIES_PROCESS_POOL_H

#include "mcrl2/utilities/boost_process.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

namespace mcrl2::utilities
{

/// \brief A pool of worker processes that process line-oriented requests.
///
/// Each request is a single line written to a worker's standard input; the
/// corresponding response is the single line the worker writes to its standard
/// output. Requests are distributed among the workers and responses are
/// returned in the same order as the requests. A request whose worker exceeds
/// the per-request time limit, or whose worker terminates without answering,
/// yields an empty response and the worker is restarted. The worker's standard
/// error is left connected to the caller's, so its diagnostics remain visible.
class process_pool
{
public:
  /// \param command The command line of the worker processes (argv[0] first).
  /// \param num_workers The number of worker processes to keep alive.
  /// \param timeout_seconds The time limit in seconds for one request; 0 means no limit.
  process_pool(std::vector<std::string> command, std::size_t num_workers, double timeout_seconds)
    : m_command(std::move(command)),
      m_timeout(timeout_seconds)
  {
    num_workers = std::max<std::size_t>(1, num_workers);
    m_slots.reserve(num_workers);
    for (std::size_t i = 0; i < num_workers; ++i)
    {
      m_slots.push_back(std::make_unique<slot>());
      m_slots.back()->start(m_command);
    }
  }

  ~process_pool() = default;

  process_pool(const process_pool&) = delete;
  process_pool& operator=(const process_pool&) = delete;

  /// \brief Runs the requests and returns one response per request, in order.
  ///        An empty response means the request was abandoned (time limit) or
  ///        the worker terminated without answering.
  std::vector<std::string> run(const std::vector<std::string>& requests)
  {
    std::vector<std::string> results(requests.size());
    std::atomic<std::size_t> next{0};
    std::atomic<bool> failed{false};
    std::mutex exception_mutex;
    std::exception_ptr first_exception = nullptr;

    std::vector<std::thread> managers;
    managers.reserve(m_slots.size());
    for (std::unique_ptr<slot>& slot_ptr : m_slots)
    {
      slot& s = *slot_ptr;
      managers.emplace_back([this, &s, &requests, &results, &next, &failed, &exception_mutex, &first_exception]() {
        auto manager_loop = [&]()
        {
          const auto duration = std::chrono::duration<double>(m_timeout);
          for (;;)
          {
            if (failed)
            {
              return;
            }
            const std::size_t i = next.fetch_add(1);
            if (i >= requests.size())
            {
              return;
            }

            *s.in << requests[i] << std::endl;
            s.in->flush();

            std::string response;
            bool answered = false;
            {
              std::unique_lock<std::mutex> lock(s.mutex);
              const auto ready = [&s]() { return s.have_response || s.eof; };
              const bool woke = (m_timeout <= 0.0) ? (s.cv.wait(lock, ready), true)
                                                   : s.cv.wait_for(lock, duration, ready);
              if (woke && s.have_response)
              {
                response = std::move(s.response);
                s.have_response = false;
                answered = true;
              }
            }

            if (answered)
            {
              results[i] = std::move(response);
            }
            else
            {
              results[i].clear();
              s.respawn(m_command);
            }
          }
        };

        try
        {
          manager_loop();
        }
        catch (...)
        {
          std::lock_guard<std::mutex> guard(exception_mutex);
          if (!first_exception)
          {
            first_exception = std::current_exception();
          }
          failed = true;
        }
      });
    }

    for (std::thread& t : managers)
    {
      t.join();
    }
    if (first_exception)
    {
      std::rethrow_exception(first_exception);
    }
    return results;
  }

private:
  struct slot
  {
    std::unique_ptr<bp::child> child;
    std::unique_ptr<bp::opstream> in;
    std::unique_ptr<bp::ipstream> out;
    std::thread reader;

    std::mutex mutex;
    std::condition_variable cv;
    std::string response;
    bool have_response = false;
    bool eof = false;

    slot() = default;
    ~slot()
    {
      stop();
    }
    slot(const slot&) = delete;
    slot& operator=(const slot&) = delete;
    slot(slot&&) = delete;
    slot& operator=(slot&&) = delete;

    void start(const std::vector<std::string>& command)
    {
      in = std::make_unique<bp::opstream>();
      out = std::make_unique<bp::ipstream>();
      std::error_code ec;
      child = std::make_unique<bp::child>(command, bp::std_in < *in, bp::std_out > *out, ec);
      if (ec || !child->valid())
      {
        child.reset();
        in.reset();
        out.reset();
        throw std::runtime_error("could not start a worker process: " + ec.message());
      }

      {
        std::lock_guard<std::mutex> guard(mutex);
        have_response = false;
        eof = false;
        response.clear();
      }

      reader = std::thread([this]() {
        std::string line;
        while (std::getline(*out, line))
        {
          std::lock_guard<std::mutex> guard(mutex);
          response = std::move(line);
          have_response = true;
          cv.notify_all();
        }
        std::lock_guard<std::mutex> guard(mutex);
        eof = true;
        cv.notify_all();
      });
    }

    void stop()
    {
      if (child)
      {
        std::error_code ec;
        if (child->running(ec))
        {
          child->terminate(ec);
        }
        child->wait(ec);
        child.reset();
      }
      if (reader.joinable())
      {
        reader.join();
      }
      in.reset();
      out.reset();
    }

    void respawn(const std::vector<std::string>& command)
    {
      stop();
      start(command);
    }
  };

  std::vector<std::string> m_command;
  double m_timeout;
  std::vector<std::unique_ptr<slot>> m_slots;
};

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_PROCESS_POOL_H
