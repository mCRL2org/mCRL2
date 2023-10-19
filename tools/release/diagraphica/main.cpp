#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/gui/qt_tool.h"
#include "mainwindow.h"

using namespace mcrl2;
using namespace mcrl2::utilities;

typedef gui::qt::qt_tool<tools::input_tool> diagraphica_base;
class diagraphica_tool : public diagraphica_base
{
  public:
    diagraphica_tool():
      diagraphica_base("DiaGraphica",
        (QStringList() << "Hannes Pretorius" << "Ruud Koolen" << "Rimco Boudewijns").join(", ").toStdString(),
        "interactive visual analysis of an LTS", // what-is
        "Multivariate state visualisation and simulation analysis for labelled " // command-line description
        "transition systems (LTS's) in the FSM format. If an INFILE is not supplied then "
        "DiaGraphica is started without opening an LTS.",
        "You are free to use images produced with DiaGraphica.\n" // gui-specific description
        "In this case, image credits would be much appreciated.\n"
        "\n"
        "Color schemes were chosen with ColorBrewer (www.colorbrewer.org).",
        "http://www.mcrl2.org/web/user_manual/tools/release/diagraphica.html")
    {    
        // Create an OpenGL 3.3 surface without depth, alpha and stencil buffers and with vsync enabled.
        QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
        surfaceFormat.setVersion(2, 0);
        // There are no Core profile features explicitly used, but some MacOS versions require Core profile as a 'hint' to 
        //   properly use OpenGL 3.x functionality. Removing this line will cause issues with MacOS eventually.
        surfaceFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
        //surfaceFormat.setStencilBufferSize(1);
        //surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        //surfaceFormat.setSwapInterval(1);

        // Enable a surface with multisampling.
        surfaceFormat.setSamples(16);

        // We use the GL_KHR_debug extension to provide realtime logging of OpenGL errors.
        surfaceFormat.setOption(QSurfaceFormat::DebugContext, true);

        // Qt: Calling QSurfaceFormat::setDefaultFormat() before constructing the QApplication instance
        //     is mandatory on some platforms (for example, macOS) when an OpenGL core profile context is requested.
        QSurfaceFormat::setDefaultFormat(surfaceFormat);
    }

    bool run()
    {
      MainWindow window;

      if (!m_input_filename.empty())
      {
        window.open(QString::fromStdString(m_input_filename));
      }

      return show_main_window(window);
    }
};

int main(int argc, char *argv[])
{
  return diagraphica_tool().execute(argc, argv);
}
