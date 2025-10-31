// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/lts_builder.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_BUILDER_H
#define MCRL2_LTS_BUILDER_H

#include "mcrl2/lps/explorer.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_io.h"

namespace mcrl2::lts {

/// \brief Removes the last element from state s
inline
lps::state remove_time_stamp(const lps::state& s)
{
  return lps::state(s.begin(), s.size() - 1);
}

struct lts_builder
{
  using indexed_set_for_states_type = atermpp::indexed_set<lps::state, mcrl2::utilities::detail::GlobalThreadSafe>;
  // All LTS classes use integers to represent actions in transitions. A mapping from actions to integers
  // is needed to avoid duplicates.
  utilities::unordered_map_large<lps::multi_action, std::size_t> m_actions;

  lts_builder()
  {
    lps::multi_action tau(process::action_list(), data::undefined_real());
    m_actions.emplace(std::make_pair(tau, m_actions.size()));
  }

  std::size_t add_action(const lps::multi_action& a)
  {
    lps::multi_action sorted_multi_action(a.sort_actions());
    auto i = m_actions.find(sorted_multi_action);
    if (i == m_actions.end())
    {
      i = m_actions.emplace(std::make_pair(sorted_multi_action, m_actions.size())).first;
    }
    return i->second;
  }

  // Add a transition to the LTS
  virtual void
  add_transition(std::size_t from, const lps::multi_action& a, std::size_t to, std::size_t number_of_threads = 0)
    = 0;

  // Add actions and states to the LTS
  virtual void finalize(const indexed_set_for_states_type& state_map, bool timed) = 0;

  // Save the LTS to a file
  virtual void save(const std::string& filename) = 0;

  virtual ~lts_builder() = default;
};

class lts_none_builder: public lts_builder
{
  public:
    void add_transition(std::size_t /* from */, const lps::multi_action& /* a */, std::size_t /* to */, const std::size_t /* number_of_threads */) override
    {}

    void finalize(const indexed_set_for_states_type& /* state_map */, bool /* timed */) override
    {}

    void save(const std::string& /* filename */) override
    {}
};

class lts_aut_builder: public lts_builder
{
  protected:
    lts_aut_t m_lts;
    std::mutex m_exclusive_transition_access;

  public:
    lts_aut_builder() = default;

    const lts_aut_t& lts() const
    {
      return m_lts;
    }

    lts_aut_t& lts()
    {
      return m_lts;
    }

    void add_transition(std::size_t from, const lps::multi_action& a, std::size_t to, const std::size_t number_of_threads) override
    {
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.lock();
      }
      std::size_t label = add_action(a);
      m_lts.add_transition(transition(from, label, to));
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.unlock();
      }
    }

    // Add actions and states to the LTS
    void finalize(const indexed_set_for_states_type& state_map, bool /* timed */) override
    {
      // add actions
      m_lts.set_num_action_labels(m_actions.size());
      for (const auto& p: m_actions)
      {
        m_lts.set_action_label(p.second, action_label_string(lps::pp(p.first)));
      }

      m_lts.set_num_states(state_map.size());

      if (state_map.size() > 0)
      {
        m_lts.set_initial_state(0);
      }
    }

    void save(const std::string& filename) override
    {
      m_lts.save(filename);
    }
};

// Write transitions immediately to disk, and add the AUT header later.
class lts_aut_disk_builder: public lts_builder
{
  protected:
    std::ofstream out;
    std::size_t m_transition_count = 0;
    std::mutex m_exclusive_transition_access;

  public:
    explicit lts_aut_disk_builder(const std::string& filename)
    {
      mCRL2log(log::verbose) << "writing state space in AUT format to '" << filename << "'." << std::endl;
      out.open(filename.c_str());
      if (!out.is_open())
      {
        throw mcrl2::runtime_error("cannot open '" + filename + "' for writing");
      }
      out << "des                                                \n"; // write a dummy header that will be overwritten
    }

    void add_transition(std::size_t from, const lps::multi_action& a, std::size_t to, const std::size_t number_of_threads) override
    {
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.lock();
      }
      m_transition_count++;
      out << "(" << from << ",\"" << lps::pp(a) << "\"," << to << ")\n";
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.unlock();
      }
    }

    // Add actions and states to the LTS
    void finalize(const indexed_set_for_states_type& state_map, bool /* timed */) override
    {
      assert(!out.fail());
      out.flush();
      out.seekp(0);
      if (out.fail())
      {
        throw mcrl2::runtime_error("seeking is not supported by the output stream");
      }
      out << "des (0," << m_transition_count << "," << state_map.size() << ")";
      out.close();
    }

    void save(const std::string& /* filename */) override
    { }
};

class lts_lts_builder: public lts_builder
{
  protected:
    lts_lts_t m_lts;
    bool m_discard_state_labels = false;
    std::mutex m_exclusive_transition_access;

  public:
    lts_lts_builder(
      const data::data_specification& dataspec,
      const process::action_label_list& action_labels,
      const data::variable_list& process_parameters,
      bool discard_state_labels = false
    )
     : m_discard_state_labels(discard_state_labels)
    {
      m_lts.set_data(dataspec);
      m_lts.set_process_parameters(process_parameters);
      m_lts.set_action_label_declarations(action_labels);
    }

