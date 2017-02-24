/**
 * @file Hit.hpp
 * @brief Définit la structure Hit, utilisée pour le lancer de rayons.
 * @author Laurent Bardoux p1108365
 * @author Mehdi   Ghesh   p
 */
#ifndef HIT_HPP_INCLUDED
#define HIT_HPP_INCLUDED

#include "../core/gkit_core.hpp"

//! representation d'un point d'intersection.
struct Hit
{
    Point p;	    //!< position.
    Vector n;	    //!< normale.
    float t;	    //!< t, abscisse sur le rayon.
    float u, v;	    //!< u, v coordonnees barycentrique dans le triangle.
    int object_id;  //! indice du triangle dans le maillage.
    
    Hit( ) : p(), n(), t(FLT_MAX), u(0), v(0), object_id(-1) {}
};

#endif
