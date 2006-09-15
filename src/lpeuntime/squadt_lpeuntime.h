#ifndef _SQUADT_LPEUNTIME_H
#define _SQUADT_LPEUNTIME_H

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include "squadt_program.h"

#include <atermpp/aterm.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>

class squadt_lpeuntime: public squadt_program
{
  public:
    squadt_lpeuntime() {};
    ~squadt_lpeuntime() {};

    int do_untime();

  protected:
#ifdef ENABLE_SQUADT_CONNECTIVITY
    void set_capabilities();
    void initialise();
    void configure(sip::configuration &configuration);
    bool check_configuration(sip::configuration &configuration);
    void execute(sip::configuration &configuration);
    void finalise();
#endif
    lpe::specification untime(const lpe::specification specification);
};

#endif //_SQUADT_LPEUNTIME_H
    
