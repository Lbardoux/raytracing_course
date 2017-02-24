/**
 * @file BinaryTree.hpp
 * @brief L'arbre binaire pour la structure acceleratrice.
 * @author Laurent BARDOUX p1108365
 * @author Mehdi   GHESH   p1209574
 */
#ifndef BINARYTREE_HPP_INCLUDED
#define BINARYTREE_HPP_INCLUDED

#include <vector>
#include <cstdint>
#include "app_core.hpp"
#include "Triangle.hpp"


class BoundingBox final
{
    public:
        Point pmin; //!< Le point minimale 
        Point pmax; //!< Le point 
};

/**
 * @class BinaryTree
 * @brief Embarque un arbre binaire pour BVHs.
 */
class BinaryTree final
{
    public:
        /**
         * @class Node
         * @brief Un node spécifique à cet arbre binaire.
         */
        class Node final
        {
            public:
                BoundingBox bbox;     //!< La boite englobante pour ce noeud.
                int32_t     right;    //!< L'offset du fils droit,  -1 --> feuille.
                int32_t     left;     //!< L'offset du fils gauche, -1 --> feuille.
                uint32_t    triangle; //!< Le triangle concerné dans le tableau.
                
        };
        
        
        std::vector<Triangle>         triangles; //!< L'ensemble des triangles dans la structure.
        std::vector<BinaryTree::Node> nodes;     //!< L'ensemble des éléments  de l'arbre.
};




#endif
