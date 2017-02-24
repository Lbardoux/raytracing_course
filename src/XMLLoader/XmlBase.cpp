#include "XmlBase.hpp"


_XmlBase::_XmlBase(void)
{
	
}


_XmlBase::~_XmlBase(void)
{
	this->root           = nullptr;
	this->currentNode    = nullptr;
	this->currentElement = nullptr;
	while(!this->visited.empty())
	{
		this->visited.pop();
	}
}

void _XmlBase::_gotoRoot(void)
{
	this->currentNode    = this->root;
	this->currentElement = nullptr;
	while(this->visited.empty() == false)
	{
		this->visited.pop();
	}
}

void _XmlBase::_prev(uint32_t of)
{
	while(of > 0 && this->visited.empty() == false)
	{
		this->currentNode = this->visited.top();
		this->visited.pop();
		--of;
	}
}
