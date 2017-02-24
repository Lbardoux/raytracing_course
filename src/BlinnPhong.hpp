/**
 * @file BlinnPhong.hpp
 * @brief Implémente les calculs d'une \b Color selon le modèle de BLinn-Phong.
 * @author Laurent Bardoux p1108365
 * @author Mehdi   Ghesh   p1209574
 */
#ifndef BLINNPHONG_HPP_INCLUDED
#define BLINNPHONG_HPP_INCLUDED

#include <vector>

#include "core/gkit_core.hpp"
#include "core/math_core.hpp"
#include "core/ray_core.hpp"

/**
 * @struct BlinnPhongWrapper
 * @brief Embarque les arguments pour faire un calcul avec blinn phong.
 */
struct BlinnPhongWrapper final
{
    const Hit*   hit;      //!< Le point d'impact trouvé, P.
    const Mesh*  mesh;     //!< Le mesh qui contient le triangle concerné.
    const Point* observer; //!< La position de l'observeur.
    const Point* src;      //!< La source de lumière avec laquelle tester.
};

/**
 * @brief Applique un éclairage de type Blinn Phong
 * @param[in] wrap L'ensemble des arguments pour effectuer ce calcul.
 * @param[in] coef Le coefficient de répartition pour l'albédo.
 * @return La couleur obtenue.
 */
Color BlinnPhong(const BlinnPhongWrapper& wrap, float coef) noexcept;


#endif
