// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/ltsmin.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_LTSMIN_H
#define MCRL2_LPS_LTSMIN_H

#define MCRL2_GUARDS 1

#include <algorithm>
#include <cassert>
#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/next_state_generator.h"

// For backwards compatibility
//using namespace mcrl2::log;
typedef mcrl2::log::log_level_t mcrl2_log_level_t;
using mcrl2::log::mcrl2_logger;

namespace mcrl2 {

namespace lps {

/// \brief Generates possible values of the data type (at most max_size).
inline
std::vector<std::string> generate_values(const data::data_specification& dataspec, const data::sort_expression& s, std::size_t max_size = 1000)
{
  std::vector<std::string> result;
  std::size_t max_internal_variables = 10000;

  data::rewriter rewr(dataspec);

  typedef data::enumerator_list_element_with_substitution<> enumerator_element;
  typedef data::enumerator_algorithm_with_iterator<> enumerator_type;

  data::enumerator_identifier_generator id_generator;
  enumerator_type enumerator(rewr, dataspec, rewr, id_generator, max_internal_variables);
  data::variable x("x", s);
  data::variable_vector v;
  v.push_back(x);
  data::mutable_indexed_substitution<> sigma;
  data::variable_list vl(v.begin(),v.end());
  std::deque<enumerator_element> enumerator_deque(1, enumerator_element(vl, data::sort_bool::true_()));
  for (auto i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end() ; ++i)
  {
    i->add_assignments(vl, sigma, rewr);
    result.push_back(to_string(sigma(vl.front())));
    if (result.size() >= max_size)
    {
      break;
    }
  }
  return result;
}

/// \brief Models a pins data type. A pins data type maintains a mapping between known values
/// of a data type and integers.
class pins_data_type
{
  protected:
    atermpp::indexed_set<atermpp::aterm> m_indexed_set;
    // lps::next_state_generator& m_generator;
    const data::data_specification& m_data;
    const process::action_label_list& m_action_labels;
    bool m_is_bounded;

  public:

    /// \brief Forward iterator used for iterating over indices.
    class index_iterator: public boost::iterator_facade<index_iterator, const std::size_t, boost::forward_traversal_tag>
    {
      public:
        index_iterator(std::size_t max_index)
          : m_index(0),
            m_max_index(max_index)
        {}

        index_iterator(std::size_t index, std::size_t max_index)
          : m_index(index),
            m_max_index(max_index)
        {}

     private:
        friend class boost::iterator_core_access;

        void increment()
        {
          m_index++;
        }

        bool equal(const index_iterator& other) const
        {
          return this->m_index == other.m_index;
        }

        const std::size_t& dereference() const
        {
          return m_index;
        }

        std::ptrdiff_t distance_to(const index_iterator& other) const
        {
          return other.m_index - this->m_index;
        }

        std::size_t m_index;
        std::size_t m_max_index;
    };

    /// \brief Constructor
    pins_data_type(const data::data_specification& data,
                   const process::action_label_list& action_labels,
                   bool is_bounded = false)
      : // m_generator(generator),
        m_data(data),
        m_action_labels(action_labels),
        m_is_bounded(is_bounded)
    {}

    /// \brief Destructor.
    virtual ~pins_data_type()
    {}

    /// \brief Serializes the i-th value of the data type to a binary string.
    /// It is guaranteed that serialize(deserialize(i)) == i.
    /// \pre i is a valid index
    virtual std::string serialize(int i) const = 0;

    /// \brief Deserializes a string to a data value, and returns the corresponding index.
    /// If the value is not in the mapping, it will be added.
    /// \return The index of the data value.
    /// \throw <std::runtime_error> { if deserialization failed }
    virtual std::size_t deserialize(const std::string& s) = 0;

    /// \brief Returns a human readable representation of the value with index i.
    /// N.B. It is not guaranteed that parse(print(i)) == i.
    /// \pre i is a valid index
    virtual std::string print(int i) const = 0;

    /// \brief Parses a string to a data value, and returns the corresponding index.
    /// If the value is not in the mapping, it will be added.
    /// \return The index of the data value.
    /// \throw <std::runtime_error> { if parsing failed }
    virtual std::size_t parse(const std::string& s) = 0;

    /// \brief Returns the name of the data type.
    virtual const std::string& name() const = 0;

