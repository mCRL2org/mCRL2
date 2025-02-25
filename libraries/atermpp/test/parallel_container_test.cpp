// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE parallel_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/utilities/configuration.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/atermpp/detail/global_aterm_pool.h"

#include <thread>

using namespace atermpp;

BOOST_AUTO_TEST_CASE(parallel_vector)
{
  if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
  {
    // One thread continuously modifies a local atermpp::vector of aterms while the main thread performs garbage collection extensively.
    std::thread local([]() 
    {
      // Push a constant to avoid term creations.
      atermpp::aterm_int value(0);

      for (std::size_t i = 0; i < 1000; ++i)
      {
        atermpp::vector<atermpp::aterm> vector;
        for (std::size_t i = 0; i < 1000; ++i)
        {
          vector.push_back(value);
        }
      }
    });

    while (true)
    {
      for (std::size_t i = 0; i < 1000; ++i)
      {
        atermpp::detail::g_thread_term_pool().collect();
      }

      if (local.joinable())
      {
        local.join();
        break;
      }
    }
  }
}

