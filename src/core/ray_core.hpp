/**
 * @file ray_core.hpp
 * @brief Inclut tout ce qu'il faut pour le raytracing.
 */
#ifndef RAY_CORE_HPP_INCLUDED
#define RAY_CORE_HPP_INCLUDED

#include "../Scene.hpp"
#include "../structures/Hit.hpp"
#include "../structures/Triangle.hpp"

#ifndef UNUSED
    #ifdef _GNUC_
        #define UNUSED(var) var __attribute__((__unused__))
    #else
        #define UNUSED(var) var
    #endif
#endif

#endif

