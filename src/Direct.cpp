/**
 * @file Direct.cpp
 */
#include <algorithm>
#include <functional>
#include <random>

#include "Direct.hpp"
#include "BlinnPhong.hpp"
#include "ConfigLoaders.hpp"
#include "core/math_core.hpp"
#include "core/gkit_core.hpp"

namespace
{
    /**
     * @brief Calcule la valeur de G pour <b>(P, nP)</b> le point d'impact du rayon et la soruce <b>(S, nS)</b>.
     * @param[in] P         La position dans le monde du point d'impact.
     * @param[in] nP        La normale de ce point dans le monde.
     * @param[in] S         La position du point de la source dans le monde.
     * @param[in] nS        La normale en ce point de la source dans le monde.
     * @param[in] costhetaP La valeur précalculé de cos(Op).
     * @return La valeur de G, résultat du calcul.
     */
    float computeG(const Point& P, const Vector& nP, const Point& S, const Vector& nS, float costhetaP) noexcept
    {
        float costhetaS = std::cos(dot(normalize(Vector(S, P)), normalize(nS)));
        return (costhetaP*costhetaS) / distance2(P, S);
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
                    BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
                    Color brdf     = BlinnPhong(wrap, RaytracingXml::interpolation);
                    float cosThetaP = std::cos(dot(normalize(Vector(o, e)), normalize(normal)));
                    float G = computeG(o, impact.n, e, normal, cosThetaP);
                    result = result + (G*brdf*cosThetaP);
                }
            }
        });
        
        return result/static_cast<float>(Scene::sources.size()*N);
    }
    
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
                    float u = static_cast<float>(iu)*step;
                    float v = static_cast<float>(jv)*step;
                    if (u+v <= 1.0f)
                    {
                        float sqrtu   = std::sqrt(u);
                        float alpha   = (1.0f - v)*sqrtu;
                        float beta    = v*sqrtu;
                        Vector normal = src.normal(alpha, beta);
                        Point  e      = shift(src.point(alpha, beta), normal);
                        Ray ray(o, e);
                        if (!Scene::intersect(ray, hit))
                        {
                            BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
                            Color brdf     = BlinnPhong(wrap, RaytracingXml::interpolation);
                            float cosThetaP = std::cos(dot(normalize(Vector(o, e)), normalize(normal)));
                            float G = computeG(o, impact.n, e, normal, cosThetaP);
                            result = result + (G*brdf*cosThetaP);
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
        n = src.normal(0.33f, 0.33f);
        return shift(src.point(0.33f, 0.33f), n);
    });
}

Color NPointPerSource::compute(const Point& observer, const Hit& impact, int N)
{
    // The new cool in age way to generate randoms
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return basicDirect(observer, impact, N, [&mt, &dist](Source& src, Vector& n){
        float dx = dist(mt);
        float dy = dist(mt);
        n = src.normal(dx, dy);
        return shift(src.point(dx, dy), n);
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
        for(int i=0;i<N;++i)
        {
            float cosTheta = 1.0f - ((2.0f*i + 1)/(2.0f*N));
            float theta2   = 2.0f*M_PI*((i+u)/phi - std::floor((i+u)/phi));
            float sinTheta = std::sqrt(1.0f - cosTheta*cosTheta);
            Point e(std::cos(theta2)*sinTheta, std::sin(theta2)*sinTheta, cosTheta);
            Ray ray(o, e);
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
    /*std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return basicDirect(observer, impact, N, [&mt, &dist](Source& src, Vector& n){
        float sqrtu1 = std::sqrt(dist(mt));
        float u2 = dist(mt);
        float x = (1.0f - u2)*sqrtu1;
        float y = u2*sqrtu1;
        n = src.normal(x, y);
        return shift(src.point(x, y), n);
    });*/
    return gridDirect(observer, impact, N);
}

Color RandomSource::compute(const Point& observer, const Hit& impact, int N)
{
    Color result;
    bool useDistance = true;
    Hit hit;
    Point o = impact.p + impact.n*0.05;
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_int_distribution<int>    dist(0, Scene::sources.size()-1);
    std::uniform_real_distribution<float> distreal(0.0f, 1.0f);
    for(int i=0;i<N;++i)
    {
        Source& src = Scene::sources.at(dist(mt));
        Point e = src.point(distreal(mt), distreal(mt));
        Ray ray(o, e);
        if (!Scene::intersect(ray, hit))
        {
            BlinnPhongWrapper wrap = {&impact, &Scene::mesh, &observer, &e};
            float Gps = 1.0f;
            if (useDistance)
            {
                Vector PO = normalize(Vector(impact.p, observer));
                Vector PS = normalize(Vector(impact.p, e));
                float cosTheta  = std::abs(dot(impact.n, PS));
                float cosThetas = std::abs(dot(impact.n, normalize((PO + PS)/2.0f)));
                Gps = (cosTheta*cosThetas)/distance2(o, e);
            }
            result = result + BlinnPhong(wrap, RaytracingXml::interpolation)*Gps;
        }
    }
    return result;
}

DirectFactory::DirectFactory(void) : Factory<std::string, Direct*>()
{
    this->addRecipes(
        "NPointPerSource" ,  [](void) -> Direct* {return new NPointPerSource();},
        "OnePointPerSource", [](void) -> Direct* {return new OnePointPerSource();},
        "NFibonacci",        [](void) -> Direct* {return new FibonacciSpiral();},
        "NGridTriangle",     [](void) -> Direct* {return new TriangleGrid();},
        "NRandomSource",     [](void) -> Direct* {return new RandomSource();}
        
    );
}

