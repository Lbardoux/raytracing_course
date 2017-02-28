#include "BinaryTree.hpp"
#include <algorithm>
#include <initializer_list>
#include <utility>
#include <array>
#include <functional>
#include <stack>
#include <fstream>


namespace 
{
    typedef BinaryTree::Node Node; //!< C'est quand meme moins long à écrire ncp ?
    
    /**
     * @brief Permet de simplifier les calculs des t*min/max en les cachant dans une fonction.
     * Effectue le calcul des min/max en swappant les deux valeurs si min est supérieur à max.
     * @param[out] min  La valeur minimale obtenue après calcul,            pour la bonne coordonné.
     * @param[out] max  La valeur maximale obtenue après calcul,            pour la bonne coordonné.
     * @param[in]  pmin La valeur du point minimale de la boite englobante, pour la bonne coordonné.
     * @param[in]  pmax La valeur du point maximale de la boite englobante, pour la bonne coordonné.
     * @param[in]  rayo La valeur du point d'origine du rayon,              pour la bonne coordonné.
     * @param[in]  rayd La valeur de la direction du rayon,                 pour la bonne coordonné.
     */
    void affect_max_min(float& min, float& max,
                        const float pmin, const float pmax, const float rayo, const float rayd) noexcept
    {
        min = (pmin - rayo)/rayd;
        max = (pmax - rayo)/rayd;
        if (min > max)
        {
            std::swap(min, max);
        }
    }
    /**
     * @brief Embarque des comparaisons et réaffectations pour le calcul de l'intersections de la boite englobante.
     * @param[out] min      La valeur minimale obtenue après calcul,            pour la bonne coordonné.
     * @param[out] max      La valeur maximale obtenue après calcul,            pour la bonne coordonné.
     * @param[in]  othermin L'autre valeur de min (pour une autre coordonné) avec laquelle comparer.
     * @param[in]  othermax L'autre valeur de max (pour une autre coordonné) avec laquelle comparer.
     * @return true si la comparaison aboutie à un souci (il faut alors sortir du calcul), false sinon.
     */
    bool compare_max_min(float& min, float& max, const float othermin, const float othermax) noexcept
    {
        if (min > othermax || othermin > max)
        {
            return true;
        }
        min = std::max(othermin, min);
        max = std::min(othermax, max);
        return false;
    }
    
}

bool BoundingBox::intersect(const Ray& ray) const noexcept
{
    float txmin, txmax, tymin, tymax, tzmin, tzmax;
    affect_max_min(txmin, txmax, this->pmin.x, this->pmax.x, ray.o.x, ray.d.x);
    affect_max_min(tymin, tymax, this->pmin.y, this->pmax.y, ray.o.y, ray.d.y);
    affect_max_min(tzmin, tzmax, this->pmin.z, this->pmax.z, ray.o.z, ray.d.z);
    if (compare_max_min(txmin, txmax, tymin, tymax) || compare_max_min(txmin, txmax, tzmin, tzmax))
    {
        return false;
    }
    // Ici, on a les bonnes coordonné pour les t[xyz]m[ia][nx] si on veut bosser avec
    return true;
}



void BinaryTree::init(const std::string& fname)
{
	this->triangles = &Scene::triangles;
    if (!this->fromFile(fname))
    {
        this->root = this->build_node(0, Scene::triangles.size());// pourquoi 1 au lieu de 0 ?
        this->dump(fname);
    }
}

BinaryTree::Node::Node(const node_ind_t r, const node_ind_t l, const triangle_ind_t t) : right(r), left(l), triangle(t)
{
    
}

BinaryTree::Node BinaryTree::Node::make_leaf(const triangle_ind_t triangleOffset) noexcept
{
    return BinaryTree::Node(-1, -1, triangleOffset);
}

