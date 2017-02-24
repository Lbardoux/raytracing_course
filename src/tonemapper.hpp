#ifndef TONEMAPPER_HPP
#define TONEMAPPER_HPP

#include <cmath>   // Il faut -lm
#include <cassert>
#include <valarray>

#define Compress(gamma)   (1.0f/(gamma))
#define Decompress(gamma) (gamma)
#define TONEMAPPER_EPSILON 0.0001f
typedef std::valarray<float> tonemapped_color_t;


/**
 * @brief Renvoie une couleur tonnemappée
 * 
 * Cette fonction va appliquer le tonnemapping avec la valeur de gamma.\n
 * Elle permet de compresser et décompresser la valeur passée en paramètre.\n
 * Ainsi, pour compresser, il faut passer une valeur comprise entre 0 et 1
 *
 * @param[in]  rgb    Tableau contenant les 3 valeurs rgb à tonnemapper
 * @param[in]  gamma  Valeur du gamma pour la compression
 * @pre        gamma doit etre différent de 0.
 *
 * @return     La couleur tonemappée selon gamma
 * Par exemple :
 * @code
 * tonemapped_color_t mappedColor = tonemapper({0.1, 0.12, 0.1});
 * tonemapped_color_t mappedColor = tonemapper({0.1, 0.12, 0.1}, Compress(2.0f));
 * tonemapped_color_t mappedColor = tonemapper({0.1, 0.12, 0.1}, Decompress(2.0f));
 * @endcode
 */
tonemapped_color_t tonemapper(const tonemapped_color_t& rgb, const float gamma = Compress(2.2f))
{
    assert(std::abs(gamma) > TONEMAPPER_EPSILON);
    return std::pow(rgb, gamma);
}



#endif

