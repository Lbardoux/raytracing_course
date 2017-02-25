/**
 * @file Direct.cpp
 */
#include <algorithm>
#include <functional>
#include <random>
#include <array>

#include "Direct.hpp"
#include "BlinnPhong.hpp"
#include "ConfigLoaders.hpp"
#include "core/math_core.hpp"
#include "core/gkit_core.hpp"
#include "structures/World.hpp"
#include "pdf.hpp"

namespace
{
    typedef std::array<float, 3> FromG_t;
    /**
     * @brief Calcule la valeur de G pour <b>(P, nP)</b> le point d'impact du rayon et la soruce <b>(S, nS)</b>.
     * @param[in] P         La position dans le monde du point d'impact.
     * @param[in] nP        La normale de ce point dans le monde.
     * @param[in] S         La position du point de la source dans le monde.
     * @param[in] nS        La normale en ce point de la source dans le monde.
     * @param[in] costhetaP La valeur précalculé de cos(Op).
     * @return Les valeurs de G, résultantes du calcul.
     */
    FromG_t computeG(const Point& P, const Vector& nP, const Point& S, const Vector& nS, float costhetaP) noexcept
    {
        FromG_t result;
        result.at(1) = std::cos(dot(normalize(Vector(S, P)), normalize(nS)));
        result.at(2) = distance2(P, S);
        result.at(0) = (costhetaP*result.at(1)) / result.at(2);
        return result;
    }
    /**
     * @brief Calcule le pas lorsque l'on veut N points sur une source, tel que le pas décompose l'interval [0, 1].
     * @param[in,out] N Le nombre de point que l'on veut sur chaque source, sera modifié pour renvoyer l'entier correspondant.
     * @return Le pas une fois calculé, et le N qui correspond à la racine entière trouvée.
     */
    float computeStep(int& N) noexcept
    {
        float floor = std::floor(std::sqrt(static_cast<float>(N)));
        N = static_cast<int>(floor) + 1;
        return 1.0f/(floor + 1.0f);
    }
    /**
     * @brief Décale @b point d'un certain pourcentage par rapport à la @b normal.
     * @param[in] point  Le point à décaler.
     * @param[in] normal La normale du point par rapport à laquelle décaler.
     * @return Le nouveau point, obtenu après décalage.
     */
    Point shift(const Point& point, const Vector& normal) noexcept
    {
        return point + normal*RaytracingXml::normalTweak;
    }
    /**
     * @brief Calcule le point d'arrivée du rayon, et la normale en ce point.
     * @param(in]  src    La source depuis laquelle travailler.
     * @param[in]  alpha  Le premier décalage sur les coordonnés barycentrique.
     * @param[in]  beta   Le second  décalage sur les coordonnés barycentrique.
     * @param[out] normal La normale au point calculé, en résultat.
     * @return Le point d'arrivée obtenu.
     */
    Point sourceShifting(const Source& src, float alpha, float beta, Vector& normal) noexcept
    {
        normal = src.normal(alpha, beta);
        return shift(src.point(alpha, beta), normal);
    }
    /**
     * @brief Tire un point sur @b src en se basant sur @b u et @b v.
     * @details Cela va également remplir la normale de ce point.
     * @param[in]  src    La source sur laquelle on veut prélever un point.
     * @param[in]  u      La première coordonné barycentrique.
     * @param[in]  v      La seconde  coordonné barycentrique.
     * @param[out] normal La normale que l'on va calculé pour le point obtenu.
     * @return Le point obtenu sur la source, décalé par rapport à la normale.
     * @pre @b u est compris entre 0.0f et 1.0f.
     * @pre @b v est compris entre 0.0f et 1.0f.
     */
    Point pointOnSource(const Source& src, const float u, const float v, Vector& normal) noexcept
    {
        const float sqrt_u = std::sqrt(u);
        const float beta   = v*sqrt_u;
        const float alpha  = sqrt_u - beta; // Gaffe ici 
        return sourceShifting(src, alpha, beta, normal);
    }
    /**
     * @brief Applique la formule de l'éclairage directe.
     * @param[in]  impact    Le hit du point vu par l'observateur.
     * @param[in]  observer  La position de l'observeur.
     * @param[in]  o         Le point d'origine du rayon allant vers la source.
     * @param[in]  e         Le point sur la source pour le rayon.
     * @param[in]  normal    La normal à la source.
     * @param[out] fromG     Un conteneur pour les calculs de G.
     * @param[out] cosThetaP Récupère ce cosTheta pour la suite.
     * @return La couleur calculé pour cette étape.
     */
    Color computeL1(const Hit& impact, const Point& observer, const Point& o, const Point& e,
                    const Vector& normal, FromG_t& fromG, float& cosThetaP) noexcept
    {
        BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
        Color brdf             = BlinnPhong(wrap, RaytracingXml::interpolation);
        cosThetaP              = std::cos(dot(normalize(Vector(o, e)), normalize(normal)));
        fromG                  = computeG(o, impact.n, e, normal, cosThetaP);
        return fromG.at(0)*brdf*cosThetaP;
    }

