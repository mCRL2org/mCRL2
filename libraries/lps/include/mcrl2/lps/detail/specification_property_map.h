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
// #include <boost/algorithm/string/join.hpp> Don't use this, it leads to stack overflows with Visual C++ 9.0 express
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/atermpp/convert.h"
#include "mcrl2/data/detail/data_property_map.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Stores the following properties of a linear process specification:
/// <table>
/// <tr><th>property                    </th><th>description                                 </th><th>format                                </th></tr>
/// <tr><td>summand_count               </td><td>The number of summands                      </td><td>NUMBER                                </td></tr>
/// <tr><td>tau_summand_count           </td><td>The number of tau summands                  </td><td>NUMBER                                </td></tr>
/// <tr><td>delta_summand_count         </td><td>The number of delta summands                </td><td>NUMBER                                </td></tr>
/// <tr><td>declared_free_variables     </td><td>The declared free variables                 </td><td>NAME:SORT; ... ; NAME:SORT            </td></tr>
/// <tr><td>declared_free_variable_names</td><td>The names of the declared free variables    </td><td>NAME; ... ; NAME                      </td></tr>
/// <tr><td>declared_variable_count     </td><td>The number of declared free variables       </td><td>NUMBER                                </td></tr>
/// <tr><td>used_free_variables         </td><td>The used free variables                     </td><td>NAME:SORT; ... ; NAME:SORT            </td></tr>
/// <tr><td>used_free_variables_names   </td><td>The names of the used free variables        </td><td>NAME; ... ; NAME                      </td></tr>
/// <tr><td>used_free_variable_count    </td><td>The number of used free variables           </td><td>NUMBER                                </td></tr>
/// <tr><td>process_parameters          </td><td>The process parameters                      </td><td>NAME:SORT; ... ; NAME:SORT            </td></tr>
/// <tr><td>process_parameter_names     </td><td>The names of the process parameters         </td><td>NAME; ... ; NAME                      </td></tr>
/// <tr><td>process_parameter_count     </td><td>The number of process parameters            </td><td>NUMBER                                </td></tr>
/// <tr><td>declared_action_labels      </td><td>The names of the declared action labels     </td><td>NAME; ... ; NAME                      </td></tr>
/// <tr><td>declared_action_label_count </td><td>The number of declared action labels        </td><td>NUMBER                                </td></tr>
/// <tr><td>used_action_labels          </td><td>The names of the used action labels         </td><td>NAME; ... ; NAME                      </td></tr>
/// <tr><td>used_action_label_count     </td><td>The number of used action labels            </td><td>NUMBER                                </td></tr>
/// <tr><td>used_multi_actions          </td><td>The used multi-actions (sets of label names)</td><td>{NAME,...,NAME}; ... ; {NAME,...,NAME}</td></tr>
/// <tr><td>used_multi_action_count     </td><td>The number of used multi-actions            </td><td>NUMBER
/// </table>
class specification_property_map : protected mcrl2::data::detail::data_property_map< specification_property_map >
{
  protected:

    // Allow base class access to protected functions
    friend class data::detail::data_property_map< specification_property_map >;

    typedef data::detail::data_property_map< specification_property_map > super;

    using super::print;

    std::string print(const action_label l) const
    {
      return core::pp(l.name());
    }

    std::string print(const action& a) const
    {
      return lps::pp(a);
    }

    std::string print(const deadlock& x) const
    {
      return lps::pp(x);
    }

    std::string print(const multi_action& x) const
    {
      return lps::pp(x);
    }

    std::string print(const std::set<std::multiset<action_label> >& v) const
    {
      std::set<std::string> elements;
      for (std::set<std::multiset<action_label> >::const_iterator i = v.begin(); i != v.end(); ++i)
      {
        elements.insert(print(*i));
      }
      return utilities::string_join(elements, "; ");
    }

    // super class compare functions
    using super::compare;

