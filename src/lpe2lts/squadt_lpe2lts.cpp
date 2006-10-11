#include <boost/lexical_cast.hpp>
#include "libnextstate.h"
#include "librewrite.h"
#include "lpe2lts.h"
#include "exploration.h"
#include <squadt_utility.h>

using namespace std;
    
#ifdef ENABLE_SQUADT_CONNECTIVITY
enum lpe2lts_options {
  option_out_info,

  option_usedummies,
  option_state_format_tree,
  option_removeunused,

  option_rewr_strat,

  option_expl_strat,
  
  option_detect_deadlock,
  option_detect_actions,
  option_trace,
  option_max_traces,
  
  option_confluence_reduction,
  option_confluent_tau,
  
  option_max_states,
  
  option_bithashing,
  option_bithashsize,
  
  option_init_tsize
};

const unsigned int lpd_file_for_input_no_lts = 0;
const unsigned int lpd_file_for_input_lts = 1;
const unsigned int lts_file_for_output = 2;

void squadt_lpe2lts::initialise()
{
}

void squadt_lpe2lts::set_capabilities(sip::tool::capabilities &cp) const
{
  /* The tool has only one main input combination it takes an LPE and then behaves as a reporter */
  cp.add_input_combination(lpd_file_for_input_no_lts, "Reporting", "lpe");
  cp.add_input_combination(lpd_file_for_input_lts, "Transformation", "lpe");
}

