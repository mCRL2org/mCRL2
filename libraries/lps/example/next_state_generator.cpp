#include <iostream>
#include <queue>
#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/next_state_generator.h"

using namespace mcrl2;

void traverse_states(const lps::specification& lps_spec, bool per_summand = false)
{
  using namespace mcrl2::lps;

  next_state_generator generator(lps_spec, data::jitty_compiling);

  atermpp::aterm initial_state = generator.initial_state();

  atermpp::set<atermpp::aterm> visited;
  atermpp::set<atermpp::aterm> seen;
  atermpp::set<atermpp::aterm_appl> transition_labels;
  size_t transitions = 0;

  std::queue<atermpp::aterm, atermpp::deque<atermpp::aterm> > q;
  q.push(initial_state);
  seen.insert(initial_state);

  while (!q.empty())
  {
    visited.insert(q.front());

    if (per_summand)
    {
      for (size_t i = 0; i < lps_spec.process().summand_count(); ++i)
      {
        next_state_generator::iterator first = generator.begin(q.front(), i);
        while (++first)
        {
          const next_state_generator::state_type& s = *first;
          transition_labels.insert(s.transition);
          ++transitions;
          if (seen.find(s.state) == seen.end())
          {
            q.push(s.state);
            seen.insert(s.state);
          }
        }
      }
    }
    else
    {
      next_state_generator::iterator first = generator.begin(q.front());
      while (++first)
      {
        const next_state_generator::state_type& s = *first;
        transition_labels.insert(s.transition);
        ++transitions;
        if (seen.find(s.state) == seen.end())
        {
          q.push(s.state);
          seen.insert(s.state);
        }
      }
    }
    q.pop();
  }
  std::cout << "visited " << visited.size() << " states" << std::endl;
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  if (argc != 2)
  {
    std::cerr << "usage: next_state_generator LPS_FILE" << std::endl;
  }
  else
  {
    std::string filename(argv[1]);
    std::cout << "loading LPS from file " << filename << std::endl;
    lps::specification spec;
    spec.load(filename);
    std::cout << "traversing states..." << std::endl;
    traverse_states(spec);
  }

  return 0;
}
