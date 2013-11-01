// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_balanced_tree.h
/// \brief Balanced binary tree of terms.

#ifndef MCRL2_ATERMPP_ATERM_BALANCED_TREE_H
#define MCRL2_ATERMPP_ATERM_BALANCED_TREE_H

#include <cassert>
#include <limits>
#include <memory>
#include <stack>
#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/function_symbol.h"

namespace atermpp
{

template < typename Value >
class term_balanced_tree_iterator;

/// \brief Read-only balanced binary tree of terms.
///
/// Models Random Access Container (STL concept)
template <typename Term>
class term_balanced_tree: public aterm_appl
{
    template < typename T >
    friend class term_balanced_tree_iterator;

    // template <typename T, typename F>
    // friend term_balanced_tree< T > apply(const term_balanced_tree< T > &l, const F f);

  protected:

    static const atermpp::function_symbol &tree_empty_function()
    {
      static const atermpp::function_symbol empty("@empty@", 0);
      return empty;
    }

    static function_symbol const& tree_node_function()
    {
      static const function_symbol node("@node@", 2);
      return node;
    }

    static const aterm_appl &empty_tree()
    {
      static const aterm_appl empty_term(detail::aterm0(tree_empty_function()));
      return empty_term;
    }

    static bool is_empty(const detail::_aterm* tree)
    {
      return tree->function() == tree_empty_function();
    }

    template < typename ForwardTraversalIterator >
    static size_t get_distance(ForwardTraversalIterator begin, ForwardTraversalIterator end)
    {
      size_t size=0;
      for (ForwardTraversalIterator i=begin; i!=end; ++i)
      {
        ++size;
      }
      return size;
    }


    template < typename ForwardTraversalIterator >
    const detail::_aterm* make_tree(ForwardTraversalIterator& p, const size_t size)
    {
      if (size>1)
      {
        size_t left_size = (size + 1) >> 1; // size/2 rounded up.
        const term_balanced_tree left_tree(make_tree(p, left_size));
        size_t right_size = size >> 1; // size/2 rounded down.
        const term_balanced_tree right_tree(make_tree(p, right_size));
        return reinterpret_cast<const detail::_aterm*>(detail::term_appl2<term_balanced_tree>(tree_node_function(),left_tree,right_tree));
      }

      if (size==1)
      {
        return atermpp::detail::address(*(p++));
      }

      assert(size==0);
      return atermpp::detail::address(empty_tree()); // must be optimised.
    }

  public:

    /// The type of object, T stored in the term_balanced_tree.
    typedef Term value_type;
    /// Pointer to T.
    typedef Term* pointer;

    /// Reference to T.
    typedef Term& reference;

    /// Const reference to T.
    typedef const Term const_reference;

    /// An unsigned integral type.
    typedef size_t size_type;

    /// A signed integral type.
    typedef ptrdiff_t difference_type;

    /// Iterator used to iterate through an term_balanced_tree.
    typedef term_balanced_tree_iterator<Term> iterator;

    /// Const iterator used to iterate through an term_balanced_tree.
    typedef term_balanced_tree_iterator<Term> const_iterator;

    /// Default constructor. Creates an empty tree.
    term_balanced_tree()
      : aterm_appl(empty_tree())
    {}

    /// Construction from aterm
    explicit term_balanced_tree(const aterm &tree)
       : aterm_appl(tree)
    {
    }

    /// Creates an term_balanced_tree with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template < typename ForwardTraversalIterator >
    term_balanced_tree(ForwardTraversalIterator first, const ForwardTraversalIterator last)
      : aterm_appl(make_tree(first,get_distance(first,last)))
    {
    }

    /// \brief Creates an term_balanced_tree with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param size The size of the range of elements.
    template < typename ForwardTraversalIterator >
    term_balanced_tree(ForwardTraversalIterator first, const size_t size)
      : aterm_appl(make_tree(first,size))
    {
    }

    /// \brief Get the left branch of the tree
    /// \details It is assumed that the tree is a node with a left branch.
    /// \return A reference t the left subtree of the current tree
    const term_balanced_tree<Term>& left_branch() const
    {
      assert(is_node());
      return aterm_cast< const term_balanced_tree<Term> >(aterm_cast<const aterm_appl>(*this)[0]);
    }

    /// \brief Get the left branch of the tree
    /// \details It is assumed that the tree is a node with a left branch.
    /// \return A reference t the left subtree of the current tree
    const term_balanced_tree<Term>& right_branch() const
    {
      assert(is_node());
      return aterm_cast< const term_balanced_tree<Term> >(aterm_cast<const aterm_appl>(*this)[1]);
    }

    /// \brief Element indexing operator.
    /// \param position Index in the tree.
    /// \details This operation behaves linear with respect to container size,
    ///          because it must calculate the size of the container. The operator
    ///          element_at behaves logarithmically.
    const Term &operator[](size_t position) const
    {
      return element_at(position, size());
    }

