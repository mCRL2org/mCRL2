// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_LTS_DETAIL_QUEUE_H
#define MCRL2_LTS_DETAIL_QUEUE_H

#include <limits>
#include <cassert>
#include <deque>
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace lts
{

template <class T>
class queue
{
  private:
    std::deque <T> queue_get;
    std::deque <T> queue_put;
    size_t queue_size_max;        // This is the maximal allowed size of a queue
    size_t queue_put_count_extra; // This represents the number of elements that
    // did not fit in the queue.
    bool queue_size_fixed;

    T add_to_full_queue(T state)
    {
      /* We wish that every state has equal chance of being in the queue.
       * Let N be the size of the queue and M the number of states from which
       * we can choose. (Note that N <= M; otherwise every state is simply in
       * the queue. We show that addition of state i, with N < i <= M, should
       * be done with chance N/i and at random in the queue. With induction
       * on the difference between M-N we show that doing so leads to a
       * uniform distribution (i.e. every state has chance N/M of being in the
       * queue):
       *
       * M-N = 0:   Trivial.
       * M-N = k+1: We added the last state, M, with probability N/M, so we
       *            need only consider the other states. Before adding state M
       *            they are in the queue with probability N/(M-1) (by
       *            induction) and if the last state is added, they are still
       *            in the queue afterwards with probability 1-1/N. So:
       *
       *              N/(M-1) ( N/M ( 1 - 1/N ) + ( 1 - N/M ) )
       *            =
       *              N/(M-1) ( N/M (N-1)/N + (M-N)/M )
       *            =
       *              N/(M-1) ( (N-1)/M + (M-N)/M )
       *            =
       *              N/(M-1) (M-1)/M
       *            =
       *              N/M
       *
       *
       * Here we have that N = queue_size and
       * i = queue_put_count + queue_put_count_extra.
       */

      assert(queue_size_max == queue_put.size());
      queue_put_count_extra++;
      if ((rand() % (queue_put.size() + queue_put_count_extra)) < queue_put.size())
      {
        size_t pos = rand() % queue_put.size();
        T old_state = queue_put[pos];
        queue_put[pos] = state;
        return old_state;
      }
      return state;
    }

  public:
    queue() :
      queue_get(),
      queue_put(),
      queue_size_max(UINT_MAX),
      queue_put_count_extra(0),
      queue_size_fixed(false)
    {
    }

    size_t max_size() const
    {
      return queue_size_max;
    }

    void set_max_size(size_t max_size)
    {
      queue_size_max = max_size;
      queue_size_fixed = true;
      if (queue_put.size() > queue_size_max)
      {
        queue_put.resize(queue_size_max);
        mCRL2log(log::warning) << "resizing put queue loses elements" << std::endl;
      }
      if (queue_get.size() > queue_size_max)
      {
        queue_get.resize(queue_size_max);
        mCRL2log(log::warning) << "resizing get queue loses elements" << std::endl;
      }
    }

    T add_to_queue(T state)
    {
      if ((queue_size_fixed) && queue_put.size() >= queue_size_max)
      {
        assert(queue_put.size() == queue_size_max);
        return add_to_full_queue(state);
      }

      queue_put.push_back(state);

      return T();
    }

    T get_from_queue()
    {
      if (remaining() == 0)
      {
        return T();
      }
      else
      {
        T result = queue_get.front();
        queue_get.pop_front();
        return result;
      }
    }

    size_t remaining()
    {
      return queue_get.size();
    }

    void swap_queues()
    {
      queue_get.swap(queue_put);
      queue_put_count_extra = 0;
    }
};

}

}

#endif // MCRL2_LTS_DETAIL_QUEUE_H
