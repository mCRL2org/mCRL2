#include <getopt.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdio>

#include <boost/filesystem/operations.hpp>

#define PROGRAM_NAME    "svn_revision"
#define PROGRAM_VERSION "1.0.1"

/* Works under the assumption of valid XML as input */

void usage() {
  std::cout << "Usage: " << PROGRAM_NAME << " [options]\n";
  std::cout << std::endl;
  std::cout << "Where options is a combination of:" << std::endl;
  std::cout << std::endl;
  std::cout << "  -h, --help         show this information\n";
  std::cout << "      --version      show program version\n";
  std::cout << std::endl;
  std::cout << "Prints the maximum svn revision of any file in the current\n";
  std::cout << "directory or any of its subdirectories.\n";
  std::cout << std::endl;
  std::cout << "Report bugs to J.v.d.Wulp@tue.nl.\n";

  exit(0);
}

void processCommandLineOptions (const int argc, char** argv) {
  static struct option long_options[] = {
    {"version"    , 0, NULL, 0},
    {"help"       , 0, NULL, 0},
  };

  int i = 0;
  int c = getopt_long(argc, argv, "h", long_options, &i);

  while (c != -1) {
    switch (c) {
      case 0:
          switch (i) {
            case 0:
              unsigned int svn_revision;

              sscanf("$Revision$", "$%*s %u $", &svn_revision);

              printf("%s %s revision %u copyright (c) 2005\n", PROGRAM_NAME, PROGRAM_VERSION, svn_revision);
              exit(0);
              break;
            case 1:
              usage();
          }
        break;
      case 'h':
      default:
        usage();
    }

    c = getopt_long(argc, argv, "h", long_options, &i);
  }
}

using namespace boost::filesystem;

const char*  target = "committed-rev=\"";

/* Returns the maximum found in <|path|> */
unsigned int ExplorePath(path apath) {
  unsigned int return_value = 0;

  try {
    directory_iterator i(apath);

    while (i != directory_iterator()) {
      if (is_directory(*i)) {
        if ((*i).leaf() == ".svn") {
          try {
            std::fstream file((*i / path("entries")).string().c_str(), std::ios_base::in);

            while (file) {
              size_t i = 0;
              char   c;
              
              file.get(c);
           
              while (file && target[i] != '\0' && c == target[i]) {
                ++i;
           
                file.get(c);
              }
           
              if (target[i] == '\0') {
                /* We have a match, read the number */
                i = 0;

                while (c != '"') {
                  i = i * 10 + (c - '0');

                  file.get(c);
                }
           
                if (return_value < i) {
                  return_value = i;
                }
              }
            }

            file.close();
          }
          catch (int n) {
            if (n < 0) {
              std::cerr << "Error: while processing XML.\n";
            }
          }
        }
        else {
          unsigned int temporary = ExplorePath(*i);

          if (return_value < temporary) {
            return_value = temporary;
          }
        }
      }

      ++i;
    }
  }
  catch (...) {
    std::cerr << "Error: Something went wrong.\n";
  }

  return (return_value);
}

/*****************************************************************
 * Validation with respect to the XML schema file is assumed
 *****************************************************************/
int main(int argc, char **argv) {
  processCommandLineOptions(argc, argv);

  std::cout << ExplorePath(current_path());

  return(0);
}
