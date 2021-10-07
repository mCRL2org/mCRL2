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

namespace mcrl2
{
namespace data
{
namespace detail
{

/// This is an exception that is thrown when the rewrite 
/// stack must be resized. As the rewrite stack is a vector
/// this invalidates all references and iterators in it,
/// and that means that when resizing is necessary, the 
/// current term must be resized. When this exception is
/// thrown, the current substitution must also be restored. 
struct recalculate_term_as_stack_is_too_small {};


class rewrite_stack : public atermpp::vector<data_expression>
{

  public:

    /// \brief Constructor
    rewrite_stack() 
    {
      reserve_more_space();
    }

    void reserve_more_space() 
    {
      reserve(std::max(2*capacity(),static_cast<std::size_t>(128)));
    }

    void increase(std::size_t distance)
    { 
      if (size()+distance>=capacity())
      { 
        throw recalculate_term_as_stack_is_too_small();
      }
      resize(size()+distance);
    }

    void decrease(std::size_t distance)
    { 
      assert(distance<=size());
      resize(size()-distance);
    }

    data_expression& top()
    { 
      assert(size()>0);
      return back();
    }

    void set_element(std::size_t pos, std::size_t frame_size, const data_expression& d)
    {
      assert(size()+pos>=frame_size && pos<frame_size);
      operator[](size()-frame_size+pos)=d;
    }

    data_expression& element(std::size_t pos, std::size_t frame_size)
    {
      assert(size()+pos>=frame_size && pos<frame_size);
      return at(size()-frame_size+pos);
    }

    atermpp::vector<data_expression>::const_iterator stack_iterator(std::size_t pos, std::size_t frame_size) const
    {
      assert(size()+pos>=frame_size && pos<frame_size);
      return begin()+size()-frame_size+pos;
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_REWRITE_STACK_H

