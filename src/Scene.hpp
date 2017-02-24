/**
 * @file Scene.hpp
 * @brief Embarque les éléments de la scène que l'on veut @i raytracer.@n
 * C'est un simple conteneur, aucun accès n'est fait en interne.
 * @author Laurent Bardoux p1108365
 * @author Mehdi   Ghesh   p1209574
 * @version 2.0
 */
#ifndef SCENE_HPP_INCLUDED
#define SCENE_HPP_INCLUDED

#include <vector>

#include "core/math_core.hpp"
#include "core/gkit_core.hpp"
#include "structures/Triangle.hpp"
#include "structures/Hit.hpp"


/**
 * @class Scene
 * @brief Embarque les variables globales de l'application.
 */
class Scene final
{
    public:
        static Orbiter               camera;    //!< Le point de vue pour le raytracing.
        static std::vector<Triangle> triangles; //!< Les triangles de la géometrie de la scène.
        static std::vector<Source>   sources;   //!< L'ensemble des sources de lumière de la scène.
        static Mesh                  mesh;      //!< Embarque la scène et les matériaux.
        
        /**
         * @brief Parcours le mesh interne pour trouver les sources de lumière.
         * @return Le nombre de sources trouvées.
         * @pre Le mesh interne doit ^etre rempli.
         */
        static unsigned int build_sources(void);
        /**
         * @brief Génère les triangles depuis le mesh interne.
         * @return Le nombre de triangles construit.
         * @pre Le mesh interne doit ^etre rempli.
         */
        static unsigned int build_triangles(void);
        /**
         * @brief Vérifie si il existe une intersection avec l'ensemble des triangles.
         * @param[in]  ray Le rayon partant de la caméra vers le far.
         * @param[out] hit Le conteneur du résultat.
         * @return true si il existe une intersection, false sinon.
         */
        static bool intersect(const Ray& ray, Hit& hit);
        
        Scene(void) = delete;
    
};




#endif