    /// \brief Generates possible values of the data type (at most max_size).
    virtual std::vector<std::string> generate_values(std::size_t max_size = 1000) const = 0;

    /// \brief Returns true if the number of elements is bounded. If this property can
    /// not be computed for a data type, false is returned.
    bool is_bounded() const
    {
      return m_is_bounded;
    }

    /// \brief Returns the number of values that are stored in the map.
    std::size_t size() const
    {
      return m_indexed_set.size();
    }

    /// \brief Returns an iterator to the beginning of the indices
    index_iterator index_begin() const
    {
      return index_iterator(0, size());
    }

    /// \brief Returns an iterator to the end of the indices
    index_iterator index_end() const
    {
      return index_iterator(size());
    }

    /// \brief Returns the index of the value x
    std::size_t operator[](const atermpp::aterm& x)
    {
      return m_indexed_set[x];
    }

    /// \brief Returns the value at index i
    atermpp::aterm get(size_t i)
    {
      return m_indexed_set.get(i);
    }

    /// \brief Returns the indexed_set holding the values
    atermpp::indexed_set<atermpp::aterm>& indexed_set()
    {
      return m_indexed_set;
    }

    /// \brief Returns the indexed_set holding the values
    const atermpp::indexed_set<atermpp::aterm>& indexed_set() const
    {
      return m_indexed_set;
    }
};

/// \brief Models the mapping of mCRL2 state values to integers.
class state_data_type: public pins_data_type
{
  protected:
    std::string m_name;
    data::sort_expression m_sort;

    std::size_t expression2index(const data::data_expression& x)
    {
      return m_indexed_set[x];
    }

    data::data_expression index2expression(std::size_t i) const
    {
      return static_cast<data::data_expression>(m_indexed_set.get(i));
    }

  public:
    state_data_type(const data::data_specification& data,
                    const process::action_label_list& action_labels,
                    const data::sort_expression& sort, bool sort_is_finite)
      : pins_data_type(data, action_labels, sort_is_finite),
        m_sort(sort)
    {
      m_name = data::pp(m_sort);
    }

    // prints the expression as an ATerm string
    std::string serialize(int i) const
    {
      data::data_expression e = index2expression(i);
      atermpp::aterm t = data::detail::remove_index(static_cast<atermpp::aterm>(e));
      return to_string(t);
    }

    std::size_t deserialize(const std::string& s)
    {
      atermpp::aterm t = data::detail::add_index(atermpp::read_term_from_string(s));
      return expression2index(atermpp::down_cast<data::data_expression>(t));
    }

    std::string print(int i) const
    {
      return data::pp(index2expression(i));
    }

    std::size_t parse(const std::string& s)
    {
      return expression2index(data::parse_data_expression(s, m_data));
    }

    const std::string& name() const
    {
      return m_name;
    }

    std::vector<std::string> generate_values(std::size_t max_size = 1000) const
    {
      return lps::generate_values(m_data, m_sort, max_size);
    }
};

/// \brief Models the mapping of mCRL2 action labels to integers.
class action_label_data_type: public pins_data_type
{
  protected:
    std::string m_name;

  public:
    action_label_data_type(const data::data_specification& data, const process::action_label_list& action_labels)
      : pins_data_type(data, action_labels, false)
    {
      m_name = "action_labels";
    }

    std::string serialize(int i) const
    {
      return to_string(m_indexed_set.get(i));
    }

    std::size_t deserialize(const std::string& s)
    {
      return m_indexed_set[atermpp::read_term_from_string(s)];
    }

    std::string print(int i) const
    {
      return lps::pp(lps::multi_action(atermpp::aterm_appl(m_indexed_set.get(i))));
    }

    std::size_t parse(const std::string& s)
    {
      lps::multi_action m = lps::parse_multi_action(s, m_action_labels, m_data);
      return m_indexed_set[detail::multi_action_to_aterm(m)];
    }

    const std::string& name() const
    {
      return m_name;
    }

    // TODO: get rid of this useless function
    std::vector<std::string> generate_values(std::size_t) const
    {
      return std::vector<std::string>();
    }
};

class pins
{
  public:
    typedef int* ltsmin_state_type; /**< the state type used by LTSMin */

