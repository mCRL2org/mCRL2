#ifndef LAYOUTFUNCTIONS_H
#define LAYOUTFUNCTIONS_H

#include "layoututility.h"
#include <QVector3D>

// Defines abstract attraction function
// Operator (a, b, ideal) should return the amount of attraction
//   a experiences towards b, where ideal is a parameter
struct AttractionFunction
{
  virtual QVector3D operator()(const QVector3D& a, const QVector3D& b, float ideal) = 0;
};

// Defines abstract repulsion function
// Operator (a, b, ideal) should return the amount of repulsion
//  a experiences as a result of b, where ideal is a parameter
struct RepulsionFunction
{
  virtual QVector3D operator()(const QVector3D& a, const QVector3D& b, float ideal) = 0;
};

#endif
