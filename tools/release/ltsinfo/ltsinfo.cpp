// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsinfo.cpp

#define NAME "ltsinfo"
#define AUTHOR "Muck van Weerdenburg"

#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_algorithm.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::log;

using ltsinfo_base = input_tool;
class ltsinfo_tool : public ltsinfo_base
{

  private:

    std::string                 infilename;
    mcrl2::lts::lts_type intype = mcrl2::lts::lts_none;
    bool print_action_labels = false;
    bool print_state_labels = false;
    bool print_branching_factor = false;

  public:

    ltsinfo_tool() :
      ltsinfo_base(NAME,AUTHOR,
                   "display basic information about an LTS",
                   "Print information about the labelled transition system (LTS) in INFILE. "
                   "If INFILE is not supplied, stdin is used.\n"
                   "\n"
                   "The format of INFILE is determined by its contents. "
                   "The option --in can be used to force the format for INFILE. "
                   "The supported formats are:\n"
                   +mcrl2::lts::detail::supported_lts_formats_text()
                  )
    {}

  protected:

    void add_options(interface_description& desc) override
    {
      using namespace mcrl2::lts;

      ltsinfo_base::add_options(desc);

      desc.
      add_option("action-label",
                 "print the labels of actions",'a').
      add_option("in", make_mandatory_argument("FORMAT"),
                 "use FORMAT as the input format", 'i').
      add_option("state-label",
                 "print the labels of states",'l').
      add_option("branching-factor",
                 "print the average, minimal and maximal branching factor",'b');
    }

    void parse_options(const command_line_parser& parser) override
    {
      using namespace mcrl2::lts;

      ltsinfo_base::parse_options(parser);

      if (0 < parser.arguments.size())
      {
        infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size())
      {
        parser.error("Too many file arguments.");
      }

      if (parser.options.count("in"))
      {
        if (1 < parser.options.count("in"))
        {
          parser.error("Multiple input formats specified; can only use one.");
        }

        intype = mcrl2::lts::detail::parse_format(parser.option_argument("in"));
        if (intype == lts_none || intype == lts_dot)
        {
          parser.error("Option -i/--in has illegal argument '" +
                       parser.option_argument("in") + "'.");
        }
      }

      print_action_labels = parser.options.count("action-label") > 0;
      print_state_labels = parser.options.count("state-label") > 0;
      print_branching_factor = parser.options.count("branching-factor") > 0;
    }

    template <class SL, class AL, class BASE>
    static void provide_probabilistic_information(const mcrl2::lts::lts<SL,AL,BASE>& )
    {
      // No probabilistic information is provided for a plain lts.
    }

    template <class SL, class AL, class PROBABILISTIC_STATE, class BASE>
    static void provide_probabilistic_information(mcrl2::lts::probabilistic_lts < SL, AL, PROBABILISTIC_STATE, BASE>&  l)
    {
      std::size_t count_non_trivial_probabilistic_states=0;
      if (l.initial_probabilistic_state().size()>1)
      {
        count_non_trivial_probabilistic_states++;
      }
      for(std::size_t i=0; i<l.num_probabilistic_states(); ++i)
      {
        if (l.probabilistic_state(i).size()>1)
        {
          count_non_trivial_probabilistic_states++;
        }
      }
      if (count_non_trivial_probabilistic_states>0)
      { 
        // The initial state can be probabilistic, so it is added separately. 
        mCRL2log(info) << "This lts has " << l.num_probabilistic_states()+1 << " probabilistic states.\n";
        mCRL2log(info) << "Out of these " << count_non_trivial_probabilistic_states << " contain" << ((count_non_trivial_probabilistic_states<2)?"s":"") 
                       << " a non-trivial probability distribution.\n";
        mCRL2log(info) << "The initial state is " << ((l.initial_probabilistic_state().size()>1)?"":"not ") << "probabilistic.\n";
      }
      else
      {
        mCRL2log(info) << "This lts has no probabilistic states.\n";
      }

    }

    template<typename T>
    static bool is_even(T t)
    {
      return t % 2 == 0;
    }

    template<typename LTS>
    void print_the_action_labels(const LTS& l) const
    {
      if (!print_action_labels) { return; }

      mCRL2log(info) << "The action labels of this transition system: \n";
      for (auto& action_label : l.action_labels())
      {
         mCRL2log(info) << action_label << "\n";
      }
    }

