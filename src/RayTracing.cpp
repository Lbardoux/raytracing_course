#include <cstdlib>
#include <iostream>

#include "gkit_core.hpp"
#include "ray_core.hpp"
#include "time_core.hpp"
#include "ConfigLoaders.hpp"
#include "Direct.hpp"
#include "tonemapper.hpp"

/**
 * @brief Crée le point d'origine de tous les rayons.
 * @param[in]     image L'image dans laquelle on va écrire le résultat.
 * @param[in,out] o     Le point résultat.
 * @return o
 */
Point& createNearPoint(const Image& image, Point& o, Point& d0, Vector& dx0, Vector& dy0)
{
    Scene::camera.frame(image.width(), image.height(), 1, ImageXml::fov, d0, dx0, dy0);
    o = Scene::camera.position();
    return o;
}

/**
 * @brief Utilise les fichiers xml pour configurer la scène.
 * @pre loadXMLs doit avoir été appelé au préalable.
 */
void initializeScene(void)
{
    Scene::mesh = read_mesh(SceneXml::obj.c_str());
    Scene::camera.read_orbiter(SceneXml::orbiter.c_str());
    Scene::build_triangles();
    Scene::build_sources();
}

/**
 * @brief Applique un tonemappage en effectuant les conversions nécessaires.
 * @param[in] initial La couleur avant tonemapping
 * @return La nouvelle couleur compressée.
 */
Color tonemap(const Color& initial)
{
    tonemapped_color_t r = tonemapper({initial.r, initial.g, initial.b});
    return Color(r[0], r[1], r[2], 1.0f);
}

/**
 * @brief Initialise les méthodes en fonctions des paramètres.
 * @param[out] direct   un pointeur de pointeur sur une méthode d'éclairage directe.
 * @param[out] indirect un pointeur de pointeur sur une méthode d'éclairage indirecte.
 */
void initializeMethod(Direct** direct/*, Indirect** indirect*/)
{
    if (RaytracingXml::directEnabled)
    {
        DirectFactory fac;
        *direct = fac.craft(RaytracingXml::directMethod);
    }
    /*if (RaytracingXml::indirectEnabled)
    {
        IndirectFactory fac;
        fac.recipes();
        *indirect = fac.make(RaytracingXml::indirectMethod);
    }*/
}

int main(UNUSED(int argc), UNUSED(char** argv))
{
    ConfigLoaders::loadXMLs();
    Image image(ImageXml::width, ImageXml::height);
    initializeScene();
    Direct*   directMethod(nullptr);
    //Indirect* indirectMethod(nullptr);
    initializeMethod(&directMethod/*, &indirectMethod*/);
    
    Point o, d0;
    Vector dx0, dy0;
    createNearPoint(image, o, d0, dx0, dy0);
    
    timeBeginFunc("Programme principal");
    #pragma omp parallel for schedule(dynamic, 16) firstprivate(o, d0, dx0, dy0)
    for(int y=0;y<image.height();++y)
    {
        for(int x=0;x<image.width();++x)
        {
            Color emited, direct, indirect;
            Hit hitFromCamera;
            Point e = d0 + x*dx0 + y*dy0;
            Ray ray(o, e);
            if (Scene::intersect(ray, hitFromCamera))
            {
                if (RaytracingXml::emitedEnabled)
                {
                    emited = Scene::mesh.triangle_material(hitFromCamera.object_id).emission;
                }
                if (RaytracingXml::directEnabled)
                {
                    direct = directMethod->compute(o, hitFromCamera, RaytracingXml::directN);
                }
                /*if (RaytracingXml::indirectEnabled)
                {
                    indirect = Black();
                }*/
            }
            image(x, y) = Color(tonemap(direct) + emited + indirect, 1.0f);
        }
    }
    timeEndFunc();
    timePrint();

    std::cout << "Sauvegarde de " << ImageXml::outputName << std::endl;
    write_image(image, ImageXml::outputName.c_str());
    return EXIT_SUCCESS;
}
