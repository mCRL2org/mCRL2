// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/specification_property_map.h
/// \brief A property map containing properties of an LPS specification.

#ifndef MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H
#define MCRL2_LPS_DETAIL_SPECIFICATION_PROPERTY_MAP_H

#include "mcrl2/data/detail/data_property_map.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/stochastic_linear_process.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Stores the following properties of a linear process specification:
/// <table>
/// <tr><th>property                    </th><th>description                                 </th><th>format                                </th></tr>
/// <tr><td>action_summand_count        </td><td>The number of action summands               </td><td>NUMBER                                </td></tr>
/// <tr><td>tau_summand_count           </td><td>The number of tau summands                  </td><td>NUMBER                                </td></tr>
/// <tr><td>delta_summand_count         </td><td>The number of delta/deadlock summands       </td><td>NUMBER                                </td></tr>
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
template <typename Specification = specification>
class specification_property_map: protected mcrl2::data::detail::data_property_map<specification_property_map<Specification> >
{
  protected:

    // Allow base class access to protected functions
    friend class data::detail::data_property_map<specification_property_map<Specification> >;

    typedef data::detail::data_property_map<specification_property_map> super;

    using super::m_data;
    using super::names;
    using super::print;
    using super::parse_unsigned_int;
    using super::parse_set_string;
    using super::parse_set_multiset_string;

    std::string print(const process::action_label& l) const
    {
      return core::pp(l.name());
    }

    std::string print(const process::action& a) const
    {
      return process::pp(a);
    }

    std::string print(const deadlock& x) const
    {
      return lps::pp(x);
    }

    std::string print(const multi_action& x) const
    {
      return lps::pp(x);
    }

    std::string print(const std::set<std::multiset<process::action_label> >& v) const
    {
      std::set<std::string> elements;
      for (const auto& s: v)
      {
        elements.insert(print(s));
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
    std::set<std::multiset<process::action_label> > compute_used_multi_actions(const Specification& spec) const
    {
      std::set<std::multiset<process::action_label> > result;
      for (auto i = spec.process().action_summands().begin(); i != spec.process().action_summands().end(); ++i)
      {
        std::multiset<process::action_label> labels;
        for (const process::action& a: i->multi_action().actions())
        {
          labels.insert(a.label());
        }
        result.insert(labels);
      }
      return result;
    }

    std::set<process::action_label> compute_used_action_labels(const Specification& spec) const
    {
      std::set<process::action_label> result;
      for (auto i = spec.process().action_summands().begin(); i != spec.process().action_summands().end(); ++i)
      {
        for (const process::action& a: i->multi_action().actions())
        {
          result.insert(a.label());
        }
      }
      return result;
    }

    std::size_t compute_tau_summand_count(const Specification& spec) const
    {
      std::size_t result = 0;
      auto const& summands = spec.process().action_summands();
      for (auto i = summands.begin(); i != summands.end(); ++i)
      {
        if (i->is_tau())
        {
          result++;
        }
      }
      return result;
    }

    std::set<data::variable> compute_used_free_variables(const Specification& spec) const
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
    specification_property_map(const Specification& spec)
    {
      std::size_t                            summand_count           = spec.process().summand_count();
      std::size_t                            action_summand_count    = spec.process().action_summands().size();
      std::size_t                            tau_summand_count       = compute_tau_summand_count(spec);
      std::size_t                            delta_summand_count     = spec.process().deadlock_summands().size();
      const std::set<data::variable>&               declared_free_variables = spec.global_variables();
      std::set<data::variable>               used_free_variables     = compute_used_free_variables(spec);
      auto const&                            params                  = spec.process().process_parameters();
      std::set<data::variable>               process_parameters(params.begin(), params.end());
      auto const&                            action_labels = spec.action_labels();
      std::set<process::action_label>        declared_action_labels(action_labels.begin(),action_labels.end());
      std::set<process::action_label>        used_action_labels      = compute_used_action_labels(spec);
      auto                                   used_multi_actions      = compute_used_multi_actions(spec);

      m_data["summand_count"               ] = print(summand_count);
      m_data["action_summand_count"        ] = print(action_summand_count);
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

    std::string compare(const specification_property_map<Specification>& other) const
    {
      return super::compare(other);
    }

    /// \brief Returns a textual overview of some properties of an LPS
    std::string info() const
    {
      std::ostringstream out;
      out << "Number of action summands           : " << (*this)["action_summand_count"        ] << "\n";
      out << "Number of deadlock/delta summands   : " << (*this)["delta_summand_count"         ] << "\n";
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
