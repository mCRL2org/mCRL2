#include <vector>
#include "libprint_c.h"
#include "lts/liblts.h"
#include "lts/detail/tree_set.h"
//#include "lts/detail/liblts_private.h"

using namespace std;

namespace mcrl2
{
namespace lts
{
// class for comparison of two transitions of an LTS l
class comp_trans_lds {
  private:
    lts *l;
  public:
    explicit comp_trans_lds(lts *l1) : l(l1) {}
    bool operator()(const unsigned int t1,const unsigned int t2) const {
      if (l->transition_label(t1) != l->transition_label(t2)) {
        return l->transition_label(t1) < l->transition_label(t2);
      } else if (l->transition_to(t1) != l->transition_to(t2)) {
        return l->transition_to(t1) < l->transition_to(t2);
      } else {
        return l->transition_from(t1) < l->transition_from(t2);
      }
    }
};

static void get_trans(unsigned int *begin,tree_set_store &tss,unsigned int d,
    vector<unsigned int> &d_trans) {
  if (!tss.is_set_empty(d)) {
    if (tss.is_set_empty(tss.get_set_child_right(d))) {
      unsigned int e = begin[tss.get_set_child_left(d)+1];
      for (unsigned int j = begin[tss.get_set_child_left(d)]; j < e; ++j) {
        d_trans.push_back(j);
      }
    }
    else {
      get_trans(begin,tss,tss.get_set_child_left(d),d_trans);
      get_trans(begin,tss,tss.get_set_child_right(d),d_trans);
    }
  }
}

void lts::determinise() {
  tree_set_store tss;
  
  vector<unsigned int> d_transs;
  vector<unsigned int> d_states;
  
  // create the initial state of the DLTS
  d_states.push_back(initial_state());
  unsigned int d_id = tss.set_set_tag(tss.create_set(d_states));
  d_states.clear();
  
  sort_transitions();
  unsigned int *begin = get_transition_indices();
  unsigned int d_ntransitions = 0;
  unsigned int d_trans_size = 10000;
  transition *d_transitions = (transition*)malloc(d_trans_size*
      sizeof(transition));

  int s;
  unsigned int i,to,lbl,n_t;
  
  while (d_id < tss.get_next_tag()) {
    // collect the outgoing transitions of every state of DLTS state d_id in
    // the vector d_transs
    get_trans(begin,tss,tss.get_set(d_id),d_transs);
    
    // sort d_transs by label and (if labels are equal) by destination
    sort(d_transs.begin(),d_transs.end(),comp_trans_lds(this));
    
    n_t = d_transs.size();
    i = 0;
    for (lbl = 0; lbl < num_labels(); ++lbl) {
      // compute the destination of the transition with label lbl
      while (i < n_t && transition_label(d_transs[i]) < lbl) {
        ++i;
      }
      while (i < n_t && transition_label(d_transs[i]) == lbl) {
        to = transition_to(d_transs[i]);
        d_states.push_back(to);
        while (i < n_t && transition_label(d_transs[i]) == lbl &&
            transition_to(d_transs[i]) == to) {
          ++i;
        }
      }
      s = tss.create_set(d_states);

      // generate the transitions to each of the next states
      if (!tss.is_set_empty(s)) {
        if (d_ntransitions >= d_trans_size) {
          d_trans_size *= 2;
          d_transitions = (transition*)realloc(d_transitions,d_trans_size*
              sizeof(transition));
          if (d_transitions == NULL) {
            gsErrorMsg("out of memory\n");
            exit(1);
          }
        }
        d_transitions[d_ntransitions].from  = d_id;
        d_transitions[d_ntransitions].label = lbl;
        d_transitions[d_ntransitions].to    = tss.set_set_tag(s);
        ++d_ntransitions;
        if (d_ntransitions%10000 == 0) {
          gsVerboseMsg(
              "generated %d states and %d transitions; explored %d states\n",
              tss.get_next_tag(),d_ntransitions,d_id);
        }
      }
      d_states.clear();
    }
    d_transs.clear();
    ++d_id;
  }

  remove_state_values();
  free(begin);
  free(states);
  free(transitions);
  states = NULL;
  states_size = 0;
  nstates = d_id;
  set_initial_state(0);
  transitions = d_transitions;
  transitions_size = d_trans_size;
  ntransitions = d_ntransitions;
}
}
}
