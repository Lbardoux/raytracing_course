/**
 * @file ConfigLoaders.cpp
 */
#include <iostream>
#include <sstream>
#include <ctime>

#include "ConfigLoaders.hpp"
#include "XmlLoader.hpp"


std::string ImageXml::outputName;
int         ImageXml::width;
int         ImageXml::height;
float       ImageXml::fov;

std::string SceneXml::obj;
std::string SceneXml::orbiter;

float       RaytracingXml::interpolation;
int         RaytracingXml::seed;
bool        RaytracingXml::directEnabled;
bool        RaytracingXml::indirectEnabled;
bool        RaytracingXml::emitedEnabled;
int         RaytracingXml::directN;
int         RaytracingXml::indirectN;
std::string RaytracingXml::directMethod;
std::string RaytracingXml::indirectMethod;
float       RaytracingXml::normalTweak;


namespace
{
    /**
     * @brief Charge le contenu du fichier @b raytracing.xml dans la classe @b RaytracingXml.
     * @throw std::ios_base::failure Si la lecture du fichier a échoué.
     * @throw std::string            Si le parsing de l'XML a échoué.
     */
    void loadRaytracing(void)
    {
        XmlLoader file("data/xml/raytracing.xml");
        if (file.element("randomSeed").attribute<bool>("time") == false)
        {
            RaytracingXml::seed = time(nullptr);
        }
        else
        {
            RaytracingXml::seed = file.text<int>();
        }
        RaytracingXml::interpolation = file.element("phongInterpolation").text<float>();
        RaytracingXml::directEnabled = file.node("direct").attribute<bool>("enable");
        if (RaytracingXml::directEnabled)
        {
            RaytracingXml::directN      = file.element("N").text<int>();
            RaytracingXml::directMethod = file.element("enumMethod").text<std::string>();
            RaytracingXml::normalTweak  = file.element("normalTweak_f").text<float>();
        }
        RaytracingXml::indirectEnabled = file.prev().node("indirect").attribute<bool>("enable");
        if (RaytracingXml::indirectEnabled)
        {
            RaytracingXml::indirectN      = file.element("N").text<int>();
            RaytracingXml::indirectMethod = file.element("enumMethod").text<std::string>();
        }
        RaytracingXml::emitedEnabled = file.prev().element("emited").attribute<bool>("enable");
    }
    
    /**
     * @brief Charge le contenu du fichier @b scene.xml dans la classe @b SceneXml.
     * @throw std::ios_base::failure Si la lecture du fichier a échoué.
     * @throw std::string            Si le parsing de l'XML a échoué.
     */
    void loadScene(void)
    {
        XmlLoader file("data/xml/scene.xml");
        SceneXml::obj     = file.element("obj").text<std::string>();
        SceneXml::orbiter = file.element("orbiter").text<std::string>();
    }
    
    /**
     * @brief Construit le nom complet du fichier résultat.
     * @param[out] stream Un flux de construction avec au préalable la base du nom du fichier.
     * @pre Tout doit etre chargé au préalable.
     */
    void buildFullname(std::stringstream& stream)
    {
        std::string obj     = SceneXml::obj.substr(SceneXml::obj.rfind('/')+1, SceneXml::obj.rfind(".obj")-5);
        std::string orbiter = SceneXml::orbiter.substr(SceneXml::orbiter.rfind('/')+1, SceneXml::orbiter.rfind(".txt"));
        stream << obj << '_' << orbiter << '_';
        if (RaytracingXml::emitedEnabled)
        {
            stream << "L0";
        }
        if (RaytracingXml::directEnabled)
        {
            stream << "_L1-N=" << RaytracingXml::directN << '-' << RaytracingXml::directMethod;
        }
        if (RaytracingXml::indirectEnabled)
        {
            stream << "_L2" << RaytracingXml::indirectMethod << RaytracingXml::indirectN;
        }
        stream << ".png";
    }
    
    /**
     * @brief Charge le contenu du fichier @b image.xml dans la classe @b ImageXml.
     * @throw std::ios_base::failure Si la lecture du fichier a échoué.
     * @throw std::string            Si le parsing de l'XML a échoué.
     */
    void loadImage(void)
    {
        XmlLoader file("data/xml/image.xml");
        ImageXml::fov        = file.element("fov").text<float>();
        ImageXml::width      = file.element("width").text<int>();
        ImageXml::height     = file.element("height").text<int>();
        std::string basename = file.element("output").text<std::string>();
        std::stringstream fullname;
        fullname << basename;
        buildFullname(fullname);
        
        // Gaffe au move ici.
        ImageXml::outputName = std::move(fullname.str());
    }
    
}



void ConfigLoaders::loadXMLs(void)
{
    // Ajouter une fonction de chargement ici pour prendre en compte un nouveau fichier.
    loadRaytracing();
    loadScene();
    loadImage();
    
}
