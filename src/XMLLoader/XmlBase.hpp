/**
 * @file XmlBase.hpp
 * @brief Defines the common aspects for XmlLoader and XmlWriter.
 * @author MTLCRBN
 * @version 1.0
 * @date October 25th 2016
 */
#ifndef XMLBASE_HPP_INCLUDED
#define XMLBASE_HPP_INCLUDED

#include <stack>
#include <cstdint>
#include "tinyxml2.h"

namespace xml2 = tinyxml2;


/**
 * @brief Implement every kind of playaround with stacks and tinyxml2.
 * @author MTLCRBN
 */
class _XmlBase
{
	protected:
		xml2::XMLDocument          doc;            //!< The xml document, to work this.
		xml2::XMLNode*             root;           //!< An address to the first node of the xml file.
		xml2::XMLNode*             currentNode;    //!< The current node    you're working with on the xml tree.
		xml2::XMLElement*          currentElement; //!< The current element you're working with.
		std::stack<xml2::XMLNode*> visited;        //!< A trace of every node visited.
		
		
		//! @brief The basic constructor, only callable by the packages classes.
		_XmlBase(void);
		
		//! @brief Go back to root
		void _gotoRoot(void);
		
		
		/**
		 * @brief Go back of \a of node you previously visited.
		 * @param[in] of The number of node you wanna go back.
		 */
		void _prev(uint32_t of = 1);
	
	public:
		//! @brief The destructor for this member.
		~_XmlBase(void);
	
};

#endif
