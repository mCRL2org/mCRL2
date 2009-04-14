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
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/atermpp/vector.h"

namespace atermpp {

  template < typename Value >
  class term_balanced_tree_iterator;

  /// \brief Read-only balanced binary tree of terms.
  ///
  /// Models Random Access Container (STL concept)
  template <typename Term>
  class term_balanced_tree: public aterm_base
  {
    template < typename T >
    friend class term_balanced_tree_iterator;

    template <typename T, typename F>
    friend term_balanced_tree< T > apply(term_balanced_tree< T > l, const F f);

    protected:

      static function_symbol create_and_protect(function_symbol& target, function_symbol const&)
      {
        target = function_symbol("@node@", 2);
        target.protect();
        return target;
      }

      static function_symbol const& tree_empty()
      {
        static function_symbol empty = create_and_protect(empty, function_symbol("@empty@", 1));
        return empty;
      }

      static function_symbol const& tree_node()
      {
        static function_symbol node = create_and_protect(node, function_symbol("@node@", 2));
        return node;
      }

      static bool is_empty(aterm tree)
      {
        return tree == aterm_appl(tree_empty());
      }

      static bool is_node(aterm tree)
      {
        return tree.type() == AT_APPL && (aterm_appl(tree).function() == tree_node());
      }

      static aterm left_branch(aterm tree)
      {
        assert(is_node(tree));
        return aterm_appl(tree)(0);
      }

      static aterm right_branch(aterm tree)
      {
        assert(is_node(tree));
        return aterm_appl(tree)(1);
      }

      static bool is_tree(aterm tree)
      {
        size_t left(tree_size(left_branch(tree)));
        size_t right(tree_size(right_branch(tree)));

        return ((right <= left) && (right - left < 2)) && (is_tree(left_branch(tree)) && is_tree(right_branch(tree)));
      }

      static aterm node(aterm left, aterm right)
      {
        return aterm_appl(tree_node(), left, right);
      }

      static size_t tree_size(aterm tree)
      {
        if (is_node(tree))
        {
          return tree_size(left_branch(tree)) + tree_size(right_branch(tree));
        }

        return (is_empty(tree)) ? 0 : 1;
      }

      Term element_at(size_t position) const
      {
        return element_at(term(), m_size, position);
      }

      static Term element_at(aterm tree, size_t size, size_t position)
      {
        assert(size == tree_size(tree));
        assert(position < size);

        if (1 < size)
        {
          size_t left_size = (size + 1) >> 1;

          return (position < left_size) ?
            element_at(left_branch(tree), left_size, position) :
            element_at(right_branch(tree), size - left_size, position - left_size);
        }

        return tree;
      }

      static aterm make_tree(atermpp::vector< aterm >::const_iterator p, size_t size)
      {
        assert(0 < size);

        size_t left_size = (size + 1) >> 1;

        return (1 < size) ? node(make_tree(p, left_size),
                                 make_tree(p + left_size, size >> 1)) : *p;
      }