    void add_transition(std::size_t from, const lps::multi_action& a, std::size_t to, const std::size_t number_of_threads) override
    {
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.lock();
      }
      std::size_t label = add_action(a);
      m_lts.add_transition(transition(from, label, to));
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.unlock();
      }
    }

    // Add actions and states to the LTS
    void finalize(const indexed_set_for_states_type& state_map, bool timed) override
    {
      // add actions
      m_lts.set_num_action_labels(m_actions.size());
      for (const auto& p: m_actions)
      {
        m_lts.set_action_label(p.second, action_label_lts(lps::multi_action(p.first.actions(), p.first.time())));
      }

      // add state labels
      if (!m_discard_state_labels)
      {
        std::size_t n = state_map.size();
        std::vector<state_label_lts> state_labels(n);
        for (std::size_t i = 0; i < n; i++)
        {
          if (timed)
          {
            state_labels[i] = state_label_lts(remove_time_stamp(state_map[i]));
          }
          else
          {
            state_labels[i] = state_label_lts(state_map[i]);
          }
        }
        m_lts.state_labels() = std::move(state_labels);
      }

      m_lts.set_num_states(state_map.size(), true);
      m_lts.set_initial_state(0);
    }

    void save(const std::string& filename) override
    {
      m_lts.save(filename);
    }
};

class lts_lts_disk_builder: public lts_builder
{
  protected:
    std::fstream fstream;
    std::unique_ptr<atermpp::binary_aterm_ostream> stream;
    bool m_discard_state_labels = false;
    std::mutex m_exclusive_transition_access;

  public:
    lts_lts_disk_builder(
      const std::string& filename,
      const data::data_specification& dataspec,
      const process::action_label_list& action_labels,
      const data::variable_list& process_parameters,
      bool discard_state_labels = false
    )
     : m_discard_state_labels(discard_state_labels)
    {
      bool to_stdout = filename.empty() || filename == "-";
      if (!to_stdout)
      {
        fstream.open(filename, std::ofstream::out | std::ofstream::binary);
        if (fstream.fail())
        {
          throw mcrl2::runtime_error("Fail to open file " + filename + " for writing.");
        }

        mCRL2log(log::verbose) << "writing state space in LTS format to '" << filename << "'." << std::endl;
      }
      stream = std::make_unique<atermpp::binary_aterm_ostream>(to_stdout ? std::cout : fstream);

      mcrl2::lts::write_lts_header(*stream, dataspec, process_parameters, action_labels);
    }

    void add_transition(std::size_t from, const lps::multi_action& a, std::size_t to, const std::size_t number_of_threads) override
    {
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.lock();
      }
      write_transition(*stream, from, a, to);
      if (mcrl2::utilities::detail::GlobalThreadSafe && number_of_threads > 1)
      {
        m_exclusive_transition_access.unlock();
      }
    }

    // Add actions and states to the LTS
    void finalize(const indexed_set_for_states_type& state_map, bool timed) override
    {
      if (!m_discard_state_labels)
      {
        // Write the state labels in the order of their indices.
        for (std::size_t i = 0; i < state_map.size(); i++)
        {
          if (is_aterm_balanced_tree(state_map[i]))  // in a parallel context not all positions may be filled.
          {
            if (timed)
            {
              write_state_label(*stream, state_label_lts(remove_time_stamp(state_map[i])));
            }
            else
            {
              write_state_label(*stream, state_label_lts(state_map[i]));
            }
          }
        }
      }

      // Write the initial state.
      write_initial_state(*stream, 0);
    }

    void save(const std::string&) override {}
};

class lts_dot_builder: public lts_lts_builder
{
  public:
    using super = lts_lts_builder;
    lts_dot_builder(const data::data_specification& dataspec, const process::action_label_list& action_labels, const data::variable_list& process_parameters)
      : super(dataspec, action_labels, process_parameters)
    { }

    void save(const std::string& filename) override
    {
      lts_dot_t dot;
      detail::lts_convert(m_lts, dot);
      dot.save(filename);
    }
};

class lts_fsm_builder: public lts_lts_builder
{
  public:
    using super = lts_lts_builder;
    lts_fsm_builder(const data::data_specification& dataspec, const process::action_label_list& action_labels, const data::variable_list& process_parameters)
      : super(dataspec, action_labels, process_parameters)
    { }

    void save(const std::string& filename) override
    {
      lts_fsm_t fsm;
      detail::lts_convert(m_lts, fsm);
      fsm.save(filename);
    }
};

inline
std::unique_ptr<lts_builder> create_lts_builder(const lps::specification& lpsspec, const lps::explorer_options& options, lts_type output_format, const std::string& output_filename = "")
{
  switch (output_format)
  {
    case lts_aut:
    {
      if (options.save_at_end)
      {
        return std::make_unique<lts_aut_builder>();
      }
      else
      {
        return std::make_unique<lts_aut_disk_builder>(output_filename);
      }
    }
    case lts_dot: return std::make_unique<lts_dot_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters());
    case lts_fsm: return std::make_unique<lts_fsm_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters());
    case lts_lts:
    {
      if (options.save_at_end)
      {
        return std::make_unique<lts_lts_builder>(lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters(), options.discard_lts_state_labels);
      }
      else
      {
        return std::make_unique<lts_lts_disk_builder>(output_filename, lpsspec.data(), lpsspec.action_labels(), lpsspec.process().process_parameters(), options.discard_lts_state_labels);
      }
    }
    default: return std::make_unique<lts_none_builder>();
  }
}

} // namespace mcrl2::lts



#endif // MCRL2_LTS_BUILDER_H
