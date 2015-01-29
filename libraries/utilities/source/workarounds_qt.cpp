#include <QApplication>
#include <QDir>

#ifdef Q_OS_DARWIN
#include <mach-o/dyld.h>
#endif

void work_around_qtbug_38598()
{
#ifdef Q_OS_DARWIN
  /// TODO: Remove the following four lines when QTBUG-38598 is fixed
  uint32_t bufsize = 0;
  char* buf = NULL;
  _NSGetExecutablePath(buf, &bufsize);
  buf = new char[bufsize];
  _NSGetExecutablePath(buf, &bufsize);
  QDir dir = QFileInfo(buf).dir();
  delete[] buf;
  dir.cdUp();
  dir.cd("PlugIns");
  QApplication::setLibraryPaths(QStringList(dir.absolutePath()) + QApplication::libraryPaths());
#endif
}
