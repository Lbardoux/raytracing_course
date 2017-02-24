/**
 * @file Direct.hpp
 * @brief La base d'une méthode directe pour le lancer de rayons.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 * @version 2.0
 */
#ifndef DIRECT_HPP_INCLUDED
#define DIRECT_HPP_INCLUDED

#include <string>
#include "core/gkit_core.hpp"
#include "core/ray_core.hpp"
#include "templates/Factory.hpp"

/**
 * @class Direct
 * @brief La super classe de l'éclairage direct.
 */
class Direct
{
    public:
        virtual ~Direct(void){}
        /**
         * @brief Effectue les calculs pour obtenir la couleur directe.
         * @param[in]  observer Le point o de l'observateur.
         * @param[in]  impact   L'intersection trouvée sur la géométrie.
         * @param[in]  N        Le nombre de point sur chaque source que l'on va tester.
         * @return La couleur obtenue.
         */
        virtual Color compute(const Point& observer, const Hit& impact, int N=1) =0;
};

#define MAKE_DIRECT_METHOD(classname) \
class classname final : public Direct \
{ \
    public: \
        classname(void) : Direct(){} \
        Color compute(const Point& observer, const Hit& impact, int N=1) override; \
};

// Ajouter des méthodes comme ceci :D et modifier le .cpp (ajout de method::compute et de la ligne dans la fabrique.
MAKE_DIRECT_METHOD(OnePointPerSource)
MAKE_DIRECT_METHOD(NPointPerSource)
MAKE_DIRECT_METHOD(FibonacciSpiral)
MAKE_DIRECT_METHOD(TriangleGrid)
MAKE_DIRECT_METHOD(RandomSource)



/**
 * @class DirectFactory
 * @brief Fabrique des méthodes directes via une chaine de caractère en entrée.
 */
class DirectFactory final : public Factory<std::string, Direct*>
{
    public:
        DirectFactory(void);
};


#endif