    /**
     * @brief Effectue un calcul de luminosité directe "générique".
     * @param[in] observer     La position de l'observateur.
     * @param[in] impact       Le point d'impact du rayon.
     * @param[in] N            Le nombre de points/d'itérations voulu(e)s.
     * @param[in] randFunction La fonction générant les points à la surface des sources.
     * @param[in] useDistance  Si on veut appliquer G(p,s) sur chaque couleur ajoutée.
     * @return La couleur obtenue.
     */
    Color basicDirect(const Point& observer, const Hit& impact, int N, std::function<Point(Source&, Vector& n)> randFunction)
    {
        Color result;
        Hit hit;
        Point o = shift(impact.p, impact.n);
        std::for_each(Scene::sources.begin(), Scene::sources.end(), [&](Source& src){
            for(int i=0;i<N;++i)
            {
                Vector normal;
                Point e = randFunction(src, normal);
                Ray ray(o, e);
                if (!Scene::intersect(ray, hit))
                {
                    FromG_t foo1;
                    float foo2;
                    result = result + computeL1(impact, observer, o, e, normal, foo1, foo2);
                }
            }
        });
        
        return result/static_cast<float>(Scene::sources.size()*N);
    }
    /**
     * @brief Méthode directe basée sur un maillage des sources.
     * @param[in] observer La position de l'observateur.
     * @param[in] impact   Le point d'impact du rayon.
     * @param[in] N        Le nombre de points/d'itérations voulu(e)s.
     * @return La couleur obtenue via l'&quation qui fait peur.
     */
    Color gridDirect(const Point& observer, const Hit& impact, int N)
    {
        Color result;
        Hit   hit;
        Point o       = shift(impact.p, impact.n);
        int   nbPoint = 0;
        float step    = computeStep(N);
        std::for_each(Scene::sources.begin(), Scene::sources.end(), [&](Source& src){
            for(int iu=0;iu<=N;++iu)
            {
                for(int jv=0;jv<=N;++jv)
                {
                    const float u = static_cast<float>(iu)*step;
                    const float v = static_cast<float>(jv)*step;
                    if (u+v <= 1.0f)
                    {
                        Vector normal;
                        Point  e = pointOnSource(src, u, v, normal);
                        Ray ray(o, e);
                        if (!Scene::intersect(ray, hit))
                        {
                            FromG_t G;
                            float cosThetaP;
                            result = result + computeL1(impact, observer, o, e, normal, G, cosThetaP);
                            
                        }
                        ++nbPoint;
                    }
                }
            }
        });
        
        return result/static_cast<float>(nbPoint);
    }
}


