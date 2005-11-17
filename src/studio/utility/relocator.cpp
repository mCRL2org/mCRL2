#include <getopt.h>
#include <string.h>
#include <iostream>
#include <cstdio>

#include "../tool_manager.h"
#include "../settings_manager.h"

#include <boost/filesystem/operations.hpp>

#define PROGRAM_NAME    "relocator"
#define PROGRAM_VERSION "1.0.0"

SettingsManager settings_manager("", "");

/* Holds filename for the input */
char*         file_name = "examples/tool_catalog.xml";
char*         path;

class MTool : private Tool {
  public:
    inline void SetLocation(const std::string new_location) {
      location = new_location;
    }
};

void usage() {
  std::cout << "Usage: " << PROGRAM_NAME << " [options] [path] [file name]\n";
  std::cout << std::endl;
  std::cout << "Where options is a combination of:" << std::endl;
  std::cout << std::endl;
  std::cout << "  -h, --help         show this information\n";
  std::cout << "      --version      show program version\n";
  std::cout << std::endl;
  std::cout << "Updates the locations where mcrl2 tools are installed for a tool catalog file.\n";
  std::cout << std::endl;
  std::cout << "Report bugs to J.v.d.Wulp@tue.nl. \n";

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

  if (argc - optind != 2) {
    std::cerr << "Error: invalid number of arguments.\n";
  }

  path      = new char[strlen(argv[optind]) + 1];

  strcpy(path, argv[optind]);

  ++optind;

  file_name = new char[strlen(argv[optind]) + 1];

  strcpy(file_name, argv[optind]);
}

/*****************************************************************
 * Validation with respect to the XML schema file is assumed
 *****************************************************************/
int main(int argc, char **argv) {
  ToolManager             tool_manager;

  processCommandLineOptions(argc, argv);

  boost::filesystem::path input_file(file_name, boost::filesystem::portable_posix_name);

  settings_manager.SetHomeDirectory(input_file.branch_path().string().c_str());
  settings_manager.SetToolCatalogName(input_file.leaf());

  /* Reading configuration from the standard data directory */
  try {
    tool_manager.Load();

    std::list < Tool* >::const_iterator i = tool_manager.GetTools().begin();
    std::list < Tool* >::const_iterator b = tool_manager.GetTools().end();

    while (i != b) {
      std::string new_path(path);

      reinterpret_cast < MTool* > (*i)->SetLocation(new_path.append("/").append(boost::filesystem::path((*i)->GetLocation(), boost::filesystem::portable_posix_name).leaf()));

      ++i;
    }

    tool_manager.Store();
  }
  catch (...) {
    std::cerr << "Error: Something went wrong.\n";

    return(1);
  }

  return(0);
}
