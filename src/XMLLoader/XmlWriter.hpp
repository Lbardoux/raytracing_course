/**
 * @file XmlWriter.hpp
 * @brief This file proposes an overlay for tinyxml2 for the writing part only !
 * @author MTLCRBN
 * @version 1.0
 * @date October 25th 2016
 */
#ifndef XMLWRITER_HPP_INCLUDED
#define XMLWRITER_HPP_INCLUDED

#include <string>
#include "XmlBase.hpp"

namespace xml2 = tinyxml2;

/**
 * @brief This class offers a way to write a xml file with a similar way than
 * XmlLoader.
 * @author MTLCRBN
 */
class XmlWriter final : public _XmlBase
{
	private:
		bool        onText;  //!< To know if we have to write a text or an attribute.
		std::string attName; //!< A name for an attribute.
		bool        onNode;  //!< In order to know which was the previously selected thing.
		
		
		XmlWriter(void)                              = delete;
		XmlWriter(const XmlWriter &other)            = delete;
		XmlWriter(XmlWriter &&other)                 = delete;
		XmlWriter& operator=(const XmlWriter &other) = delete;
		XmlWriter& operator=(XmlWriter &&other)      = delete;
	
	public:
		/**
		 * @brief Create a new XmlWriter with \a root as first node.
		 * @param[in] root The name of the first node, so the name of the root of your xml tree under construction.
		 */
		XmlWriter(const std::string &root);
		
		/**
		 * @brief The destructor of this XmlWriter.
		 */
		~XmlWriter(void);
		
		/**
		 * @brief Create an element (leaf of the tree) named \a name inside the current node.
		 * @param[in] name The name of the leaf you wanna create.
		 * @return A reference to your XmlWriter, in order to continue a chain.
		 */
		XmlWriter& element(const std::string &name);
		
		/**
		 * @brief Create and move to this newly created node named \a name.
		 * @param[in] name The name of the node you wanna create and move to.
		 * @return A reference to your XmlWriter, in order to continue a chain.
		 */
		XmlWriter& node(const std::string &name);
		
		/**
		 * @brief Go back of \a of node you previously visited.
		 * @param[in] of The number of node you wanna go back.
		 * @return A reference to your XmlLoader.
		 */
		XmlWriter& prev(uint32_t of = 1);
		
		/**
		 * @brief Write the tree you build into the file \a fname.
		 * @param[in] fname The path+name of your fresh new xml file.
		 * @throw std::string If there is any issue while saving your xml file.
		 * @pre  \a fname is a valid file name, and you do have rights to write into the destination directory.
		 * @post Your xml file is succesfully saved.
		 */
		void saveAs(const std::string &fname);
		
		/**
		 * @brief Reset the "iterators" (such a big word for that kind of stuff)
		 * 
		 * It will send you at the very beginning of your parsing (like you don't even start to parse)
		 * @return A reference on your XmlWriter.
		 */
		XmlWriter& backToRoot(void);
		
		/**
		 * @brief Prepare your writer to write into a leaf.
		 * @return A reference to your XmlWriter, in order to allow you to write your value.
		 */
		XmlWriter& text(void);
		
		/**
		 * @brief Prepare your writer to write an attribute.
		 * It will write this attribute on the previously selected thing (node or element).
		 * @param[in] name The name of the attribute.
		 * @return A reference to your XmlWriter, in order to allow you to write your value.
		 */
		XmlWriter& attribute(const std::string &name);
		
		/**
		 * @brief Allow the user to write \a value as a boolean.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(bool value);
		
		/**
		 * @brief Allow the user to write \a value as an unsigned int.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(unsigned int value);
		
		/**
		 * @brief Allow the user to write \a value as an int.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(int value);
		
		/**
		 * @brief Allow the user to write \a value as a double.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(double value);
		
		/**
		 * @brief Allow the user to write \a value as a float.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(float value);
		
		/**
		 * @brief Allow the user to write \a value as a string.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(const std::string &value);
		
		/**
		 * @brief Allow the user to write \a value as a string.
		 * @param[in] value The value you wanna write.
		 * @return A reference to the XmlWriter, but you don't have to use it again !
		 */
		XmlWriter& operator=(const char *const value);
	
};

#endif
