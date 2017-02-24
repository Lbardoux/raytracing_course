#include "XmlLoader.hpp"


XmlLoader::XmlLoader(const std::string &fname) : _XmlBase()
{
	xml2::XMLError err = this->doc.LoadFile(fname.c_str());
	if (err != xml2::XML_SUCCESS)
	{
		std::cerr << "[ERROR]: while loading " << fname << std::endl;
		throw std::string("File not found");
	}
	this->root = doc.FirstChild();
	if (root == nullptr)
	{
		std::cerr << "[ERROR]: First element does not exist" << std::endl;
		throw std::string("Bad format");
	}
	this->currentNode    = this->root;
	this->currentElement = nullptr;
	this->onNode         = true;
}

XmlLoader::~XmlLoader(void)
{
	this->onNode = false;
}

XmlLoader& XmlLoader::backToRoot(void)
{
	this->_gotoRoot();
	this->onNode = true;
	return *this;
}

void XmlLoader::forEachElementNamed(const std::string &name, std::function<void(void)> lambda)
{
	this->element(name);
	while(this->currentElement != nullptr)
	{
		lambda();
		this->currentElement = this->currentElement->NextSiblingElement(name.c_str());
	}
}

void XmlLoader::forEachNodeNamed(const std::string &name, std::function<void(void)> lambda)
{
	this->node(name);
	while(this->currentNode != nullptr)
	{
		lambda();
		this->currentNode = this->currentNode->NextSiblingElement(name.c_str());
	}
	this->prev();
}

XmlLoader& XmlLoader::element(const std::string &elementName)
{
	this->currentElement = this->currentNode->FirstChildElement(elementName.c_str());
	if (this->currentElement == nullptr)
	{
		std::cerr << "[WARNING]: <" << elementName << "> does not exist" << std::endl;
	}
	this->onNode = false;
	return *this;
}

XmlLoader& XmlLoader::prev(uint32_t of)
{
	this->_prev(of);
	this->onNode = true;
	return *this;
}

XmlLoader& XmlLoader::node(const std::string &name)
{
	xml2::XMLNode *tmp = this->currentNode->FirstChildElement(name.c_str());
	if (tmp != nullptr)
	{
		this->visited.push(this->currentNode);
		this->currentNode = tmp;
	}
	else
	{
		std::cerr << "[WARNING] : There is no child named " << name << std::endl;
	}
	this->onNode = true;
	return *this;
}

#define ATTRIBUTE_MATCH(type, function) \
template<> \
type XmlLoader::attribute(const std::string &att) \
{ \
	xml2::XMLElement* workOn = nullptr; \
	if (this->onNode) \
	{ \
		workOn = (xml2::XMLElement*)this->currentNode; \
	} \
	else \
	{ \
		if (this->currentElement == nullptr) \
		{ \
			std::cerr << "[WARNING] : No node selected." << std::endl; \
			return this->sentinel<type>(); \
		} \
		workOn = this->currentElement; \
	} \
	type tmp; \
	if (workOn->function(att.c_str(), &tmp) != xml2::XML_SUCCESS) \
	{ \
		return this->sentinel<type>(); \
	} \
	return tmp; \
}

#define TEXT_MATCH(type, function)                                 \
template<>                                                         \
type XmlLoader::text(void)                                         \
{                                                                  \
	if (this->currentElement == nullptr)                           \
	{                                                              \
		std::cerr << "[WARNING] : No node selected." << std::endl; \
		return this->sentinel<type>();                             \
	}                                                              \
	type tmp;                                                      \
	if (this->currentElement->function(&tmp) != xml2::XML_SUCCESS) \
	{                                                              \
		return this->sentinel<type>();                             \
	}                                                              \
	return tmp;                                                    \
}

template<>
std::string XmlLoader::text(void)
{
	if (this->currentElement == nullptr)
	{
		std::cerr << "[WARNING] : No node selected." << std::endl;
		return this->sentinel<std::string>();
	}
	const char *value = this->currentElement->GetText();
	if (value == nullptr)
	{
		return this->sentinel<std::string>();
	}
	return std::string(value);
}

template<>
std::string XmlLoader::attribute(const std::string &att)
{
	xml2::XMLElement* workOn = nullptr;
	if (this->onNode)
	{
		workOn = (xml2::XMLElement*)this->currentNode;
	}
	else
	{
		if (this->currentElement == nullptr)
		{
			std::cerr << "[WARNING] : No node selected." << std::endl;
			return this->sentinel<std::string>();
		}
		workOn = this->currentElement;
	}
	const char *value = workOn->Attribute(att.c_str());
	if (value == nullptr)
	{
		return this->sentinel<std::string>();
	}
	return std::string(value);
}

ATTRIBUTE_MATCH(float,        QueryFloatAttribute)
ATTRIBUTE_MATCH(int,          QueryIntAttribute)
ATTRIBUTE_MATCH(unsigned int, QueryUnsignedAttribute)
ATTRIBUTE_MATCH(bool,         QueryBoolAttribute)
ATTRIBUTE_MATCH(double,       QueryDoubleAttribute)

TEXT_MATCH(float,        QueryFloatText)
TEXT_MATCH(int,          QueryIntText)
TEXT_MATCH(unsigned int, QueryUnsignedText)
TEXT_MATCH(double,       QueryDoubleText)
TEXT_MATCH(bool,         QueryBoolText)
