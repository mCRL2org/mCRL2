#ifndef _EXPLORATION_H
#define _EXPLORATION_H

#include <string>
#include <aterm2.h>
#include "lts.h"
#include "libnextstate.h"
#include "librewrite.h"
#include "squadt_lpe2lts.h"

#ifndef ULLONG_MAX
#define ULLONG_MAX      18446744073709551615ULL
#endif

#define DEFAULT_MAX_STATES ULLONG_MAX
#define DEFAULT_MAX_TRACES 10UL
#define DEFAULT_BITHASHSIZE 209715200ULL // ~25 MB 
#define DEFAULT_INIT_TSIZE 10000UL 

enum exploration_strategy { es_none, es_breadth, es_depth, es_random };

struct lts_generation_options {
  lts_generation_options()
    :
      quiet(false),
      verbose(false),
      strat(GS_REWR_INNER),
      usedummies(true),
      removeunused(true),
      stateformat(GS_STATE_VECTOR),
      outformat(OF_UNKNOWN),
      outinfo(true),
      max_states(DEFAULT_MAX_STATES),
      trace(false),
      num_trace_actions(0),
      trace_actions(NULL),
      max_traces(DEFAULT_MAX_TRACES),
      detect_deadlock(false),
      detect_action(false),
      save_error_trace(false),
      error_trace_saved(false),
      expl_strat(es_breadth),
      bithashing(false),
      bithashsize(DEFAULT_BITHASHSIZE),
      todo_max(ULONG_MAX),
      initial_table_size(DEFAULT_INIT_TSIZE),
      squadt(NULL)
  {}
  
  bool quiet;
  bool verbose;
  RewriteStrategy strat;
  bool usedummies;
  bool removeunused;
  int stateformat;
  int outformat;
  bool outinfo;
  unsigned long long max_states;
  std::string priority_action;
  bool trace;
  int num_trace_actions;
  ATermAppl *trace_actions;
  unsigned long max_traces;
  bool detect_deadlock;
  bool detect_action;
  bool save_error_trace;
  bool error_trace_saved;
  exploration_strategy expl_strat;
  bool bithashing;
  unsigned long long bithashsize;
  unsigned long todo_max;
  unsigned long initial_table_size;
  std::string specification;
  std::string lts;
  squadt_lpe2lts *squadt;
};

exploration_strategy str_to_expl_strat(const char *s);
const char *expl_strat_to_str(exploration_strategy es);

bool initialise_lts_generation(lts_generation_options *opts);
bool generate_lts();
bool finalise_lts_generation();

#endif
