/**
 * @file Factory.hpp
 * @brief This file defines a templated factory for further usages.
 * 
 * You should be use to, it requires -std=c++11 or any further version to compile.
 * @author MTLCRBN
 * @version 1.0
 */
#ifndef MTLCRBN_FACTORY_HPP_INCLUDED
#define MTLCRBN_FACTORY_HPP_INCLUDED

#include <vector>     // For std::vector
#include <map>        // For std::map.
#include <functional> // For std::function.
#include <utility>    // For std::make_pair.
#include <stdexcept>  // For std::invalid_argument.


/**
 * @class Factory
 * @brief This class defines a fully parametrable factory process.
 * @tparam Command The input value, which decides which thing to craft.
 * @tparam Product The kind of object as output.
 * @tparam Any type which is callable (function pointer, Functor, lambda, etc).
 * 
 * To use this class, you shall instanciate it, and then provide recipes, as follow :
 * @code
 * Factory<std::string, int> intFactory;
 * // Creating recipe with default Crafter param.
 * intFactory.addRecipe("default", [](void) -> int {return 25;});
 * 
 * std::cout << intFactory.craft("default") << std::endl; // output : 25
 * @endcode
 * Of course, fabricator (the third template argument) could be :
 *     - A lambda [](<b>void</b>) <b>-> Product</b>{...}
 *     - An explicit functor (class with Product operator()(void) defined)
 *     - A function pointer Product(*)(void)
 * 
 */
template<typename Command, typename Product, typename Crafter = std::function<Product(void)>>
class Factory
{
    public:
        // Provides a bunch of typedefs for any utility.
        typedef Command input_type;    //!< To retrieve Command type.
        typedef Product output_type;   //!< To retrieve Product type.
        typedef Crafter callable_type; //!< To retrieve Crafter type.
        
        // Default functions.
        Factory(void) = default;
        Factory(const Factory<Command, Product, Crafter>& other) = default;
        Factory(Factory<Command, Product, Crafter>&& other) = default;
        Factory<Command, Product, Crafter>& operator=(const Factory<Command, Product, Crafter>& other) = default;
        Factory<Command, Product, Crafter>& operator=(Factory<Command, Product, Crafter>&& other) = default;
        
        /**
         * @brief Settings up for legacy.
         * It does empty the internal container.
         */
        virtual ~Factory(void) noexcept
        {
            this->recipesList.clear();
            
        }
        /**
         * @brief Constructs a factory by precising recipes directly.
         * @param[in] args Every recipes you wanna precise.
         * Follow this format : Command, Crafter, Command, Crafter, else it will not compute.
         */
        template<typename... Args>
        Factory(const Args&... args) noexcept
        {
            this->buildUp(args...);
        }
        /**
         * @brief Adds a recipe into the craft list if it doesn't exist
         * @param[in] command          The input to trigger the craft.
         * @param[in] crafter          The task to produce the ouput.
         * @param[in] overrideIfExists If you shall override the previously defined recipe for this input.
         * @return true if the recipe was added succesfully, false otherwise.
         */
        bool addRecipe(const Command& command, const Crafter& crafter, bool overrideIfExists = false) noexcept
        {
            auto it = this->recipesList.find(command);
            if (it == this->recipesList.end())
            {
                this->recipesList.insert(std::make_pair(command, crafter));
                return true;
            }
            else if (overrideIfExists)
            {
                it->second = crafter;
                return true;
            }
            return false;
        }
        /**
         * @brief Allows you to add multiple recipes in one call.
         * @param[in] args Every arguments.
         * @pre args must match this pattern command crafter command crafter etc
         */
        template<typename... Args>
        void addRecipes(const Args&... args) noexcept
        {
            this->buildUp(args...);
        }
        /**
         * @brief Start the production if @b command is available on any recipe.
         * @param[in] command What you wanna craft.
         * @return The producted item.
         * @throw std::invalid_argument If @b command is unavailable.
         */
        Product craft(const Command& command) const
        {
            auto it = this->recipesList.find(command);
            if (it != this->recipesList.end())
            {
                return it->second();
            }
            throw std::invalid_argument("Invalid command required for this factory");
        }
        /**
         * @brief Crafts @b n objects and stores them into a container you could choose (@b std::vector by default).
         * @tparam Container A container that supports @b push_back() statement.
         * @param[in] command The input key to starts the production.
         * @param[in] n       The number of objects you wanna craft.
         * @return A container by move fulled with your @b n objects.
         * @throw std::invalid_argument If @b command is unavailable.
         */
        template<template<typename...> class Container=std::vector>
        Container<Product> craftn(const Command& command, int n)
        {
            Container<Product> container;
            for(int i=0;i<n;++i)
            {
                container.push_back(this->craft(command));
            }
            return container;
        }
        /**
         * @brief Counts how many recipes this factory owns.
         * @return The number of recipes.
         */
        unsigned int size(void) const noexcept
        {
            return this->recipesList.size();
        }
        /**
         * @brief Checks if @b command is a valid input for a recipe.
         * @param[in] command The input value to test.
         * @return true if it exists, false otherwise.
         */
        bool exists(const Command& command) const noexcept
        {
            return this->recipesList.find(command) != this->recipesList.end();
        }
        
        
    private:
        std::map<Command, Crafter> recipesList; //!< All recipes known by this factory.
        
        /**
         * @brief Terminal case, does nothing.
         */
        void buildUp(void)
        {
            
        }
        /**
         * @brief Add one recipe to the recipe list.
         * @param[in] command The input for this recipe.
         * @param[in] crafter The task to execute in order to produce the ouput.
         * @param[in] left    All remaining arguments.
         */
        template<typename... Args>
        void buildUp(const Command& command, const Crafter& crafter, const Args&... left)
        {
            this->addRecipe(command, crafter, true);
            this->buildUp(left...);
        }
    
};


#endif