    /// \brief guard evaluations have ternary logic. A guard may not always rewrite
    /// to true or false
    enum guard_evaluation_t { GUARD_FALSE = 0, GUARD_TRUE = 1, GUARD_MAYBE = 2 };

    typedef data::rewriter::substitution_type substitution_t;

  protected:
    size_t m_group_count;
    size_t m_state_length; /**< the number of process parameters */
    std::vector<std::vector<size_t> > m_read_group;
    std::vector<std::vector<size_t> > m_write_group;
    std::vector<std::vector<size_t> > m_update_group;
    lps::specification m_specification;
    lps::next_state_generator m_generator;
    std::vector<data::variable> m_parameters_list;
    std::vector<std::string> m_process_parameter_names;

    atermpp::indexed_set<atermpp::aterm> m_guards;
    std::vector<std::vector<size_t> > guard_parameters_list;
    std::vector<std::vector<size_t> > m_guard_info;
    std::vector<std::string> m_guard_names;

    // For guard-splitting we use a different generator with a different spec.
    // This second spec has guards removed from the conditions.
    lps::specification m_specification_reduced;
    lps::next_state_generator m_generator_reduced;

    // The type mappings
    // m_data_types[0] ... m_data_types[N-1] contain the state parameter mappings
    // m_data_types[N] contains the action label values
    // where N is the number of process parameters
    std::vector<pins_data_type*> m_data_types;

    /// The unique type mappings (is contained in m_data_types).
    std::vector<pins_data_type*> m_unique_data_types;

    // maps process parameter index to the corresponding index in m_unique_data_types
    std::vector<std::size_t> m_unique_data_type_index;

    /// \brief Returns the action data type
    pins_data_type& action_label_type_map()
    {
      return *m_data_types.back();
    }

    /// \brief Returns the action data type
    const pins_data_type& action_label_type_map() const
    {
      return *m_data_types.back();
    }

    /// \brief Returns the data type of the i-th state parameter
    pins_data_type& state_type_map(std::size_t i)
    {
      return *m_data_types[i];
    }

    /// \brief Returns the data type of the i-th state parameter
    const pins_data_type& state_type_map(std::size_t i) const
    {
      return *m_data_types[i];
    }

    /// \brief Returns the process of the LPS specification
    const linear_process& process() const
    {
      return m_specification.process();
    }

    /// \brief Returns the reduced process of the LPS specification,
    /// i.e. with conditions with guards removed.
    const linear_process& process_reduced() const
    {
      return m_specification_reduced.process();
    }

    /// \brief Returns the data specification of the LPS specification
    const data::data_specification& data() const
    {
      return m_specification.data();
    }

    template <typename Iter>
    std::string print_vector(Iter first, Iter last) const
    {
      std::ostringstream out;
      out << "[";
      for (Iter i = first; i != last; ++i)
      {
        if (i != first)
        {
          out << ", ";
        }
        out << *i;
      }
      out << "]";
      return out.str();
    }

    template <typename Container>
    std::string print_vector(const Container& c) const
    {
      return print_vector(c.begin(), c.end());
    }