    /// \brief Prints the min, max, median and average of the branching factor for the given LTS.
    template<typename LTS>
    void print_the_branching_factor(const LTS& lts) const
    {
      if (!print_branching_factor) { return; }

      // Count the number of transitions outgoing from each state
      std::vector<std::uint64_t> branching_factor(lts.num_states());
      for (auto& transition : lts.get_transitions())
      {
        ++branching_factor[transition.from()];
      }

      // Sort the counts to obtain min, max and median.
      std::sort(branching_factor.begin(), branching_factor.end());
      std::uint64_t min = branching_factor.front();
      std::uint64_t max = branching_factor.back();

      double median = static_cast<double>(branching_factor[branching_factor.size() / 2]);
      if (is_even(branching_factor.size()))
      {
        // For even number of observations is even we average the observations left and right of the middle,
        // because indices start at zero the left observation is located at n / 2 - 1.
        median = static_cast<double>(branching_factor[branching_factor.size() / 2 - 1] + branching_factor[branching_factor.size() / 2]) / 2.0;
      }

      // Calculate the average, (sum i from zero to n of i) divided by n or equivalently sum i from zero to n of i/n.
      double average_branching_factor = 0;
      for (auto& factor : branching_factor)
      {
        average_branching_factor += static_cast<double>(factor) / lts.num_states();
      }

      // Print the results.
      mCRL2log(info) << "The branching factor is min: " << min
        << ", max: " << max
        << ", median: " << median
        << " and average: " << average_branching_factor << "\n";
    }

    // Code to print the state labels. There is a specialisation for an probabilistic_lts_lts_t.
    void print_the_state_labels(const mcrl2::lts::probabilistic_lts_fsm_t& l) const
    {
      if (print_state_labels)
      {
        if (!l.has_state_info())
        {
          mCRL2log(info) << "This transition system has no state labels. Therefore they cannot be printed.\n";
        }
        else 
        {
          mCRL2log(info) << "The state labels of this .fsm format. Note that state labels in .fsm files are only partly preserved by state space reductions.\n";
          for(std::size_t i=0; i<l.num_states(); ++i)
          {
            mCRL2log(info) << i << ": " << pp(l.state_label(i)) << "\n";
          }
        }
      }
    }

    // Code to print the state labels. There is a specialisation for an probabilistic_lts_lts_t.
    void print_the_state_labels(const mcrl2::lts::probabilistic_lts_aut_t& ) const
    {
      if (print_state_labels)
      {
        mCRL2log(info) << "Transition systems in .aut format have no state labels. Therefore, they cannot be listed.\n"; 
      }
    }

    // Print the state labels for a probabilistic lts.
    void print_the_state_labels(const mcrl2::lts::probabilistic_lts_lts_t& l) const
    {
      if (print_state_labels)
      {
        if (!l.has_state_info())
        {
          mCRL2log(info) << "This transition system has no state labels. Therefore they cannot be printed.\n";
        }
        else 
        {
          mCRL2log(info) << "The state labels of this labelled transition system:\n";
          for(std::size_t i=0; i<l.num_states(); ++i)
          {
            if (l.state_label(i).size()==0)
            {
              mCRL2log(info) << i << ": no label.\n";
            }
            else 
            { 
              for(const mcrl2::lps::state& lab: l.state_label(i))
              { 
                mCRL2log(info) << i << ": (" << pp(lab) << ").\n";
              }
            }
          }
        }
      }
    }

    template < class LTS_TYPE >
    bool provide_information() const
    {
      LTS_TYPE l;
      l.load(infilename);

      mCRL2log(info) 
          << "Number of states: " << l.num_states() << ".\n"
          << "Number of action labels: " << l.num_action_labels() << " (including a tau label).\n"
          << "Number of transitions: " << l.num_transitions() << ".\n";

      if (l.has_state_info())
      {
        mCRL2log(info) << "Number of state labels: " << l.num_state_labels() << ".\n";
      }
      else
      {
        if (!print_state_labels) // This is to prevent the same message being printed twice.
        {
          mCRL2log(info) << "There are no state labels." << std::endl;
        }
      }

      mCRL2log(verbose) << "Checking reachability..." << std::endl;
      if (!reachability_check(l))
      {
        mCRL2log(info) << "Warning: some states are not reachable from the initial state! (This might result in unspecified behaviour of LTS tools.)" << std::endl;
      }

      mCRL2log(verbose) << "Checking whether lts is deterministic..." << std::endl;
      mCRL2log(info) << "LTS is ";
      if (!is_deterministic(l))
      {
        mCRL2log(info) << "not ";
      }
      mCRL2log(info) << "deterministic." << std::endl;

      provide_probabilistic_information(l);

      print_the_action_labels(l);
      print_the_state_labels(l);
      print_the_branching_factor(l);

      return true;
    }

  public:

    bool run() override
    {
      using namespace mcrl2::lts;
      using namespace mcrl2::lts::detail;

      if (intype==lts_none)
      {
        intype = guess_format(infilename);
      }

      switch (intype)
      {
        case lts_lts:
        case lts_lts_probabilistic:
        {
          return provide_information<probabilistic_lts_lts_t>();
        }
        case lts_none:
          mCRL2log(warning) << "No input format is specified. Assuming .aut format.\n";
          [[fallthrough]];
        case lts_aut:
        case lts_aut_probabilistic:
        {
          return provide_information<probabilistic_lts_aut_t>();
        }
        case lts_fsm:
        case lts_fsm_probabilistic:
        {
          return provide_information<probabilistic_lts_fsm_t>();
        }
        case lts_dot:
        {
          throw mcrl2::runtime_error("Cannot read .dot files anymore.");
        }
      }
      return true;
    }
};

int main(int argc, char** argv)
{
  return ltsinfo_tool().execute(argc, argv);
}