void squadt_lpe2lts::user_interactive_configuration(sip::configuration &configuration)
{
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;
  
  bool make_lts = configuration.object_exists(lpd_file_for_input_lts);

  //status_display = sip::layout::tool_display::sptr(new layout::tool_display);
  //layout::tool_display::sptr display(status_display);
  tool_display::sptr display(new tool_display);

  /* Create and add the top layout manager */
  manager::aptr layout_manager = horizontal_box::create();

  /* First column */
  vertical_box* column = new vertical_box();

  
  char buf[21];

  checkbox* cb_aut = make_lts?(new checkbox("generate aut file", false)):NULL;
  checkbox* cb_out_info = new checkbox("save state information", true);
  
  checkbox* cb_usedummies = new checkbox("fill in free variables", true);
  checkbox* cb_state_format_tree = new checkbox("memory efficient state repr.", false);
  checkbox* cb_removeunused = new checkbox("remove unused data", true);
  
  label* lb_rewr_strat = new label("Rewriter:");
  
  label* lb_expl_strat = new label("Strategy:");
  
  checkbox* cb_deadlock = new checkbox("detect deadlocks", false);
  checkbox* cb_actions = new checkbox("detect actions:",false);
  text_field* tf_actions = new text_field("", sip::datatype::string::standard);
  checkbox* cb_trace = new checkbox("save action/deadlock traces, but at most:", false);
  sprintf(buf,"%lu",DEFAULT_MAX_TRACES);
  text_field* tf_max_traces = new text_field(buf, sip::datatype::integer::standard);
  
  checkbox* cb_confluence = new checkbox("confluence reduction with confluent tau:", false);
  text_field* tf_conf_tau = new text_field("ctau", sip::datatype::string::standard);
  
  
  checkbox* cb_max_states = new checkbox("maximum number of states:",false);
  sprintf(buf,"%llu",1000ULL);
  text_field* tf_max_states = new text_field(buf, sip::datatype::integer::standard);

  checkbox* cb_bithashing = new checkbox("bit hashing; number of states:", false);
  sprintf(buf,"%llu",DEFAULT_BITHASHSIZE);
  text_field* tf_bithashsize = new text_field(buf, sip::datatype::integer::standard);

  label* lb_init_tsize = new label("initial hash tables size:");
  sprintf(buf,"%lu",DEFAULT_INIT_TSIZE);
  text_field* tf_init_tsize = new text_field(buf, sip::datatype::integer::standard);

  horizontal_box* cbsbox = new horizontal_box();
  vertical_box* cbslbox = new vertical_box();
  if ( make_lts )
  cbslbox->add(cb_aut, layout::left);
  cbslbox->add(cb_out_info, layout::left);
  cbslbox->add(cb_usedummies, layout::left);
  cbsbox->add(cbslbox, top);
  vertical_box* cbsrbox = new vertical_box();
  cbsrbox->add(cb_state_format_tree, layout::left);
  cbsrbox->add(cb_removeunused, layout::left);
  cbsbox->add(cbsrbox, top);
  column->add(cbsbox, center);

  column->add(new label(" "),layout::left);

  manager* rewrbox = new horizontal_box();
  rewrbox->add(lb_rewr_strat);
  squadt_utility::radio_button_helper<RewriteStrategy>
    rewr_selector(rewrbox,GS_REWR_INNER,"innermost");
  rewr_selector.associate(rewrbox,GS_REWR_JITTY,"JITty");
  rewr_selector.associate(rewrbox,GS_REWR_INNERC,"compiling innermost");
  rewr_selector.associate(rewrbox,GS_REWR_JITTYC,"compiling JITty");
  column->add(rewrbox,layout::left);

  column->add(new label(" "),layout::left);

  manager* explbox = new horizontal_box();
  explbox->add(lb_expl_strat);
  squadt_utility::radio_button_helper<exploration_strategy>
    expl_selector(explbox,es_breadth, "breadth-first");
  expl_selector.associate(explbox,es_depth, "depth-first");
  expl_selector.associate(explbox,es_random, "random-first");
  column->add(explbox,layout::left);

  column->add(new label(" "),layout::left);

  column->add(cb_deadlock, layout::left);
  horizontal_box* actionsbox = new horizontal_box();
  actionsbox->add(cb_actions, top);
  actionsbox->add(tf_actions, top);
  column->add(actionsbox, layout::left);
  horizontal_box* maxtracesbox = new horizontal_box();
  maxtracesbox->add(cb_trace,      top);
  maxtracesbox->add(tf_max_traces, top);
  column->add(maxtracesbox,layout::left);

  column->add(new label(" "),layout::left);

  horizontal_box* confbox = new horizontal_box();
  confbox->add(cb_confluence, middle);
  confbox->add(tf_conf_tau,   middle);
  column->add(confbox, layout::left);

  column->add(new label(" "),layout::left);

  horizontal_box* maxstatesbox = new horizontal_box();
  maxstatesbox->add(cb_max_states, middle);
  maxstatesbox->add(tf_max_states, middle);
  column->add(maxstatesbox, layout::left);

  column->add(new label(" "),layout::left);

  horizontal_box* bithashbox = new horizontal_box();
  bithashbox->add(cb_bithashing,  middle);
  bithashbox->add(tf_bithashsize, middle);
  column->add(bithashbox, layout::left);
  
  column->add(new label(" "),layout::left);

  horizontal_box* tsizebox = new horizontal_box();
  tsizebox->add(lb_init_tsize, bottom);
  tsizebox->add(tf_init_tsize, bottom);
  column->add(tsizebox, layout::left);

  button* okay_button = new button("OK");

  column->add(okay_button, layout::right);

  /* Attach columns*/
  layout_manager->add(column, margins(0,5,0,5));

  display->set_top_manager(layout_manager);

  m_communicator.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  /* Update the current configuration */
  sip::configuration &c = configuration;

  /* Values for the options */
  if ( make_lts )
  {
    std::string input_file_name = c.get_object(lpd_file_for_input_lts)->get_location();
    /* Add output file to the configuration */
    c.add_output(lts_file_for_output, (cb_aut->get_status()?"aut":"svc"), c.get_output_name(cb_aut->get_status()?".aut":".svc"));
  }
  c.add_option(option_out_info).append_argument(sip::datatype::boolean::standard, cb_out_info->get_status());

  c.add_option(option_usedummies).append_argument(sip::datatype::boolean::standard, cb_usedummies->get_status());
  c.add_option(option_state_format_tree).append_argument(sip::datatype::boolean::standard, cb_state_format_tree->get_status());
  c.add_option(option_removeunused).append_argument(sip::datatype::boolean::standard, cb_removeunused->get_status());
  
  c.add_option(option_rewr_strat).append_argument(sip::datatype::integer::standard,rewr_selector.get_selection());
  
  c.add_option(option_expl_strat).append_argument(sip::datatype::integer::standard,expl_selector.get_selection());

  c.add_option(option_detect_deadlock).append_argument(sip::datatype::boolean::standard, cb_deadlock->get_status());
  c.add_option(option_detect_actions).append_argument(sip::datatype::string::standard, cb_actions->get_status()?tf_actions->get_text():"");
  c.add_option(option_trace).append_argument(sip::datatype::boolean::standard, cb_trace->get_status());
  c.add_option(option_max_traces).append_argument(sip::datatype::string::standard, tf_max_traces->get_text());
  
  c.add_option(option_confluence_reduction).append_argument(sip::datatype::boolean::standard, cb_confluence->get_status());
  c.add_option(option_confluent_tau).append_argument(sip::datatype::string::standard, tf_conf_tau->get_text());
  
  c.add_option(option_max_states).append_argument(sip::datatype::string::standard, cb_max_states->get_status()?tf_max_states->get_text():"");
  
  c.add_option(option_bithashing).append_argument(sip::datatype::boolean::standard, cb_bithashing->get_status());
  c.add_option(option_bithashsize).append_argument(sip::datatype::string::standard, tf_bithashsize->get_text());
  
  c.add_option(option_init_tsize).append_argument(sip::datatype::string::standard, tf_init_tsize->get_text());
  
  m_communicator.send_clear_display();
}

