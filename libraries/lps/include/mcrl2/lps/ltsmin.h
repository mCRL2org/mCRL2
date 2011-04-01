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
            
/// \brief Models a pins data type. A pins data type maintains a mapping between known values
/// of a data type and integers. The mapping is dense, meaning that all integers in the interval
/// [0, ..., size()[ have a corresponding value.
class pins_data_type
{
  /// \brief Serializes the i-th value of the data type to a binary string.
  /// It is guaranteed that serialize(deserialize(i)) == i.
  /// \pre 0 <= i < size()
  virtual std::string serialize(int i) const = 0;

  /// \brief Deserializes a string to a data value, and returns the corresponding index.
  /// \return The index of the data value.
  /// \throw <std::runtime_error> { if deserialization failed, or if the value was not found in the mapping }
  virtual std::size_t deserialize(const std::string& s) const = 0;

  /// \brief Returns a human readable representation of the value with index i.
  /// It is guaranteed that parse(print(i)) == i.
  /// \pre 0 <= i < size()
  virtual std::string print(int i) const = 0;

  /// \brief Parses a string to a data value, and returns the corresponding index.
  /// \return The index of the data value.
  /// \throw <std::runtime_error> { if deserialization failed, or if the value was not found in the mapping }
  virtual std::size_t parse(const std::string& s) const = 0;

  /// \brief Returns the name of the data type.
  virtual const std::string& name() const = 0;

  /// \brief Returns the number of values that are stored in the map.
  virtual std::size_t size() const = 0;
  
  /// \brief Destructor.  
  virtual ~pins_data_type()
  {}
};

class pins
{
  public:
    typedef int* ltsmin_state_type; /**< the state type used by LTSMin */
    typedef lps::next_state_generator::state_type next_state_type; /**< the state type used by the next state generator */

