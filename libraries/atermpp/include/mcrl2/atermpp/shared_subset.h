// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/shared_subset.h
/// \brief Shared subset of a give set.

#ifndef MCRL2_ATERMPP_SHARED_SUBSET_H
#define MCRL2_ATERMPP_SHARED_SUBSET_H

#include <vector>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/construction_utility.h"

static inline int highest_bit(size_t x)
{
  int i = -1;
  while (x)
  {
    x = x >> 1;
    i++;
  }
  return i;
}

namespace atermpp
{

/// \brief Stores a subset of a given base set using maximum sharing.
///
/// Subsets are stored as a relation on the element index, in a BDD whose variables each represent one bit of the index.
template <typename T>
class shared_subset
{
  friend struct atermpp::aterm_traits<shared_subset>;
  protected:
    static atermpp::aterm_string &get_true()
    {
      static atermpp::aterm_string true_ = mcrl2::core::detail::initialise_static_expression(true_, atermpp::aterm_string("true"));
      return true_;
    }

    static atermpp::aterm_string &get_false()
    {
      static atermpp::aterm_string false_ = mcrl2::core::detail::initialise_static_expression(false_, atermpp::aterm_string("false"));
      return false_;
    }

    static atermpp::function_symbol &get_node()
    {
      static atermpp::function_symbol node_ = mcrl2::core::detail::initialise_static_expression(node_, atermpp::function_symbol("node", 3));
      return node_;
    }

    class bdd_node : public atermpp::aterm_appl
    {
      public:
        bdd_node()
        {}

        bdd_node(const atermpp::aterm_appl &t)
          : atermpp::aterm_appl(t)
        {}

        bdd_node(bool value)
          : atermpp::aterm_appl(value ? get_true() : get_false())
        {}

        bdd_node(int bit, const bdd_node &true_node, const bdd_node &false_node)
          : atermpp::aterm_appl(get_node(), atermpp::aterm_int(bit), true_node, false_node)
        {}

        bool is_true()
        {
          return *this == get_true();
        }

        bool is_false()
        {
          return *this == get_false();
        }

        bool is_node()
        {
          return function() == get_node();
        }

        int bit()
        {
          return atermpp::aterm_int((*this)(0)).value();
        }

        bdd_node true_node()
        {
          return bdd_node(atermpp::aterm_appl((*this)(1)));
        }

        bdd_node false_node()
        {
          return bdd_node(atermpp::aterm_appl((*this)(2)));
        }
    };

  protected:
    std::vector<T> *m_set;
    size_t m_bits;
    bdd_node m_bdd_root;

  public:
    // Iterates over elements in the subset *in order*. The predicate-constructor of shared_subset depends on this property.
    class iterator: public boost::iterator_facade<iterator, T, boost::forward_traversal_tag>
    {
      protected:
        const shared_subset<T> *m_subset;
        size_t m_index;

      public:
        iterator()
          : m_index(-1)
        {
        }

        iterator(const shared_subset &subset)
          : m_subset(&subset),
            m_index(0)
        {
          find_next_index();
        }

        size_t index() const
        {
          return m_index;
        }

        operator bool() const
        {
          return m_index != (size_t)(-1);
        }

      private:
        friend class boost::iterator_core_access;

        bool equal(iterator const& other) const
        {
          return m_index == other.m_index;
        }

        T &dereference() const
        {
          return (*m_subset->m_set)[m_index];
        }

        void increment()
        {
          m_index++;
          find_next_index();
        }

