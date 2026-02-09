// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer_todo_set.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_EXPLORER_TODO_SET_H
#define MCRL2_LPS_EXPLORER_TODO_SET_H

#include <random>
#include "mcrl2/atermpp/standard_containers/deque.h"
#include "mcrl2/lps/stochastic_state.h"

namespace mcrl2::lps
{

class todo_set
{
  protected:
    atermpp::deque<state> todo;

  public:
    explicit todo_set() = default;

    explicit todo_set(const state& init)
      : todo{init}
    {}

    template<typename ForwardIterator>
    todo_set(ForwardIterator first, ForwardIterator last)
      : todo(first, last)
    {}

    virtual ~todo_set() = default;

    virtual void choose_element(state& result)
    {
      result = todo.front();
      todo.pop_front();
    }

    virtual void insert(const state& s)
    {
      todo.push_back(s);
    }

    virtual void finish_state()
    { }

    virtual bool empty() const
    {
      return todo.empty();
    }

    virtual std::size_t size() const
    {
      return todo.size();
    }
};

class breadth_first_todo_set : public todo_set
{
  public:
    explicit breadth_first_todo_set()
      : todo_set()
    {}

    explicit breadth_first_todo_set(const state& init)
      : todo_set(init)
    {}

    template<typename ForwardIterator>
    breadth_first_todo_set(ForwardIterator first, ForwardIterator last)
      : todo_set(first, last)
    {}

    void choose_element(state& result) override
    {
      result = todo.front();
      todo.pop_front();
    }

    void insert(const state& s) override
    {
      todo.push_back(s);
    }

    atermpp::deque<state>& todo_buffer()
    {
      return todo;
    }

    void swap(breadth_first_todo_set& other) noexcept { todo.swap(other.todo); }
};

class depth_first_todo_set : public todo_set
{
  public:
    explicit depth_first_todo_set(const state& init)
      : todo_set(init)
    {}

    template<typename ForwardIterator>
    depth_first_todo_set(ForwardIterator first, ForwardIterator last)
      : todo_set(first, last)
    {}

    void choose_element(state& result) override
    {
      result = todo.back();
      todo.pop_back();
    }

    void insert(const state& s) override
    {
      todo.push_back(s);
    }
};

class highway_todo_set : public todo_set
{
  protected:
    std::size_t N;
    breadth_first_todo_set new_states;
    std::size_t n=0;    // This is the number of new_states that are seen, of which at most N are stored in new_states.
    std::random_device device;
    std::mt19937 generator;

  public:
    explicit highway_todo_set(const state& init, std::size_t N_)
      : N(N_),
        new_states(init),
        n(1),
        device(),
        generator(device())
    {
    }

    template<typename ForwardIterator>
    highway_todo_set(ForwardIterator first, ForwardIterator last, std::size_t N_)
      : N(N_),
        device(),
        generator(device())
    {
      for(ForwardIterator i=first; i!=last; ++i)
      {
        insert(*i);
      }
    }

    void choose_element(state& result) override
    {
      if (todo.empty())
      {
        assert(new_states.size()>0);
        todo.swap(new_states.todo_buffer());
      }
      result = todo.front();
      todo.pop_front();
    }

    void insert(const state& s) override
    {
      if (new_states.size() < N-1)
      {
        new_states.insert(s);
      }
      else
      {
        std::uniform_int_distribution<> distribution(0, n-1);
        std::size_t k = distribution(generator);
        if (k < N)
        {
          assert(N==new_states.size());
          (new_states.todo_buffer())[k] = s;
        }
      }
      n++;
    }

    std::size_t size() const override
    {
      return todo.size() + new_states.size();
    }

    bool empty() const override
    {
      return todo.empty() && new_states.empty();
    }

    void finish_state() override
    {
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_TODO_SET_H
