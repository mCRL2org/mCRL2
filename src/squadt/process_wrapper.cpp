#include <io.h>
#include <process.h>

/* TODO
 *
 * add signal handler for termination that kills the spawned tasks
 */

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
  if (2 < argc) {
    if (_chdir(argv[1]) == 0) {
      if (3 < argc) {
        execvp(argv[2], const_cast < char* const* > (argv + 3));
      }
      else {
        execlp(argv[2], 0);
      }
    }
  }

  return (1);
}

