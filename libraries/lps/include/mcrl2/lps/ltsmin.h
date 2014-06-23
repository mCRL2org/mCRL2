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
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/data/substitutions/mutable_indexed_substitution.h"
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
  typedef data::classic_enumerator<data::rewriter> enumerator_type;
  enumerator_type enumerator(rewr, dataspec);
  data::variable x("x", s);
  data::variable_vector v;
  v.push_back(x);
  data::mutable_indexed_substitution<> sigma;
  data::variable_list vl(v.begin(),v.end());
  std::deque < data::enumerator_list_element_with_substitution<data::data_expression> >
      enumerator_deque(1,data::enumerator_list_element_with_substitution<data::data_expression>(vl, data::sort_bool::true_()));
  for (enumerator_type::iterator
       i = enumerator.begin(sigma, enumerator_deque, max_internal_variables); i != enumerator.end() ; ++i)
  {
    i->add_assignments(vl,sigma,rewr);
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
    atermpp::indexed_set m_indexed_set;
    lps::next_state_generator& m_generator;
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
    pins_data_type(lps::next_state_generator& generator, bool is_bounded = false)
      : m_generator(generator),
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
    atermpp::indexed_set& indexed_set()
    {
      return m_indexed_set;
    }

    /// \brief Returns the indexed_set holding the values
    const atermpp::indexed_set& indexed_set() const
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
    state_data_type(lps::next_state_generator& generator, const data::sort_expression& sort)
      : pins_data_type(generator, generator.get_specification().data().is_certainly_finite(sort)),
        m_sort(sort)
    {
      m_name = data::pp(m_sort);
    }

    std::string serialize(int i) const
    {
      return to_string(index2expression(i));
    }

    std::size_t deserialize(const std::string& s)
    {
      return expression2index(data::data_expression(atermpp::read_term_from_string(s)));
    }

    std::string print(int i) const
    {
      return data::pp(index2expression(i));
    }

    std::size_t parse(const std::string& s)
    {
      return expression2index(data::parse_data_expression(s, m_generator.get_specification().data()));
    }

    const std::string& name() const
    {
      return m_name;
    }

    std::vector<std::string> generate_values(std::size_t max_size = 1000) const
    {
      return lps::generate_values(m_generator.get_specification().data(), m_sort, max_size);
    }
};

/// \brief Models the mapping of mCRL2 action labels to integers.
class action_label_data_type: public pins_data_type
{
  protected:
    std::string m_name;

  public:
    action_label_data_type(lps::next_state_generator& generator)
      : pins_data_type(generator, false)
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
      lps::multi_action m = lps::parse_multi_action(s, m_generator.get_specification().action_labels(), m_generator.get_specification().data());
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

