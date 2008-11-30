// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2lts.cpp

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <string>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/lps/nextstate.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/exception.h"
#include "lps2lts.h"
#include "exploration.h"

#include "mcrl2/core/messaging.h"

#include "squadt_interactor.h"

#include "workarounds.h" // for strdup

const char*  ::squadt_interactor::option_lts_type             = "lts_type";
const char*  ::squadt_interactor::option_out_info             = "out_info";

const char*  ::squadt_interactor::option_usedummies           = "use_dummies";
const char*  ::squadt_interactor::option_state_format_tree    = "state_format_tree";
const char*  ::squadt_interactor::option_removeunused         = "remove_unused";

const char*  ::squadt_interactor::option_rewrite_strategy     = "rewrite_strategy";

const char*  ::squadt_interactor::option_exploration_strategy = "expl_strat";

const char*  ::squadt_interactor::option_detect_deadlock      = "detect_deadlock";
const char*  ::squadt_interactor::option_detect_actions       = "detect_actions";
const char*  ::squadt_interactor::option_trace                = "trace";
const char*  ::squadt_interactor::option_max_traces           = "max_traces";
const char*  ::squadt_interactor::option_error_trace          = "error_trace";

const char*  ::squadt_interactor::option_confluence_reduction = "confluence_reduction";
const char*  ::squadt_interactor::option_confluent_tau        = "confluent_tau";

const char*  ::squadt_interactor::option_max_states           = "max_states";

const char*  ::squadt_interactor::option_bithashing           = "bithashing";
const char*  ::squadt_interactor::option_bithashsize          = "bithash_size";

const char*  ::squadt_interactor::option_init_tsize           = "init_tsize";

const char*  ::squadt_interactor::lps_file_for_input          = "lps_in";
const char*  ::squadt_interactor::lts_file_for_output         = "lts_out";
const char*  ::squadt_interactor::trc_file_for_output         = "trc_out";

static bool initialise_types() {
  tipi::datatype::enumeration< exploration_strategy > exploration_enumeration;

  exploration_enumeration.
    add(es_breadth, "breadth-first").
    add(es_depth, "depth-first").
    add(es_random, "random");

  return true;
}

squadt_interactor::squadt_interactor() {
  static bool initialised = initialise_types();

  static_cast< void > (initialised); // harmless, and prevents unused variable warnings
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities &cp) const {
  /* The tool has only one main input combination it takes an LPS and then behaves as a reporter */
  cp.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::reporting);
  cp.add_input_configuration(lps_file_for_input, tipi::mime_type("lps", tipi::mime_type::application), tipi::tool::category::transformation);
}

class squadt_interactor::storage_configuration {

  private:

    tipi::layout::elements::checkbox& cb_out_info;
    tipi::layout::elements::checkbox& cb_usedummies;
    tipi::layout::elements::checkbox& cb_state_format_tree;
    tipi::layout::elements::checkbox& cb_removeunused;

  public:

    template < typename M >
    storage_configuration(tipi::configuration& c, tipi::tool_display& d, M& m) :
      cb_out_info(d.create< tipi::layout::elements::checkbox >()),
      cb_usedummies(d.create< tipi::layout::elements::checkbox >()),
      cb_state_format_tree(d.create< tipi::layout::elements::checkbox >()),
      cb_removeunused(d.create< tipi::layout::elements::checkbox >()) {

      using namespace tipi;
      using namespace tipi::layout;
      using namespace tipi::layout::elements;

      m.append(d.create< horizontal_box >().set_default_margins(margins(8, 5, 8, 5)).
            append(d.create< vertical_box >().
                append(cb_out_info.set_label("save state information")).
                append(cb_usedummies.set_label("fill in free variables"))).
            append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(cb_state_format_tree.set_label("memory efficient state repr.")).
                append(cb_removeunused.set_label("remove unused data"))));

      set_defaults(c);

