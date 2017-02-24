#include <iostream>

#include "XmlWriter.hpp"


XmlWriter::XmlWriter(const std::string &root) : _XmlBase()
{
	this->root = this->doc.NewElement(root.c_str());
	this->doc.InsertFirstChild(this->root);
	this->currentNode    = this->root;
	this->currentElement = nullptr;
	this->onNode         = true;
	this->onText         = true;
}

XmlWriter::~XmlWriter(void)
{
	this->onNode = false;
	this->onText = false;
	this->attName.clear();
}

void XmlWriter::saveAs(const std::string &fname)
{
	if (this->doc.SaveFile(fname.c_str()) != xml2::XML_SUCCESS)
	{
		throw std::string("Error while writing ") + fname;
	}
}

XmlWriter& XmlWriter::element(const std::string &name)
{
	this->currentElement = this->doc.NewElement(name.c_str());
	this->currentNode->InsertEndChild(this->currentElement);
	this->onNode = false;
	return *this;
}

XmlWriter& XmlWriter::node(const std::string &name)
{
	xml2::XMLNode* tmp = this->doc.NewElement(name.c_str());
	this->currentNode->InsertEndChild(tmp);
	this->visited.push(this->currentNode);
	this->currentNode = tmp;
	this->onNode = true;
	return *this;
}

XmlWriter& XmlWriter::prev(uint32_t of)
{
	this->_prev(of);
	this->onNode = true;
	return *this;
}

XmlWriter& XmlWriter::text(void)
{
	this->onText = true;
	this->attName.clear();
	return *this;
}

XmlWriter& XmlWriter::attribute(const std::string &name)
{
	this->onText = false;
	this->attName = name;
	return *this;
}

XmlWriter& XmlWriter::backToRoot(void)
{
	this->_gotoRoot();
	this->onNode = true;
	return *this;
}


#define AFFECT(value) \
	if (this->onText) \
	{ \
		if (this->currentElement == nullptr) \
		{ \
			std::cerr << "[WARNING] : Cannot attach a text without any element selected !" << std::endl; \
			return *this; \
		} \
		this->currentElement->SetText(value); \
	} \
	else \
	{ \
		xml2::XMLElement* here = nullptr; \
		if (this->onNode) \
		{ \
			here = (xml2::XMLElement*)this->currentNode; \
		} \
		else \
		{ \
			if (this->currentElement == nullptr) \
			{ \
				std::cerr << "[WARNING] : Cannot attach an attribute without any element selected !" << std::endl; \
				return *this; \
			} \
			here = this->currentElement; \
		} \
		here->SetAttribute(this->attName.c_str(), value); \
	}



XmlWriter& XmlWriter::operator=(bool value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(unsigned int value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(int value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(double value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(float value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(const char *const value)
{
	AFFECT(value);
	return *this;
}

XmlWriter& XmlWriter::operator=(const std::string &value)
{
	AFFECT(value.c_str());
	return *this;
}
