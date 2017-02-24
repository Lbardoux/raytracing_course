/**
 * @file DirectFactory.hpp
 * @brief Instancie la liste de recette pour les méthodes directes.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 * @version 2.0
 */
#ifndef DIRECTFACTORY_HPP_INCLUDED
#define DIRECTFACTORY_HPP_INCLUDED

#include <string>
#include "templates/Factory.hpp"
#include "Direct.hpp"


/**
 * @class DirectFactory
 * @brief Produit des méthodes directes allouées sur le tas.
 */
class DirectFactory final : public Factory<std::string, Direct>
{
    public:

};



#endif

