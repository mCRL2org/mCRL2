#ifndef __LIBLTS_H
#define __LIBLTS_H

#include <string>
#include <iostream>
#include <aterm/aterm2.h>

namespace mcrl2
{
namespace lts
{

  enum lts_type { lts_none, lts_mcrl2, lts_aut, lts_mcrl, lts_svc };

  #include "liblts_private.h"

  class lts;

  class state_iterator : p_state_iterator
  {
    public:
      state_iterator(lts *l);

      bool more();

      unsigned int operator *();
      void operator ++();
  };

  class label_iterator : p_label_iterator
  {
    public:
      label_iterator(lts *l);
      
      bool more();

      unsigned int operator *();
      void operator ++();
  };

  class transition_iterator : p_transition_iterator
  {
    public:
      transition_iterator(lts *l);
      
      bool more();

      unsigned int from();
      unsigned int label();
      unsigned int to();

      void operator ++();
  };

  class lts : p_lts
  {
    public:
      lts();
      lts(std::string &filename, lts_type type = lts_none);
      lts(std::istream &is, lts_type type = lts_none);
      ~lts();

      bool read_from(std::string &filename, lts_type type = lts_none);
      bool read_from(std::istream &is, lts_type type = lts_none);
      bool write_to(std::string &filename, lts_type type = lts_mcrl2);
      bool write_to(std::ostream &os, lts_type type = lts_mcrl2);

      unsigned int num_states();
      unsigned int num_transitions();
      unsigned int num_labels();

      unsigned int initial_state();
      void set_initial_state(unsigned int state);
      
      unsigned int add_state(ATerm value = NULL);
      unsigned int add_label(ATerm value = NULL);
      unsigned int add_transition(unsigned int from,
                                  unsigned int label,
                                  unsigned int to);

      void set_state(unsigned int state, ATerm value);
      void set_label(unsigned int label, ATerm value);

      ATerm state_value(unsigned int state);
      ATerm label_value(unsigned int label);
      unsigned int transition_from(unsigned int transition);
      unsigned int transition_label(unsigned int transition);
      unsigned int transition_to(unsigned int transition);

      state_iterator get_states();
      label_iterator get_labels();
      transition_iterator get_transitions();

      bool has_creator();
      std::string get_creator();
      void set_creator(std::string creator);
      
      lts_type get_type();

      bool has_state_info();
      bool has_label_info();

      friend class state_iterator;
      friend class label_iterator;
      friend class transition_iterator;
  };

}
}

#endif
