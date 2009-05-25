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
#include "mcrl2/data/detail/specification_property_map.h"
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
  class specification_property_map : protected mcrl2::data::detail::specification_property_map< specification_property_map >
  {
    protected:

      // Allow base class access to protected functions
      friend class data::detail::specification_property_map< specification_property_map >;

      typedef data::detail::specification_property_map< specification_property_map > part;

      using part::print;

      std::string print(const action_label l) const
      {
        return core::pp(l.name());
      }

      std::string print(const action& a) const
      {
        return core::pp(a);
      }

      std::string print(const deadlock& d) const
      {
        return "<pp(deadlock) Not yet implemented>";
      }

      std::string print(const multi_action& m) const
      {
        return "<pp(multi_action) Not yet implemented>";
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

      // part class compare functions
      using part::compare;

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

      using part::to_string;
      using part::data;

      std::string compare(const specification_property_map& other) const
      {
        return part::compare(other);
      }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H
