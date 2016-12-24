#include <QGLWidget>

QImage convertToGLFormat(const QImage& img)
{
  return QGLWidget::convertToGLFormat(img);
}
