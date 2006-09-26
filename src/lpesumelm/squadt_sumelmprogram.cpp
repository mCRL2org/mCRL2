#ifndef ENABLE_SQUADT_CONNECTIVITY

#include "squadt_utility.h"
#include "squadt_program.h"

squadt_program::squadt_program()
{
  active = false;
}

squadt_program::~squadt_program()
{
}

bool squadt_program::run(int argc, char **argv)
{
  active = false;
  return false;
}

#else

#include <string>
#include "squadt_utility.h"
#include "squadt_program.h"

using namespace std;

squadt_program::squadt_program()
{
  active = false;
}

squadt_program::~squadt_program()
{
}

void squadt_program::set_capabilities()
{
}

void squadt_program::initialise()
{
}

void squadt_program::configure(sip::configuration &configuration)
{
}

bool squadt_program::check_configuration(sip::configuration &configuration)
{
  return true;
}

void squadt_program::execute(sip::configuration &configuration)
{
}

void squadt_program::finalise()
{
}

bool squadt_program::run(int argc, char **argv)
{
  //sip::tool::communicator::get_standard_error_logger()->set_filter_level(3);

  set_capabilities();

  if ( tc.activate(argc,argv) )
  {
    active = true;

    squadt_utility::initialise(tc);
  
    initialise();

    bool notdone = true;
    while ( notdone )
    {
      sip::message_ptr m = tc.await_message(sip::message_any);
  
      switch ( m->get_type() )
      {
        case sip::message_offer_configuration:
          {
            sip::configuration &conf = tc.get_configuration();
            if ( conf.is_fresh() )
            {
              configure(conf);
            }
	    if ( check_configuration(conf) )
            {
              tc.send_accept_configuration();
            }
          }
          break;
        case sip::message_signal_start:
          {
            sip::configuration &conf = tc.get_configuration();
            execute(conf);
            tc.send_signal_done(true);
            break;
          }
        case sip::message_request_termination:
          tc.send_signal_termination();
          notdone = false;
          break;
        default:
          break;
      }
    }

    finalise();

    return true;
  } else {
    active = false;
    return false;
  }
}

bool squadt_program::is_active()
{
  return active;
}

void squadt_program::error(string message)
{
  tc.send_status_report(sip::report::error,message);
}

#endif