      template < typename ForwardTraversalIterator >
      aterm make_tree(ForwardTraversalIterator begin, ForwardTraversalIterator end)
      {
        atermpp::vector< aterm > nodes(begin, end);

        return (nodes.empty()) ? aterm(aterm_appl(tree_empty())) : make_tree(nodes.begin(), nodes.size());
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
        : aterm_base(aterm_appl(tree_empty())), m_size(0)
      {}

      /// Construction from aterm
      term_balanced_tree(aterm tree)
        : aterm_base(tree), m_size(tree_size(tree))
      {
        assert(is_tree(tree));
      }

      /// Construction from ATermList.
      /// \param l A list.
      term_balanced_tree(atermpp::aterm_list l)
        : aterm_base(make_tree(l.begin(), l.end())), m_size(tree_size(term()))
      { }

      /// Construction from a term_balanced_tree.
      /// \param t A term containing a list.
      template <typename SpecificTerm>
      term_balanced_tree(term_balanced_tree< SpecificTerm > const& t) :
         aterm_base(t.m_term), m_size(t.m_size)
      { }

      /// Creates an term_balanced_tree with a copy of a range.
      /// \param first The start of a range of elements.
      /// \param last The end of a range of elements.
      template < typename ForwardTraversalIterator >
      term_balanced_tree(ForwardTraversalIterator const& first, ForwardTraversalIterator const& last)
        : aterm_base(make_tree(first, last))
      { }

      /// Assignment operator.
      /// \param t A term containing a list.
      term_balanced_tree<Term>& operator=(ATermList t)
      {
        m_term = make_tree(t);
        m_size = tree_size(t);
        return *this;
      }

      /// Assignment operator.
      /// \param t A term containing a list.
      /// This operation behaves logarithmically with respect to container size
      Term operator[](size_t position) const
      {
        return Term(element_at(position));
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
        return ATisEmpty(term()) == ATtrue;
      }

      /// \brief Conversion to ATermList.
      /// \return The wrapped ATermList pointer.
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
          Value const&                          // Reference
      >
  {
    private:

      friend class boost::iterator_core_access;

      std::stack< aterm > m_trees;

      /// \brief Dereference operator
      /// \return The value that the iterator references
      Value const& dereference() const
      {
        return m_trees.top();
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
        assert(term_balanced_tree< Value >::is_tree(tree));
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
    term_list< Term > result;

    for (typename term_balanced_tree< Term >::const_iterator i = l.begin(); i != l.end(); ++i)
    {
      result = push_front(result, f(*i));
    }

    return term_balanced_tree< Term >(result);
  }

  /// \cond INTERNAL_DOCS
  template <typename Term>
  struct aterm_traits<term_balanced_tree<Term> >
  {
    typedef ATermList aterm_type;
    static void protect(term_balanced_tree<Term> t)   { t.protect(); }
    static void unprotect(term_balanced_tree<Term> t) { t.unprotect(); }
    static void mark(term_balanced_tree<Term> t)      { t.mark(); }
    static ATerm term(term_balanced_tree<Term> t)     { return t.term(); }
    static ATerm* ptr(term_balanced_tree<Term>& t)    { return &t.term(); }
  };
  /// \endcond

  /// \brief Equality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are equal.
  template <typename Term>
  bool operator==(const term_balanced_tree<Term>& x, const term_balanced_tree<Term>& y)
  {
    return ATisEqual(aterm_traits<term_balanced_tree<Term> >::term(x), aterm_traits<term_balanced_tree<Term> >::term(y)) == ATtrue;
  }

  /// \brief Equality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are equal.
  template <typename Term>
  bool operator==(const term_balanced_tree<Term>& x, ATermList y)
  {
    return ATisEqual(aterm_traits<term_balanced_tree<Term> >::term(x), y) == ATtrue;
  }

  /// \brief Equality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are equal.
  template <typename Term>
  bool operator==(ATermList x, const term_balanced_tree<Term>& y)
  {
    return ATisEqual(x, aterm_traits<term_balanced_tree<Term> >::term(y)) == ATtrue;
  }

  /// \brief Inequality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are not equal.
  template <typename Term>
  bool operator!=(const term_balanced_tree<Term>& x, const term_balanced_tree<Term>& y)
  {
    return ATisEqual(aterm_traits<term_balanced_tree<Term> >::term(x), aterm_traits<term_balanced_tree<Term> >::term(y)) == ATfalse;
  }

  /// \brief Inequality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are not equal.
  template <typename Term>
  bool operator!=(const term_balanced_tree<Term>& x, ATermList y)
  {
    return ATisEqual(aterm_traits<term_balanced_tree<Term> >::term(x), y) == ATfalse;
  }

  /// \brief Inequality operator.
  /// \param x A list.
  /// \param y A list.
  /// \return True if the arguments are not equal.
  template <typename Term>
  bool operator!=(ATermList x, const term_balanced_tree<Term>& y)
  {
    return ATisEqual(x, aterm_traits<term_balanced_tree<Term> >::term(y)) == ATfalse;
  }

} // namespace atermpp

#include "mcrl2/atermpp/make_list.h"

#endif // MCRL2_ATERMPP_ATERM_LIST_H
