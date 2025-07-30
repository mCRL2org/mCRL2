// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewrite_stack.h
/// \brief This file contains a rewrite stack that can operate
///        alongside the normal stack. As it is an atermpp container
///        protection of aterm derived data types is guaranteed, 
///        without explicitly putting each entry in a protection set.
///        This makes the use of this stack much cheaper than using
///        the processor stack. An additional advantage is that
///        rewriting does not run out of stack space that quickly 
///        anymore. 

#ifndef MCRL2_DATA_DETAIL_REWRITE_STACK_H
#define MCRL2_DATA_DETAIL_REWRITE_STACK_H

#include <algorithm>
#include "mcrl2/atermpp/standard_containers/vector.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2::data::detail
{

/// This is an exception that is thrown when the rewrite 
/// stack must be resized. As the rewrite stack is a vector
/// this invalidates all references and iterators in it,
/// and that means that when resizing is necessary, the 
/// current term must be resized. When this exception is
/// thrown, the current substitution must also be restored. 
///
/// Note that the vector leaves terms on the stack as protected
/// members when the stack is decreased. This is done intentionally
/// because the stack is intensively used by the rewriters, and
/// destroying and creating elements in the vector is a relatively
/// expensive operation. 

struct recalculate_term_as_stack_is_too_small {};


class rewrite_stack : protected atermpp::vector<data_expression>
{
  protected:
    std::size_t m_stack_size = 0;
    std::size_t m_reserved_stack_size = 0UL; // This is equal to the size() of the underlying stack.
                                             // Access of "size()" expensive as it is surrounded by a mutex.

  public:

    /// \brief Constructor
    rewrite_stack()
    {
      reserve_more_space();
    }

    void reserve_more_space() 
    {
      resize(std::max(2*size(),static_cast<std::size_t>(128)));
      m_reserved_stack_size=size();
      m_stack_size=0;
    }

    void increase(std::size_t distance)
    { 
      if (m_stack_size+distance>=m_reserved_stack_size)
      { 
        throw recalculate_term_as_stack_is_too_small();
      }
      m_stack_size=m_stack_size+distance;
    }

    void decrease(std::size_t distance)
    { 
      assert(distance<=m_stack_size);
      m_stack_size=m_stack_size-distance;
    }

    template <class T>
    T& new_stack_position()
    { 
      if (m_stack_size+1>=m_reserved_stack_size)
      { 
        throw recalculate_term_as_stack_is_too_small();
      }
      data_expression& result=operator[](m_stack_size);
      m_stack_size++;
      return dynamic_cast<T&>(result);
    }

    data_expression& top()
    { 
      assert(m_stack_size>0);
      return operator[](m_stack_size-1);
    }

    void set_element(std::size_t pos, std::size_t frame_size, const data_expression& d)
    {
      assert(m_stack_size+pos>=frame_size && pos<frame_size);
      operator[](m_stack_size-frame_size+pos)=d;
    }

    data_expression& element(std::size_t pos, std::size_t frame_size)
    {
      assert(m_stack_size+pos>=frame_size && pos<frame_size);
      return operator[](m_stack_size-frame_size+pos);
    }

    atermpp::vector<data_expression>::const_iterator stack_iterator(std::size_t pos, std::size_t frame_size) const
    {
      assert(m_stack_size+pos>=frame_size && pos<frame_size);
      return begin()+m_stack_size-frame_size+pos;
    }

    std::size_t stack_size() const
    {
      return m_stack_size;
    }

    void reset_stack_size(std::size_t n)
    {
      m_stack_size=n;
    }
};

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_REWRITE_STACK_H