bool squadt_lpe2lts::check_configuration(sip::configuration const &configuration) const
{
  return (
      configuration.object_exists(lpd_file_for_input_no_lts) ||
      (configuration.object_exists(lpd_file_for_input_lts) &&
       configuration.object_exists(lts_file_for_output))
      );
}

bool squadt_lpe2lts::perform_task(sip::configuration &configuration)
{
  lts_generation_options lgopts; initialise_lts_generation_options(lgopts);

  lgopts.squadt = this;

  if ( configuration.object_exists(lpd_file_for_input_lts) )
  {
    lgopts.specification = configuration.get_object(lpd_file_for_input_lts)->get_location();
    lgopts.lts = configuration.get_object(lts_file_for_output)->get_location();
  } else {
    lgopts.specification = configuration.get_object(lpd_file_for_input_no_lts)->get_location();
  }

  lgopts.outinfo = boost::any_cast <bool> (*(configuration.get_option(option_out_info)->get_value_iterator()));

  lgopts.usedummies = boost::any_cast <bool> (*(configuration.get_option(option_usedummies)->get_value_iterator()));
  lgopts.stateformat = (boost::any_cast <bool> (*(configuration.get_option(option_state_format_tree)->get_value_iterator())))?GS_STATE_TREE:GS_STATE_VECTOR;
  lgopts.removeunused = boost::any_cast <bool> (*(configuration.get_option(option_removeunused)->get_value_iterator()));
  
  lgopts.max_traces = strtoul((boost::any_cast <string> (*(configuration.get_option(option_max_traces)->get_value_iterator()))).c_str(),NULL,0);

  lgopts.strat = (RewriteStrategy) boost::any_cast <long int> (*(configuration.get_option(option_rewr_strat)->get_value_iterator()));
  
  lgopts.expl_strat = (exploration_strategy) boost::any_cast <long int> (*(configuration.get_option(option_expl_strat)->get_value_iterator()));
  
  lgopts.detect_deadlock = boost::any_cast <bool> (*(configuration.get_option(option_detect_deadlock)->get_value_iterator()));
  string actions_str = boost::any_cast <string> (*(configuration.get_option(option_detect_actions)->get_value_iterator()));
  if ( actions_str != "" )
  {
    lgopts.detect_action = true;
    lgopts.trace_actions = parse_action_list(actions_str.c_str(),&lgopts.num_trace_actions);
  }
  lgopts.trace = boost::any_cast <bool> (*(configuration.get_option(option_trace)->get_value_iterator()));
  lgopts.max_traces = strtoul((boost::any_cast <string> (*(configuration.get_option(option_max_traces)->get_value_iterator()))).c_str(),NULL,0);
  
  if ( boost::any_cast <bool> (*(configuration.get_option(option_confluence_reduction)->get_value_iterator())) )
  {
    lgopts.priority_action = strdup((boost::any_cast <string> (*(configuration.get_option(option_confluent_tau)->get_value_iterator()))).c_str());
  }
  
  string max_states_str(boost::any_cast <string> (*(configuration.get_option(option_max_states)->get_value_iterator())));
  if ( max_states_str != "" )
  {
    lgopts.max_states = boost::lexical_cast < unsigned long long > (max_states_str);
  } else {
    lgopts.max_states = DEFAULT_MAX_STATES;
  }
  
  lgopts.bithashing = boost::any_cast <bool> (*(configuration.get_option(option_bithashing)->get_value_iterator()));
  lgopts.bithashsize = boost::lexical_cast < unsigned long long > (
      (boost::any_cast <string> (*(configuration.get_option(option_bithashsize)->get_value_iterator()))));
  
  lgopts.initial_table_size = strtoul((boost::any_cast <string> (*(configuration.get_option(option_init_tsize)->get_value_iterator()))).c_str(),NULL,0);

  bool ok = false;
  if ( initialise_lts_generation(&lgopts) )
  {
    ok = generate_lts();

    finalise_lts_generation();
  }

  return ok;
}

