// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_private.h

  class p_lts_extra
  {
    protected:
      lts_extra_type type;
      union {
        ATerm mcrl1_spec;
        ATermAppl mcrl2_spec;
	lts_dot_options dot_options;
      } content;
  };

  class lts;

  struct transition
  {
    unsigned int from;
    unsigned int label;
    unsigned int to;

    transition():from(0),label(0),to(0)
    {}

    transition(const unsigned int f, 
               const unsigned int l,
               const unsigned int t):from(f),label(l),to(t)
    {}
    
     bool operator <(const transition &t) const
     { return from < t.from ||
              (from==t.from && (label<t.label ||
                   (label==t.label && to<t.to)));
     }

  };

  class p_state_iterator
  {
    protected:
      lts *l;
      unsigned int pos;
      unsigned int max;
  };

  class p_label_iterator
  {
    protected:
      lts *l;
      unsigned int pos;
      unsigned int max;
  };

  class p_transition_iterator
  {
    protected:
      lts *l;
      unsigned int pos;
      unsigned int max;
  };

  class p_lts
  {
    protected:
      lts *lts_object;
      lts_type type;
      std::string creator;
      bool state_info;
      bool label_info;
      unsigned int states_size;
      unsigned int labels_size;
      unsigned int transitions_size;
      unsigned int nstates;
      unsigned int nlabels;
      unsigned int ntransitions;
      transition *transitions;
      ATerm *state_values;
      bool *taus;
      ATerm *label_values;
      unsigned int init_state;
      ATerm extra_data;

      p_lts(lts *l);

      lts_type detect_type(std::string const& filename);
      lts_type detect_type(std::istream &is);

      void init(bool state_info = true, bool label_info = true);
      void init(p_lts const &l);

      void clear(bool state_info = true, bool label_info = true);
      void clear_states();
      void clear_labels();
      void clear_transitions();
      void clear_type();

      void merge(lts *l);

      unsigned int p_add_state(ATerm value = NULL);
      unsigned int p_add_label(ATerm value = NULL, bool is_tau = false);
      unsigned int p_add_transition(unsigned int from,
                                    unsigned int label,
                                    unsigned int to);

      std::string p_state_value_str(unsigned int state);
      std::string p_label_value_str(unsigned int label);

      void p_remove_state_values();

      bool read_from_aut(std::string const& filename);
      bool read_from_aut(std::istream &is);
      bool write_to_aut(std::string const& filename);
      bool write_to_aut(std::ostream& os);

      static lps::specification const& empty_specification();

      bool read_from_svc(std::string const& filename, lts_type type);
      bool write_to_svc(std::string const& filename, lts_type type, lps::specification const& spec = empty_specification());

      lts_type fsm_get_lts_type();
      bool read_from_fsm(std::string const& filename, lts_type type, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::string const& filename, ATerm lps);
      bool read_from_fsm(std::string const& filename, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::istream& is, lts_type type, lps::specification const& spec = empty_specification());
      bool read_from_fsm(std::istream& is, ATerm lps);
      bool read_from_fsm(std::istream& is, lps::specification const& spec = empty_specification());
      bool write_to_fsm(std::string const& filename, lts_type type, ATermList params);
      bool write_to_fsm(std::string const& filename, ATerm lps);
      bool write_to_fsm(std::string const& filename, lps::specification const& spec = empty_specification());
      bool write_to_fsm(std::ostream& os, lts_type type, ATermList params);
      bool write_to_fsm(std::ostream& os, ATerm lps);
      bool write_to_fsm(std::ostream& os, lps::specification const& spec = empty_specification());

      bool read_from_dot(std::string const& filename);
      bool read_from_dot(std::istream &is);
      bool write_to_dot(std::ostream& os, lts_dot_options opts);
      bool write_to_dot(std::string const& filename, lts_dot_options opts);

#ifdef USE_BCG
      bool read_from_bcg(std::string const& filename);
      bool write_to_bcg(std::string const& filename);
#endif

      void p_sort_transitions(transition_sort_style ts = src_lbl_tgt);
      unsigned int* p_get_transition_indices();
      unsigned int** p_get_transition_pre_table();

      void tau_star_reduce(); // This method assumes the absence of tau loops!
  };
