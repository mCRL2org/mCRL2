#include <cstdio>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include <getopt.h>

#include <boost/filesystem/operations.hpp>

#define PROGRAM_NAME    "maximum_revision"
#define PROGRAM_VERSION "1.0.2"

/* Works under the assumption of valid XML as input */

void usage() {
  std::cout << "Usage: " << PROGRAM_NAME << " [options] [directory]\n"
            << std::endl
            << " Recursively computes the maximum revision number from any"
            << " subdirectory of a working copy of a subversion repository."
            << std::endl
            << "Where options is a combination of:" << std::endl
            << std::endl
            << "      --cpp-define [ name ]   output CPP definition\n"
            << "      --cpp-file < filename > create or update CPP header file\n"
            << "  -h, --help                  show this information\n"
            << "      --version               show program version\n"
            << std::endl
            << "Prints the maximum SVN revision of any file in <directory>\n"
            << "and otherwise current directory, or any of its subdirectories.\n"
            << std::endl
            << "Report bugs to J.v.d.Wulp@tue.nl.\n";

  exit(0);
}

namespace bf = boost::filesystem;

/* Whether the output is a CPP define or not */
bool        output_cpp_definition = false;
bool        output_cpp_file       = false;

/* The name of the CPP macro to define as the maximum revision number */
std::string cpp_variable_name     = "REVISION";

/* The name of the file to update with the maximum revision number */
std::string cpp_file_name         = "revision.h";

bf::path    target_directory = bf::current_path();

void process_command_line_options (const int argc, char** argv) {
  static struct option long_options[] = {
    { "cpp-define" , optional_argument, 0, 0   },
    { "cpp-file"   , required_argument, 0, 0   },
    { "version"    , no_argument,       0, 0   },
    { "help"       , no_argument,       0, 'h' },
    { 0            , 0,                 0, 0   }
  };

  int i = 0;
  int c = getopt_long(argc, argv, "h", long_options, &i);

  while (c != -1) {
    switch (c) {
      case 0:
          switch (i) {
            case 0:
              output_cpp_definition = true;

              if (optarg != 0) {
                cpp_variable_name.assign(optarg);
              }

              break;
            case 1:
              output_cpp_file = true;

              cpp_file_name.assign(optarg);

              break;
            case 2:
              unsigned int svn_revision;

              sscanf("$Revision$", "$%*s %u $", &svn_revision);

              printf("`%s' %s revision %u copyright (c) 2005\n", PROGRAM_NAME, PROGRAM_VERSION, svn_revision);
              exit(0);
              break;
          }
        break;
      case 'h':
        usage();
    }

    c = getopt_long(argc, argv, "h", long_options, &i);
  }

  while (optind < argc) {
    target_directory = bf::path(argv[argc - 1]);
  }
}

const char*  target = "committed-rev=\"";

/* Returns the maximum found in <|path|> */
unsigned int explore_path(bf::path apath) {
  unsigned int return_value = 0;

  try {
    bf::directory_iterator i(apath);

    while (i != bf::directory_iterator()) {
      if (bf::is_directory(*i)) {
        if ((*i).leaf() == ".svn") {
          try {
            std::ifstream file((*i / bf::path("entries")).string().c_str());

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
          unsigned int temporary = explore_path(*i);

          if (return_value < temporary) {
            return_value = temporary;
          }
        }
      }

      ++i;
    }
  }
  catch (...) {
    std::cerr << "Error: Cannot not read directory `" << apath.string() << "'\n";
  }

  return (return_value);
}

/**
 * Tries to extract the revision number and returns it, otherwise returns 0
 **/
unsigned int extract_revision(bf::path& f) {
  unsigned int revision = 0;

  std::ifstream file(f.string().c_str());

  while (file) {
    std::string::iterator i = cpp_variable_name.begin();

    char c;
    
    file.get(c);
  
    while (file && *i != '\0' && c == *i) {
      ++i;
  
      file.get(c);
    }
  
    if (i == cpp_variable_name.end()) {
      /* We have a match, read the number */
      revision = 0;

      while (file && !isdigit(c) && !iscntrl(c)) {
        file.get(c);
      }

      while (file && isdigit(c)) {
        revision = revision * 10 + (c - '0');

        file.get(c);
      }
    }
  }

  file.close();
           
  return (revision);
}

int main(int argc, char **argv) {
  process_command_line_options(argc, argv);

  if (!bf::exists(target_directory)) {
    std::cerr << "Error: Cannot not read directory `" << target_directory.string() << "'\n";

    return (1);
  }

  unsigned int revision = explore_path(target_directory);

  if (output_cpp_file) {
    bf::path target_file(cpp_file_name);

    if (bf::exists(target_file)) {
      /* File exists, try to extract the revision number */
      if (revision <= extract_revision(target_file)) {
        output_cpp_file = false;
      }
    }

    if (output_cpp_file) {
      /* Create or overwrite file */
      std::ofstream f(cpp_file_name.c_str(), std::ios::out | std::ios::trunc);

      std::string guard = bf::path(cpp_file_name).leaf();

      std::replace(guard.begin(), guard.end(), '.', '_');
      std::replace(guard.begin(), guard.end(), '/', '_');
      std::replace(guard.begin(), guard.end(), '\\', '_');
      std::transform(guard.begin(), guard.end(), guard.begin(), toupper);

      f << "#ifndef " << guard << std::endl
        << "# define " << guard << std::endl
        << "# define " << cpp_variable_name << " " << revision << std::endl
        << "#endif" << std::endl;

      f.close(); 
    }
  }
  else if (output_cpp_definition) {
    std::cout << "#define " << cpp_variable_name << " " << revision << std::endl;
  }
  else {
    std::cout << revision;
  }

  return(0);
}
