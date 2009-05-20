// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/specification_property_map.h
/// \brief A property map containing properties of an LPS specification.

#ifndef MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H
#define MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H

#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/detail/convert.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Stores the following properties of a linear process specification:
  /// \li summand_count                The number of summands
  /// \li tau_summand_count            The number of tau summands
  /// \li delta_summand_count          The number of delta summands
  /// \li declared_free_variables      The declared free variables
  /// \li declared_free_variable_names The names of the declared free variables
  /// \li declared_variable_count      The number of declared free variables
  /// \li used_free_variables          The used free variables
  /// \li used_free_variables_names    The names of the used free variables
  /// \li used_free_variable_count     The number of used free variables
  /// \li process_parameters           The process parameters
  /// \li process_parameter_names      The names of the process parameters
  /// \li process_parameter_count      The number of process parameters
  /// \li declared_action_labels       The names of the declared action labels
  /// \li declared_action_label_count  The number of declared action labels
  /// \li used_action_labels           The names of the used action labels
  /// \li used_action_label_count      The number of used action labels
  /// \li used_multi_actions           The used multi-actions (sets of label names)
  /// \li used_multi_action_count      The number of used multi-actions
  class specification_property_map
  {
    protected:
      /// \brief Contains a normalized string representation of the properties.
      std::map<std::string, std::string> m_data;

      //--------------------------------------------//
      // print functions
      //--------------------------------------------//     
      std::string print(unsigned int n) const
      {
        std::ostringstream out;
        out << n;
        return out.str();
      }

      std::string print(std::string s) const
      {       
        return s;
      }

      std::string print(const core::identifier_string& s) const
      {       
        return s;
      }

      std::string print(const data::variable& v) const
      {
        return pp(v) + ":" + pp(v.sort());
      }
      
      std::string print(const action_label l) const
      {
        return core::pp(l.name());
      }

      template <typename T>
      std::string print(const std::set<T>& v, bool print_separators = true) const
      {
        std::set<std::string> elements;
        for (typename std::set<T>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          elements.insert(print(*i));
        }
        std::string result = boost::algorithm::join(elements, ", ");
        if (print_separators)
        {
          result = "{" + result + "}";
        }                
        return result;
      }
           
      template <typename T>
      std::string print(const std::multiset<T>& v, bool print_separators = true) const
      {
        std::multiset<std::string> elements;
        for (typename std::multiset<T>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          elements.insert(print(*i));
        }
        std::string result = boost::algorithm::join(elements, ",");
        if (print_separators)
        {
          result = "{" + result + "}";
        }
        return result;
      } 
       
      std::string print(const std::set<std::multiset<action_label> >& v) const
      {
        std::set<std::string> elements;
        for (std::set<std::multiset<action_label> >::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          elements.insert(print(*i));
        }
        return boost::algorithm::join(elements, "; ");
      }

      //--------------------------------------------//
      // parse functions
      //--------------------------------------------//     
      unsigned int parse_unsigned_int(std::string text) const
      {
        text = core::remove_whitespace(text);
        return boost::lexical_cast<unsigned int>(text);
      }

      std::set<std::string> parse_set_string(std::string text) const
      {
        std::vector<std::string> v;
        boost::algorithm::split(v, text, boost::algorithm::is_any_of(","));
        std::for_each(v.begin(), v.end(), boost::bind(boost::algorithm::trim<std::string>, _1, std::locale()));
        return std::set<std::string>(v.begin(), v.end());
      }
      
      std::set<std::multiset<std::string> > parse_set_multiset_string(std::string text) const
      {
        std::set<std::multiset<std::string> > result;

        std::set<std::string> multisets; 
        boost::algorithm::split(multisets, text, boost::algorithm::is_any_of(";"));
        for (std::set<std::string>::iterator i = multisets.begin(); i != multisets.end(); ++i)
        {
          std::string s = core::regex_replace("[{}]", "", *i);
          std::vector<std::string> v;
          boost::algorithm::split(v, s, boost::algorithm::is_any_of(","));
          std::for_each(v.begin(), v.end(), boost::bind(boost::algorithm::trim<std::string>, _1, std::locale()));
          result.insert(std::multiset<std::string>(v.begin(), v.end()));
        }
        return result;
      }

      //--------------------------------------------//
      // compare functions
      //--------------------------------------------//     
      /// \brief Compares two integers, and returns a message if they are different.
      /// If if they are equal the empty string is returned.
      std::string compare(std::string property, unsigned int x, unsigned int y) const
      {
        if (x != y)
        {
          std::ostringstream out;
          out << "Difference in property " << property << " detected: " << x << " versus " << y << "\n";
          return out.str();
        }
        return "";
      }

      /// \brief Compares two sets and returns a string with the differences encountered.
      /// Elements present in the first one but not in the second are printed with a '+'
      /// in front of it, elements present in the seconde but not in the first one with a '-'
      /// in front of it. A value x of the type T is printed using print(x), so this
      /// operation must be defined.
      /// If no differences are found the empty string is returned.
      template <typename T>
      std::string compare(std::string property, const std::set<T>& x, const std::set<T>& y) const
      {
        std::ostringstream out;
        
        // compute elements in x but not in y
        std::set<T> plus;
        std::set_difference(x.begin(), x.end(), y.begin(), y.end(), std::inserter(plus, plus.end()));

        // compute elements in y but not in x
        std::set<T> minus;
        std::set_difference(y.begin(), y.end(), x.begin(), x.end(), std::inserter(minus, minus.end()));

        if (!plus.empty() || !minus.empty())
        {
          out << "Difference in property " << property << " detected:";
          for (typename std::set<T>::iterator i = plus.begin(); i != plus.end(); ++i)
          {
            out << " +" << print(*i);
          }
          for (typename std::set<T>::iterator i = minus.begin(); i != minus.end(); ++i)
          {
            out << " -" << print(*i);
          }
          out << "\n";
          return out.str();
        }
        return "";
      }

      std::string compare_property(std::string property, std::string x, std::string y) const
      {
        if     (property == "summand_count"               ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "tau_summand_count"           ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "delta_summand_count"         ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "declared_free_variables"     ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "declared_free_variable_names") { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "declared_free_variable_count") { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "used_free_variables"         ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "used_free_variable_names"    ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "used_free_variable_count"    ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "process_parameters"          ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "process_parameter_names"     ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "process_parameter_count"     ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "declared_action_labels"      ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "declared_action_label_count" ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "used_action_labels"          ) { return compare(property, parse_set_string(x), parse_set_string(y)); }
        else if(property == "used_action_label_count"     ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }
        else if(property == "used_multi_actions"          ) { return compare(property, parse_set_multiset_string(x), parse_set_multiset_string(y)); }
        else if(property == "used_multi_action_count"     ) { return compare(property, parse_unsigned_int(x), parse_unsigned_int(y)); }        
        return "ERROR: unknown property " + property + " encountered!";
      }
      
      //--------------------------------------------//
      // compute functions
      //--------------------------------------------//     
      // TODO: the compute functions can be combined for efficiency
      std::set<std::multiset<action_label> > compute_used_multi_actions(const specification& spec) const
      {
        std::set<std::multiset<action_label> > result;
        summand_list summands = spec.process().summands();
        for(summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
        {
          if (i->is_delta())
          {
            continue;
          }
          action_list a = i->multi_action().actions();
          std::multiset<action_label> labels;
          for (action_list::iterator j = a.begin(); j != a.end(); ++j)
          {
            labels.insert(j->label());
          }
          result.insert(labels);
        }
        return result;
      }

      std::set<action_label> compute_used_action_labels(const specification& spec) const
      {
        std::set<action_label> result;
        summand_list summands = spec.process().summands();
        for(summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
        {
          if (i->is_delta())
          {
            continue;
          }
          action_list a = i->multi_action().actions();
          for (action_list::iterator j = a.begin(); j != a.end(); ++j)
          {
            result.insert(j->label());
          }
        }
        return result;
      }

      unsigned int compute_tau_summand_count(const specification& spec) const
      {
        unsigned int result = 0;
        summand_list summands = spec.process().summands();
        for(summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
        {
          if (i->is_tau())
          {
            result++;
          }
        }
        return result;
      }

      unsigned int compute_delta_summand_count(const specification& spec) const
      {
        unsigned int result = 0;
        summand_list summands = spec.process().summands();
        for(summand_list::iterator i = summands.begin(); i != summands.end(); ++i)
        {
          if (i->is_delta())
          {
            result++;
          }
        }
        return result;
      }

      std::set<data::variable> compute_declared_free_variables(const specification& spec) const
      {
        std::set<data::variable> result;
        result.insert(spec.process().free_variables().begin(), spec.process().free_variables().end());
        result.insert(spec.initial_process().free_variables().begin(), spec.initial_process().free_variables().end());
        return result;
      }

      std::set<data::variable> compute_used_free_variables(const specification& spec) const
      {
        return compute_free_variables(spec.process());
      }

      //--------------------------------------------//
      // miscellaneous functions
      //--------------------------------------------//     
      /// \brief Returns the maximum length of the property names
      unsigned int max_key_length() const
      {
        unsigned int result = 0;
        for (std::map<std::string, std::string>::const_iterator i = m_data.begin(); i != m_data.end(); ++i)
        {
          result = (std::max)(result, i->first.size());
        }
        return result;
      }

      std::string align(const std::string& s, unsigned int n) const
      {
        if (s.size() >= n)
        {
          return s;
        }
        return s + std::string(n - s.size(), ' ');
      }

      std::set<core::identifier_string> names(const std::set<data::variable>& v) const
      {
        std::set<core::identifier_string> result;
        for (std::set<data::variable>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          // TODO: change this when the interface of variable has been repaired
          result.insert(atermpp::arg1(*i));
        }
        return result;
      }
      
    public:
      /// \li summand_count                = NUMBER   
      /// \li tau_summand_count            = NUMBER
      /// \li delta_summand_count          = NUMBER
      /// \li declared_free_variables      = NAME:SORT; ... ; NAME:SORT
      /// \li declared_free_variable_names = NAME; ... ; NAME
      /// \li declared_variable_count      = NUMBER
      /// \li used_free_variables          = NAME:SORT; ... ; NAME:SORT
      /// \li used_free_variable_names     = NAME; ... ; NAME
      /// \li used_free_variable_count     = NUMBER
      /// \li process_parameters           = NAME:SORT; ... ; NAME:SORT
      /// \li process_parameter_names      = NAME; ... ; NAME
      /// \li process_parameter_count      = NUMBER
      /// \li declared_action_labels       = NAME; ... ; NAME
      /// \li declared_action_label_count  = NUMBER
      /// \li used_action_labels           = NAME; ... ; NAME
      /// \li used_action_label_count      = NUMBER
      /// \li used_multi_actions           = {NAME,...,NAME}; ... ; {NAME,...,NAME}
      /// \li used_multi_action_count      = NUMBER
      /// The strings may appear in a random order, and not all of them need to be present
      specification_property_map(const std::string& text)
      {
        std::vector<std::string> lines = core::split(text, "\n");
        for (std::vector<std::string>::iterator i = lines.begin(); i != lines.end(); ++i)
        {
          std::vector<std::string> words = core::split(*i, "=");
          if (words.size() == 2)
          {
            boost::trim(words[0]);
            boost::trim(words[1]);
            m_data[words[0]] = words[1];
          }
        }
      }

      /// \brief Constructor
      /// Initializes the specification_property_map with a linear process specification
      specification_property_map(const specification& spec)
      {
        unsigned int                           summand_count                = spec.process().summands().size();
        unsigned int                           tau_summand_count            = compute_tau_summand_count(spec);
        unsigned int                           delta_summand_count          = compute_delta_summand_count(spec);
        std::set<data::variable>               declared_free_variables      = compute_declared_free_variables(spec);
        unsigned int                           declared_free_variable_count = declared_free_variables.size();
        std::set<data::variable>               used_free_variables          = compute_used_free_variables(spec);
        unsigned int                           used_free_variable_count     = used_free_variables.size();
        std::set<data::variable>               process_parameters           = data::convert<std::set<data::variable> >(spec.process().process_parameters());
        unsigned int                           process_parameter_count      = process_parameters.size();
        std::set<action_label>                 declared_action_labels       = data::convert<std::set<action_label> >(spec.action_labels());
        unsigned int                           declared_action_label_count  = declared_action_labels.size();
        std::set<action_label>                 used_action_labels           = compute_used_action_labels(spec);
        unsigned int                           used_action_label_count      = used_action_labels.size();
        std::set<std::multiset<action_label> > used_multi_actions           = compute_used_multi_actions(spec);
        unsigned int                           used_multi_action_count      = used_multi_actions.size();

        m_data["summand_count"               ] = print(summand_count                 );
        m_data["tau_summand_count"           ] = print(tau_summand_count             );
        m_data["delta_summand_count"         ] = print(delta_summand_count           );
        m_data["declared_free_variables"     ] = print(declared_free_variables, false);
        m_data["declared_free_variable_names"] = print(names(declared_free_variables), false);
        m_data["declared_free_variable_count"] = print(declared_free_variable_count  );
        m_data["used_free_variables"         ] = print(used_free_variables, false    );
        m_data["used_free_variable_names"    ] = print(names(used_free_variables), false);
        m_data["used_free_variable_count"    ] = print(used_free_variable_count      );
        m_data["process_parameters"          ] = print(process_parameters, false     );
        m_data["process_parameter_names"     ] = print(names(process_parameters), false);
        m_data["process_parameter_count"     ] = print(process_parameter_count       );
        m_data["declared_action_labels"      ] = print(declared_action_labels, false );
        m_data["declared_action_label_count" ] = print(declared_action_label_count   );
        m_data["used_action_labels"          ] = print(used_action_labels, false     );
        m_data["used_action_label_count"     ] = print(used_action_label_count       );
        m_data["used_multi_actions"          ] = print(used_multi_actions            );
        m_data["used_multi_action_count"     ] = print(used_multi_action_count       );
      }
      
      /// \brief Returns a string representation of the properties
      std::string to_string() const
      {
        unsigned int n = max_key_length();
        std::vector<std::string> lines;         
        for (std::map<std::string, std::string>::const_iterator i = m_data.begin(); i != m_data.end(); ++i)
        {          
          lines.push_back(align(i->first, n) + " = " + i->second);
        }
        return boost::algorithm::join(lines, "\n");
      } 
      
      const std::map<std::string, std::string>& data() const
      {                       
        return m_data;
      }
       
      std::string compare(const specification_property_map& other) const
      {
        std::ostringstream out;
        for (std::map<std::string, std::string>::const_iterator i = m_data.begin(); i != m_data.end(); ++i)
        {
          std::map<std::string, std::string>::const_iterator j = other.data().find(i->first);
          if (j != other.data().end())
          {
            out << compare_property(i->first, i->second, j->second);
          }
        }
        return out.str();
      }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H
