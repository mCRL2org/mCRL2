#ifndef _SQUADT_LPE2LTS_H
#define _SQUADT_LPE2LTS_H

#include <string>

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include "utility/squadt_tool_interface.h"
#endif

#ifdef ENABLE_SQUADT_CONNECTIVITY
class squadt_lpe2lts: public squadt_tool_interface
#else
class squadt_lpe2lts
#endif
{
  public:
    squadt_lpe2lts() {};
  
  public:
    void create_status_display();
    void update_status_display(unsigned long level,
                               unsigned long long explored,
                               unsigned long long seen,
                               unsigned long long num_found_same,
                               unsigned long long transitions);
    std::string add_output_file(std::string info, std::string ext);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:
    void initialise();
    void set_capabilities(sip::tool::capabilities &capabilities) const;
    void user_interactive_configuration(sip::configuration &configuration);
    bool check_configuration(sip::configuration const &configuration) const;
    bool perform_task(sip::configuration &configuration);
    void finalise();

  protected:
    sip::layout::tool_display::sptr status_display;
    sip::layout::manager::aptr layout_manager;
    sip::layout::elements::label *lb_level;
    sip::layout::elements::label *lb_explored;
    sip::layout::elements::label *lb_seen;
    sip::layout::elements::label *lb_transitions;
    sip::layout::elements::progress_bar *progbar;
    sip::configuration *config;
    bool config_changed;
    unsigned int output_count;
#else
  public:
    inline bool is_active() { return false; }
#endif
};

#endif