void squadt_lpe2lts::finalise()
{
}
#endif

void squadt_lpe2lts::create_status_display()
{
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if ( is_active() )
  {
    using namespace sip;
    using namespace sip::layout;
    using namespace sip::layout::elements;
  
    status_display = sip::layout::tool_display::sptr(new layout::tool_display);
  
    /* Create and add the top layout manager */
    layout_manager = layout::vertical_box::create();
    labels = layout::horizontal_box::create();
  
    /* First column */
    layout::vertical_box* column1 = new layout::vertical_box();
    layout::vertical_box* column2 = new layout::vertical_box();
  
    label* lb_level_text = new label("Level:");
    lb_level = new label("0");
    label* lb_explored_text = new label("States explored:");
    lb_explored = new label("0");
    label* lb_seen_text = new label("States seen:");
    lb_seen = new label("0");
    label* lb_transitions_text = new label("Transitions:");
    lb_transitions = new label("0");
  
    column1->add(lb_level_text, layout::left);
    column2->add(lb_level, layout::right);
    column1->add(lb_explored_text, layout::left);
    column2->add(lb_explored, layout::right);
    column1->add(lb_seen_text, layout::left);
    column2->add(lb_seen, layout::right);
    column1->add(lb_transitions_text, layout::left);
    column2->add(lb_transitions, layout::right);
  
    progbar = new progress_bar(0,0,0);
  
    /* Attach columns*/
    labels->add(column1, margins(0,5,0,5));
    labels->add(column2, margins(0,5,0,5));
    layout_manager->add(labels.get(), margins(0,5,0,5));
    layout_manager->add(progbar, margins(0,5,0,5));
  
    status_display->set_top_manager(layout_manager);
  
    m_communicator.send_display_layout(status_display);
  }
#endif
}

void squadt_lpe2lts::update_status_display(unsigned long level, unsigned long long explored, unsigned long long seen, unsigned long long num_found_same, unsigned long long transitions)
{
#ifdef ENABLE_SQUADT_CONNECTIVITY
  if ( is_active() )
  {
    char buf[21];
    sprintf(buf,"%lu",level);
    lb_level->set_text(buf,&m_communicator);
    sprintf(buf,"%llu",explored);
    lb_explored->set_text(buf,&m_communicator);
    sprintf(buf,"%llu",seen);
    lb_seen->set_text(buf,&m_communicator);
    sprintf(buf,"%llu",transitions);
    lb_transitions->set_text(buf,&m_communicator);
    if ( seen > 1000000ULL )
    {
      explored = explored/(seen/1000000);
      seen = 1000000ULL;
    }
    if ( explored > seen )
    {
      seen = explored;
    }
    progbar->set_maximum(seen,&m_communicator);
    progbar->set_value(explored,&m_communicator);
  }
#endif
}
