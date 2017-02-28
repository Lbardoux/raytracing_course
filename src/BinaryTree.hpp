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
#include <string>
#include "core/gkit_core.hpp"
#include "core/ray_core.hpp"

typedef uint32_t triangle_ind_t;
typedef int32_t  node_ind_t;

class BoundingBox final
{
    public:
        vec3 pmin; //!< Le point minimale.
        vec3 pmax; //!< Le point maximale.
        
        /**
         * @brief Vérifie si @b ray intersecte la boite englobante.
         * @param[in] ray Le rayon avec lequel tester.
         * @return true si @b ray intersecte la boite, false sinon.
         */
        bool intersect(const Ray& ray) const noexcept;
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
                Node(const node_ind_t r=-1, const node_ind_t l=-1, const triangle_ind_t t=0);
                /**
                 * @brief Indique si un Node est une feuille
                 * @return Vrai si le Node est une feuille, Faux sinon
                 */
                bool isLeaf(void) const
                {
                    return right == -1 && left == -1;
                }
                /**
                 * @brief Crée une feuille embarquant le triangle numéro @b triangleOffset.
                 * @param[in] triangleOffset L'indice du triangle que cette feuille référence.
                 * @return Le Node nouvellement créé, pour etre copié.
                 */
                static Node make_leaf(const triangle_ind_t triangleOffset) noexcept;

                BoundingBox    bbox;     //!< La boite englobante pour ce noeud.
                node_ind_t     right;    //!< L'offset du fils droit,  -1 --> feuille.
                node_ind_t     left;     //!< L'offset du fils gauche, -1 --> feuille.
                triangle_ind_t triangle; //!< Le triangle concerné dans le tableau. --> Que si Node
        };

        /**
         * @brief Initialisation de l'arbre en fonction de la scene
         */
        void init(const std::string& fname);
        node_ind_t build_node(const triangle_ind_t begin, const triangle_ind_t end);
        bool intersect(const Ray& ray, Hit& hit) const;
        bool fromFile(const std::string& fname);
        void dump(const std::string& fname);
        
        
        std::vector<Triangle>*        triangles; //!< L'ensemble des triangles dans la structure.
        std::vector<BinaryTree::Node> nodes;     //!< L'ensemble des éléments de l'arbre.
        triangle_ind_t                root;      //!< Indice du noeud racine
    
};

#endif
