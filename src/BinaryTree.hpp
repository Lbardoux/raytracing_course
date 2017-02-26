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
#include "Triangle.hpp"
#include "Scene.hpp"

typedef uint32_t triangle_ind_t;
typedef int32_t  node_ind_t;
typedef u_char   axes_t;

class BoundingBox final
{
    public:
        float pmin[3]; //!< Le point minimale 
        float pmax[3]; //!< Le point œ
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
                Node() : right(-1), left(-1), triangle(0) {}
                /**
                 * @brief      Indique si un Node est une feuille
                 * @return     Vrai si le Node est une feuille, Faux sinon
                 */
                // bool feuille() { return right == -1 && left == -1; }

                BoundingBox    bbox;     //!< La boite englobante pour ce noeud.
                node_ind_t     right;    //!< L'offset du fils droit,  -1 --> feuille.
                node_ind_t     left;     //!< L'offset du fils gauche, -1 --> feuille.
                triangle_ind_t triangle; //!< Le triangle concerné dans le tableau. --> Que si Node
        };

        /**
         * @brief      Initialisation de l'arbre en fonction de la scene
         * @param[in]  scene  La scene
         */
        void init(const Scene& scene);

        node_ind_t build_node(const triangle_ind_t begin, const triangle_ind_t end);
        
        
        std::vector<Triangle>*        triangles; //!< L'ensemble des triangles dans la structure.
        std::vector<BinaryTree::Node> nodes;     //!< L'ensemble des éléments  de l'arbre.
        triangle_ind_t                root;      //!< Indice du noeud racine

    private:
        /**
         * @brief      Fonction qui construit la boite englobant les triangles
         *             contenu entre begin et end
         * @param[in]  begin  Indice de début des triangles à englober
         * @param[in]  end    Indice de fin des triangles à englober
         * @param      bbox   La boite englobant les points
         */
        void findBox(const triangle_ind_t begin, const triangle_ind_t end, BoundingBox& bbox);

        /**
         * @brief      Fonction qui cherche l'axe le plus long d'une Boite
         *             englobante
         * @param[in]  bbox  Boite englobante sur laquelle on recherche l'axe
         * @return     Axe qui est le plus long
         */
        axes_t largerAxe(const BoundingBox& bbox);
};


struct predicat
{
    axes_t axe;
    float coupe;

    predicat( const axes_t _axe, const float _coupe ) : axe(_axe), coupe(_coupe) {}
    bool operator() ( const Triangle& t ) const
    {
        const float * triangle = static_cast<const float*>(&t.a.x);
        float coord = 0.0f;
        
        for(int i = 0; i < 3; ++i)
        {
            coord += triangle[(i * 3) + this->axe];
        }
        
        coord /= 3.0f;
        
        return coord < this->coupe;
    }
};

#endif