      /* Update state of controls */
      cb_out_info.set_status(c.get_option_argument< bool >(option_out_info));
      cb_usedummies.set_status(c.get_option_argument< bool >(option_usedummies));
      cb_state_format_tree.set_status(c.get_option_argument< bool >(option_state_format_tree));
      cb_removeunused.set_status(c.get_option_argument< bool >(option_removeunused));
    }

    void set_defaults(tipi::configuration& c) {
      /* Set default configuration, for unspecified options */
      if (!c.option_exists(option_out_info)) {
        c.add_option(option_out_info).set_argument_value< 0 >(true);
      }
      if (!c.option_exists(option_usedummies)) {
        c.add_option(option_usedummies).set_argument_value< 0 >(true);
      }
      if (!c.option_exists(option_state_format_tree)) {
        c.add_option(option_state_format_tree).set_argument_value< 0 >(false);
      }
      if (!c.option_exists(option_removeunused)) {
        c.add_option(option_removeunused).set_argument_value< 0 >(true);
      }
    }

    void update_configuration(boost::shared_ptr< squadt_interactor::storage_configuration >, tipi::configuration& c) {
      /* Add output file to the configuration */
      std::string outfile;
      tipi::mime_type mt("application/unknown");

      if ( c.option_exists(option_lts_type) )
      {
        outfile = c.get_output_name("."+mcrl2::lts::lts::extension_for_type(c.get_option_argument< mcrl2::lts::lts_type >(option_lts_type)));
        switch ( c.get_option_argument< mcrl2::lts::lts_type >(option_lts_type) )
        {
          case mcrl2::lts::lts_aut:
            mt = tipi::mime_type("text/aut");
            break;
          case mcrl2::lts::lts_svc:
            mt = tipi::mime_type("application/svc");
            break;
          case mcrl2::lts::lts_mcrl:
            mt = tipi::mime_type("application/svc+mcrl");
            break;
          case mcrl2::lts::lts_mcrl2:
            mt = tipi::mime_type("application/mcrl2-lts");
            break;
#ifdef USE_BCG
          case mcrl2::lts::lts_bcg:
            mt = tipi::mime_type("application/bcg");
            break;
#endif
          case mcrl2::lts::lts_fsm:
            mt = tipi::mime_type("text/fsm");
            break;
          case mcrl2::lts::lts_dot:
            mt = tipi::mime_type("text/dot");
            break;
          default:
            assert(0);
            throw mcrl2::runtime_error("unsupported LTS format used for output");
        }
      } else {
        outfile = c.get_output_name(".lts");
      }

      if (c.output_exists(squadt_interactor::lts_file_for_output)) {
        tipi::configuration::object& o = c.get_output(lts_file_for_output);

        o.type(mt);
        o.location(outfile);
      }
      else {
        c.add_output(lts_file_for_output, mt, outfile);
      }

      c.add_option(option_out_info).set_argument_value< 0 >(cb_out_info.get_status());

      c.add_option(option_usedummies).set_argument_value< 0 >(cb_usedummies.get_status());
      c.add_option(option_state_format_tree).set_argument_value< 0 >(cb_state_format_tree.get_status());
      c.add_option(option_removeunused).set_argument_value< 0 >(cb_removeunused.get_status());
    }
};

