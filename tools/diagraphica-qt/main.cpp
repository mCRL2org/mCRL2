#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/qt_tool.h"
#include "mainwindow.h"

using namespace mcrl2::utilities;

typedef qt::qt_tool<tools::input_tool> diagraphica_base;
class diagraphica_tool : public diagraphica_base
{
  public:
    diagraphica_tool():
      diagraphica_base("DiaGraphica",
        (QStringList() << "Hannes Pretorius" << "Ruud Koolen" << "Rimco Boudewijns").join(", ").toStdString(),
        "interactive visual analysis of an LTS", // what-is
        "", // command-line description
        "You are free to use images produced with DiaGraphica.\n" // gui-specific description
        "In this case, image credits would be much appreciated.\n"
        "\n"
        "Color schemes were chosen with ColorBrewer (www.colorbrewer.org).",
        "Multivariate state visualisation and simulation analysis for labelled "
        "transition systems (LTS's) in the FSM format. If an INFILE is not supplied then "
        "DiaGraphica is started without opening an LTS.",
        "http://mcrl2.org/release/user_manual/tools/diagraphica.html")
    {}

    bool run()
    {
      MainWindow *window = new MainWindow();

      if (!m_input_filename.empty())
      {
        window->open(QString::fromStdString(m_input_filename));
      }

      return show_main_window(window);
    }
};

int main(int argc, char *argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return diagraphica_tool().execute(argc, argv);
}