namespace
{
    /**
     * @brief Décide si on est dans un cas terminal pour la construction du BVH.
     * @details Un cas terminal ----> une feuille.
     * @param[in] begin L'indice  de départ    lorsque l'on prélève dans le conteneur de triangles.
     * @param[in] end   L'indice  de fin exclu lorsque l'on prélève dans le conteneur de triangles.
     * @return @b true si on est dans un cas terminal, false sinon.
     */
    bool build_terminal_case(triangle_ind_t begin, triangle_ind_t end) noexcept
    {
        return end - begin <= 1;
    }
    //! Un enum pour les axes.
    enum AXIS {
        AXIS_X = 0,
        AXIS_Y = 1,
        AXIS_Z = 2
    };
    //! Returne la distance entre a et b (b-a) en valeur absolue.
    inline float len(float a, float b) noexcept
    {
        return std::abs(b - a);
    }
    /**
     * @brief Un peu de ruse ça vous tente ? On va en fonction duquel des 3 calculs est le plus grand
     * renvoyer le bon AXIS, en se basant sur le principe suivant :
     * @code
     *       a>b
     *      /   \
     *    a>c   b>c
     *   /   \ /   \
     *  a     c     b
     * @endcode
     * Le tout en évitant une copie du flottant conservé :D@n
     * L'astuce étant de changer la valeur de axis dans la première partie de l'opérateur ternaire (là ou s'est possible).
     * @param[in] bbox La boite englobante avec laquelle faire les mesures.
     * @return L'enum de l'axe le plus long.
     */
    AXIS findLongestAxis(const BoundingBox& bbox)
    {
        const float lenX = len(bbox.pmin.x, bbox.pmax.x);
        const float lenY = len(bbox.pmin.y, bbox.pmax.y);
        const float lenZ = len(bbox.pmin.z, bbox.pmax.z);
        AXIS axis = AXIS::AXIS_Y;
        const float& intermediate = (lenX > lenY) ? axis = AXIS::AXIS_X, lenX
                                                  : lenY;
        return (intermediate > lenZ) ? axis : AXIS::AXIS_Z;
    }
    /**
     * @brief Construit une boite englobante @b bbox en ne se basant que sur les triangles de @b triangles
     * compris entre @b begin et @b end @b exclu !
     * @param[in]  triangles Un ensemble de triangles.
     * @param[in]  begin     L'offset       du premier triangle à traiter.
     * @param[in]  end       L'offset exclu du dernier triangle à traiter.
     * @param[out] bbox      La boite englobante que l'on va remplir.
     * @pre @b begin doit etre dans les bornes de @b triangles.
     * @pre @b end   doit etre dans les bornes de @b triangles.
     * @throw std::out_of_range Si on sort des limites de @b triangles.
     */
    void buildBbox(const std::vector<Triangle>& triangles, int begin, int end, BoundingBox& bbox)
    {
        bbox.pmin.x = bbox.pmax.x = triangles.at(begin).a.x;
        bbox.pmin.y = bbox.pmax.y = triangles.at(begin).a.y;
        bbox.pmin.z = bbox.pmax.z = triangles.at(begin).a.z;
        for(int i=begin;i<end;++i)
        {
            bbox.pmin.x = std::min({bbox.pmin.x, triangles.at(i).a.x, triangles.at(i).b.x, triangles.at(i).c.x});
            bbox.pmin.y = std::min({bbox.pmin.y, triangles.at(i).a.y, triangles.at(i).b.y, triangles.at(i).c.y});
            bbox.pmin.z = std::min({bbox.pmin.z, triangles.at(i).a.z, triangles.at(i).b.z, triangles.at(i).c.z});
            bbox.pmax.x = std::max({bbox.pmax.x, triangles.at(i).a.x, triangles.at(i).b.x, triangles.at(i).c.x});
            bbox.pmax.y = std::max({bbox.pmax.y, triangles.at(i).a.y, triangles.at(i).b.y, triangles.at(i).c.y});
            bbox.pmax.z = std::max({bbox.pmax.z, triangles.at(i).a.z, triangles.at(i).b.z, triangles.at(i).c.z});
        }
    }
    /**
     * @brief Trouve la coupure sur l'axe le plus long (qui est @b axis).
     * @param[in] bbox La boite englobante que l'on considère.
     * @param[in] axis L'enum de l'axe le plus long.
     * @return 
     */
    float cutOff(const BoundingBox& bbox, AXIS axis) noexcept
    {
        float result = 0.0f;
        switch(axis)
        {
            case AXIS::AXIS_X:
                result = bbox.pmin.x + bbox.pmax.x;
                break;
            case AXIS::AXIS_Y:
                result = bbox.pmin.y + bbox.pmax.y;
                break;
            default:
                result = bbox.pmin.z + bbox.pmax.z;
        }
        return result/2.0f;
    }
    
}

