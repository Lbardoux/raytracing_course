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

namespace
{
    /**
     * @brief Calcule un point sur un @b hit par rapport à sa normale.
     * @param[in] hit Le hit dont on veut récupérer le point décalé par rapport à la normale.
     * @return Le point origine d'un rayon, décalé par rapport à la normale.
     */
    Point defineImpact(const Hit& hit) noexcept
    {
        return hit.p + hit.n*RaytracingXml::normalTweak;
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
    Color basicDirect(const Point& observer, const Hit& impact, int N, std::function<Point(Source&)> randFunction, bool useDistance = false)
    {
        Color result;
        Hit hit;
        Point o = defineImpact(impact);
        std::for_each(Scene::sources.begin(), Scene::sources.end(), [&](Source& src){
            for(int i=0;i<N;++i)
            {
                Point e = randFunction(src);
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
        });
        
        return result/(static_cast<float>(Scene::sources.size()*N));
    }
}


Color OnePointPerSource::compute(const Point& observer, const Hit& impact, int N)
{
    (void)N;
    return basicDirect(observer, impact, 1, [](Source& src){return src.point(0.33f, 0.33f) + src.normal(0.33f, 0.33f)*RaytracingXml::normalTweak;}, true);
}

Color NPointPerSource::compute(const Point& observer, const Hit& impact, int N)
{
    // The new cool in age way to generate randoms
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return basicDirect(observer, impact, N, [&mt, &dist](Source& src){
        float dx = dist(mt);
        float dy = dist(mt);
        return src.point(dx, dy) + src.normal(dx, dy)*RaytracingXml::normalTweak;
    }, true);
}

#define SQRT_5 2.236067977f
Color FibonacciSpiral::compute(const Point& observer, const Hit& impact, int N)
{
    Color result;
    Hit hit;
    Point o = defineImpact(impact);
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
    return result/(static_cast<float>(Scene::sources.size()));
}

Color TriangleGrid::compute(const Point& observer, const Hit& impact, int N)
{
    std::random_device rd;
    std::mt19937       mt(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return basicDirect(observer, impact, N, [&mt, &dist](Source& src){
        float sqrtu1 = std::sqrt(dist(mt));
        float u2 = dist(mt);
        return src.point((1.0f - u2)*sqrtu1, u2*sqrtu1);
    });
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