    void initialize_read_write_groups()
    {
      std::set<data::variable> parameters(process().process_parameters().begin(), process().process_parameters().end());

      const lps::linear_process& proc = process();

      m_group_count  = proc.action_summands().size();
      m_state_length = proc.process_parameters().size();

      m_read_group.resize(m_group_count);
      m_write_group.resize(m_group_count);

      // iterate over the list of summands
      {
        size_t i = 0;
        for (auto p = proc.action_summands().begin(); p != proc.action_summands().end(); ++p)
        {
          const auto& summand = *p;
          std::set<data::variable> used_read_variables;
          std::set<data::variable> used_write_variables;

          data::find_free_variables(summand.condition(), std::inserter(used_read_variables, used_read_variables.end()));
          lps::find_free_variables(summand.multi_action(), std::inserter(used_read_variables, used_read_variables.end()));

          for (auto q = summand.assignments().begin(); q != summand.assignments().end(); ++q)
          {
            const auto& assignment = *q;
            if (assignment.lhs() != assignment.rhs())
            {
              data::find_all_variables(assignment.lhs(), std::inserter(used_write_variables, used_write_variables.end()));
              data::find_all_variables(assignment.rhs(), std::inserter(used_read_variables, used_read_variables.end()));
            }
          }

          // process parameters used in condition, action or assignment of summand
          std::set<data::variable> used_read_parameters;
          std::set<data::variable> used_write_parameters;

          std::set_intersection(used_read_variables.begin(),
                                used_read_variables.end(),
                                parameters.begin(),
                                parameters.end(),
                                std::inserter(used_read_parameters,
                                              used_read_parameters.begin()));
          std::set_intersection(used_write_variables.begin(),
                                used_write_variables.end(),
                                parameters.begin(),
                                parameters.end(),
                                std::inserter(used_write_parameters,
                                              used_write_parameters.begin()));

          size_t j = 0;
          for (auto q = m_parameters_list.begin(); q != m_parameters_list.end(); ++q)
          {
            const auto& param = *q;
            if (used_read_parameters.find(param) != used_read_parameters.end())
            {
              m_read_group[i].push_back(j);
            }
            if (used_write_parameters.find(param) != used_write_parameters.end())
            {
              m_write_group[i].push_back(j);
            }
            j++;
          }
          i++;
        }
      }

      m_update_group.resize(m_group_count);

      // iterate over the list of reduced summands
      {
        size_t i = 0;
        for (auto p = process_reduced().action_summands().begin(); p != process_reduced().action_summands().end(); ++p)
        {
          const auto& reduced_summand = *p;
          std::set<data::variable> used_update_variables;

          data::find_free_variables(reduced_summand.condition(), std::inserter(used_update_variables, used_update_variables.end()));
          lps::find_free_variables(reduced_summand.multi_action(), std::inserter(used_update_variables, used_update_variables.end()));

          for (auto q = reduced_summand.assignments().begin(); q != reduced_summand.assignments().end(); ++q)
          {
            const auto& assignment = *q;
            if (assignment.lhs() != assignment.rhs())
            {
              data::find_all_variables(assignment.rhs(), std::inserter(used_update_variables, used_update_variables.end()));
            }
          }

          // process parameters used in the action and assignment of summand
          std::set<data::variable> used_update_parameters;

          std::set_intersection(used_update_variables.begin(),
                                used_update_variables.end(),
                                parameters.begin(),
                                parameters.end(),
                                std::inserter(used_update_parameters,
                                              used_update_parameters.begin()));

          size_t j = 0;
          for (auto q = m_parameters_list.begin(); q != m_parameters_list.end(); ++q)
          {
            const auto& param = *q;
            if (used_update_parameters.find(param) != used_update_parameters.end())
            {
              m_update_group[i].push_back(j);
            }
            j++;
          }
          i++;
        }
      }
    }

  public:
    typedef std::size_t datatype_index; /**< the index type for datatype maps */

    /// \brief Returns the number of process parameters of the LPS
    std::size_t process_parameter_count() const
    {
      return m_specification.process().process_parameters().size();
    }

    /// \brief Returns the number of available groups. This equals the number of action summands of the LPS.
    std::size_t group_count() const
    {
      return m_specification.process().action_summands().size();
    }

    static lps::specification load_specification(const std::string& filename)
    {
      lps::specification specification;
      load_lps(specification, filename);
      return specification;
    }

