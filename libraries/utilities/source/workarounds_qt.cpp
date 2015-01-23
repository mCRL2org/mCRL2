#include <QApplication>
#include <QDir>

void work_around_qtbug_38598(const char* argv0)
{
#ifdef Q_OS_DARWIN
  /// TODO: Remove the following four lines when QTBUG-38598 is fixed
  QDir dir = QFileInfo(argv0).dir();
  dir.cdUp();
  dir.cd("PlugIns");
  QApplication::addLibraryPath(dir.absolutePath());
#endif
}
