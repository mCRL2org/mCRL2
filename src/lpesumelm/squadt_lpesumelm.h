#ifndef _SQUADT_LPESUMELM_H
#define _SQUADT_LPESUMELM_H

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif

#include "squadt_program.h"

#include <atermpp/aterm.h>
#include <lpe/lpe.h>
#include <lpe/specification.h>

class squadt_lpesumelm: public squadt_program
{
  public:
    squadt_lpesumelm() {};
    ~squadt_lpesumelm() {};

    int do_sumelm();

  protected:
#ifdef ENABLE_SQUADT_CONNECTIVITY
    void set_capabilities();
    void initialise();
    void configure(sip::configuration &configuration);
    bool check_configuration(sip::configuration &configuration);
    void execute(sip::configuration &configuration);
    void finalise();
#endif
    lpe::specification sumelm(const lpe::specification& specification);
};

#endif //_SQUADT_LPESUMELM_H
    
