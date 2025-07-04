// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/small_progress_measures.h
/// \brief add your file description here.

#ifndef MCRL2_BES_SMALL_PROGRESS_MEASURES_H
#define MCRL2_BES_SMALL_PROGRESS_MEASURES_H

// TODO: Make it possible to undefine this flag
#define MCRL2_SMALL_PROGRESS_MEASURES_DEBUG

#include <iomanip>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/normal_forms.h"
#include "mcrl2/pbes/print.h"

namespace mcrl2::pbes_system
{

template <typename T>
bool is_odd(T t)
{
  return t % 2 != 0;
}

template <typename T>
bool is_even(T t)
{
  return t % 2 == 0;
}

template <typename InputIterator1, typename InputIterator2>
int lexicographical_compare_3way(InputIterator1 first1, InputIterator1 last1,
                                 InputIterator2 first2, InputIterator2 last2)
{
  while (first1 != last1 && first2 != last2)
  {
    if (*first1 < *first2)
    {
      return -1;
    }
    if (*first2 < *first1)
    {
      return 1;
    }
    ++first1;
    ++first2;
  }
  if (first2 == last2)
  {
    return !(first1 == last1);
  }
  else
  {
    return -1;
  }
}

inline
bool is_disjunctive(const pbes_expression& x)
{
  return is_or(x);
}

inline
int maximum_rank(const pbes& b)
{
  int result = 0;
  fixpoint_symbol last_symbol;
  for (auto i = b.equations().begin(); i != b.equations().end(); ++i)
  {
    if (i == b.equations().begin())
    {
      result = i->symbol().is_nu() ? 0 : 1;
    }
    else if (i->symbol() != last_symbol)
    {
      result++;
    }
    last_symbol = i->symbol();
  }
  return result;
}

struct progress_measure
{
  explicit progress_measure(std::size_t d)
    : v(d, 0)
  {}

  // N.B. The special value top is represented by alpha[0] == -1
  std::vector<int> v;

  bool is_top() const
  {
    return v[0] == -1;
  }
};

inline
std::ostream& operator<<(std::ostream& out, const progress_measure& pm)
{
  if (pm.is_top())
  {
    out << "top";
  }
  else
  {
    out << core::detail::print_list(pm.v);
  }
  return out;
}

// increment position m of vector alpha
inline
void inc(std::vector<int>& alpha, int m, const std::vector<int>& beta)
{
  if (alpha[0] == -1)
  {
    return;
  }
  else if (m == -1)
  {
    alpha[0] = -1;
    return;
  }
  else if (alpha[m] == beta[m])
  {
    alpha[m] = 0;
    inc(alpha, m - 1, beta);
  }
  else
  {
    alpha[m]++;
  }
}

/// \brief Vertex of the progress measures graph
struct progress_measures_vertex
{
  explicit progress_measures_vertex(bool even_ = false, int rank_ = 0, unsigned int d = 1)
    : even(even_),
      rank(rank_),
      alpha(d)
  {}

  std::vector<progress_measures_vertex*> successors;
  bool even;
  int rank;
  progress_measure alpha;

#ifdef MCRL2_SMALL_PROGRESS_MEASURES_DEBUG
  std::string name;
#endif
};

// compare the positions with index in [0, ... ,m]
struct compare_progress_measures_vertex
{
  unsigned int m;

  explicit compare_progress_measures_vertex(unsigned int m_)
    : m(m_)
  {}

  bool operator()(const progress_measures_vertex* x, const progress_measures_vertex* y) const
  {
    if (x->alpha.is_top())
    {
      return false;
    }
    else if (y->alpha.is_top())
    {
      return true;
    }
    int n = lexicographical_compare_3way(x->alpha.v.begin(), x->alpha.v.begin() + m + 1, y->alpha.v.begin(), y->alpha.v.begin() + m + 1);
    //if (n == 0)
    //{
    //  return is_even(x->rank) && is_odd(y->rank);
    //}
    return n < 0;
  }
};

inline
std::ostream& operator<<(std::ostream& out, const progress_measures_vertex& v)
{
  out << " alpha = " << v.alpha;
  out << " successors = {";
  for (auto i = v.successors.begin(); i != v.successors.end(); ++i)
  {
    if (i != v.successors.begin())
    {
      out << ", ";
    }
    out << std::string((*i)->name);
  }
  out << "}";
  out << " rank = " << v.rank;
  out << " disjunctive = " << std::boolalpha << v.even;
  return out;
}

/// \brief Algorithm class for the small progress measures algorithm
class small_progress_measures_algorithm
{
  protected:
    using vertex = progress_measures_vertex;
    using vertex_map = std::map<propositional_variable_instantiation, vertex>;