    /// \brief extracts all guards from the original specification and
    /// returns a new one with the guards removed.
    lps::specification reduce_specification(const lps::specification& spec) {
      // the list of summands
      std::vector<lps::action_summand> reduced_summands;
      for (auto p = spec.process().action_summands().begin(); p != spec.process().action_summands().end(); ++p)
      {
        const auto& summand = *p;

        // contains info about which guards this transition group has.
        std::vector<size_t> guard_info;

        // the initial new condition of a summand is always true.
        // this maybe joined with conjuncts which have local variables.
        data::data_expression reduced_condition = data::sort_bool::true_();

        // process variables and guards in condition
        std::set<data::data_expression> conjuncts = data::split_and(summand.condition());
        std::set<data::variable> summation_variables(summand.summation_variables().begin(), summand.summation_variables().end());

        for (auto q = conjuncts.begin(); q != conjuncts.end(); ++q)
        {
          const auto& conjunct = *q;

          // check if the conjunct is new
          size_t at = m_guards.index(conjunct);
          bool is_new = (at == atermpp::indexed_set<atermpp::aterm>::npos);
          bool use_conjunct_as_guard = true;

          if (is_new) { // we have not encountered the guard yet
            std::set<data::variable> conjunct_variables;
            data::find_free_variables(conjunct, std::inserter(conjunct_variables, conjunct_variables.end()));

            if (!summand.summation_variables().empty())
            {
              // the conjunct may contain summation variables, in which case it can not be
              // used as guard.
              std::set<data::variable> summation_variables_in_conjunct;
              std::set_intersection(conjunct_variables.begin(),
                  conjunct_variables.end(),
                  summation_variables.begin(),
                  summation_variables.end(),
                  std::inserter(summation_variables_in_conjunct, summation_variables_in_conjunct.begin()));

              if (!summation_variables_in_conjunct.empty()) {
                // this conjunct contains summation variables and will not be used as guard.
                use_conjunct_as_guard = false;

                std::string printed_guard(data::pp(conjunct).substr(0, 80));
                mCRL2log(log::verbose)
                    << "Guard '" << printed_guard + (printed_guard.size() > 80?"...":"") << "' in summand "
                    << reduced_summands.size()
                    << " introduces local variables. To remove the guard from the condition, try instantiating the summand with 'lpssuminst'."
                    << std::endl;

                // add conjunct to new summand condition
                if (reduced_condition == data::sort_bool::true_()) {
                  reduced_condition = conjunct;
                } else {
                  reduced_condition = data::sort_bool::and_(reduced_condition, conjunct);
                }
              }
            }
            if (use_conjunct_as_guard) {
              // add conjunct to guards
              std::vector<size_t> guard_parameters;
              if (!conjunct_variables.empty())
              {
                // compute indexes of parameters used by the guard
                size_t p = 0;
                for (auto r = m_parameters_list.begin(); r != m_parameters_list.end(); ++r)
                {
                  const auto& param = *r;

                  if (conjunct_variables.find(param) != conjunct_variables.end())
                  {
                    guard_parameters.push_back(p);
                  }
                  p++;
                }
              }
              // add conjunct to the set of guards
              at = m_guards[conjunct];
              m_guard_names.push_back(data::pp(conjunct));
              guard_parameters_list.push_back(guard_parameters);
            }
          }
          if (use_conjunct_as_guard) {
            // add the guard index to the list of guards of this summand.
            guard_info.push_back(at);
          }
        }

        // set which guards belong to which transition group
        m_guard_info.push_back(guard_info);

        // add the new summand to the list of summands.
        reduced_summands.push_back(lps::action_summand(summand.summation_variables(), reduced_condition, summand.multi_action(), summand.assignments()));
      }

      // create a new LPS
      lps::linear_process lps_reduced(process().process_parameters(), process().deadlock_summands(), reduced_summands);

      // create a new spec
      lps::specification specification_reduced(m_specification.data(), m_specification.action_labels(), m_specification.global_variables(), lps_reduced, m_specification.initial_process());
      return specification_reduced;
    }

    /// \brief Constructor
    /// \param filename The name of a file containing an mCRL2 specification
    /// \param rewriter_strategy The rewriter strategy used for generating next states
    pins(const std::string& filename, const std::string& rewriter_strategy)
      : m_specification(load_specification(filename)),
        m_generator(stochastic_specification(m_specification), data::rewriter(m_specification.data(), data::used_data_equation_selector(m_specification.data(), lps::find_function_symbols(m_specification), m_specification.global_variables()), data::parse_rewrite_strategy(rewriter_strategy))),
        m_parameters_list(process().process_parameters().begin(), process().process_parameters().end()),
        m_specification_reduced(reduce_specification(m_specification)),
        m_generator_reduced(stochastic_specification(m_specification_reduced), m_generator.get_rewriter())
    {
      initialize_read_write_groups();

      // store the process parameter names in a vector, to have random access to them
      data::variable_list params = m_specification.process().process_parameters();
      for (data::variable_list::iterator i = params.begin(); i != params.end(); ++i)
      {
        m_process_parameter_names.push_back(i->name());
      }

      // Each state parameter type gets it's own pins_data_type. State parameters of the same
      // type share the pins_data_type.
      std::map<data::sort_expression, pins_data_type*> existing_type_maps;
      std::vector<data::variable> parameters(process().process_parameters().begin(),process().process_parameters().end());
      for (std::size_t i = 0; i < params.size(); i++)
      {
        data::sort_expression s = parameters[i].sort();
        std::map<data::sort_expression, pins_data_type*>::const_iterator j = existing_type_maps.find(s);
        if (j == existing_type_maps.end())
        {
          pins_data_type* dt = new state_data_type(m_specification.data(), m_specification.action_labels(), s, m_specification.data().is_certainly_finite(s));
          m_data_types.push_back(dt);
          m_unique_data_types.push_back(dt);
          existing_type_maps[s] = dt;
        }
        else
        {
          m_data_types.push_back(j->second);
        }
      }
      pins_data_type* dt = new action_label_data_type(m_specification.data(), m_specification.action_labels());
      m_data_types.push_back(dt);
      m_unique_data_types.push_back(dt);

      for (std::size_t i = 0; i < m_data_types.size(); i++)
      {
        std::vector<pins_data_type*>::const_iterator j = std::find(m_unique_data_types.begin(), m_unique_data_types.end(), m_data_types[i]);
        assert(j != m_unique_data_types.end());
        m_unique_data_type_index.push_back(j - m_unique_data_types.begin());
      }
    }

