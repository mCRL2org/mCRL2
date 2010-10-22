// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/small_progress_measures.h
/// \brief add your file description here.

#ifndef MCRL2_BES_SMALL_PROGRESS_MEASURES_H
#define MCRL2_BES_SMALL_PROGRESS_MEASURES_H

#include <iomanip>
#include <sstream>
#include <vector>
#include "mcrl2/core/algorithm.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/find.h"

namespace mcrl2 {

namespace bes {

  inline
  bool is_disjunctive(const boolean_expression& x)
  {
    return is_or(x);
  }

  inline
  unsigned int mu_block_count(const boolean_equation_system<>& b)
  {
    unsigned int result = 0;
    fixpoint_symbol last_symbol = fixpoint_symbol::nu();
    for (atermpp::vector<boolean_equation>::const_iterator i = b.equations().begin(); i != b.equations().end(); ++i)
    {
      if (i->symbol().is_mu() && last_symbol.is_nu())
      {
        result++;
      }
      last_symbol = i->symbol();
    }
    return result;
  }

  inline
  std::string print_variable_set(const std::set<boolean_variable>& v)
  {
    std::ostringstream out;
    out << "{";
    for (std::set<boolean_variable>::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      if (i != v.begin())
      {
        out << ", ";
      }
      out << std::string(i->name());
    }
    out << "}";
    return out.str();
  }

  struct progress_measure
  {
    progress_measure(std::size_t d)
      : v(d, 0)
    {}

    // N.B. The special value top is represented by alpha[0] == -1
    std::vector<int> v;
      
    bool is_top() const
    {
      return v[0] < 0;
    }
  };

  // increment progress measure
  inline
  void inc(progress_measure& pm, unsigned int m, const std::vector<int>& beta)
  {
    if (pm.v[0] == -1)
    {
      return;
    }
    else if (m == 0)
    {
      pm.v[0] = -1;
      return;
    }
    else if (pm.v[m] == beta[m])
    {
      pm.v[m] = 0;
      inc(pm, m - 1, beta);
    }
    else
    {
      pm.v[m]++;
    }
  }

  /// \brief Algorithm class for the small progress measures algorithm
  class small_progress_measures_algorithm: public core::algorithm
  {
    protected:
      struct vertex
      {
        vertex(const std::set<boolean_variable>& successors_, bool disjunctive_, int rank_, unsigned int d)
          : successors(successors_),
            disjunctive(disjunctive_),
            rank(rank_),
            alpha(d)
        {}
        
        std::set<boolean_variable> successors;
        bool disjunctive;
        unsigned int rank;
        progress_measure alpha;
      };

      struct compare_progress_measures
      {
        const std::vector<int>& beta;
          
        compare_progress_measures(const std::vector<int>& beta_)
          : beta(beta_)
         {}
         
         bool operator()(const progress_measure& x, const progress_measure& y)
         {
           return true;
         }
      };

/*
      std::vector<int> f(const vertex& v, const vertex& w, int m, const std::vector<int>& beta)
      {
        if (v.rank % 2 == 0)
        {
          
        }
      }

      // returns the 
      std::vector<vertex>::const_iterator minimize_f(const vertex& v)
      {
      }
*/
      void initialize_vertices()
      {
        m_d = mu_block_count(m_bes);
        
        unsigned int last_rank = 0;
        fixpoint_symbol last_symbol = fixpoint_symbol::nu();
        for (atermpp::vector<boolean_equation>::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
        {
          if (i->symbol() != last_symbol)
          {
            last_rank++;
            last_symbol = i->symbol();
          }
          m_vertices.push_back(vertex(bes::find_variables(i->formula()), is_disjunctive(i->formula()), last_rank, m_d));
        }
      }

      /// \brief Prints the vertices of the dependency graph.
      void LOG_VERTICES(unsigned int level, const std::string& msg = "") const
      {
        if (check_log_level(level))
        {
          std::clog << msg;
          for (std::vector<vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
          {
            std::string name(m_bes.equations()[i - m_vertices.begin()].variable().name());
            std::clog << std::setw(5) << name << " successors = " << print_variable_set(i->successors) << " rank = " << std::setw(3) << i->rank << " disjunctive = " << std::boolalpha << i->disjunctive << " alpha = " << core::detail::print_list(i->alpha.v) << std::endl;
          }
        }
      }

      std::vector<vertex> m_vertices;
      unsigned int m_d;
      const boolean_equation_system<>& m_bes;   

    public:
      small_progress_measures_algorithm(const boolean_equation_system<>& b, unsigned int log_level = 0)
        : core::algorithm(log_level),
          m_bes(b)
      {}
      
      void run()
      {
        initialize_vertices();
        LOG_VERTICES(0, "--- vertices ---\n");
        //for (;;) // forever
        //{
        //}
      }
           
  };

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_SMALL_PROGRESS_MEASURES_H
