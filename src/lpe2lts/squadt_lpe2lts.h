#ifndef _SQUADT_LPE2LTS_H
#define _SQUADT_LPE2LTS_H

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include "squadt_program.h"

class squadt_lpe2lts: public squadt_program
{
  public:
    squadt_lpe2lts() {};
    virtual ~squadt_lpe2lts() {};
  
  public:
    void create_status_display();
    void update_status_display(unsigned long level,
                               unsigned long long explored,
                               unsigned long long seen,
                               unsigned long long num_found_same,
                               unsigned long long transitions);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:
    void initialise();
    void configure(sip::configuration::sptr configuration);
    bool check_configuration(sip::configuration::sptr configuration);
    void execute(sip::configuration::sptr configuration);
    void finalise();

  protected:
    sip::layout::tool_display::sptr status_display;
    sip::layout::manager::aptr layout_manager;
    sip::layout::manager::aptr labels;
    sip::layout::elements::label *lb_level;
    sip::layout::elements::label *lb_explored;
    sip::layout::elements::label *lb_seen;
    sip::layout::elements::label *lb_transitions;
    sip::layout::elements::progress_bar *progbar;
#endif
};

#endif