    ~pins()
    {
      // make sure the pins data types are not deleted twice
      std::set<pins_data_type*> deleted;
      for (std::vector<pins_data_type*>::const_iterator i = m_data_types.begin(); i != m_data_types.end(); ++i)
      {
        if (deleted.find(*i) == deleted.end())
        {
          delete *i;
          deleted.insert(*i);
        }
      }
    }

    /// \brief Returns the number of unique datatype maps. Note that the datatype map for action labels is included,
    /// so this number equals the number of different process parameter types + 1.
    std::size_t datatype_count() const
    {
      return m_unique_data_types.size();
    }

    /// \brief Returns a reference to the datatype map with index i.
    /// \pre 0 <= i < datatype_count()
    pins_data_type& data_type(std::size_t i)
    {
      assert (i < m_data_types.size());
      return *m_unique_data_types[i];
    }

    /// \brief Indices of process parameters that influence event or next state of a summand by being read
    /// \param[in] index the selected summand
    /// \returns reference to a vector of indices of parameters
    /// \pre 0 <= i < group_count()
    const std::vector<size_t>& read_group(size_t index) const
    {
      return m_read_group[index];
    }

    /// \brief Indices of process parameters that influence event or next state of a summand by being written
    /// \param[in] index the selected summand
    /// \returns reference to a vector of indices of parameters
    /// \pre 0 <= i < group_count()
    const std::vector<size_t>& write_group(size_t index) const
    {
      return m_write_group[index];
    }

    /// \brief Indices of process parameters that influence event or next state of a summand by being read except from the guards.
    /// \param[in] index the selected summand
    /// \returns reference to a vector of indices of parameters
    /// \pre 0 <= i < group_count()
    const std::vector<size_t>& update_group(size_t index) const
    {
      return m_update_group[index];
    }

    const std::vector<size_t>& guard_parameters(size_t index) const
    {
      return guard_parameters_list[index];
    }

    const std::vector<size_t>& guard_info(size_t index) const
    {
      return m_guard_info[index];
    }

    size_t guard_count() const
    {
      return m_guards.size();
    }

    const std::string& guard_name(size_t index) {
      return m_guard_names[index];
    }

    /// \brief Returns a human-readable, unique name for process parameter i
    std::string process_parameter_name(std::size_t i) const
    {
      return m_process_parameter_names[i];
    }

    /// \brief Returns the datatype map index corresponding to process parameter i
    /// \pre 0 <= i < process_parameter_count()
    datatype_index process_parameter_type(std::size_t i) const
    {
      return m_unique_data_type_index[i];
    }

    /// \brief Returns the number of labels per edge.
    std::size_t edge_label_count() const
    {
      return 1;
    }

    /// \brief Returns the datatype map index corresponding to edge label i
    /// \pre 0 <= i < edge_label_count()
    datatype_index edge_label_type(std::size_t) const
    {
      return m_unique_data_types.size() - 1;
    }

    /// \brief Returns the name of the i-th action label (always "action").
    /// \pre 0 <= i < edge_label_count()
    std::string edge_label_name(std::size_t) const
    {
      return action_label_type_map().name();
    }

