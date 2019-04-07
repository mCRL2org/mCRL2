// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_UTILITY_H
#define MCRL2_LTSGRAPH_UTILITY_H

#include <QPainter>
#include <QVector3D>

#include <cmath>

/// \file This file contains various utility commands, instead of defining them inline in source files, such that they can be reused and/or
/// moved to more logical locations.

/// \brief Execute the given QT OpenGL function that returns a boolean; logs error and aborts when it failed.
#define MCRL2_QGL_VERIFY(x) \
  if (!x) { mCRL2log(mcrl2::log::error) << #x " failed.\n"; std::abort(); }

/// \brief Checks for OpenGL errors, prints it and aborts.
#define MCRL2_OGL_CHECK() \
  { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(result) << "\n"; std::abort(); } }

/// \brief Executes x and checks for errors afterwards.
#define MCRL2_OGL_VERIFY(x) \
  x; { GLenum result = glGetError(); if (result != GL_NO_ERROR) { mCRL2log(mcrl2::log::error) << "OpenGL error: " #x " failed with " << gluErrorString(result) << "\n"; std::abort(); } }

/// \brief Constants for pi and pi/2.
constexpr float PI = 3.14159265358979323846f;
constexpr float PI_2 = PI * 0.5f;

/// \brief Renders text, centered around the window coordinates at x and y (in pixels)
inline static
void drawCenteredText(QPainter& painter, float x, float y, const QString& text, const QColor& color = Qt::black)
{
  QFontMetrics metrics(painter.font());
  QRect bounds = metrics.boundingRect(text);
  qreal w = bounds.width();
  qreal h = bounds.height();
  painter.setPen(color);
  painter.drawText(x - w / 2, y - h / 2, text);
}

/// \brief Converts a QVector3D of floats [0,1] to a QColor object with integers [0,255] for colors.
inline static
QColor vectorToColor(const QVector3D& vector)
{
  return QColor(vector.x() * 255, vector.y() * 255, vector.z() * 255);
}

/// \returns The given value clamped in a range [min, max] (given by min and max values).
template<typename T>
inline static
T clamp(T value, T min, T max)
{
  return std::min(std::max(value, min), max);
}

/// \returns A linear interpolation between a and b using the given value. Often called lerp, but called mix in GLSL.
inline static
QVector3D mix(float value, QVector3D a, QVector3D b)
{
  return (1 - value) * a + (value * b);
}

/// \returns The angle in degrees [0, 180] from a given angle in radians [0, PI].
inline static
float radiansToDegrees(float radians)
{
  return 180.0f / PI * radians;
}

/// \returns The angle in radians [0, pi] from a given angle in degrees [0, 180].
inline static
float degreesToRadians(float degrees)
{
  return degrees / 180.0f * PI;
}

/// \returns True whenever the given window coordinates (and device coordinate depth), given by pos, is within a circle of radius threshold
///          centered at (x, y) in window coordinates. Returns the smallest Z coordinate that is close.
inline static
bool isCloseCircle(int x, int y, const QVector3D& pos, float threshold, float& bestZ)
{
  float distance = std::sqrt(std::pow(pos.x() - x, 2) + std::pow(pos.y() - y, 2));
  if (distance < threshold && pos.z() < bestZ)
  {
    bestZ = pos.z();
    return true;
  }

  return false;
}

/// \returns True whenever the given window coordinate, given by pos, is within a square
/// centered at (x,y) with a width and height of 2*threshold. The smallest Z coordinate that is close is stored in bestZ.
inline static
bool isCloseSquare(int x, int y, const QVector3D& pos, float threshold, float& bestZ)
{
  if(std::abs(x - pos.x()) < threshold && std::abs(y - pos.y()) < threshold && pos.z() < bestZ)
  {
    bestZ = pos.z();
    return true;
  }
  return false;
}


/// \returns True whenever the given (x, y) position (in pixels) is on the text positioned at the window coordinates.
inline static
bool isOnText(int x,
  int y,
  const QString& text,
  const QVector3D& window,
  const QFontMetrics& metrics)
{
  if (text.isEmpty())
  {
    return false;
  }

  QRect bounds = metrics.boundingRect(text);
  int w = bounds.width() / 2;
  int h = bounds.height() / 2;
  bounds.adjust(window.x() - w, window.y() - h, window.x() - w, window.y() - h);
  return bounds.contains(x, y);
}


#endif // MCRL2_LTSGRAPH_UTILITY_H