Color OnePointPerSource::compute(const Point& observer, const Hit& impact, int N)
{
    (void)N;
    return basicDirect(observer, impact, 1, [](Source& src, Vector& n){
        return sourceShifting(src, 0.33f, 0.33f, n);
    });
}

Color NPointPerSource::compute(const Point& observer, const Hit& impact, int N)
{
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return basicDirect(observer, impact, N, [&mt, &dist](Source& src, Vector& n){
        return pointOnSource(src, dist(mt), dist(mt), n);
    });
}

#define SQRT_5 2.236067977f
Color FibonacciSpiral::compute(const Point& observer, const Hit& impact, int N)
{
    Color result;
    Hit hit;
    Point o = shift(impact.p, impact.n);
    float phi = (SQRT_5 + 1.0f)/2.0f;
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    float u = dist(mt); //< Perturbation
    
    // On va construire une spirale pour définir des points sur chaque source.
    std::for_each(Scene::sources.begin(), Scene::sources.end(), [&](Source& src){
        World world(src.normal(0.33f, 0.33f));
        for(int i=0;i<N;++i)
        {
            float cosTheta = 1.0f - ((2.0f*i + 1)/(2.0f*N));
            float theta2   = 2.0f*M_PI*((i+u)/phi - std::floor((i+u)/phi));
            float sinTheta = std::sqrt(1.0f - cosTheta*cosTheta);
            Point e(std::cos(theta2)*sinTheta, std::sin(theta2)*sinTheta, cosTheta);
            Vector direction(world(Vector(o, e)));
            Ray ray(o, direction);
            if (!Scene::intersect(ray, hit))
            {
                BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
                result = result + BlinnPhong(wrap, RaytracingXml::interpolation);
            }
        }
    });
    return result/(static_cast<float>(Scene::sources.size()*N));
}

Color TriangleGrid::compute(const Point& observer, const Hit& impact, int N)
{
    return gridDirect(observer, impact, N);
}

Color RandomSource::compute(const Point& observer, const Hit& impact, int N)
{
    Color result;
    Hit hit;
    Point o = shift(impact.p, impact.n);
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_int_distribution<int>    dist(0, Scene::sources.size()-1);
    std::uniform_real_distribution<float> distreal(0.0f, 1.0f);
    for(int i=0;i<N;++i)
    {
        Source& src = Scene::sources.at(dist(mt));
        /*const float x = distreal(mt);
        const float y = distreal(mt);
        const float sqrtu   = std::sqrt(x);
        const float alpha   = (1.0f - y)*sqrtu;
        const float beta    = y*sqrtu;*/
        Vector normal;// = src.normal(alpha, beta);
        //Point e = shift(src.point(alpha, beta), normal);
        Point e = pointOnSource(src, distreal(mt), distreal(mt), normal);
        Ray ray(o, e);
        if (!Scene::intersect(ray, hit))
        {
            FromG_t G;
            float cosThetaP;
            result = result + computeL1(impact, observer, o, e, normal, G, cosThetaP);
            /*BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
            Color brdf     = BlinnPhong(wrap, RaytracingXml::interpolation);
            float cosThetaP = std::cos(dot(normalize(Vector(o, e)), normalize(normal)));
            FromG_t G = computeG(o, impact.n, e, normal, cosThetaP);
            result = result + (G.at(0)*brdf*cosThetaP);*/
        }
    }
    return result/static_cast<float>(N);
}

#define DIRECT_RECIPE(str, classname) str ,  [](void) -> Direct* {return new classname();}
DirectFactory::DirectFactory(void) : Factory<std::string, Direct*>()
{
    this->addRecipes(
        DIRECT_RECIPE("NPointPerSource",   NPointPerSource),
        DIRECT_RECIPE("OnePointPerSource", OnePointPerSource),
        DIRECT_RECIPE("NFibonacci",        FibonacciSpiral),
        DIRECT_RECIPE("NGridTriangle",     TriangleGrid),
        DIRECT_RECIPE("NRandomSource",     RandomSource)
        
    );
}

