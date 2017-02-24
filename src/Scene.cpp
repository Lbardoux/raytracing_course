/**
 * @file Scene.cpp
 */
#include <iostream>

#include "Scene.hpp"

Orbiter               Scene::camera;
std::vector<Triangle> Scene::triangles;
std::vector<Source>   Scene::sources;
Mesh                  Scene::mesh;


unsigned int Scene::build_sources(void)
{
    for(int i=0;i<Scene::mesh.triangle_count();++i)
    {
        Material material = Scene::mesh.triangle_material(i);
        if((material.emission.r + material.emission.g + material.emission.b) > 0)
        {
            Scene::sources.push_back(Source(Scene::mesh.triangle(i), material.emission));
        }
    }
    std::cout << "Nombre de sources : " << Scene::sources.size() << std::endl;
    return Scene::sources.size();
}

unsigned int Scene::build_triangles(void)
{
    Scene::triangles.reserve(Scene::mesh.triangle_count());
    for(int i=0;i<Scene::mesh.triangle_count();++i)
    {
        Scene::triangles.push_back(Triangle(Scene::mesh.triangle(i)));
    }
    std::cout << "Nombre de triangles : " << Scene::triangles.size() << std::endl;
    return Scene::triangles.size();
}

bool Scene::intersect(const Ray& ray, Hit& hit)
{
    hit.t = ray.tmax;
    for(std::size_t i=0;i<Scene::triangles.size();++i)
    {
        float t, u, v;
        if(Scene::triangles.at(i).intersect(ray, hit.t, t, u, v))
        {
            hit.t = t;
            hit.u = u;
            hit.v = v;
            hit.p = ray(t); // evalue la positon du point d'intersection sur le rayon
            hit.n = Scene::triangles.at(i).normal(u, v);
            hit.object_id = i; // permet de retrouver toutes les infos associees au triangle
        }
    }
    return (hit.object_id != -1);
}
