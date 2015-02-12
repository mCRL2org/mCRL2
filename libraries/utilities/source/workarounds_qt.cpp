#include <QApplication>
#include <QDir>
#include "mcrl2/utilities/basename.h"

/// TODO: remove this function when QTBUG-38498 is resolved
void work_around_qtbug_38598()
{
#ifdef Q_OS_DARWIN
  QDir dir = QDir(QString::fromStdString(mcrl2::utilities::get_executable_basename()));
  dir.cdUp();
  dir.cd("PlugIns");
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()) + QApplication::libraryPaths());
#endif
}
