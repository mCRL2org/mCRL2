#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include "mcrl2/lps/next_state_generator.h"

using namespace mcrl2;

void traverse_states(const lps::stochastic_specification& lps_spec, data::rewriter& r, bool per_summand = false)
{
  using namespace mcrl2::lps;

  next_state_generator generator(lps_spec, r);

  const mcrl2::lps::state initial_state = generator.initial_states().front().state();

  std::set<atermpp::aterm> visited;
  std::set<lps::state> seen;
  std::set<lps::multi_action> transition_labels;
  size_t transitions = 0;

  std::queue<lps::state, std::deque<lps::state> > q;
  q.push(initial_state);
  seen.insert(initial_state);

  next_state_generator::enumerator_queue_t enumeration_queue;
  while (!q.empty())
  {
    visited.insert(q.front());

    if (per_summand)
    {
      for (size_t i = 0; i < lps_spec.process().summand_count(); ++i)
      {
        enumeration_queue.clear();
        for(next_state_generator::iterator j = generator.begin(q.front(), i, &enumeration_queue); j; ++j)
        {
          const lps::state s=j->target_state();
          transition_labels.insert(j->action());
          ++transitions;
          if (seen.find(s) == seen.end())
          {
            q.push(s);
            seen.insert(s);
          }
        }
      }
    }
    else
    {
      for(next_state_generator::iterator j = generator.begin(q.front(), &enumeration_queue); j; ++j)
      {
        const lps::state s=j->target_state();
        transition_labels.insert(j->action());
        ++transitions;
        if (seen.find(s) == seen.end())
        {
          q.push(s);
          seen.insert(s);
        }
      }
    }
    q.pop();
  }
  std::cout << "visited " << visited.size() << " states" << std::endl;
}

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    mCRL2log(log::warning) << "usage: next_state_generator LPS_FILE" << std::endl;
  }
  else
  {
    std::string filename(argv[1]);
    std::cout << "loading LPS from file " << filename << std::endl;
    lps::stochastic_specification spec;
    std::ifstream stream(filename);
    spec.load(stream);
    std::cout << "traversing states..." << std::endl;
    data::rewriter r;
    traverse_states(spec,r);
  }

  return 0;
}