void squadt_interactor::user_interactive_configuration(tipi::configuration& c)
{
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  bool make_lts = c.category() == tipi::tool::category::transformation;

  /* Set default configuration, for unspecified options */
  if (!c.option_exists(option_detect_deadlock)) {
    c.add_option(option_detect_deadlock).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_trace)) {
    c.add_option(option_trace).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_max_traces)) {
    c.add_option(option_max_traces).set_argument_value< 0 >(boost::lexical_cast< std::string > (DEFAULT_MAX_TRACES));
  }
  if (!c.option_exists(option_error_trace)) {
    c.add_option(option_error_trace).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_confluence_reduction)) {
    c.add_option(option_confluence_reduction).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_confluent_tau)) {
    c.add_option(option_confluent_tau).set_argument_value< 0 >("ctau");
  }
  if (!c.option_exists(option_bithashing)) {
    c.add_option(option_bithashing).set_argument_value< 0 >(false);
  }
  if (!c.option_exists(option_bithashsize)) {
    c.add_option(option_bithashsize).set_argument_value< 0 >(boost::lexical_cast< std::string > (DEFAULT_BITHASHSIZE));
  }
  if (!c.option_exists(option_init_tsize)) {
    c.add_option(option_init_tsize).set_argument_value< 0 >(boost::lexical_cast< std::string > (DEFAULT_INIT_TSIZE));
  }

  /* Function for updating the configuration (specifics for state space storage) */
  boost::function < void (tipi::configuration&) > update_configuration;
  
  /* Create display */
  tipi::tool_display d;

  // Helper for exploration strategy selection
  mcrl2::utilities::squadt::radio_button_helper< exploration_strategy > exploration_strategy_selector(d);

  // Helper for rewrite strategy selection
  mcrl2::utilities::squadt::radio_button_helper< RewriteStrategy >      rewrite_strategy_selector(d);
  
  // Helper for rewrite strategy selection
  mcrl2::utilities::squadt::radio_button_helper< mcrl2::lts::lts_type > lts_type_selector(d);

  layout::vertical_box& m = d.create< vertical_box >();

  m.append(d.create< label >().set_text("Exploration strategy")).
    append(d.create< horizontal_box >().set_default_margins(margins(0,5,0,5)).
                append(exploration_strategy_selector.associate(es_breadth, "breadth-first", true)).
                append(exploration_strategy_selector.associate(es_depth, "depth-first")).
                append(exploration_strategy_selector.associate(es_random, "random"))).
    append(d.create< label >().set_text("Rewrite strategy")).
    append(d.create< horizontal_box >().set_default_margins(margins(0,5,0,5)).
                append(rewrite_strategy_selector.associate(GS_REWR_INNER, "Inner")).
                append(rewrite_strategy_selector.associate(GS_REWR_INNERC, "Innerc")).
                append(rewrite_strategy_selector.associate(GS_REWR_JITTY, "Jitty", true)).
                append(rewrite_strategy_selector.associate(GS_REWR_JITTYC, "Jittyc")));

  if ( make_lts )
  {
    if ( !c.option_exists(option_lts_type) ) // do not allow format to be changed once it has been set
    {
      m.append(d.create< label >().set_text("Ouput format")).
        append(d.create< horizontal_box >().set_default_margins(margins(0,5,0,5)).
                    append(lts_type_selector.associate(mcrl2::lts::lts_aut, "Aldebaran")).
#ifdef USE_BCG
                    append(lts_type_selector.associate(mcrl2::lts::lts_bcg, "BCG")).
#endif
                    append(lts_type_selector.associate(mcrl2::lts::lts_dot, "DOT")).
                    append(lts_type_selector.associate(mcrl2::lts::lts_fsm, "FSM")).
                    append(lts_type_selector.associate(mcrl2::lts::lts_svc, "SVC")).
                    append(lts_type_selector.associate(mcrl2::lts::lts_mcrl, "SVC/mCRL")).
                    append(lts_type_selector.associate(mcrl2::lts::lts_mcrl2, "mCRL2", true)));
    }

    boost::shared_ptr < storage_configuration > storage_controls(new storage_configuration(c, d, m));

    update_configuration = boost::bind(&storage_configuration::update_configuration, storage_controls.get(), storage_controls, _1);
  }

  checkbox&       cb_deadlock    = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_detect_deadlock));
  checkbox&       cb_actions     = d.create< checkbox >().set_status(c.option_exists(option_detect_actions));
  text_field&     tf_actions     = d.create< text_field >();
  checkbox&       cb_trace       = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_trace));
  text_field&     tf_max_traces  = d.create< text_field >().set_text(c.get_option_argument< std::string >(option_max_traces));
  checkbox&       cb_error_trace = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_error_trace));
  checkbox&       cb_confluence  = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_confluence_reduction));
  text_field&     tf_conf_tau    = d.create< text_field >().set_text(c.get_option_argument< std::string >(option_confluent_tau));
  checkbox&       cb_max_states  = d.create< checkbox >().set_status(c.option_exists(option_max_states));
  text_field&     tf_max_states  = d.create< text_field >().set_text((!c.option_exists(option_max_states)) ?
                        boost::lexical_cast< std::string > (1000ULL) : c.get_option_argument< std::string >(option_max_states));
  checkbox&       cb_bithashing  = d.create< checkbox >().set_status(c.get_option_argument< bool >(option_bithashing));
  text_field&     tf_bithashsize = d.create< text_field >().set_text(c.get_option_argument< std::string >(option_bithashsize));
  text_field&     tf_init_tsize  = d.create< text_field >().set_text(c.get_option_argument< std::string >(option_init_tsize));

  m.append(d.create< horizontal_box >().set_default_margins(margins(4,0,5,0)).
      append(d.create< vertical_box >().set_default_alignment(layout::left).set_default_margins(margins(1,0,1,0)).
          append(cb_deadlock.set_label("detect deadlocks")).
          append(cb_actions.set_label("detect actions")).
          append(cb_trace.set_label("save action/deadlock traces, but at most:")).
          append(cb_error_trace.set_label("save trace on error")).
          append(cb_confluence.set_label("confluence reduction with confluent tau:")).
          append(cb_max_states.set_label("maximum number of states")).
          append(cb_bithashing.set_label("bit hashing; number of states")).
          append(d.create< label >().set_text("initial hash table size:"))).
      append(d.create< vertical_box >().set_default_alignment(layout::right).
          append(d.create< checkbox >(), layout::hidden).
          append(tf_actions).
          append(tf_max_traces).
          append(d.create< checkbox >(), layout::hidden).
          append(tf_conf_tau).
          append(tf_max_states).
          append(tf_bithashsize).
          append(tf_init_tsize)));

  button& okay_button = d.create< button >().set_label("OK");

  m.append(d.create< label >().set_text(" ")).
    append(okay_button, layout::right);

  // Set default values specified by the configuration
  if (c.option_exists(option_detect_actions)) {
    tf_actions.set_text(c.get_option_argument< std::string >(option_detect_actions));
  }
  if (c.option_exists(option_rewrite_strategy)) {
    rewrite_strategy_selector.set_selection(
        c.get_option_argument< RewriteStrategy >(option_rewrite_strategy, 0));
  }
  if (c.option_exists(option_exploration_strategy)) {
    exploration_strategy_selector.set_selection(
        c.get_option_argument< exploration_strategy >(option_exploration_strategy, 0));
  }

  send_display_layout(d.manager(m));

  /* Wait until the ok button was pressed */
  okay_button.await_change();

  /* Values for the options */
  if (make_lts) {
    if (!c.option_exists(option_lts_type)) {
      c.add_option(option_lts_type);
      c.get_option(option_lts_type).set_argument_value< 0 >(lts_type_selector.get_selection());
    }

    update_configuration(c);
  }

  if (!c.option_exists(option_rewrite_strategy)) {
    c.add_option(option_rewrite_strategy);
  }
  if (!c.option_exists(option_exploration_strategy)) {
    c.add_option(option_exploration_strategy);
  }

  c.get_option(option_rewrite_strategy).set_argument_value< 0 >(rewrite_strategy_selector.get_selection());
  c.get_option(option_exploration_strategy).set_argument_value< 0 >(exploration_strategy_selector.get_selection());
  c.add_option(option_detect_deadlock).set_argument_value< 0 >(cb_deadlock.get_status());

  if (cb_actions.get_status() && !tf_actions.get_text().empty()) {
    c.add_option(option_detect_actions).set_argument_value< 0 >(tf_actions.get_text());
  }

  c.add_option(option_trace).set_argument_value< 0 >(cb_trace.get_status());
  c.add_option(option_error_trace).set_argument_value< 0 >(cb_error_trace.get_status());

  if (cb_trace.get_status() || cb_error_trace.get_status()) {
    c.add_option(option_max_traces).set_argument_value< 0 >(tf_max_traces.get_text());
  }
  
  c.add_option(option_confluence_reduction).set_argument_value< 0 >(cb_confluence.get_status());

  if (cb_confluence.get_status()) {
    c.add_option(option_confluent_tau).set_argument_value< 0 >(tf_conf_tau.get_text());
  }
  
  if (cb_max_states.get_status() && !tf_max_states.get_text().empty()) {
    c.add_option(option_max_states).set_argument_value< 0 >(tf_max_states.get_text());
  }
  
  c.add_option(option_bithashing).set_argument_value< 0 >(cb_bithashing.get_status());
  c.add_option(option_bithashsize).set_argument_value< 0 >(tf_bithashsize.get_text());

  c.add_option(option_init_tsize).set_argument_value< 0 >(tf_init_tsize.get_text());
  
  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const &c) const
{
  return c.input_exists(lps_file_for_input) &&
       (!(c.category() == tipi::tool::category::transformation) || c.output_exists(lts_file_for_output));
}

