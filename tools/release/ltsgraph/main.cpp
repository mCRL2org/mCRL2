// Author(s): Rimco Boudewijns
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/input_tool.h"
#include "mainwindow.h"
#include "mcrl2/gui/qt_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;

typedef gui::qt::qt_tool<tools::input_tool> ltsgraph_base;
class ltsgraph_tool : public ltsgraph_base
{
  public:
    ltsgraph_tool():
      ltsgraph_base("LTSGraph",
                    "Rimco Boudewijns and Sjoerd Cranen",
                    "visualise an LTS as a graph and manipulate its layout in 2D and 3D",
                    "Tool for visualizing a labelled transition systems as a graph, and optimizing graph layout. "
                    "When INFILE is -, an LTS is read from standard input.",
                    "Draws a labelled transition system (LTS). A convential graph drawing is provided either in 2D or 3D.",
                    "http://www.mcrl2.org/web/user_manual/tools/release/ltsgraph.html")
    {
      // Create an OpenGL 3.3 surface without depth, alpha and stencil buffers and with vsync enabled.
      QSurfaceFormat surfaceFormat = QSurfaceFormat::defaultFormat();
      surfaceFormat.setVersion(3, 3);
      // There are no Core profile features explicitly used, but some MacOS versions require Core profile as a 'hint' to 
      //   properly use OpenGL 3.x functionality. Removing this line will cause issues with MacOS eventually.
      surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
      surfaceFormat.setStencilBufferSize(1);
      //surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
      surfaceFormat.setSwapInterval(1);

      // Enable a surface with multisampling.
      surfaceFormat.setSamples(16);

      // We use the GL_KHR_debug extension to provide realtime logging of OpenGL errors.
      // Ruben: Disabled because debug logging became unusable and every text draw call produced 3 lines of output per node/label
      surfaceFormat.setOption(QSurfaceFormat::DebugContext, false);

      // Qt: Calling QSurfaceFormat::setDefaultFormat() before constructing the QApplication instance
      //     is mandatory on some platforms (for example, macOS) when an OpenGL core profile context is requested.
      QSurfaceFormat::setDefaultFormat(surfaceFormat);
    }

    bool run() override
    {
      // This tool requires at least Qt 5.9 as runtime library.
      QVersionNumber runtime_version = QVersionNumber::fromString(qVersion());
      QVersionNumber required_version(5,9,0);

      if (QVersionNumber::compare(required_version, runtime_version) > 0)
      {
        // Print a message to the console and show a message box.
        std::stringstream message;

        message << "The runtime version of Qt (" << runtime_version.toString().toStdString() << ") is below the least supported version of Qt ("
          << required_version.toString().toStdString() << ").";
        mCRL2log(mcrl2::log::error) << message.str().c_str() << "\n";

        QMessageBox box(QMessageBox::Warning, "Unsupported Qt Version", message.str().c_str(), QMessageBox::Ok);
        box.exec();
      }

      MainWindow window;

      if (!m_input_filename.empty())
      {
        window.delayedOpenFile(QString::fromStdString(m_input_filename));
      }

      return show_main_window(window);
    }
};

int main(int argc, char* argv[])
{
  return ltsgraph_tool().execute(argc, argv);
}
