/**
 * @file ConfigLoaders.hpp
 * @brief Ces classes vont chargées les différents paramètres depuis les fichiers de configurations
 * Pour l'application.@n
 * Chaque classe porte le nom du fichier qu'elle "charge".
 * @author Laurent Bardoux p1108365
 * @author Mehdi   Ghesh   p1209574
 * @version 2.0
 */
#ifndef CONFIGLOADERS_HPP_INCLUDED
#define CONFIGLOADERS_HPP_INCLUDED

#include <string>


/**
 * @class ImageXml
 * @brief Porte le contenu du fichier image.xml
 */
class ImageXml final
{
    public:
        static std::string outputName; //!< Le nom complet de sauvegarde du résultat.
        static int         width;      //!< La longueur de l'image résultat.
        static int         height;     //!< La largeur de l'image résultat.
        static float       fov;        //!< L'ouverture de la focale.
        
        ImageXml(void) = delete;
    
};

/**
 * @class SceneXml
 * @brief Porte le contenu du fichier scene.xml
 */
class SceneXml final
{
    public:
        static std::string obj;     //!< Le nom de l'obj que l'on va raytracer.
        static std::string orbiter; //!< Le nom de l'orbiter que l'on veut charger.
        
        SceneXml(void) = delete;
    
};

/**
 * @class RaytracingXml
 * @brief Porte le contenu du fichier raytracing.xml
 */
class RaytracingXml final
{
    public:
        static float       interpolation;   //!< Le coefficient pour l'interpolation de Blinn-Phong.
        static int         seed;            //!< La graine pour l'utilisation des randoms.
        static bool        directEnabled;   //!< Pour savoir si on veut faire la luminosité directe.
        static bool        indirectEnabled; //!< Pour savoir si on veut faire la luminosité indirecte.
        static bool        emitedEnabled;   //!< Pour savoir si on veut faire la luminosité émise.
        static int         directN;         //!< Le nombre d'itération   pour la luminosité directe.
        static int         indirectN;       //!< Le nombre d'itération   pour la luminosité indirecte.
        static std::string directMethod;    //!< Le type de méthode directe.
        static std::string indirectMethod;  //!< Le type de méthode indirecte.
        static float       normalTweak;     //!< Le décalage par rapport à la normale.
        
        RaytracingXml(void) = delete;
    
};

/**
 * @class ConfigLoaders
 * @brief Charge les fichiers dans les différentes classes.
 */
class ConfigLoaders final
{
    public:
        /**
         * @brief Charge tous les fichiers XMLs.
         * @throw std::ios_base::failure Si la lecture d'un fichier a échoué.
         * @throw std::string            Si le parsing de l'XML a échoué.
         */
        static void loadXMLs(void);
        
        ConfigLoaders(void) = delete;
    
};


#endif
