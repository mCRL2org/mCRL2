// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/enumerate_quantifiers_builder.h
/// \brief Simplifying rewriter for pbes expressions that eliminates quantifiers using enumeration.

#ifndef MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H
#define MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H

#include <numeric>
#include <set>
#include <utility>
#include <deque>
#include <sstream>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/optimized_boolean_operators.h"
#include "mcrl2/core/sequence.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/pbes/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Computes the subset with variables of finite sort and infinite.
  // TODO: this should be done more efficiently, by avoiding aterm lists
  /// \param variables A sequence of data variables
  /// \param data A data specification
  /// \param finite_variables A sequence of data variables
  /// \param infinite_variables A sequence of data variables
  inline
  void split_finite_variables(data::variable_list variables, const data::data_specification& data, data::variable_list& finite_variables, data::variable_list& infinite_variables)
  {
    std::vector<data::variable> finite;
    std::vector<data::variable> infinite;
    for (data::variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (data.is_certainly_finite(i->sort()))
      {
        finite.push_back(*i);
      }
      else
      {
        infinite.push_back(*i);
      }
    }
    finite_variables = data::variable_list(finite.begin(), finite.end());
    infinite_variables = data::variable_list(infinite.begin(), infinite.end());
  }

  /// \brief Returns a string representation of a container
  /// \param c A container
  /// \return A string representation of a container
  template <typename Container>
  std::string print_term_container(const Container& c)
  {
    std::stringstream result;
    result << "[";
    for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
    {
      result << (i == c.begin() ? "" : ", ") << core::pp(*i);
    }
    result << "]";
    return result.str();
  }

  /// \brief Determines if the unordered sequences s1 and s2 have an empty intersection
  /// \param s1 A sequence
  /// \param s2 A sequence
  /// \return True if the intersection of s1 and s2 is empty
  template <typename Sequence>
  bool empty_intersection(Sequence s1, Sequence s2)
  {
    for (typename Sequence::const_iterator i = s1.begin(); i != s1.end(); ++i)
    {
      if (std::find(s2.begin(), s2.end(), *i) != s2.end())
      {
        return false;
      }
    }
    return true;
  }

  template <typename PbesRewriter, typename DataEnumerator>
  class quantifier_enumerator
  {
    protected:
      PbesRewriter pbesr;
      DataEnumerator datae;

      typedef typename PbesRewriter::term_type term_type;
      typedef typename core::term_traits<term_type> tr;
      typedef typename tr::variable_sequence_type variable_sequence_type;
      typedef typename tr::variable_type variable_type;
      typedef typename tr::data_term_type data_term_type;

      /// Exception that is used as an early escape of the foreach_sequence algorithm.
      struct stop_early
      {};

      /// Joins a sequence of pbes expressions with operator and
      template <typename Term>
      struct join_and
      {
        /// \brief Returns the conjunction of a sequence of pbes expressions
        /// \param first Start of a sequence of pbes expressions
        /// \param last End of a sequence of pbes expressions
        /// \return The conjunction of the expressions
        template <typename FwdIt>
        Term operator()(FwdIt first, FwdIt last) const
        {
          return std::accumulate(first, last, core::term_traits<Term>::true_(), &core::optimized_and<Term>);
        }
      };

      /// Joins a sequence of pbes expressions with operator or
      template <typename Term>
      struct join_or
      {
        /// \brief Returns the disjunction of a sequence of pbes expressions
        /// \param first Start of a sequence of pbes expressions
        /// \param last End of a sequence of pbes expressions
        /// \return The disjunction of the expressions
        template <typename FwdIt>
        Term operator()(FwdIt first, FwdIt last) const
        {
          return std::accumulate(first, last, core::term_traits<Term>::false_(), &core::optimized_or<Term>);
        }
      };

      /// The assign operation used to create sequences in the foreach_sequence algorithm
      template <typename SubstitutionFunction>
      struct sequence_assign
      {
        typedef typename SubstitutionFunction::variable_type   variable_type;
        typedef typename SubstitutionFunction::expression_type term_type;

        SubstitutionFunction& sigma_;

        sequence_assign(SubstitutionFunction& sigma)
          : sigma_(sigma)
        {}

        /// \brief Function call operator
        /// \param v A variable
        /// \param t A term
        void operator()(variable_type v, term_type t)
        {
          sigma_[v] = t;
        }
      };

      /// The action that is triggered for each sequence generated by the
      /// foreach_sequence algorithm. It is invoked for every sequence of
      /// substitutions of the set Z in the algorithm.
      template <typename PbesTermSet,
                typename PbesTerm,
                typename SubstitutionFunction,
                typename VariableSet,
                typename StopCriterion
               >
      struct sequence_action
      {
        PbesTermSet&          A_;
        PbesRewriter&         r_;
        const term_type&      phi_;
        SubstitutionFunction& sigma_;
        const VariableSet&    v_;
        bool&                 is_constant_;
        StopCriterion         stop_;

        /// \brief Determines if the unordered sequences s1 and s2 have an empty intersection
        /// \param s1 A sequence
        /// \param s2 A sequence
        /// \return True if the intersection of s1 and s2 is empty
        template <typename Sequence, typename Set>
        bool empty_intersection(const Sequence& s1, const Set& s2)
        {
          for (typename Sequence::const_iterator i = s1.begin(); i != s1.end(); ++i)
          {
            if (s2.find(*i) != s2.end())
            {
              return false;
            }
          }
          return true;
        }

        sequence_action(PbesTermSet& A,
                        PbesRewriter &r,
                        const PbesTerm& phi,
                        SubstitutionFunction& sigma,
                        const VariableSet& v,
                        bool& is_constant,
                        StopCriterion stop
                       )
          : A_(A), r_(r), phi_(phi), sigma_(sigma), v_(v), is_constant_(is_constant), stop_(stop)
        {}

        /// \brief Function call operator
        void operator()()
        {
          PbesTerm c = r_(phi_, sigma_);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "        Z = Z + " << core::pp(c) << (empty_intersection(c.variables(), v_) ? " (constant)" : "") << " sigma = " << to_string(sigma_) << " dependencies = " << print_term_container(v_) << std::endl;
#endif
          if (stop_(c))
          {
            throw stop_early();
          }
          else if (empty_intersection(c.variables(), v_))
          {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "        A = A + " << core::pp(c) << std::endl;
#endif
            A_.insert(c);
          }
          else
          {
            is_constant_ = false;
          }
        }
      };

      /// Convenience function for generating a sequence action
      template <typename PbesTermSet,
                typename PbesTerm,
                typename SubstitutionFunction,
                typename VariableSet,
                typename StopCriterion
               >
      sequence_action<PbesTermSet, PbesTerm, SubstitutionFunction, VariableSet, StopCriterion>
      make_sequence_action(PbesTermSet& A,
                           PbesRewriter &r,
                           const PbesTerm& phi,
                           SubstitutionFunction& sigma,
                           const VariableSet& v,
                           bool& is_constant,
                           StopCriterion stop
                          )
      {
        return sequence_action<PbesTermSet, PbesTerm, SubstitutionFunction, VariableSet, StopCriterion>(A, r, phi, sigma, v, is_constant, stop);
      }

      /// \brief Prints debug information to standard error
      /// \param x A sequence of variables
      /// \param phi A term
      /// \param sigma A substitution function
      /// \param stop_value A term
      template <typename SubstitutionFunction>
      void print_arguments(variable_sequence_type x, const term_type& phi, SubstitutionFunction& sigma, term_type stop_value) const
      {
        std::cerr << "<enumerate>"
                  << (tr::is_false(stop_value) ? "forall " : "exists ")
                  << core::pp(x) << ". "
                  << core::pp(phi)
                  << to_string(sigma) << std::endl;
      }

      /// \brief Returns a string representation of D[i]
      /// \param Di A sequence of data terms
      /// \param i A positive integer
      /// \return A string representation of D[i]
      std::string print_D_element(const atermpp::vector<data_term_type>& Di, unsigned int i) const
      {
        std::ostringstream out;
        out << "D[" << i << "] = " << print_term_container(Di) << std::endl;
        return out.str();
      }

      /// \brief Prints debug information to standard error
      /// \param D The sequence D of the algorithm
      void print_D(const std::vector<atermpp::vector<data_term_type> >& D) const
      {
        for (unsigned int i = 0; i < D.size(); i++)
        {
          std::cerr << "  " << print_D_element(D[i], i);
        }
      }

      /// \brief Returns a string representation of a todo list element
      /// \param e A todo list element
      /// \return A string representation of a todo list element
      std::string print_todo_list_element(const boost::tuple<variable_type, data_term_type, unsigned int>& e) const
      {
        // const variable_type& xk = boost::get<0>(e);
        const data_term_type& y = boost::get<1>(e);
        unsigned int k          = boost::get<2>(e);
        return "(" + core::pp(y) + ", " + boost::lexical_cast<std::string>(k) + ")";
      }

      /// \brief Prints a todo list to standard error
      /// \param todo A todo list
      void print_todo_list(const std::deque<boost::tuple<variable_type, data_term_type, unsigned int> >& todo) const
      {
        std::cerr << "  todo = [";
        for (typename std::deque<boost::tuple<variable_type, data_term_type, unsigned int> >::const_iterator i = todo.begin(); i != todo.end(); ++i)
        {
          std::cerr << (i == todo.begin() ? "" : ", ") << print_todo_list_element(*i);
        }
        std::cerr << "]" << std::endl;
      }

      template <typename SubstitutionFunction,
                typename StopCriterion,
                typename PbesTermJoinFunction
               >
      term_type enumerate(variable_sequence_type x,
                          const term_type& phi,
                          SubstitutionFunction& sigma,
                          StopCriterion stop,
                          term_type stop_value,
                          PbesTermJoinFunction join
                         )
      {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  print_arguments(x, phi, sigma, stop_value);
#endif
        term_type Rphi = pbesr(phi, sigma);
        if (tr::is_constant(Rphi))
        {
          return Rphi;
        }

        typedef std::pair<variable_type, data_term_type> data_assignment;

        atermpp::set<term_type> A;
        std::vector<atermpp::vector<data_term_type> > D;
        atermpp::set<variable_type> dependencies;

        // For an element (v, t, k) of todo, we have the invariant v == x[k].
        // The variable v is stored for efficiency reasons, it avoids the lookup x[k].
        std::deque<boost::tuple<variable_type, data_term_type, unsigned int> > todo;

        // initialize D and todo
        unsigned int j = 0;
        for (typename variable_sequence_type::const_iterator i = x.begin(); i != x.end(); ++i)
        {
          data_term_type t = core::term_traits<data_term_type>::variable2term(*i);
          D.push_back(atermpp::vector<data_term_type>(1, t));
          todo.push_back(boost::make_tuple(*i, t, j++));
          dependencies.insert(t.variables().begin(), t.variables().end());
        }

        try
        {
          while (!todo.empty())
          {
            boost::tuple<variable_type, data_term_type, unsigned int> front = todo.front();
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  print_D(D);
  print_todo_list(todo);
  std::cerr << "    (y, k) = " << print_todo_list_element(front) << std::endl;
#endif
            todo.pop_front();
            const variable_type& xk = boost::get<0>(front);
            const data_term_type& y = boost::get<1>(front);
            unsigned int k          = boost::get<2>(front);
            bool is_constant = true;

            D[k].erase(std::find(D[k].begin(), D[k].end(), y));
            variable_sequence_type variables(y.variables());
            for (typename variable_sequence_type::iterator i = variables.begin(); i != variables.end(); ++i)
            {
              dependencies.erase(*i);
            }

            // save D[k] in variable Dk, as a preparation for the foreach_sequence algorithm
            atermpp::vector<data_term_type> Dk = D[k];
            atermpp::vector<data_term_type> z = datae.enumerate(y);
            for (typename atermpp::vector<data_term_type>::iterator i = z.begin(); i != z.end(); ++i)
            {
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "      e = " << core::pp(*i) << std::endl;
#endif
              dependencies.insert(i->variables().begin(), i->variables().end());
              sigma[xk] = *i;
              D[k].clear();
              D[k].push_back(*i);
              core::foreach_sequence(D,
                                     x.begin(),
                                     make_sequence_action(A, pbesr, phi, sigma, dependencies, is_constant, stop),
                                     sequence_assign<SubstitutionFunction>(sigma)
                                    );
              if (!is_constant)
              {
                Dk.push_back(*i);
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "        " << print_D_element(Dk, k) << std::endl;
#endif
                if (!core::term_traits<data_term_type>::is_constant(*i))
                {
                  todo.push_back(boost::make_tuple(xk, *i, k));
                }
                else
                {
                  variable_sequence_type variables(i->variables());

                  for (typename variable_sequence_type::iterator j = variables.begin(); j != variables.end(); ++j)
                  {
                    dependencies.erase(*j);
                  }
                }
              }
            }

            // restore D[k]
            D[k] = Dk;
          }
        }
        catch (stop_early /* a */)
        {
          // remove the added substitutions from sigma
          for (typename variable_sequence_type::const_iterator j = x.begin(); j != x.end(); ++j)
          {
            sigma[*j] = *j; // erase *j
          }
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "<return>stop early: " << core::pp(stop_value) << std::endl;
#endif
          return stop_value;
        }

        // remove the added substitutions from sigma
        for (typename variable_sequence_type::const_iterator i = x.begin(); i != x.end(); ++i)
        {
          sigma[*i] = *i; // erase *i
        }
        term_type result = join(A.begin(), A.end());
#ifdef MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
  std::cerr << "<return> " << core::pp(result) << std::endl;
#endif
        return result;
      }

    public:
      quantifier_enumerator(PbesRewriter r, DataEnumerator e)
        : pbesr(r), datae(e)
      {}

      /// \brief Enumerates a universal quantification
      /// \param x A sequence of variables
      /// \param phi A term
      /// \param sigma A substitution function
      /// \return The enumeration result
      template <typename SubstitutionFunction>
      term_type enumerate_universal_quantification(variable_sequence_type x, term_type phi, SubstitutionFunction& sigma)
      {
        return enumerate(x, phi, sigma, tr::is_false, tr::false_(), join_and<term_type>());
      }

      /// \brief Enumerates an existential quantification
      /// \param x A sequence of variables
      /// \param phi A term
      /// \param sigma A substitution function
      /// \return The enumeration result
      template <typename SubstitutionFunction>
      term_type enumerate_existential_quantification(variable_sequence_type x, term_type phi, SubstitutionFunction& sigma)
      {
        return enumerate(x, phi, sigma, tr::is_true, tr::true_(), join_or<term_type>());
      }
  };

  // Simplifying PBES rewriter that eliminates quantifiers using enumeration.
  /// \param SubstitutionFunction This must be a MapSubstitution.
  template <typename Term, typename DataRewriter, typename DataEnumerator, typename SubstitutionFunction>
  struct enumerate_quantifiers_builder: public simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction>
  {
    typedef enumerate_quantifiers_builder<Term, DataRewriter, DataEnumerator, SubstitutionFunction> self;
    typedef simplify_rewrite_builder<Term, DataRewriter, SubstitutionFunction> super;
    typedef SubstitutionFunction                                               argument_type;
    typedef typename super::term_type                                          term_type;
    typedef typename core::term_traits<term_type>::data_term_type              data_term_type;
    typedef typename core::term_traits<term_type>::data_term_sequence_type     data_term_sequence_type;
    typedef typename core::term_traits<term_type>::variable_sequence_type      variable_sequence_type;
    typedef typename core::term_traits<term_type>::propositional_variable_type propositional_variable_type;
    typedef core::term_traits<Term> tr;

    DataEnumerator& m_data_enumerator;

    /// If true, quantifier variables of infinite sort are enumerated.
    bool m_enumerate_infinite_sorts;

    /// \brief Constructor.
    /// \param r A data rewriter
    /// \param enumerator A data enumerator
    /// \param enumerate_infinite_sorts If true, quantifier variables of infinite sort are enumerated as well
    enumerate_quantifiers_builder(DataRewriter& r, DataEnumerator& enumerator, bool enumerate_infinite_sorts = true)
      : super(r), m_data_enumerator(enumerator), m_enumerate_infinite_sorts(enumerate_infinite_sorts)
    { }


    /// \brief Visit forall node
    /// Visit forall node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param phi A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_forall(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      if (m_enumerate_infinite_sorts)
      {
        return quantifier_enumerator<self, DataEnumerator>(*this, m_data_enumerator).enumerate_universal_quantification(variables, phi, sigma);
      }
      else
      {
        data::variable_list finite;
        data::variable_list infinite;
        split_finite_variables(variables, m_data_enumerator.data(), finite, infinite);
        if (finite.empty())
        {
          return x;
        }
        else
        {
          return core::optimized_forall(infinite, quantifier_enumerator<self, DataEnumerator>(*this, m_data_enumerator).enumerate_universal_quantification(finite, phi, sigma));
        }
      }
    }

    /// \brief Visit exists node
    /// Visit exists node.
    /// \param x A term
    /// \param variables A sequence of variables
    /// \param phi A term
    /// \param sigma A substitution function
    /// \return The result of visiting the node
    term_type visit_exists(const term_type& x, const variable_sequence_type& variables, const term_type& phi, SubstitutionFunction& sigma)
    {
      if (m_enumerate_infinite_sorts)
      {
        return quantifier_enumerator<self, DataEnumerator>(*this, m_data_enumerator).enumerate_existential_quantification(variables, phi, sigma);
      }
      else
      {
        data::variable_list finite;
        data::variable_list infinite;
        split_finite_variables(variables, m_data_enumerator.data(), finite, infinite);
        if (finite.empty())
        {
          return x;
        }
        else
        {
          return core::optimized_exists(infinite, quantifier_enumerator<self, DataEnumerator>(*this, m_data_enumerator).enumerate_existential_quantification(finite, phi, sigma));
        }
      }
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_ENUMERATE_QUANTIFIERS_BUILDER_H
