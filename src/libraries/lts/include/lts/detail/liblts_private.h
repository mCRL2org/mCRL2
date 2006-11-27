  class p_lts_extra
  {
    protected:
      lts_extra_type type;
      union {
        ATerm mcrl1_spec;
        lpe::specification *mcrl2_spec;
	lts_dot_options dot_options;
      } content;
  };

  class lts;

  typedef struct
  {
    unsigned int from;
    unsigned int label;
    unsigned int to;
  } transition;

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
      unsigned int *states;
      ATerm *state_values;
      unsigned int *labels;
      bool *taus;
      ATerm *label_values;
      transition *transitions;
      unsigned int init_state;

      lts_type detect_type(std::string const& filename);
      lts_type detect_type(std::istream &is);

      void init(lts_type type = lts_mcrl2, bool state_info = true, bool label_info = true);
      void clear(lts_type type = lts_mcrl2, bool state_info = true, bool label_info = true);

      unsigned int p_add_state(ATerm value = NULL);
      unsigned int p_add_label(ATerm value = NULL, bool is_tau = false);
      unsigned int p_add_transition(unsigned int from,
                                    unsigned int label,
                                    unsigned int to);
      
      std::string p_state_value_str(unsigned int state);
      std::string p_label_value_str(unsigned int label);

      bool read_from_aut(std::string const& filename);
      bool read_from_aut(std::istream &is);
      bool write_to_aut(std::string const& filename);
      bool write_to_aut(std::ostream& os);

      bool read_from_svc(std::string const& filename, lts_type type);
      bool write_to_svc(std::string const& filename, lts_type type, lpe::specification *spec = NULL);

      lts_type fsm_get_lts_type();
      bool write_to_fsm(std::string const& filename, lts_type type, ATermList params);
      bool write_to_fsm(std::string const& filename, ATerm lpe = NULL);
      bool write_to_fsm(std::string const& filename, lpe::specification &spec);
      bool write_to_fsm(std::ostream& os, lts_type type, ATermList params);
      bool write_to_fsm(std::ostream& os, ATerm lpe = NULL);
      bool write_to_fsm(std::ostream& os, lpe::specification &spec);

      bool write_to_dot(std::ostream& os, lts_dot_options opts);
      bool write_to_dot(std::string const& filename, lts_dot_options opts);

#ifdef MCRL2_BCG
      bool read_from_bcg(std::string const& filename);
      bool write_to_bcg(std::string const& filename);
#endif
  };