    void initialize_vertices()
    {
      // first build the vertex map without successor information
      m_d = maximum_rank(m_bes) + 1;
      unsigned int block_size = 0;
      int last_rank = 0;
      fixpoint_symbol last_symbol = fixpoint_symbol::nu();
      for (const pbes_equation& eqn: m_bes.equations())
      {
        if (eqn.symbol() != last_symbol)
        {
          if (is_even(m_beta.size()))
          {
            m_beta.push_back(0);
          }
          else
          {
            m_beta.push_back(block_size);
          }
          block_size = 0;
          last_rank++;
          last_symbol = eqn.symbol();
        }
        block_size++;
        m_vertices[propositional_variable_instantiation(eqn.variable().name())] = vertex(is_disjunctive(eqn.formula()), last_rank, m_d);
      }
      if (is_even(m_beta.size()))
      {
        m_beta.push_back(0);
      }
      else
      {
        m_beta.push_back(block_size);
      }

      // add successor information
      for (const pbes_equation& eqn: m_bes.equations())
      {
        std::set<propositional_variable_instantiation> succ = pbes_system::find_propositional_variable_instantiations(eqn.formula());
        auto k = m_vertices.find(propositional_variable_instantiation(eqn.variable().name()));
        std::vector<vertex*>& k_successors = k->second.successors;
        for (const propositional_variable_instantiation& v: succ)
        {
          k_successors.push_back(&m_vertices[v]);
        }

#ifdef MCRL2_SMALL_PROGRESS_MEASURES_DEBUG
        k->second.name = std::string(eqn.variable().name());
#endif
      }
    }

    std::string print_vertices() const
    {
      std::ostringstream out;
      for (const pbes_equation& eqn: m_bes.equations())
      {
        const vertex& v = m_vertices.find(propositional_variable_instantiation(eqn.variable().name()))->second;
        out << v.name << " " << v << std::endl;
      }
      return out.str();
    }

    std::string print_vertex(const vertex& v) const
    {
      std::ostringstream out;
      out << v.name << " (alpha = " << v.alpha << ", rank = " << v.rank << ")";
      return out.str();
    }

    /// \brief Logs the neighbors of a vertex
    std::string print_neighbors(const progress_measures_vertex& v) const
    {
      std::ostringstream out;
      for (progress_measures_vertex* successor: v.successors)
      {
        out << "\n      " << print_vertex(*successor);
      }
      return out.str();
    }

    vertex_map m_vertices;
    int m_d = 0;
    const pbes& m_bes;
    std::vector<int> m_beta;

  public:
    explicit small_progress_measures_algorithm(const pbes& b)
      : m_bes(b)
    {}

    bool run(const propositional_variable_instantiation& first_variable)
    {
      mCRL2log(log::verbose) << "Applying small progress measures.\n";
      mCRL2log(log::debug)  << "BES " << pbes_system::pp(m_bes) << "\n\n";
      initialize_vertices();
      mCRL2log(log::debug) << "--- vertices ---\n" << print_vertices();
      mCRL2log(log::debug) << "\nbeta = " << core::detail::print_list(m_beta) << "\n";
      for (;;) // forever
      {
        bool changed = false;
        for (auto &i: m_vertices)
        {
          vertex& v = i.second;
          mCRL2log(log::debug) << "\nchoose vertex " << print_vertex(v);
          int m = v.rank;
          std::vector<progress_measures_vertex*>::const_iterator j;
          mCRL2log(log::debug) << "\n    neighbors:" << print_neighbors(v);
          if (v.even)
          {
            j = std::min_element(v.successors.begin(), v.successors.end(), compare_progress_measures_vertex(m));
            mCRL2log(log::debug) << "\n    minimum neighbor " << print_vertex(**j);
          }
          else
          {
            j = std::max_element(v.successors.begin(), v.successors.end(), compare_progress_measures_vertex(m));
            mCRL2log(log::debug) << "\n    maximum neighbor " << print_vertex(**j);
          }
          std::vector<int> alpha(m_d, 0);
          const progress_measures_vertex& w = **j;
          std::copy(w.alpha.v.begin(),  w.alpha.v.begin() + m + 1, alpha.begin());
          if (is_odd(m))
          {
            mCRL2log(log::debug) << "\n    inc(" << core::detail::print_list(alpha) << ", " << std::to_string(m) << ") = ";
            inc(alpha, m, m_beta);
            mCRL2log(log::debug) << (alpha[0] < 0 ? "top" : core::detail::print_list(alpha));
          }

          if (!std::equal(alpha.begin(), alpha.end(), v.alpha.v.begin()))
          {
            changed = true;
            v.alpha.v = alpha;
            mCRL2log(log::debug) << "\nupdate vertex " << print_vertex(v);
          }
        }
        if (!changed)
        {
          break;
        }
      }
      mCRL2log(log::debug) << "\n--- vertices ---\n" << print_vertices();
      return !m_vertices[first_variable].alpha.is_top();
    }
};

inline
bool small_progress_measures(pbes& b)
{
  propositional_variable_instantiation first(b.equations().front().variable().name());
  assert(b.equations().front().variable().parameters().empty());
  make_standard_form(b, true);
  small_progress_measures_algorithm algorithm(b);
  return algorithm.run(first);
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_BES_SMALL_PROGRESS_MEASURES_H