    /// \brief Get an element at the indicated position.
    /// \param position The required position
    /// \param size The number of elements in the tree.
    ///                    This is required to make the complexity logarithmic.
    /// \details By providing the size this operation is logarithmic. If a wrong
    ///         size is provided the outcome is not determined. See also operator [].
    /// \return The element at the indicated position.
    const Term &element_at(size_t position, size_t size) const
    {
      assert(size == this->size());
      assert(position < size);

      if (size>1)
      {
        size_t left_size = (size + 1) >> 1;

        return (position < left_size) ?
               left_branch().element_at(position, left_size) :
               right_branch().element_at(position-left_size, size - left_size);
      }

      return *this;
    }

    /// \brief Returns a const_iterator pointing to the beginning of the term_balanced_tree.
    /// \return The beginning of the list.
    const_iterator begin() const
    {
      return const_iterator(*this);
    }

    /// \brief Returns a const_iterator pointing to the end of the term_balanced_tree.
    /// \return The end of the list.
    const_iterator end() const
    {
      return const_iterator();
    }

    /// \brief Returns the size of the term_balanced_tree.
    /// \details This operator is linear in the size of the balanced tree.
    /// \return The size of the tree.
    size_type size() const
    {
      if (is_node())
      {
        return left_branch().size() + right_branch().size();
      }
      return (empty()) ? 0 : 1;
    }

    /// \brief Returns true if tree is empty.
    /// \return True iff the tree is empty.
    bool empty() const
    {
      return m_term->function()==tree_empty_function();
    }

    /// \brief Returns true iff the tree is a node with a left and right subtree.
    /// \return True iff the tree is a node with a left and right subtree.
    bool is_node() const
    {
      return function()==tree_node_function();
    }
};

template < typename Value >
class term_balanced_tree_iterator: public boost::iterator_facade<
  term_balanced_tree_iterator< Value >, // Derived
  const Value,                          // Value
  boost::forward_traversal_tag,         // CategoryOrTraversal
  const Value &                         // Reference
  >
{
  private:

    friend class boost::iterator_core_access;

    std::stack< const atermpp::detail::_aterm* > m_trees;

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const Value &dereference() const
    {
      return reinterpret_cast<const Value &>(m_trees.top());
    }

    /// \brief Determine if a stack is empty
    bool is_empty(const std::stack<const atermpp::detail::_aterm*>& tree) const
    {
      return tree.empty() || (tree.size() == 1 && term_balanced_tree<Value>::is_empty(tree.top()));
    }

    /// \brief Equality operator
    bool equal(const term_balanced_tree_iterator &other) const
    {
      return m_trees == other.m_trees || (is_empty(m_trees) && is_empty(other.m_trees));
    }

    /// \brief Increments the iterator
    void increment()
    {

      m_trees.pop();

      if (!m_trees.empty())
      {
        const detail::_aterm* current = m_trees.top();

        if (current->function()!=term_balanced_tree < Value >::tree_node_function())
        {
          return;
        }
        m_trees.pop();
        do
        {
          m_trees.push(atermpp::detail::address((reinterpret_cast<const detail::_aterm_appl<aterm> *>(current)->arg[1])));
          current=atermpp::detail::address(reinterpret_cast<const detail::_aterm_appl<aterm> *>(current)->arg[0]);
        }
        while (current->function()==term_balanced_tree < Value >::tree_node_function());

        m_trees.push(current);
      }
    }

    void initialise(const aterm &tree)
    {
      const detail::_aterm_appl<aterm>* current = reinterpret_cast<const detail::_aterm_appl<aterm> *>(atermpp::detail::address(tree));

      while (current->function()==term_balanced_tree< Value >::tree_node_function())
      {
        m_trees.push(atermpp::detail::address(current->arg[1]));
        current=reinterpret_cast<const detail::_aterm_appl<aterm > *>(atermpp::detail::address(current->arg[0]));
      }
      m_trees.push(current);
    }

  public:

    term_balanced_tree_iterator()
    { }

    term_balanced_tree_iterator(const aterm &tree)
    {
      initialise(tree);
    }

    template < typename OtherTermType >
    term_balanced_tree_iterator(term_balanced_tree< OtherTermType > const& tree)
    {
      initialise(tree);
    }

    term_balanced_tree_iterator(term_balanced_tree_iterator const& other) : m_trees(other.m_trees)
    { }
};


/// \brief A term_balanced_tree with elements of type aterm.
typedef term_balanced_tree<aterm> aterm_balanced_tree;

/// \brief Swaps two balanced trees.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term
template <class T>
inline void swap(atermpp::term_balanced_tree<T> &t1, atermpp::term_balanced_tree<T> &t2)
{
  t1.swap(t2);
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_BALANCED_TREE_H
