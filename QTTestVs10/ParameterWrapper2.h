#pragma once

#include <opencv2/opencv.hpp>
template <typename T>
class ParameterWrapper
{
public:
	ParameterWrapper(T val, std::string name, std::string description)			
	{
		this->mValue		= val; 
		this->mName			= name;
		this->mDescription	= description;
		this->mValue
	};
	ParameterWrapper()	{};

	~ParameterWrapper(void){};

	std::string mName;
	std::string mDescription;
	T			mValue;
};