#pragma once
#include <string>


//pseudo GUID
class IGUID {
	public:
		std::string m_name;

	public:
		IGUID(std::string name): m_name(name){}
		inline virtual void OnUnparent(){}

};