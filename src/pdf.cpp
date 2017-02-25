/**
 * @file pdf.cpp
 */
#include <numeric>
#include <functional>

#include "pdf.hpp"
#include "core/ray_core.hpp"
#include "core/math_core.hpp"

float MIS_strategy_1(void) noexcept
{
    return 1.0f/std::accumulate(Scene::sources.begin(), Scene::sources.end(), 0.0f, [](float f, const Source& src) -> float {
        return f + src.area();
    });
}

float MIS_strategy_2(float cosx, float cosy, float d2) noexcept
{
    return (cosx/M_PI)*(cosy/d2);
}
