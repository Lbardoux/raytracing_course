/**
 * @file Indirect.hpp
 * @brief Définit les fonctionnalités pour l'éclairage indirect.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 */
#ifndef INDIRECT_HPP_INCLUDED
#define INDIRECT_HPP_INCLUDED

#include <string>
#include "ray_core.hpp"
#include "Factory.hpp"


class Indirect
{
    public:
        virtual ~Indirect(void){}
        virtual Color compute(void) =0;
};


class IndirectFactory final : public Factory<std::string, Indirect*>
{
    public:
        IndirectFactory(void);
};


#endif

