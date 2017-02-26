#ifndef _TIME_CORE_HPP__
#define _TIME_CORE_HPP__ 

#include <time.h>
#include <iostream>
#include <string>
#include <unistd.h>

/**
 * @date       25-Feb-2017
 * @brief      Enregistre le temps en début d'appel de fonction
 * @param[in]  func_name  Nom de la fonction
 */
void timeBeginFunc(const std::string& func_name);
/**
 * @date       25-Feb-2017
 * @brief      Enregistre le temps en fin d'appel de fonction
 */
void timeEndFunc();
/**
 * @date       25-Feb-2017
 * @brief      Affiche le temps qu'a pris la dernière fonction
 * @pre        Il faut avoir appelé timeBeginFunc et timeEndFunc avant
 */
void timePrint();

#endif