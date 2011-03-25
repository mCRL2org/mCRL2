// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps/include/mcrl2/lps/ltsmin.h
/// \brief add your file description here.

#ifndef LPS_INCLUDE_MCRL2_LPS_LTSMIN_H
#define LPS_INCLUDE_MCRL2_LPS_LTSMIN_H

#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/next_state_generator.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"

namespace mcrl2 {

namespace lps {

inline
data::rewriter::strategy parse_rewriter_strategy(const std::string& rewriter_strategy)
{
  if (rewriter_strategy == "jitty")
  {
		return data::rewriter::jitty;
  }
  else if (rewriter_strategy == "inner")
  {
    return data::rewriter::innermost;
  }
#ifdef MCRL2_INNERC_AVAILABLE
  else if (rewriter_strategy == "jittyc")
  {
		return data::rewriter::jitty_compiling;
  }
  else if (rewriter_strategy == "innerc")
  {
    return data::rewriter::innermost_compiling;
  }
#endif
  else
  {
    throw std::runtime_error("Error: unknown rewriter strategy " + rewriter_strategy + "!");
  }
  return data::rewriter::jitty;
}

class pins
{
  public:
    typedef int* ltsmin_state_type; /**< the state type used by LTSMin */
    typedef lps::next_state_generator::state_type next_state_type; /**< the state type used by the next state generator */

  protected:
    size_t m_group_count;
    size_t m_state_length; /**< the number of process parameters */
    std::vector<std::vector<size_t> > m_group_read_info;
    std::vector<std::vector<size_t> > m_group_write_info;
    lps::next_state_generator m_generator;
    atermpp::function_symbol m_state_fun; /**< the function symbol used for 'next states' */
    std::vector<std::string> m_process_parameter_names;

    /// \brief The pins class maintains for each type a mapping of integers to known values of the type.
    /// There are always at least two entries in the map:
    /// - index 0: the state type
    /// - index 1: the label type
    /// N.B. The mappings contain values in the next state format, so they need to be translated to data expressions before they can be printed.
    std::vector<atermpp::indexed_set> m_datatype_mappings;

    /// \brief Returns the process of the LPS specification
    const linear_process& process()
    {
      return m_generator.get_specification().process();
    }
       
    // N.B. This is meant as a possible future extension.
    // \brief m_process_parameter_index[i] is the index of the typemap that corresponds to the i-th process parameter.
    // std::vector<int> m_process_parameter_index;

