#ifndef APPLICATIONFUNCTIONS_H
#define APPLICATIONFUNCTIONS_H

#include "layoutfunctions.h"


enum ApplicationFunctionID
{
    ltsgraph_appl,       ///< Treat forces as speed
    force_directed_appl, ///< Treat forces as suggested direction
};


namespace ApplicationFunctions
{
    struct LTSGraph : ApplicationFunction
    {
        const float scaling = 0.01f;
        const float limit = 1e4f;
        void operator()(QVector3D& pos, const QVector3D& f,
            const float speed) override
        {
            if (scaling * std::abs(f.x()) >= limit ||
                scaling * std::abs(f.y()) >= limit ||
                scaling * std::abs(f.z()) >= limit)
                return;
            pos += f * (speed * scaling * *temperature);
        }
    };

    struct ForceDirected : ApplicationFunction
    {
        const float scaling = 2.0f;
        const float stability_param = 0.01f;
        // precompute
        const float thres = scaling * stability_param;
        // easing such that at threshold translation is 50% of stepsize
        const float ease_width = .1f;
        const float ease_floor =
            1e-06f; // Always keep ease_floor force applied. May cause jitter
        // precompute
        const float one_minus_ease_floor = 1 - ease_floor;
        void operator()(QVector3D& pos, const QVector3D& f,
            const float speed) override
        {
            float amplitude = speed * scaling * (*temperature);
            float threshold = speed * thres;
            threshold /= *temperature;
            float L = f.length();
            if (L < (1 + ease_width) * threshold)
            {
                // smoothstep the amplitude
                amplitude *= ease_floor + one_minus_ease_floor *
                    smoothstep(threshold * (1 - ease_width),
                        threshold * (1 + ease_width), L);
            }
            if (L == 0)
                return;
            pos += (amplitude / L) * f;
        }
    };
}; // namespace ApplicationFunctions

#endif
