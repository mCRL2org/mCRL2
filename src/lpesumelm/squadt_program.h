#ifndef _SQUADT_PROGRAM_H
#define _SQUADT_PROGRAM_H

#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <sip/tool.h>
#endif // _ENABLE_SQUADT_CONNECTIVITY

class squadt_program
{
#ifdef ENABLE_SQUADT_CONNECTIVITY
  // Methods to be implemented
  protected:
    virtual void set_capabilities();
    virtual void initialise();
    virtual void configure(sip::configuration &configuration);
    virtual bool check_configuration(sip::configuration &configuration);
    virtual void execute(sip::configuration &configuration);
    virtual void finalise();
#endif // _ENABLE_SQUADT_CONNECTIVITY

  public:
    // Main function that enters event loop and calls above functions
    squadt_program();
    virtual ~squadt_program();
    bool run(int argc = 0, char **argv = 0);
  
#ifdef ENABLE_SQUADT_CONNECTIVITY
  protected:
    bool is_active();
    void error(std::string message);

  protected:
    sip::tool::communicator tc;

  private:
    bool active;
#endif // _ENABLE_SQUADT_CONNECTIVITY
};

#endif // _SQUADT_PROGRAM_H
