#ifndef LAYOUTFUNCTIONS_H
#define LAYOUTFUNCTIONS_H

#include "layoututility.h"
#include <QVector3D>

// Defines abstract attraction function
// Operator (a, b, ideal) should return the amount of attraction
//   a experiences towards b, where ideal is a parameter
struct AttractionFunction
{
    virtual QVector3D operator()(const QVector3D& a, const QVector3D& b,
        const float ideal) = 0;
    void update() {};
    void reset() {};
};

// Defines abstract repulsion function
// Operator (a, b, ideal) should return the amount of repulsion
//  a experiences as a result of b, where ideal is a parameter
struct RepulsionFunction
{
    virtual QVector3D operator()(const QVector3D& a, const QVector3D& b,
        const float ideal) = 0;
    virtual void update() {};
    virtual void reset() {};
};


// Defines abstract application function
// Operator (&pos, force, speed) should update pos according to
//  a combination of force, temperature, and speed
// Temperature pointer should point towards temperature variable
//  of a simulated annealing object
struct ApplicationFunction
{
    float* temperature = nullptr;
    virtual void operator()(QVector3D& pos, const QVector3D& f,
        const float speed) = 0;
    virtual void update() {};
    virtual void reset() {};
};

#endif