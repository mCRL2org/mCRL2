#include <io.h>
#include <process.h>

/*
 * Process wrapper that changes the working directory and then executes another program
 *
 * Command line arguments
 *
 *  1    working directory
 *  2    executable
 *  3... command arguments to pass along
 */
int main(int argc, char** argv) {

  if (3 < argc) {
    if (_chdir(argv[0]) == 0) {
      execv(argv[1], const_cast < char* const* > (argv + 2));
    }
  }

  return (1);
}