    std::string compare_property(std::string property, std::string x, std::string y) const
    {
      if (property == "summand_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "tau_summand_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "delta_summand_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "declared_free_variables")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "declared_free_variable_names")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "declared_free_variable_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "used_free_variables")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "used_free_variable_names")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "used_free_variable_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "process_parameters")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "process_parameter_names")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "process_parameter_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "declared_action_labels")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "declared_action_label_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "used_action_labels")
      {
        return compare(property, parse_set_string(x), parse_set_string(y));
      }
      else if (property == "used_action_label_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      else if (property == "used_multi_actions")
      {
        return compare(property, parse_set_multiset_string(x), parse_set_multiset_string(y));
      }
      else if (property == "used_multi_action_count")
      {
        return compare(property, parse_unsigned_int(x), parse_unsigned_int(y));
      }
      return "ERROR: unknown property " + property + " encountered!";
    }

    //--------------------------------------------//
    // compute functions
    //--------------------------------------------//
    // TODO: the compute functions can be combined for efficiency
    std::set<std::multiset<action_label> > compute_used_multi_actions(const specification& spec) const
    {
      std::set<std::multiset<action_label> > result;
      for (action_summand_vector::const_iterator i = spec.process().action_summands().begin(); i != spec.process().action_summands().end(); ++i)
      {
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
      for (action_summand_vector::const_iterator i = spec.process().action_summands().begin(); i != spec.process().action_summands().end(); ++i)
      {
        action_list a = i->multi_action().actions();
        for (action_list::iterator j = a.begin(); j != a.end(); ++j)
        {
          result.insert(j->label());
        }
      }
      return result;
    }

    size_t compute_tau_summand_count(const specification& spec) const
    {
      size_t result = 0;
      const action_summand_vector& summands = spec.process().action_summands();
      for (action_summand_vector::const_iterator i = summands.begin(); i != summands.end(); ++i)
      {
        if (i->is_tau())
        {
          result++;
        }
      }
      return result;
    }

    std::set<data::variable> compute_used_free_variables(const specification& spec) const
    {
      return lps::find_free_variables(spec.process());
    }

  public:
    /// \brief Constructor.
    /// The strings may appear in a random order, and not all of them need to be present
    specification_property_map(const std::string& text)
      : super(text)
    {}

    /// \brief Constructor
    /// Initializes the specification_property_map with a linear process specification
    specification_property_map(const specification& spec)
    {
      size_t                                 summand_count           = spec.process().summand_count();
      size_t                                 tau_summand_count       = compute_tau_summand_count(spec);
      size_t                                 delta_summand_count     = spec.process().deadlock_summands().size();
      std::set<data::variable>               declared_free_variables = spec.global_variables();
      std::set<data::variable>               used_free_variables     = compute_used_free_variables(spec);
      std::set<data::variable>               process_parameters      = atermpp::convert<std::set<data::variable> >(spec.process().process_parameters());
      std::set<action_label>                 declared_action_labels  = atermpp::convert<std::set<action_label> >(spec.action_labels());
      std::set<action_label>                 used_action_labels      = compute_used_action_labels(spec);
      std::set<std::multiset<action_label> > used_multi_actions      = compute_used_multi_actions(spec);

      m_data["summand_count"               ] = print(summand_count);
      m_data["tau_summand_count"           ] = print(tau_summand_count);
      m_data["delta_summand_count"         ] = print(delta_summand_count);
      m_data["declared_free_variables"     ] = print(declared_free_variables, false);
      m_data["declared_free_variable_names"] = print(names(declared_free_variables), false);
      m_data["declared_free_variable_count"] = print(declared_free_variables.size());
      m_data["used_free_variables"         ] = print(used_free_variables, false);
      m_data["used_free_variable_names"    ] = print(names(used_free_variables), false);
      m_data["used_free_variable_count"    ] = print(used_free_variables.size());
      m_data["process_parameters"          ] = print(process_parameters, false);
      m_data["process_parameter_names"     ] = print(names(process_parameters), false);
      m_data["process_parameter_count"     ] = print(process_parameters.size());
      m_data["declared_action_labels"      ] = print(declared_action_labels, false);
      m_data["declared_action_label_count" ] = print(declared_action_labels.size());
      m_data["used_action_labels"          ] = print(used_action_labels, false);
      m_data["used_action_label_count"     ] = print(used_action_labels.size());
      m_data["used_multi_actions"          ] = print(used_multi_actions);
      m_data["used_multi_action_count"     ] = print(used_multi_actions.size());
    }

    using super::to_string;
    using super::data;
    using super::operator[];

    std::string compare(const specification_property_map& other) const
    {
      return super::compare(other);
    }

    /// \brief Returns a textual overview of some properties of an LPS
    std::string info() const
    {
      std::ostringstream out;
      out << "Number of summands                  : " << (*this)["summand_count"               ] << "\n";
      out << "Number of tau-summands              : " << (*this)["tau_summand_count"           ] << "\n";
      out << "Number of declared global variables : " << (*this)["declared_free_variable_count"] << "\n";
      out << "Number of process parameters        : " << (*this)["process_parameter_count"     ] << "\n";
      out << "Number of declared action labels    : " << (*this)["declared_action_label_count" ] << "\n";
      out << "Number of used actions              : " << (*this)["used_action_label_count"     ] << "\n";
      out << "Number of used multi-actions        : " << (*this)["used_multi_action_count"     ] << "\n";
      return out.str();
    }
};

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H