    void initialize_read_write_groups()
    {
      const lps::linear_process& proc = process();

      m_group_count  = proc.action_summands().size();
      m_state_length = proc.process_parameters().size();

      m_group_read_info.resize(m_group_count);
      m_group_write_info.resize(m_group_count);

      // the set with process parameters
      std::set<data::variable> parameters(proc.process_parameters().begin(), proc.process_parameters().end());

      // the list of summands
      std::vector<lps::action_summand> const& summands = proc.action_summands();
      for (std::vector<lps::action_summand>::const_iterator i = summands.begin(); i != summands.end(); ++i)
      {
        std::set<data::variable> used_read_variables;
        std::set<data::variable> used_write_variables;

        lps::find_free_variables(i->multi_action(), std::inserter(used_read_variables, used_read_variables.end()));

        data::assignment_list assignments(i->assignments());
        for (data::assignment_list::const_iterator j = assignments.begin(); j != assignments.end(); ++j)
        {
          if(j->lhs() != j->rhs())
          {
            data::find_variables(j->lhs(), std::inserter(used_write_variables, used_write_variables.end()));
            data::find_variables(j->rhs(), std::inserter(used_read_variables, used_read_variables.end()));
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

        std::vector<data::variable> parameters_list = atermpp::convert<std::vector<data::variable> >(proc.process_parameters());

        for (std::vector<data::variable>::const_iterator j = parameters_list.begin(); j != parameters_list.end(); ++j)
        {
          if (!used_read_parameters.empty() && used_read_parameters.find(*j) != used_read_parameters.end())
          {
            m_group_read_info[i - summands.begin()].push_back(j - parameters_list.begin());
          }
          if (!used_read_parameters.empty() && used_write_parameters.find(*j) != used_write_parameters.end())
          {
            m_group_write_info[i - summands.begin()].push_back(j - parameters_list.begin());
          }
        }
      }
    }

    void initialize_type_mappings()
    {
      m_datatype_mappings.push_back(atermpp::indexed_set()); // state type
      m_datatype_mappings.push_back(atermpp::indexed_set()); // label type
      //const lps::linear_process& proc = process();
      //for (data::variable_list::const_iterator i = proc.process_parameters().begin(); i != proc.process_parameters().end(); ++i)
      //{
      //  m_process_parameter_index.push_back(0);
      //}
    }

    std::size_t aterm2index(const atermpp::aterm& x) const
    {
      return m_datatype_mappings[0].index(x);
    }

    atermpp::aterm index2aterm(std::size_t i) const
    {
      return m_datatype_mappings[0].get(i);
    }

    std::size_t expression2index(const data::data_expression& x) const
    {
      return aterm2index(m_generator.expression2aterm(x));
    }

    const data::data_expression& index2expression(std::size_t i) const
    {
      return m_generator.aterm2expression(index2aterm(i));
    }

    std::size_t label2index(const atermpp::aterm_appl& x)
    {
      return m_datatype_mappings[1].index(x);
    }
                                   
    /// \brief Converts state component represented as integers into aterms (in the next state format).
    struct state_component_converter: public std::unary_function<std::size_t, atermpp::aterm>
    {
      const atermpp::indexed_set& datatype_map;
      
      state_component_converter(const atermpp::indexed_set& datatype_map_)
        : datatype_map(datatype_map_)
      {}
      
      atermpp::aterm operator()(std::size_t i) const
      {
        return datatype_map.get(i);
      }
    };

  public:
    typedef std::size_t datatype_index; /**< the index type for datatype maps */

    /// \brief Returns the number of process parameters of the LPS
    std::size_t process_parameter_count() const
    {
      return m_generator.get_specification().process().process_parameters().size();
    }

    /// \brief Constructor
    /// \param filename The name of a file containing an mCRL2 specification
    /// \param rewriter_strategy The rewriter strategy used for generating next states
    pins(const std::string& filename, const std::string& rewriter_strategy)
      : m_generator(filename, parse_rewriter_strategy(rewriter_strategy))
    {
      initialize_read_write_groups();
      initialize_type_mappings();
      
      // TODO: this is ugly, is there a better way to create states?
      m_state_fun = atermpp::function_symbol("STATE", process_parameter_count(), false);

      // store the process parameter names in a vector, to have random access to them
      data::variable_list params = m_generator.get_specification().process().process_parameters();
      for (data::variable_list::iterator i = params.begin(); i != params.end(); ++i)
      {
        m_process_parameter_names.push_back(i->name());
      }
    }

    /// \brief Serializes the i-th value of the datatype map with index d.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return A string that uniquely corresponds with the value.
    std::string serialize(datatype_index d, std::size_t i)
    {
      return m_datatype_mappings[d].get(i).to_string();
    }

    /// \brief Deserializes a string to a data value, and returns the corresponding index
    /// in a datatype map.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return The index of the data value in the datatype map with index d, or -1 if it is not found in this map.
    std::size_t deserialize(datatype_index d, const std::string& s)
    {
      return m_datatype_mappings[d].index(atermpp::read_from_string(s));
    }

    /// \brief Returns a pretty printed representation of the i-th value of the datatype map with index d.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return A string representation that does not uniquely correspond with the value.
    std::string print(datatype_index d, int i)
    {
      return core::pp(m_datatype_mappings[d].get(i));
    }

    /// \brief Parses a textual representation of a data value, and returns the corresponding index
    /// in a datatype map.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return The index of the data value in the datatype map with index d, or -1 if it is not found in this map.
    std::size_t parse(datatype_index d, const std::string& s) const
    {
      if (d == 0)
      {
        data::data_expression e = data::parse_data_expression(s, m_generator.get_specification().data());
        return expression2index(e);
      }
      if (d == 1)
      {
        lps::multi_action a = lps::parse_multi_action(s);
        // TODO: change this when multi_action becomes a proper term
        ATermAppl t = core::detail::gsMakeMultAct(a.actions());
        return m_datatype_mappings[d].index(atermpp::aterm_appl(t));
      }
      throw std::runtime_error("Out of bounds error in pins::parse!");
    }

    /// \brief Returns the name of the datatype map with index d
    /// - The datatype map with index 0 is named "state"
    /// - The datatype map with index 1 is named "action_label"
    std::string datatype_name(datatype_index d) const
    {
      switch (d)
      {
        case 0: return "state";
        case 1: return "action_label";
      }
      return "unknown";
    }

    /// \brief Returns the number of datatype maps.
    std::size_t datatype_count() const
    {
      return m_datatype_mappings.size();
    }

    /// \brief Returns the number of elements in datatype map d.
    std::size_t datatype_size(datatype_index d) const
    {
      return m_datatype_mappings[d].size();
    }

    /// \brief Indices of process parameters that influence event or next state of a summand by being read
    /// \param[in] index the selected summand
    /// \returns reference to a vector of indices of parameters
    const std::vector<size_t>& read_group(size_t index) const
    {
      return m_group_read_info[index];
    }

    /// \brief Indices of process parameters that influence event or next state of a summand by being written
    /// \param[in] index the selected summand
    /// \returns reference to a vector of indices of parameters
    const std::vector<size_t>& write_group(size_t index) const
    {
      return m_group_write_info[index];
    }

    /// \brief Returns the number of action summands of the LPS.
    std::size_t summand_count() const
    {
      return m_generator.get_specification().process().action_summands().size();
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
      return 0;
    }

    /// \brief Returns the number of labels per edge (always 1).
    std::size_t edge_label_count() const
    {
      return 1;
    }

    /// \brief Returns the datatype map index corresponding to edge label i
    /// \pre 0 <= i < edge_label_count()
    datatype_index edge_label_type(std::size_t i) const
    {
      return 0;
    }

    /// \brief Returns the name of the i-th action label (always "action").
    /// \pre 0 <= i < edge_label_count()
    std::string edge_label_name(std::size_t i) const
    {
      return "action";
    }

    /// \brief Assigns the initial state to s.
    void get_initial_state(ltsmin_state_type& s) const
    {
      ATerm a = m_generator.initial_state();
      atermpp::aterm_appl initial_state(reinterpret_cast<ATermAppl>(a));
	    for (size_t i = 0; i < m_state_length; ++i)
	    {
	      s[i] = aterm2index(initial_state(i));
      }
    }

    /// \brief Iterates over the 'next states' of state src, and invokes a callback function for each discovered state.
    ///
    /// StateFunction is a callback function that must provide the function operator() with the following interface:
    ///
    /// \code
    /// void operator()(int edge_label, const ltsmin_state_type& next_state, int group);
    /// \endcode
    /// where
    /// - edge_label corresponds to the label of the transition to the next state
    /// - state is the next state
    /// - group is the number of the summand from which the next state was generated, or -1 if it is unknown which summand
    template <typename StateFunction>
    void next_state_all(const int* src, StateFunction& f)
    {
      std::size_t nparams = process_parameter_count();
      atermpp::aterm_appl init(m_state_fun,
                               boost::make_transform_iterator(src, state_component_converter(m_datatype_mappings[0])),
                               boost::make_transform_iterator(src + nparams, state_component_converter(m_datatype_mappings[0]))
                              );
      next_state_generator::iterator i = m_generator.begin(init);
      while (++i)
      {
        const lps::next_state_generator::state_type& s = *i;
        for (size_t i = 0; i < nparams; ++i)
        {
          f[i] = aterm2index(s[i]);
        }
        f(label2index(s.label()), -1);
      }
    }

    /// \brief Iterates over the 'next states' of state src that are generated by a group of summands, and invokes a
    /// callback function for each discovered state.
    ///
    /// StateFunction is a callback function that must provide the function operator() with the following interface:
    ///
    /// \code
    /// void operator()(int edge_label, const ltsmin_state_type& next_state, int group);
    /// \endcode
    /// where
    /// - edge_label corresponds to the label of the transition to the next state
    /// - state is the next state
    /// - group is the number of the summand from which the next state was generated, or -1 if it is unknown which summand
    template <typename StateFunction>
    void next_state_long(const ltsmin_state_type& src, std::size_t group, StateFunction& f)
    {
      std::size_t nparams = process_parameter_count();
      atermpp::aterm_appl init(m_state_fun,
                               boost::make_transform_iterator(src, state_component_converter(m_datatype_mappings[0])),
                               boost::make_transform_iterator(src + nparams, state_component_converter(m_datatype_mappings[0]))
                              );
      next_state_generator::iterator i = m_generator.begin(init, group);
      while (++i)
      {
        const lps::next_state_generator::state_type& s = *i;
        for (size_t i = 0; i < nparams; ++i)
        {
          f[i] = aterm2index(s[i]);
        }
        f(label2index(s.label()), group);
      }
    }
};

} // namespace lps

} // namespace mcrl2

#endif // LPS_INCLUDE_MCRL2_LPS_LTSMIN_H