  protected:
    size_t m_group_count;
    size_t m_state_length; /**< the number of process parameters */
    std::vector<std::vector<size_t> > m_read_group;
    std::vector<std::vector<size_t> > m_write_group;
    lps::specification m_specification;
    lps::next_state_generator m_generator;
    std::vector<std::string> m_process_parameter_names;

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
      return m_generator.get_specification().process();
    }

    /// \brief Returns the data specification of the LPS specification
    const data::data_specification& data() const
    {
      return m_generator.get_specification().data();
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
      const lps::linear_process& proc = process();

      m_group_count  = proc.action_summands().size();
      m_state_length = proc.process_parameters().size();

      m_read_group.resize(m_group_count);
      m_write_group.resize(m_group_count);

      // the set with process parameters
      std::set<data::variable> parameters(proc.process_parameters().begin(), proc.process_parameters().end());

      // the list of summands
      std::vector<lps::action_summand> const& summands = proc.action_summands();
      for (std::vector<lps::action_summand>::const_iterator i = summands.begin(); i != summands.end(); ++i)
      {
        std::set<data::variable> used_read_variables;
        std::set<data::variable> used_write_variables;

        data::find_free_variables(i->condition(), std::inserter(used_read_variables, used_read_variables.end()));
        lps::find_free_variables(i->multi_action(), std::inserter(used_read_variables, used_read_variables.end()));

        data::assignment_list assignments(i->assignments());
        for (data::assignment_list::const_iterator j = assignments.begin(); j != assignments.end(); ++j)
        {
          if(j->lhs() != j->rhs())
          {
            data::find_all_variables(j->lhs(), std::inserter(used_write_variables, used_write_variables.end()));
            data::find_all_variables(j->rhs(), std::inserter(used_read_variables, used_read_variables.end()));
          }
        }

        // process parameters used in condition or action of summand
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

        std::vector<data::variable> parameters_list(proc.process_parameters().begin(),proc.process_parameters().end());

        for (std::vector<data::variable>::const_iterator j = parameters_list.begin(); j != parameters_list.end(); ++j)
        {
          if (!used_read_parameters.empty() && used_read_parameters.find(*j) != used_read_parameters.end())
          {
            m_read_group[i - summands.begin()].push_back(j - parameters_list.begin());
          }
          if (!used_write_parameters.empty() && used_write_parameters.find(*j) != used_write_parameters.end())
          {
            m_write_group[i - summands.begin()].push_back(j - parameters_list.begin());
          }
        }
      }
    }

  public:
    typedef std::size_t datatype_index; /**< the index type for datatype maps */

    /// \brief Returns the number of process parameters of the LPS
    std::size_t process_parameter_count() const
    {
      return m_generator.get_specification().process().process_parameters().size();
    }

    /// \brief Returns the number of available groups. This equals the number of action summands of the LPS.
    std::size_t group_count() const
    {
      return m_generator.get_specification().process().action_summands().size();
    }

    static lps::specification load_specification(const std::string& filename)
    {
      lps::specification specification;
      load_lps(specification, filename);
      return specification;
    }

    /// \brief Constructor
    /// \param filename The name of a file containing an mCRL2 specification
    /// \param rewriter_strategy The rewriter strategy used for generating next states
    pins(const std::string& filename, const std::string& rewriter_strategy)
      : m_specification(load_specification(filename)),
        m_generator(m_specification, data::rewriter(m_specification.data(), data::used_data_equation_selector(m_specification.data(),lps::find_function_symbols(m_specification),m_specification.global_variables()), data::parse_rewrite_strategy(rewriter_strategy)))
    {
      initialize_read_write_groups();

      // store the process parameter names in a vector, to have random access to them
      data::variable_list params = m_generator.get_specification().process().process_parameters();
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
          pins_data_type* dt = new state_data_type(m_generator, s);
          m_data_types.push_back(dt);
          m_unique_data_types.push_back(dt);
          existing_type_maps[s] = dt;
        }
        else
        {
          m_data_types.push_back(j->second);
        }
      }
      pins_data_type* dt = new action_label_data_type(m_generator);
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
      data::data_expression_vector initial_state = m_generator.internal_initial_state();
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
      auto const& l = process().action_summands()[i].multi_action().actions();
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
      data::data_expression_vector source = state_arguments;

      for (next_state_generator::iterator i = m_generator.begin(source); i; i++)
      {
        data::data_expression_vector destination = i->internal_state();
        for (size_t j = 0; j < nparams; j++)
        {
          dest[j] = state_type_map(j)[destination[j]];
        }
        labels[0] = action_label_type_map()[detail::multi_action_to_aterm(i->action())];
        f(dest, labels);
      }
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
    /// \param src An LTSMin state
    /// \param group the number of the summand, which
    /// \param dest A destination state, which is modified and passed to the callback.
    ///        Must provide space for at least process_parameter_count() items.
    /// \param labels An array of labels, which is modified and passed to the callback.
    ///        Must provide space for at least edge_label_count() items.
    template <typename StateFunction>
    void next_state_long(ltsmin_state_type const& src, std::size_t group, StateFunction& f, ltsmin_state_type const& dest, int* const& labels)
    {
      std::size_t nparams = process_parameter_count();
      data::data_expression_vector state_arguments(nparams);
      for (size_t i = 0; i < nparams; i++)
      {
        state_arguments[i] = static_cast<data::data_expression>(state_type_map(i).get(src[i]));
      }
      data::data_expression_vector source = state_arguments;

      for (next_state_generator::iterator i = m_generator.begin(source, group); i; i++)
      {
        data::data_expression_vector destination = i->internal_state();
        for (size_t j = 0; j < nparams; j++)
        {
          dest[j] = state_type_map(j)[destination[j]];
        }
        labels[0] = action_label_type_map()[detail::multi_action_to_aterm(i->action())];
        f(dest, labels);
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
        out << "\n";
        out << " read_group(" << i << ") = " << print_vector(read_group(i)) << std::endl;
        out << "write_group(" << i << ") = " << print_vector(write_group(i)) << std::endl;
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
