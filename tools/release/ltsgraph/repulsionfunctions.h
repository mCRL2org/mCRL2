#ifndef REPULSIONFUNCTIONS_H
#define REPULSIONFUNCTIONS_H

#include <layoutfunctions.h>


enum RepulsionFunctionID
{
    ltsgraph_rep,
    electricalsprings_rep,
    none_rep,
};


namespace RepulsionFunctions
{
    struct LTSGraph : RepulsionFunction
    {
        QVector3D diff;
        float r;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float natlength) override
        {
            diff = a - b;
            r = cube(natlength);
            r /= cube((std::max)(diff.length() * 0.5f, natlength * 0.1f));
            diff = diff * r + QVector3D(fast_frand(-0.01f, 0.01f),
                fast_frand(-0.01f, 0.01f),
                fast_frand(-0.01f, 0.01f));
            return diff;
        }
    };

    struct ElectricalSpring : RepulsionFunction
    {
        QVector3D diff;
        const float scaling = 1e-2f;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float K) override
        {
            diff = a - b;
            return ((scaling * K * K) /
                std::max(diff.lengthSquared(), 0.00001f)) *
                diff;
        }
    };

    struct None : RepulsionFunction
    {
        QVector3D ZERO = { 0, 0, 0 };
        QVector3D operator()(const QVector3D&, const QVector3D&,
            const float) override
        {
            return ZERO;
        }
    };
}; // namespace RepulsionFunctions


#endif