    /// \brief Assigns the initial state to s.
    void get_initial_state(ltsmin_state_type& s)
    {
      state initial_state = m_generator.initial_states().front().state(); // Only the first state of this state distribution is considered.
      for (size_t i = 0; i < m_state_length; i++)
      {
        s[i] = state_type_map(i)[initial_state[i]];
      }
    }

    /// \brief Returns the names of the actions that appear in the summand with index i,
    /// with 0 <= i < group_count().
    std::set<std::string> summand_action_names(std::size_t i) const
    {
      std::set<std::string> result;
      auto const l = process().action_summands()[i].multi_action().actions(); // Using a reference in whatever this type
                                                                              // is, leads to a failing lps_ltsmin_test.
      for (auto i = l.begin(); i != l.end(); ++i)
      {
        result.insert(std::string(i->label().name()));
      }
      return result;
    }

    /// \brief Iterates over the 'next states' of state src, and invokes a callback function for each discovered state.
    ///
    /// StateFunction is a callback function that must provide the function operator() with the following interface:
    /// <code>void operator()(ltsmin_state_type const& next_state, int* const& edge_labels, int group=-1);</code>
    /// where
    /// - \a next_state is the target state of the transition
    /// - \a edge_labels is an array of edge labels
    /// - \a group is the number of the summand from which the next state was generated, or -1 if it is unknown which summand
    ///
    /// \param src An LTSMin state
    /// \param f A 'callback' function object
    /// \param dest A destination state, which is modified and passed to the callback.  Must provide space for at least process_parameter_count() items.
    /// \param labels An array of labels, which is modified and passed to the callback.  Must provide space for at least edge_label_count() items.
    template <typename StateFunction>
    void next_state_all(ltsmin_state_type const& src, StateFunction& f, ltsmin_state_type const& dest, int* const& labels)
    {
      std::size_t nparams = process_parameter_count();
      data::data_expression_vector state_arguments(nparams);
      for (size_t i = 0; i < nparams; i++)
      {
        state_arguments[i] = static_cast<data::data_expression>(state_type_map(i).get(src[i]));
      }
      // data::data_expression_vector source = state_arguments;
      state source(state_arguments.begin(),nparams);

      next_state_generator::enumerator_queue_t enumeration_queue;
      for (next_state_generator::iterator i = m_generator.begin(source, &enumeration_queue); i; i++)
      {
        state destination = i->target_state();
        for (size_t j = 0; j < nparams; j++)
        {
          dest[j] = state_type_map(j)[destination[j]];
        }
        labels[0] = action_label_type_map()[detail::multi_action_to_aterm(i->action())];
        f(dest, labels);
      }
    }

    template <typename StateFunction>
    void next_state_long(ltsmin_state_type const& src, std::size_t group, StateFunction& f, ltsmin_state_type const& dest, int* const& labels)
    {
      _long(src, group, f, dest, labels, &m_generator);
    }

    template <typename StateFunction>
    void update_long(ltsmin_state_type const& src, std::size_t group, StateFunction& f, ltsmin_state_type const& dest, int* const& labels)
    {
      _long(src, group, f, dest, labels, &m_generator_reduced);
    }

    /// \brief Iterates over the 'next states' of a particular summand
    /// of state src that are generated by a group of summands, and
    /// invokes a callback function for each discovered state.
    ///
    /// StateFunction is a callback function that must provide the function operator() with the following interface:
    /// <code>void operator()(ltsmin_state_type const& next_state, int* const& edge_labels, int group);</code>
    /// where
    /// - next_state is the target state of the transition
    /// - edge_labels is an array of edge labels
    /// - group is the number of the summand from which the next state was generated, or -1 if it is unknown which summand
    ///
    /// \param src An LTSMin state.
    /// \param group the number of the summand.
    /// \param f A state function.
    /// \param dest A destination state, which is modified and passed to the callback.
    ///        Must provide space for at least process_parameter_count() items.
    /// \param labels An array of labels, which is modified and passed to the callback.
    ///        Must provide space for at least edge_label_count() items.
    /// \param generator The next state generator to use.
    template <typename StateFunction>
    void _long(ltsmin_state_type const& src,
               std::size_t group,
               StateFunction& f,
               const ltsmin_state_type& dest,
               int* const& labels,
               lps::next_state_generator* generator)
    {
      std::size_t nparams = process_parameter_count();
      data::data_expression_vector state_arguments(nparams);
      for (size_t i = 0; i < nparams; i++)
      {
        state_arguments[i] = static_cast<data::data_expression>(state_type_map(i).get(src[i]));
      }
      state source(state_arguments.begin(),nparams);

      next_state_generator::enumerator_queue_t enumeration_queue;
      for (next_state_generator::iterator i = (*generator).begin(source, group, &enumeration_queue); i; i++)
      {
        state destination = i->target_state();
        for (size_t j = 0; j < nparams; j++)
        {
          dest[j] = state_type_map(j)[destination[j]];
        }
        labels[0] = action_label_type_map()[detail::multi_action_to_aterm(i->action())];
        f(dest, labels);
      }
    }

