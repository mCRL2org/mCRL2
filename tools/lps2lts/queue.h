// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file queue.h

#include "boost.hpp" // precompiled headers

#ifndef MCRL2_LPS2LTS_QUEUE_H
#define MCRL2_LPS2LTS_QUEUE_H

#include <aterm2.h>
#include "mcrl2/core/messaging.h"

class queue
{
  private:
    ATerm *queue_get;
    ATerm *queue_put;
    unsigned long queue_size;
    unsigned long queue_size_max;
    unsigned long queue_get_pos;
    unsigned long queue_get_count;
    unsigned long queue_put_count;
    unsigned long queue_put_count_extra;
    bool queue_size_fixed;

    ATerm
    add_to_full_queue(ATerm state)
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
      queue_put_count_extra++;
      if ((rand() % (queue_put_count + queue_put_count_extra)) < queue_size)
      {
        unsigned long pos = rand() % queue_size;
        ATerm old_state = queue_put[pos];
        if (!ATisEqual(old_state, state))
        {
          queue_put[pos] = state;
          return old_state;
        }
      }
      return state;
    }

  public:
    queue() :
      queue_get(NULL),
      queue_put(NULL),
      queue_size(0),
      queue_size_max(UINT_MAX),
      queue_get_pos(0),
      queue_get_count(0),
      queue_put_count(0),
      queue_put_count_extra(0),
      queue_size_fixed(false)
    {}

    ~queue()
    {
      if(queue_size != 0)
      {
        ATunprotectArray(queue_get);
        ATunprotectArray(queue_put);
        free(queue_get);
        free(queue_put);
      }
    }

    unsigned long
    max_size() const
    {
      return queue_size_max;
    }

    void
    set_max_size(unsigned long max_size)
    {
      queue_size_max = max_size;
      if (queue_size > queue_size_max)
      {
        std::cerr << "Warning: resizing queue loses elements" << std::endl;
        queue_size = queue_size_max;
      }
    }

    // Queue
    ATerm
    add_to_queue(ATerm state)
    {
      if (queue_put_count == queue_size)
      {
        if (queue_size_fixed)
        {
          return add_to_full_queue(state);
        }
        if (queue_size == 0)
        {
          queue_size = (queue_size_max < 128) ? queue_size_max : 128;
        }
        else
        {
          if (2 * queue_size > queue_size_max)
          {
            queue_size_fixed = true;
            if (queue_size == queue_size_max)
            {
              return add_to_full_queue(state);
            }
            else
            {
              queue_size = queue_size_max;
            }
          }
          else
          {
            queue_size = queue_size * 2;
          }
          ATunprotectArray(queue_get);
          ATunprotectArray(queue_put);
        }
        ATerm *tmp;
        tmp = (ATerm *) realloc(queue_get, queue_size * sizeof(ATerm));
        if (tmp == NULL)
        {
          if (queue_size != 0)
          {
            mcrl2::core::gsWarningMsg(
                "cannot store all unexplored states (more than %lu); dropping some states from now on\n",
                queue_put_count);
            queue_size = queue_put_count;
            ATprotectArray(queue_get, queue_size);
            ATprotectArray(queue_put, queue_size);
          }
          queue_size_fixed = true;
          return add_to_full_queue(state);
        }
        queue_get = tmp;
        tmp = (ATerm *) realloc(queue_put, queue_size * sizeof(ATerm));
        if (tmp == NULL)
        {
          mcrl2::core::gsWarningMsg(
              "cannot store all unexplored states (more than %lu); dropping some states from now on\n",
              queue_put_count);
          tmp = (ATerm *) realloc(queue_get, queue_size * sizeof(ATerm));
          if (tmp != NULL)
          {
            queue_get = tmp;
          }
          queue_size = queue_put_count;
          ATprotectArray(queue_get, queue_size);
          ATprotectArray(queue_put, queue_size);
          queue_size_fixed = true;
          return add_to_full_queue(state);
        }
        queue_put = tmp;
        for (unsigned long i = queue_put_count; i < queue_size; i++)
        {
          queue_get[i] = NULL;
          queue_put[i] = NULL;
        }
        ATprotectArray(queue_get, queue_size);
        ATprotectArray(queue_put, queue_size);
      }

      queue_put[queue_put_count++] = state;
      return NULL;
    }

    ATerm
    get_from_queue()
    {
      if (queue_get_pos == queue_get_count)
      {
        return NULL;
      }
      else
      {
        return queue_get[queue_get_pos++];
      }
    }

    void
    swap_queues()
    {
      ATerm *t = queue_get;
      queue_get = queue_put;
      queue_put = t;
      queue_get_pos = 0;
      queue_get_count = queue_put_count;
      queue_put_count = 0;
      queue_put_count_extra = 0;
    }
};

#endif // MCRL2_LPS2LTS_QUEUE_H

