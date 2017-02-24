/**
 * @file XmlLoader.hpp
 * @brief This file proposes an overlay for tinyxml2 for the reading
 * part only !
 * @author MTLCRBN
 * @version 2.0
 * @date October 15th 2016
 */
#ifndef XMLLOADER_HPP_INCLUDED
#define XMLLOADER_HPP_INCLUDED

#include <string>
#include <iostream>
#include <functional>
#include "XmlBase.hpp"

namespace xml2 = tinyxml2;

/**
 * @brief This is the overlay for TinyXML2 for the reading part.
 * @author MTLCRBN
 */
class XmlLoader final : public _XmlBase
{
	private:
		bool onNode; //!< If the last access was on a Node.
		
		/**
		 * @brief Return the default value of the type \b T.
		 * @warning \b T must defines a basic constructor.
		 * @return T()
		 */
		template<typename T>
		inline T sentinel(void) const
		{
			return T();
		}
		
		XmlLoader(void)                              = delete;
		XmlLoader(const XmlLoader &other)            = delete;
		XmlLoader(XmlLoader &&other)                 = delete;
		XmlLoader& operator=(const XmlLoader &other) = delete;
		XmlLoader& operator=(XmlLoader &&other)      = delete;
	
	public:
		/**
		 * @brief Create a XmlLoader, and load the file \a fname with TinyXML2.
		 * @pre  A valid \a fname (file exists, is readable, is xml).
		 * @post A valid XmlLoader ready to use.
		 * @throw std::string if there is issues when opening \a fname.
		 * @throw std::string if there is issues when reading root of xml tree.
		 */
		XmlLoader(const std::string &fname);
		
		/**
		 * @brief Close and erase every things possible from the XMlLoader.
		 */
		~XmlLoader(void);
		
		/**
		 * @brief Select \a elementName as the current element to work with.
		 * It will print a warning if \a nodeName doesn't exist.
		 * @param[in] elementName The name of the node you want to select.
		 * @pre  A valid and existing \a elementName (not "").
		 * @post If the precondition goes well, it binds the desired element.
		 * @return A reference to the XmlLoader, in order to chain it with \b .text() or \b .attribute()
		 */
		XmlLoader& element(const std::string &elementName);
		
		/**
		 * @brief This function allows you to get the value into an attribute named \a att
		 *  from a previously selected element (with element("field") method).
		 * 
		 * @code
		 * <field att="what i want">55</field>
		 * <!-- or -->
		 * <field att="what i want" />
		 * @endcode
		 * 
		 * This function works only with the following template parameters :
		 *    - float
		 *    - double
		 *    - int
		 *    - unsigned int
		 *    - std::string
		 *    - bool
		 * 
		 * @param[in] att The attribute name you want to extract the internal value.
		 * @return the readed value, or a default one if any error occurs.
		 */
		template<typename T>
		T attribute(const std::string &att);
		
		/**
		 * @brief This function allows you to get the text converting into \b T
		 * from the current element (Which was selected by using the element("field") method).
		 * @code
		 * <field>value you want</field>
		 * @endcode
		 * 
		 * This function works only with the following template parameters :
		 *    - float
		 *    - double
		 *    - int
		 *    - unsigned int
		 *    - std::string
		 *    - bool
		 * 
		 * @return the readed value, or a default one if any error occurs.
		 */
		template<typename T>
		T text(void);
		
		/**
		 * @brief Allow the user to iterate over some elements which have the same \a name,
		 * and apply \a lambda at each iteration.
		 *
		 * For example, on a xml like that :
		 * @code
		 * <name>value1</name>
		 * <name>value2</name>
		 * <name>value3</name>
		 * <name>value4</name>
		 * @endcode
		 * It will call .element("name") for you, and you just have to call \b .text() or \b .attribute() statement
		 *  to work the current element.
		 * @param[in] name   The name of the list of element
		 * @param[in] lambda The function you wanna apply for each element.
		 */
		void forEachElementNamed(const std::string &name, std::function<void(void)> lambda);
		
		/**
		 * @brief Offer a way to iterate over some nodes with the same \a name.
		 * 
		 * For example, on a xml like that :
		 * @code
		 * <person>
		 * 	<name>foo</name>
		 * 	<nickname>bar</nickname>
		 * </person>
		 * <person>
		 * 	<name>foo2</name>
		 * 	<nickname>bar2</nickname>
		 * </person>
		 * @endcode
		 * You'll be able to get \b element() within that function.
		 * @param[in] name   The name of the node to iterate with.
		 * @param[in] lambda THe function to apply for each node.
		 *
		 * @warning Actually, you can't get the attribute of the node you iterate with.
		 *          You'll have to use \b forEachElementNamed
		 */
		void forEachNodeNamed(const std::string &name, std::function<void(void)> lambda);
		
		/**
		 * @brief Reset the "iterators" (such a big word for that kind of stuff)
		 * 
		 * It will send you at the very beginning of your parsing (like you don't even start to parse)
		 * @return A reference on your XmlLoader.
		 */
		XmlLoader& backToRoot(void);
		
		/**
		 * @brief Change the current node you're working with, in order to
		 * go down onto the xml tree.
		 * For example, on a xml like that :
		 * @code
		 * <m>
		 * 	<foo />
		 * 	<a>
		 * 		<bar />
		 * 	</a>
		 * </m>
		 * @endcode
		 * If you want to get \b bar, use \b node("a") to change your current node
		 * to this one, and then you could call \b .element("bar").
		 * @param[in] name The node you wanna select.
		 * @return A reference on your XmlLoader.
		 */
		XmlLoader& node(const std::string &name);
		
		/**
		 * @brief Go back of \a of node you previously visited.
		 * @param[in] of The number of node you wanna go back.
		 * @return A reference to your XmlLoader.
		 */
		XmlLoader& prev(uint32_t of = 1);
	
};


#endif
