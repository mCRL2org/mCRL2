#define MCRL2_MAX_LOG_LEVEL debug3
#include "mcrl2/utilities/logger.h"

using namespace mcrl2::log;

void do_something_special()
{
  mCRL2log(debug3, "my_algorithm") << "doing something special" << std::endl;
}

std::string my_algorithm()
{
  mCRL2log(debug, "my_algorithm") << "Starting my_algorithm" << std::endl;
  int iterations = 3;
  mCRL2log(debug1, "my_algorithm") << "A loop with " << iterations << " iterations" << std::endl;
  for(int i = 0; i < iterations; ++i)
  {
    mCRL2log(debug2, "my_algorithm") << "Iteration " << i << std::endl;
    if(i >= 2)
    {
      mCRL2log(debug3, "my_algorithm") << "iteration number >= 2, treating specially" << std::endl;
      do_something_special();
    }
  }
  return "my_algorithm";
}

int main(int /*argc*/, char** /*argv*/)
{
  mcrl2_logger::set_reporting_level(debug3);

  mCRL2log(info) << "This shows the way info messages are printed, using the default messages" << std::endl;
  mCRL2log(debug) << "This line is not printed, and the function " << my_algorithm() << " is not evaluated" << std::endl;

  FILE* plogfile;
  plogfile = fopen("logger_test_file.txt" , "w");
  if(plogfile == nullptr)
  {
    throw std::runtime_error("Cannot open logfile for writing");
  }
  file_output::set_stream(plogfile, "my_algorithm");
  mcrl2_logger::set_reporting_level(debug3, "my_algorithm");

  // Execute algorithm
  my_algorithm();

  // Do not forget to close the file.
  fclose(plogfile);
}