class squadt_interactor::status_display {

  private:

    squadt_interactor&  m_communicator;

    tipi::tool_display display;

    tipi::layout::elements::label&        lb_level;
    tipi::layout::elements::label&        lb_explored;
    tipi::layout::elements::label&        lb_seen;
    tipi::layout::elements::label&        lb_transitions;
    tipi::layout::elements::progress_bar& progbar;

  public:

    status_display(squadt_interactor& c, lts_generation_options&);

    void update(unsigned long, unsigned long long, unsigned long long,
                unsigned long long, unsigned long long);
};

squadt_interactor::status_display::status_display(squadt_interactor& c, lts_generation_options& lgopts) :
        m_communicator(c), display(), lb_level(display.create< tipi::layout::elements::label >()),
        lb_explored(display.create< tipi::layout::elements::label >()),
        lb_seen(display.create< tipi::layout::elements::label >()),
        lb_transitions(display.create< tipi::layout::elements::label >()),
        progbar(display.create< tipi::layout::elements::progress_bar >()) {

  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;
  
  /* Create and add the top layout manager */
  layout::vertical_box& m = display.create< vertical_box >().set_default_margins(margins(0,5,0,5));
  
  m.append(display.create< horizontal_box >().set_default_margins(margins(0,5,0,5)).
        append(display.create< vertical_box >().set_default_alignment(layout::left).
          append(display.create< label >().set_text("Level:")).
          append(display.create< label >().set_text("States explored:")).
          append(display.create< label >().set_text("States seen:")).
          append(display.create< label >().set_text("Transitions:"))).
        append(display.create< vertical_box >().set_default_alignment(layout::right).
          append(lb_level).
          append(lb_explored).
          append(lb_seen).
          append(lb_transitions))).
    append(progbar);

  lgopts.display_status = boost::bind(&squadt_interactor::status_display::update, this, _1, _2, _3, _4, _5);

  m_communicator.send_display_layout(display.manager(m));
}

