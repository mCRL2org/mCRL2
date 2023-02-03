#ifndef ATTRACTIONFUNCTIONS_H
#define ATTRACTIONFUNCTIONS_H

#include <layoutfunctions.h>
/**
 * @brief An enumeration that identifies the types of spring types which can
 * be selected.
 */
enum AttractionFunctionID
{
    ltsgraph_attr,          ///< LTSGraph implementation.
    electricalsprings_attr, ///< LTSGraph implementation using approximated
                            ///< repulsion forces.
    linearsprings_attr,     ///< Linear spring implementation.
                            // simplespring_attr,
};

namespace AttractionFunctions {
    struct LTSGraph : AttractionFunction
    {
        QVector3D diff = { 0, 0, 0 };
        const float scaling = 1e3f;
        float dist = 0.0f;
        float factor = 0.0f;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float ideal) override
        {
            diff = (a - b);
            dist = (std::max)(diff.length(), 1.0f);
            factor = scaling * std::log(dist / (ideal + 1.0f)) / dist;
            return diff * factor;
        }
    };

    struct LinearSprings : AttractionFunction
    {
        const float spring_constant = 1e-4f;
        const float scaling = 1.0f / 10000;
        QVector3D diff = { 0, 0, 0 };
        float dist = 0.0f;
        float factor = 0.0f;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float ideal) override
        {
            diff = (a - b);
            dist = diff.length() - ideal;
            factor = spring_constant * std::max(dist, 0.0f);
            if (dist > 0.0f)
            {
                factor = std::max(factor, 100 / (std::max)(dist * dist / 10000.0f, 0.1f));
            }
            return diff * (factor * scaling);
        }
    };

    struct ElectricalSprings : AttractionFunction
    {
        QVector3D diff = { 0, 0, 0 };
        const float scaling = 1e-2f;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float ideal) override
        {
            diff = (a - b);
            return (scaling * diff.length() / std::max(0.0001f, ideal)) *
                diff;
        }
    };

    struct SimpleSpring : AttractionFunction
    {
        QVector3D diff = { 0, 0, 0 };
        const float spring_constant = 1e-4f;
        QVector3D operator()(const QVector3D& a, const QVector3D& b,
            const float ideal) override
        {
            diff = a - b;
            return spring_constant * (diff.length() - ideal) * diff;
        }
    };
}; // namespace AttractionFunctions

#endif
