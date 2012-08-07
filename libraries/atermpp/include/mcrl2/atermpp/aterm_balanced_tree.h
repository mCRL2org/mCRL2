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
#include <boost/type_traits/is_convertible.hpp>
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
// class term_balanced_tree: public aterm_base OLD
class term_balanced_tree: public aterm 
{
    template < typename T >
    friend class term_balanced_tree_iterator;

    template <typename T, typename F>
    friend term_balanced_tree< T > apply(term_balanced_tree< T > l, const F f);

  protected:

    static const atermpp::function_symbol &tree_empty()
    {
      static atermpp::function_symbol empty("@empty@", 0);
      return empty;
    }

    static const atermpp::function_symbol &tree_node()
    {
      static atermpp::function_symbol node("@node@", 2);
      return node;
    }

    static bool is_empty(const aterm &tree)
    {
      return tree == tree_empty();
    }

    static bool is_node(const aterm &tree)
    {
      return tree.type() == AT_APPL && (aterm_appl(tree).function() == tree_node());
    }

    static const aterm &left_branch(const aterm &tree)
    {
      assert(is_node(tree));
      return (aterm_cast<const aterm_appl>(tree))(0);
    }

    static const aterm &right_branch(const aterm &tree)
    {
      assert(is_node(tree));
      return (aterm_cast<const aterm_appl>(tree))(1);
    }

    static aterm node(const aterm &left, const aterm &right)
    {
      return aterm_appl(tree_node(), left, right);
    }

    static size_t tree_size(const aterm &tree)
    {
      if (is_node(tree))
      {
        return tree_size(left_branch(tree)) + tree_size(right_branch(tree));
      }

      return (is_empty(tree)) ? 0 : 1;
    }

    const Term &element_at(size_t position) const
    {
      return element_at(*this, m_size, position);
    }

    static const Term &element_at(const aterm &tree, size_t size, size_t position)
    {
      assert(size == tree_size(tree));
      assert(position < size);

      if (size>1)
      {
        size_t left_size = (size + 1) >> 1;

        return (position < left_size) ?
               element_at(left_branch(tree), left_size, position) :
               element_at(right_branch(tree), size - left_size, position - left_size);
      }

      return aterm_cast<const Term>(tree);
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
    static aterm make_tree(ForwardTraversalIterator& p, const size_t size)
    {
      if (size==0)
      {
        return tree_empty();
      }

      if (size==1)
      {
        const aterm result=*p;
        ++p;
        return result;
      }
      else
      {
        size_t left_size = (size + 1) >> 1; // size/2 rounded up.
        const aterm left_tree=make_tree(p, left_size);
        size_t right_size = size >> 1; // size/2 rounded down.
        const aterm right_tree=make_tree(p, right_size);

        return node(left_tree,right_tree);
      }
    }

    /// \brief the number of elements in the container
    size_t m_size;

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
      : aterm(aterm_appl(tree_empty())), m_size(0)
    {}

    /// Construction from aterm
    term_balanced_tree(const aterm &tree)
      : aterm(tree), m_size(tree_size(tree))
    {
    }

    /// Construction from aterm_list. THIS IS A VERY ODD ASSIGNMENT OPERATOR. UGLY.
    /// \param l A list.
    term_balanced_tree(const atermpp::aterm_list &l)
      : m_size(l.size())
    {
      atermpp::aterm_list::const_iterator first=l.begin();
      copy_term(&*make_tree(first,m_size));
    } 

    /// Construction from a term_balanced_tree.
    /// \param t A term containing a list.
    template <typename SpecificTerm>
    term_balanced_tree(term_balanced_tree< SpecificTerm > const& t) :
      aterm(t.m_term), m_size(t.m_size)
    { }

    /// Creates an term_balanced_tree with a copy of a range.
    /// \param first The start of a range of elements.
    /// \param last The end of a range of elements.
    template < typename ForwardTraversalIterator >
    term_balanced_tree(ForwardTraversalIterator first, const ForwardTraversalIterator last)
    {
      m_size=get_distance(first,last);
      copy_term(&*make_tree(first,m_size));
    }

    /// Creates an term_balanced_tree with a copy of a range. THIS IS A VERY NON STANDARD ITERATOR 
    /// \param first The start of a range of elements.
    /// \param size The size of the range of elements.
    template < typename ForwardTraversalIterator >
    term_balanced_tree(ForwardTraversalIterator first, const size_t size)
    {
      m_size=size;
      copy_term(&*make_tree(first,m_size));
    }

    /// Assignment operator. THIS IS ALSO A NON STANDARD CONSTRUCTOR.
    /// \param t A term containing a list.
    term_balanced_tree<Term>& operator=(aterm_list &t)
    {
      m_size=t.size();
      copy_term(make_tree(t,m_size));
      return *this;
    } 

    /// Element indexing operator.
    /// \param position Index in the tree.
    /// This operation behaves logarithmically with respect to container size
    const Term &operator[](size_t position) const
    {
      return element_at(position);
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

    /// \brief Swaps contents of containers
    void swap(term_balanced_tree< Term >& other)
    {
      std::swap(m_term, other.m_term);
      std::swap(m_size, other.m_size);
    }

    /// \brief Returns the size of the term_balanced_tree.
    /// \return The size of the tree.
    size_type size() const
    {
      return m_size;
    }

    /// \brief Returns true if the list's size is 0.
    /// \return True if the list is empty.
    bool empty() const
    {
      return m_term->function()==detail::function_adm.AS_EMPTY_LIST;
    }

    /// \brief Conversion to aterm_list.
    /// \return The wrapped aterm_list pointer.
    operator term_list< Term >() const
    {
      return term_list< Term >(begin(), end());
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> nodes appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    term_balanced_tree<Term> substitute(Substitution f) const
    {
      return term_balanced_tree<Term>(f(*this));
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

    std::stack< aterm > m_trees;

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const Value &dereference() const
    {
      return aterm_cast<const Value>(m_trees.top());
    }

    /// \brief Equality operator
    bool equal(term_balanced_tree_iterator const& other) const
    {
      return m_trees == other.m_trees;
    }

    /// \brief Increments the iterator
    void increment()
    {
      m_trees.pop();

      if (!m_trees.empty())
      {
        while (term_balanced_tree< Value >::is_node(m_trees.top()))
        {
          aterm top(m_trees.top());

          m_trees.pop();

          m_trees.push(term_balanced_tree< Value >::right_branch(top));
          m_trees.push(term_balanced_tree< Value >::left_branch(top));
        }
      }
    }

    void initialise(aterm tree)
    {
      m_trees.push(tree);
      m_trees.push(tree);
      increment();
    }

  public:

    term_balanced_tree_iterator()
    { }

    term_balanced_tree_iterator(aterm tree)
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

/// \brief Applies a function to all elements of the list and returns the result.
/// \param l The list that is transformed.
/// \param f The function that is applied to the elements of the list.
/// \return The transformed list.
template <typename Term, typename Function>
inline
term_balanced_tree< Term > apply(term_balanced_tree<Term> l, const Function f)
{
  std::vector < Term > result;

  for (typename term_balanced_tree< Term >::const_iterator i = l.begin(); i != l.end(); ++i)
  {
    result.push_back(f(*i));
  }

  return term_balanced_tree< Term >(result.begin(),result.size());
} 

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_BALANCED_TREE_H