node_ind_t BinaryTree::build_node(const triangle_ind_t begin, const triangle_ind_t end)
{
    this->nodes.emplace_back();
    Node&      node   = this->nodes.back();
    node_ind_t offset = this->nodes.size() - 1;
    if (build_terminal_case(begin, end))
    {
        node.triangle = begin;
    }
    else
    {
        buildBbox(*this->triangles, begin, end, node.bbox);
        AXIS axis = findLongestAxis(node.bbox);
        float cut = cutOff(node.bbox, axis);
        Triangle* pmid = std::partition(this->triangles->data() + begin, this->triangles->data() + end, [axis, cut](const Triangle& t) -> bool {
            Point center = t.point(0.33f, 0.33f);
            switch(axis)
            {
                case AXIS::AXIS_X:
                    return center.x < cut;
                case AXIS::AXIS_Y:
                    return center.y < cut;
                default:
                    return center.z < cut;
            }
        });
        triangle_ind_t mid = std::distance(this->triangles->data(), pmid);
        node.left  = this->build_node(begin, mid);
        node.right = this->build_node(mid, end);
    }
    return offset;
}

bool BinaryTree::intersect(const Ray& ray, Hit& hit) const
{
    std::vector<node_ind_t> leaves;
    std::stack<node_ind_t>  stack;
    
    stack.push(this->root);
    while(!stack.empty())
    {
        node_ind_t current = stack.top();
        stack.pop();
        if (this->nodes.at(current).isLeaf())
        {
            leaves.push_back(current);
        }
        else if(this->nodes.at(current).bbox.intersect(ray))
        {
            stack.push(this->nodes.at(current).left);
            stack.push(this->nodes.at(current).right);
        }
    }
    if (leaves.empty())
    {
        return false;
    }
    hit.t = ray.tmax;
    std::for_each(leaves.begin(), leaves.end(), [&ray, &hit, this](node_ind_t id){
        float t, u, v;
        if(this->triangles->at(id).intersect(ray, hit.t, t, u, v))
        {
            hit.t = t;
            hit.u = u;
            hit.v = v;
            hit.p = ray(t);
            hit.n = this->triangles->at(id).normal(u, v);
            hit.object_id = id;
        }
    });
    return true;
}

void BinaryTree::dump(const std::string& fname)
{
    std::ofstream file(fname);
    file << this->nodes.size() << std::endl;
    file << this->root << std::endl;
    std::for_each(this->nodes.begin(), this->nodes.end(), [&file](const BinaryTree::Node& node){
        file << node.left << ' ' << node.right << ' ' << node.triangle << ' ';
        file << node.bbox.pmin.x << ' ' << node.bbox.pmin.y << ' ' << node.bbox.pmin.z << ' ';
        file << node.bbox.pmax.x << ' ' << node.bbox.pmax.y << ' ' << node.bbox.pmax.z << ' ' << std::endl;
    });
    
    file.close();
}

bool BinaryTree::fromFile(const std::string& fname)
{
    std::ifstream file(fname);
    if (file.good())
    {
        int size;
        file >> size;
        this->nodes.resize(size);
        file >> this->root;
        for(unsigned int i=0;i<this->nodes.size();++i)
        {
            file >> this->nodes.at(i).left >> this->nodes.at(i).right;
            file >> this->nodes.at(i).triangle;
            file >> this->nodes.at(i).bbox.pmin.x >> this->nodes.at(i).bbox.pmin.y >> this->nodes.at(i).bbox.pmin.z;
            file >> this->nodes.at(i).bbox.pmax.x >> this->nodes.at(i).bbox.pmax.y >> this->nodes.at(i).bbox.pmax.z;
        }
        file.close();
        return true;
    }
    return false;
}
