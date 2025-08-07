#pragma once
#include <string>
#include <Settings.h>
#include <Game Engine/EngineTime.h>
//pseudo GUID
class IGUID {
	public:
		std::string m_name;
		std::string GUID;
	public:

		IGUID(std::string name): m_name(name){
			std::stringstream s;
			s <<  Settings::Global_Object_Count << EngineTime::now();
			GUID = s.str();
			Settings::Global_Object_Count++;
		}

		inline virtual void OnUnparent(){}
		inline virtual void OnParent(IGUID* new_parent){}
};