    guard_evaluation_t eval_guard_long(ltsmin_state_type const& src, std::size_t guard) {
      std::size_t nparams = process_parameter_count();
      substitution_t substitution;
      for (size_t i = 0; i < nparams; i++)
      {
        data::data_expression value(static_cast<data::data_expression>(state_type_map(i).get(src[i])));
        substitution[m_parameters_list[i]] = value;
      }

      // get the result by rewriting the guard with the substitution.
      data::data_expression result = m_generator_reduced.get_rewriter()(
          static_cast<data::data_expression>(m_guards.get(guard)),
          substitution);

      if(result == data::sort_bool::false_()) { // the guard rewrites to false.
        return GUARD_FALSE;
      } else if(result == data::sort_bool::true_()) { // the guard rewrites to true.
        return GUARD_TRUE;
      } else { // the guard does not rewrite to true or false, so maybe...
        return GUARD_MAYBE;
      }
    }

    /// \brief Prints an overview of several relevant attributes.
    std::string info()
    {
      std::ostringstream out;

      out << "\n--- EDGE LABELS ---\n";
      out << "edge_label_count() = " << edge_label_count() << std::endl;
      for (std::size_t i = 0; i < edge_label_count(); i++)
      {
        out << "\n";
        out << "edge_label_name(" << i << ") = " << edge_label_name(i) << std::endl;
        out << "edge_label_type(" << i << ") = " << edge_label_type(i) << std::endl;
      }

      out << "\n--- PROCESS PARAMETERS ---\n";
      out << "process_parameter_count() = " << process_parameter_count() << std::endl;
      for (std::size_t i = 0; i < process_parameter_count(); i++)
      {
        out << "\n";
        out << "process_parameter_name(" << i << ") = " << process_parameter_name(i) << std::endl;
        out << "process_parameter_type(" << i << ") = " << process_parameter_type(i) << std::endl;
      }

      out << "\n--- SUMMANDS ---\n";
      out << "group_count() = " << group_count() << std::endl;
      for (std::size_t i = 0; i < group_count(); i++)
      {
        out << std::endl;
        out << "  read_group(" << i << ") = " << print_vector(read_group(i)) << std::endl;
        out << " write_group(" << i << ") = " << print_vector(write_group(i)) << std::endl;
        out << "update_group(" << i << ") = " << print_vector(update_group(i)) << std::endl;
      }

      out << "\n--- GUARDS ---\n";
      out << "guard_count() = " << guard_count() << std::endl;
      for (std::size_t i = 0; i < guard_count(); i++)
      {
        out << "guard(" << i << ") = " << print_vector(guard_parameters(i)) << std::endl;
      }

      out << "\n--- INITIAL STATE ---\n";
      ltsmin_state_type init = new int[process_parameter_count()];
      get_initial_state(init);
      out << "initial state = " << print_vector(init, init + process_parameter_count()) << std::endl;
      delete[] init;

      out << "\n--- DATA TYPE MAPS ---\n";
      out << "datatype_count() = " << datatype_count() << std::endl;
      for (std::size_t i = 0; i < datatype_count(); i++)
      {
        const pins_data_type& type = data_type(i);
        out << "\n";
        out << "datatype " << i << "\n"
            << " name    = " << type.name() << "\n"
            << " size    = " << type.size() << "\n"
            << " bounded = " << std::boolalpha << type.is_bounded() << "\n";
        if (type.is_bounded())
        {
          out << " possible values: " << core::detail::print_set(type.generate_values(10)) << std::endl;
        }
        out << std::endl;
      }

      return out.str();
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LTSMIN_H