  protected:
    size_t m_group_count;
    size_t m_state_length; /**< the number of process parameters */
    std::vector<std::vector<size_t> > m_read_group;
    std::vector<std::vector<size_t> > m_write_group;
    lps::next_state_generator m_generator;
    atermpp::function_symbol m_state_fun; /**< the function symbol used for 'next states' */
    std::vector<std::string> m_process_parameter_names;
    ltsmin_state_type m_ltsmin_state; /**< used by the functions next_state_all and next_state_long */

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
            m_read_group[i - summands.begin()].push_back(j - parameters_list.begin());
          }
          if (!used_read_parameters.empty() && used_write_parameters.find(*j) != used_write_parameters.end())
          {
            m_write_group[i - summands.begin()].push_back(j - parameters_list.begin());
          }
        }
      }
    }

    void initialize_type_mappings()
    {
      m_datatype_mappings.push_back(atermpp::indexed_set()); // state type
      m_datatype_mappings.push_back(atermpp::indexed_set()); // label type
    }

    /// \brief Returns the index of the aterm x in the first datatype map. If it is not present yet, it will be added.
    std::size_t aterm2index(const atermpp::aterm& x)
    {
      return m_datatype_mappings[0][x];
    }

    atermpp::aterm index2aterm(std::size_t i) const
    {
      return m_datatype_mappings[0].get(i);
    }

    std::size_t expression2index(const data::data_expression& x)
    {
      return aterm2index(m_generator.expression2aterm(x));
    }

    data::data_expression index2expression(std::size_t i) const
    {
      return m_generator.aterm2expression(index2aterm(i));
    }

    std::size_t label2index(const atermpp::aterm_appl& x)
    {
      return m_datatype_mappings[1][x];
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

    /// \brief Returns the number of available groups. This equals the number of action summands of the LPS.
    std::size_t group_count() const
    {
      return m_generator.get_specification().process().action_summands().size();
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

      // TODO: allocate this in a safer way by using some kind of smart pointer
      m_ltsmin_state = new int[process_parameter_count()];
    }

    ~pins()
    {
      delete[] m_ltsmin_state;
    }

    /// \brief Serializes the i-th value of the datatype map with index d.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return A string that uniquely corresponds with the value.
    std::string serialize(datatype_index d, std::size_t i)
    {
      if (d == 0)
      {
        return m_generator.aterm2expression(m_datatype_mappings[d].get(i)).to_string();
      }
      else
      {
        return m_datatype_mappings[d].get(i).to_string();
      }
    }

    /// \brief Deserializes a string to a data value, and returns the corresponding index
    /// in a datatype map.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return The index of the data value in the datatype map with index d, or -1 if it is not found in this map.
    std::size_t deserialize(datatype_index d, const std::string& s)
    {
      ATerm t = atermpp::read_from_string(s);
      atermpp::aterm_appl a(reinterpret_cast<ATermAppl>(t));
      if (d == 0)
      {
        return m_datatype_mappings[0].index(m_generator.expression2aterm(a));
      }
      else
      {
        return m_datatype_mappings[d].index(a);
      }
    }

    /// \brief Returns a pretty printed representation of the i-th value of the datatype map with index d.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \pre 0 <= i < datatype_size(d)
    /// \return A pretty print representation of the value. Note that the pretty print representation is
    /// not guaranteed to correspond uniquely with the value.
    std::string print(datatype_index d, int i)
    {
      if (d == 0)
      {
        return core::pp(m_generator.aterm2expression(m_datatype_mappings[0].get(i)));
      }
      else
      {
        return core::pp(m_datatype_mappings[d].get(i));
      }     
    }

    /// \brief Parses a textual representation of a data value, and returns the corresponding index
    /// in a datatype map.
    /// \param d A datatype index (0 <= i < datatype_count()).
    /// \return The index of the data value in the datatype map with index d, or -1 if it is not found in this map.
    std::size_t parse(datatype_index d, const std::string& s)
    {
      if (d == 0)
      {
        data::data_expression e = data::parse_data_expression(s, m_generator.get_specification().data());
        return expression2index(e);
      }
      if (d == 1)
      {
        lps::multi_action a = lps::parse_multi_action(s, m_generator.get_specification().action_labels(), m_generator.get_specification().data());
        // TODO: change this when multi_action becomes a proper term
        ATermAppl t = core::detail::gsMakeMultAct(a.actions());
        return m_datatype_mappings[0][atermpp::aterm_appl(t)];
      }
      throw std::runtime_error("Out of bounds error in pins::parse!");
    }

    /// \brief Returns the number of datatype maps.
    std::size_t datatype_count() const
    {
      return m_datatype_mappings.size();
    }

    /// \brief Returns the name of the datatype map with index d
    /// - The datatype map with index 0 is named "state"
    /// - The datatype map with index 1 is named "action_label"
    /// \pre 0 <= i < datatype_count()
    std::string datatype_name(datatype_index d) const
    {
      switch (d)
      {
        case 0: return "state";
        case 1: return "action_label";
      }
      return "unknown";
    }

    /// \brief Returns the number of elements in datatype map d.
    /// \pre 0 <= i < datatype_count()
    std::size_t datatype_size(datatype_index d) const
    {
      return m_datatype_mappings[d].size();
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
      return 1;
    }

    /// \brief Returns the name of the i-th action label (always "action").
    /// \pre 0 <= i < edge_label_count()
    std::string edge_label_name(std::size_t i) const
    {
      return "action";
    }

    /// \brief Assigns the initial state to s.
    void get_initial_state(ltsmin_state_type& s)
    {
      ATerm a = m_generator.initial_state();
      atermpp::aterm_appl initial_state(reinterpret_cast<ATermAppl>(a));
	    for (size_t i = 0; i < m_state_length; ++i)
	    {
	      s[i] = aterm2index(initial_state(i));
      }
    }

    /// \brief Iterates over the 'next states' of state src, and invokes a callback function for each discovered state.
    /// \param src An LTSMin state
    /// \param f A 'callback' function object
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
    void next_state_all(const ltsmin_state_type& src, StateFunction& f)
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
          m_ltsmin_state[i] = aterm2index(s[i]);
        }
        std::size_t label = label2index(s.label());
        int group = -1; // we don't know the summand
        f(label, m_ltsmin_state, group);
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
          m_ltsmin_state[i] = aterm2index(s[i]);
        }
        std::size_t label = label2index(s.label());
        f(label, m_ltsmin_state, group);
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

      out << "\n--- DATA TYPE MAPS ---\n";
      out << "datatype_count() = " << datatype_count() << std::endl;
      for (std::size_t i = 0; i < datatype_count(); i++)
      {
        out << "\n";
        out << "datatype_name(" << i << ") = " << datatype_name(i) << std::endl;
        out << "datatype_size(" << i << ") = " << datatype_size(i) << std::endl;
      }

      return out.str();
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_LTSMIN_H
