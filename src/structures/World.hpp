/**
 * @file World.hpp
 * @brief Encapsule le code World du prof.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 * @version 2.0
 */
#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include "../core/gkit_core.hpp"

// construit un repere ortho tbn, a partir d'un seul vecteur...
// cf "generating a consistently oriented tangent space" 
// http://people.compute.dtu.dk/jerf/papers/abstracts/onb.html
struct World
{
    World( const Vector& _n ) : n(_n) 
    {
        if(n.z < -0.9999999f)
        {
            t= Vector(0, -1, 0);
            b= Vector(-1, 0, 0);
        }
        else
        {
            float a= 1.f / (1.f + n.z);
            float d= -n.x * n.y * a;
            t= Vector(1.f - n.x * n.x * a, d, -n.x);
            b= Vector(d, 1.f - n.y * n.y * a, -n.y);
        }
    }
    
    Vector operator( ) ( const Vector& local )  const
    {
        return local.x * t + local.y * b + local.z * n;
    }
    
    Vector t;
    Vector b;
    Vector n;
};

#endif

