/**
 * @file pdf.hpp
 * @brief Embarque un certain nombre de pdf pour les méthodes de Monte-Carlo.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 */
#ifndef PDF_HPP_INCLUDED
#define PDF_HPP_INCLUDED

/**
 * @brief La première stratégie, basée sur l'aire des sources.
 * 
 * @code
 *                        1.0f
 * p1(UNUSED(y)) = -------------------
 *                 sigma(Aire(source))
 * @endcode
 * Il peut etre intéressant de la stocker ailleurs
 * @return La valeur obtenue par ce calcul.
 */
float MIS_strategy_1(void) noexcept;

/**
 * @brief Une seconde tentative, basée sur les angles entres les rayons et les normales (sur G quoi).
 * @param[in] cosx Le cosinus de l'angle entre le rayon partant de la source et la normale du point d'impact.
 * @param[in] cosy Le cosinus de l'angle entre le rayon partant de la source et la normale de la source.
 * @param[in] d2   La distance au carré entre le point et la source.
 * 
 * @code
 *           cosx      cosy
 * p2(y) = -------- x ------
 *           M_PI       d2
 * @endcode
 * 
 * @return La valeur obtenue par ce calcul.
 */
float MIS_strategy_2(float cosx, float cosy, float d2) noexcept;



#endif
