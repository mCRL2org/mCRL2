#include "mcrl2/utilities/tool.h"
#include "mcrl2/gui/qt_tool.h"
#include "mainwindow.h"

using namespace mcrl2;

typedef gui::qt::qt_tool<utilities::tools::tool> mcrl2ide_base;

class mcrl2ide_tool : public mcrl2ide_base
{
  public:
    mcrl2ide_tool():
      mcrl2ide_base("mCRL2ide",
                   "Olav Bunte",
                   "A graphical IDE for creating, analyzing, visualizing and verifying mCRL2 specifications.",
                   "A graphical IDE for creating, analyzing, visualizing and verifying mCRL2 specifications.",
                   "A graphical IDE for creating, analyzing, visualizing and verifying mCRL2 specifications.",
                   "http://www.mcrl2.org/web/user_manual/tools/experimental/mcrl2ide.html")
    {}

    bool run()
    {
      MainWindow *window = new MainWindow();
      return show_main_window(window);
    }
};

int main(int argc, char *argv[])
{
    return mcrl2ide_tool().execute(argc, argv);
}