        void find_next_index()
        {
          //bdd_node path_stack[m_subset->m_bits];
          MCRL2_SYSTEM_SPECIFIC_ALLOCA(path_stack, bdd_node, m_subset->m_bits);
          size_t path_stack_index = 0;
          bdd_node node = m_subset->m_bdd_root;

          while (true)
          {
            if (m_index >= m_subset->m_set->size())
            {
              m_index = -1;
              return;
            }

            while (node.is_node())
            {
              path_stack[path_stack_index++] = node;
              node = (m_index & (1UL << node.bit())) ? node.true_node() : node.false_node();
            }

            if (node.is_true())
            {
              return;
            }

            while (true)
            {
              if (path_stack_index == 0)
              {
                m_index = -1;
                return;
              }

              size_t bit = path_stack[path_stack_index - 1].bit();
              if (!node.is_false())
              {
                bool found = false;
                for (size_t i = node.bit() + 1; i < bit; i++)
                {
                  if (!(m_index & (1UL << i)))
                  {
                    m_index |= (1UL << i);
                    m_index &= ~((1UL << i) - 1);
                    found = true;
                    break;
                  }
                }
                if (found)
                {
                  break;
                }
              }

              node = path_stack[--path_stack_index];
              if (!(m_index & (1UL << bit)) && !node.true_node().is_false())
              {
                m_index |= (1UL << bit);
                m_index &= ~((1UL << bit) - 1);
                break;
              }
            }
          }
        }
    };

    shared_subset()
      : m_bits(0),
        m_bdd_root(false)
    {
    }

    /// \brief Constructor.
    shared_subset(std::vector<T> &set)
      : m_set(&set),
        m_bdd_root(true)
    {
      m_bits = 0;
      while (m_set->size() > (1UL << m_bits))
      {
        m_bits++;
      }
    }

    template <class Predicate>
    shared_subset(const shared_subset<T> &set, Predicate p)
      : m_set(set.m_set),
        m_bits(set.m_bits)
    {
      //bdd_node trees[m_bits + 1];
      MCRL2_SYSTEM_SPECIFIC_ALLOCA(trees, bdd_node, m_bits + 1);
      size_t completed = 0;
      for (iterator i = set.begin(); i != set.end(); i++)
      {
        if (p(*i))
        {
          size_t target = i.index();

          for (int bit = highest_bit(target); bit >= 0; bit--)
          {
            if ((target & (1UL << bit)) && !(completed & (1UL << bit)))
            {
              bdd_node tree(false);
              for (int j = 0; j < bit; j++)
              {
                bdd_node true_node;
                bdd_node false_node;
                if (completed & (1UL << j))
                {
                  true_node = tree;
                  false_node = trees[j];
                }
                else
                {
                  true_node = false;
                  false_node = tree;
                }
                if (true_node == false_node)
                {
                  tree = true_node;
                }
                else
                {
                  tree = bdd_node(j, true_node, false_node);
                }
              }
              trees[bit] = tree;
              completed |= (1UL << bit);
              completed &= ~((1UL << bit) - 1);
            }
          }

          bdd_node tree(true);
          size_t bit;
          for (bit = 0; target & (1UL << bit); bit++)
          {
            if (tree != trees[bit])
            {
              tree = bdd_node(bit, tree, trees[bit]);
            }
          }
          trees[bit] = tree;
          completed = target + 1;
        }
      }

      if (completed != (1UL << m_bits))
      {
        bdd_node tree(false);
        for (size_t j = 0; j < m_bits; j++)
        {
          bdd_node true_node;
          bdd_node false_node;
          if (completed & (1UL << j))
          {
            true_node = tree;
            false_node = trees[j];
          }
          else
          {
            true_node = false;
            false_node = tree;
          }
          if (true_node == false_node)
          {
            tree = true_node;
          }
          else
          {
            tree = bdd_node(j, true_node, false_node);
          }
        }
        trees[m_bits] = tree;
      }

      m_bdd_root = trees[m_bits];
    }

    iterator begin() const
    {
      return iterator(*this);
    }

    iterator end() const
    {
      return iterator();
    }
};

template <typename T>
struct aterm_traits<shared_subset<T> >
{
  static void protect(const shared_subset<T> &subset) { subset.m_bdd_root.protect(); }
  static void unprotect(const shared_subset<T> &subset) { subset.m_bdd_root.unprotect(); }
  static void mark(const shared_subset<T> &subset) { subset.m_bdd_root.mark(); }
};

} // namespace atermpp

#endif
