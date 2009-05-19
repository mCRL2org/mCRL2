// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_info.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_INFO_H
#define MCRL2_LPS_DETAIL_LPS_INFO_H

#include <map>
#include <sstream>
#include <utility>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include "mcrl2/data/detail/convert.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Stores the following properties of a linear process specification:
  /// \li summand_count               The number of summands
  /// \li tau_summand_count           The number of tau summands
  /// \li delta_summand_count         The number of delta summands
  /// \li declared_free_variables     The declared free variables (names + sorts)
  /// \li declared_variable_count     The number of declared free variables
  /// \li used_free_variables         The used free variables (names + sorts)
  /// \li used_free_variable_count    The number of used free variables
  /// \li process_parameters          The process parameters (names + sorts)
  /// \li process_parameter_count     The number of process parameters
  /// \li declared_action_labels      The declared action labels (names)
  /// \li declared_action_label_count The number of declared action labels
  /// \li used_action_labels          The used action labels (names)
  /// \li used_action_label_count     The number of used action labels
  /// \li used_multi_actions          The used multi-actions (sets of label names)
  /// \li used_multi_action_count     The number of used multi-actions
  class lps_info
  {
    protected:
      /// \brief Contains a normalized string representation of the properties.
      std::map<std::string, std::string> m_data;

      std::string print_line(unsigned int n)
      {
        std::ostringstream out;
        out << n;
        return out.str();
      }

      std::string print_line(const std::set<data::variable>& v) const
      {
        std::set<std::string> words;
        for (std::set<data::variable>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          words.insert(pp(*i));
        }
        return boost::algorithm::join(words, "; ");
      }

      std::string print_line(const std::set<action_label>& v) const
      {
        std::set<std::string> words;
        for (std::set<action_label>::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          words.insert(pp(i->name()));
        }
        return boost::algorithm::join(words, "; ");
      }

      std::string print_line(const std::set<std::multiset<action_label> >& v) const
      {
        std::set<std::string> word_sets;
        for (std::set<std::multiset<action_label> >::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          std::multiset<std::string> words;
          for (std::multiset<action_label>::const_iterator j = i->begin(); j != i->end(); ++j)
          {
            words.insert(pp(j->name()));
          }
          word_sets.insert(boost::algorithm::join(words, ","));
        }
        return boost::algorithm::join(word_sets, "; ");
      }

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

      // TODO: combine the functions below for efficiency (?)
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

      std::string align(const std::string& s, unsigned int n) const
      {
        if (s.size() >= n)
        {
          return s;
        }
        return s + std::string(n - s.size(), ' ');
      }

    public:
      /// \li summand_count               = NUMBER
      /// \li tau_summand_count           = NUMBER
      /// \li delta_summand_count         = NUMBER
      /// \li declared_free_variables     = NAME:SORT; ... ; NAME:SORT
      /// \li declared_variable_count     = NUMBER
      /// \li used_free_variables         = NAME:SORT; ... ; NAME:SORT
      /// \li used_free_variable_count    = NUMBER
      /// \li process_parameters          = NAME:SORT; ... ; NAME:SORT
      /// \li process_parameter_count     = NUMBER
      /// \li declared_action_labels      = NAME; ... ; NAME
      /// \li declared_action_label_count = NUMBER
      /// \li used_action_labels          = NAME; ... ; NAME
      /// \li used_action_label_count     = NUMBER
      /// \li used_multi_actions          = {NAME,...,NAME}; ... ; {NAME,...,NAME}
      /// \li used_multi_action_count     = NUMBER
      /// The strings may appear in a random order, and not all of them need to be present
      lps_info(const std::string& text)
      {
      }

      /// \brief Constructor
      /// Initializes the lps_info with a linear process specification
      lps_info(const specification& spec)
      {
        unsigned int                           summand_count                = spec.process().summands().size();
        unsigned int                           tau_summand_count            = compute_tau_summand_count(spec);
        unsigned int                           delta_summand_count          = compute_delta_summand_count(spec);
        // TODO: add this when the free variables are properly integrated in a specification
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

        m_data["summand_count"               ] = print_line(summand_count               );
        m_data["tau_summand_count"           ] = print_line(tau_summand_count           );
        m_data["delta_summand_count"         ] = print_line(delta_summand_count         );
        m_data["declared_free_variables"     ] = print_line(declared_free_variables     );
        m_data["declared_free_variable_count"] = print_line(declared_free_variable_count);
        m_data["used_free_variables"         ] = print_line(used_free_variables         );
        m_data["used_free_variable_count"    ] = print_line(used_free_variable_count    );
        m_data["process_parameters"          ] = print_line(process_parameters          );
        m_data["process_parameter_count"     ] = print_line(process_parameter_count     );
        m_data["declared_action_labels"      ] = print_line(declared_action_labels      );
        m_data["declared_action_label_count" ] = print_line(declared_action_label_count );
        m_data["used_action_labels"          ] = print_line(used_action_labels          );
        m_data["used_action_label_count"     ] = print_line(used_action_label_count     );
        m_data["used_multi_actions"          ] = print_line(used_multi_actions          );
        m_data["used_multi_action_count"     ] = print_line(used_multi_action_count     );
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
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_INFO_H