void squadt_interactor::status_display::update(unsigned long level, unsigned long long explored,
    unsigned long long seen, unsigned long long num_found_same, unsigned long long transitions) {

  lb_level.set_text(boost::lexical_cast < std::string > (level));
  lb_explored.set_text(boost::lexical_cast < std::string > (explored));
  lb_seen.set_text(boost::lexical_cast < std::string > (seen));
  lb_transitions.set_text(boost::lexical_cast < std::string > (transitions));
  if ( seen > 1000000ULL )
  {
    explored = explored/(seen/1000000);
    seen = 1000000ULL;
  }
  if ( explored > seen )
  {
    seen = explored;
  }
  progbar.set_maximum(seen);
  progbar.set_value(explored);
}

std::string add_output_file(tipi::configuration& c, const char* identifier, std::string const& info, std::string const& ext) {
  static unsigned int output_count = 3;

  std::string s(c.get_output_name("_" + info + "." + ext));

  c.add_output(identifier + boost::lexical_cast < std::string> (++output_count), ext, s);

  return s;
}

bool squadt_interactor::perform_task(tipi::configuration &configuration)
{
  lts_generation_options lgopts;
  status_display         display(*this, lgopts);

  lgopts.specification = configuration.get_input(lps_file_for_input).location();

  if (configuration.category() == tipi::tool::category::transformation) {
    lgopts.lts = configuration.get_output(lts_file_for_output).location();

    lgopts.outinfo = configuration.get_option_argument< bool >(option_out_info);

    lgopts.outformat = configuration.get_option_argument< mcrl2::lts::lts_type >(option_lts_type);

    lgopts.usedummies   = configuration.get_option_argument< bool >(option_usedummies);
    lgopts.stateformat  = (configuration.get_option_argument< bool >(option_state_format_tree))?GS_STATE_TREE:GS_STATE_VECTOR;
    lgopts.removeunused = configuration.get_option_argument< bool >(option_removeunused);
  }
  
  lgopts.strat      = configuration.get_option_argument< RewriteStrategy >(option_rewrite_strategy);
  lgopts.expl_strat = configuration.get_option_argument< exploration_strategy >(option_exploration_strategy);
  
  lgopts.detect_deadlock  = configuration.get_option_argument< bool >(option_detect_deadlock);

  if (configuration.option_exists(option_detect_actions)) {
    lgopts.detect_action = true;
    lgopts.trace_actions = parse_action_list(configuration.get_option_argument< std::string >(option_detect_actions).c_str(),&lgopts.num_trace_actions);
  }

  lgopts.save_error_trace = configuration.get_option_argument< bool >(option_error_trace);
  lgopts.trace = configuration.get_option_argument< bool >(option_trace) || lgopts.save_error_trace;

  if (lgopts.trace) {
    lgopts.max_traces = strtoul(configuration.get_option_argument< std::string >(option_max_traces).c_str(),0,0);
  }
  
  if (configuration.get_option_argument< bool >(option_confluence_reduction)) {
    lgopts.priority_action = strdup(configuration.get_option_argument< std::string >(option_confluent_tau).c_str());
  }
  
  lgopts.max_states = (configuration.option_exists(option_max_states)) ?
     boost::lexical_cast < boost::uint64_t > (configuration.get_option_argument< std::string >(option_max_states)) :
     DEFAULT_MAX_STATES;

  lgopts.bithashing = configuration.get_option_argument< bool >(option_bithashing);

  if (lgopts.bithashing) {
    std::string bithashsize_as_string = configuration.get_option_argument< std::string >(option_bithashsize);

    lgopts.bithashsize = (bithashsize_as_string.empty()) ?
          0 : boost::lexical_cast < boost::uint64_t > (bithashsize_as_string);
  }
  
  lgopts.initial_table_size = strtoul(configuration.get_option_argument< std::string >(option_init_tsize).c_str(),0,0);

  /* Register trace add function */
  lgopts.generate_filename_for_trace = boost::bind(::add_output_file, configuration, trc_file_for_output, _1, _2);

  bool ok = false;
  if ( initialise_lts_generation(&lgopts) )
  {
    ok = generate_lts();
    ok = ok || lgopts.error_trace_saved;

    ok &= finalise_lts_generation();
  }
  
  return ok;
}

#endif
