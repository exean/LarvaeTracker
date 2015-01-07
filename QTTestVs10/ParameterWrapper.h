/**
 *  @file ParameterWrapper.h
 *
 * @section DESCRIPTION
 * Wrapper for a value of any type specified
 * in EDataType. Also has fields for a name
 * and description. Used for interaction with
 * user. Completely declared inline for easier
 * access from multiple projects.
 */

#pragma once

#include <opencv2/opencv.hpp>
/**
 * Wrapper for a value of any type specified
 * in EDataType. Also has fields for a name
 * and description. Used for interaction with
 * user. Completely declared inline for easier
 * access from multiple projects.
 */
class ParameterWrapper
{
public:
	/**
	* Enumeration of types supported by this
	* class.
	*/
	enum EDataType 
	{
		INT,
		FLOAT,
		DOUBLE,
		SCALAR,
		MAT,
		BOOLEAN
	};

	ParameterWrapper(EDataType type, std::string name, std::string description)	
	{
		this->mType			= type;
		this->mName			= name;
		this->mDescription	= description;
	};
	ParameterWrapper(EDataType type, std::string parsableValue, std::string name, std::string description)	
	{
		this->mType			= type;
		this->mName			= name;
		this->mDescription	= description;
	};
	ParameterWrapper(int val, std::string name, std::string description)			
	{
		this->mInt			= val; 
		this->mType			= INT;
		this->mName			= name;
		this->mDescription	= description;		
	};
	ParameterWrapper(float val, std::string name, std::string description)			
	{
		this->mFloat		= val; 
		this->mType			= FLOAT;
		this->mName			= name;
		this->mDescription	= description;		
	};
	ParameterWrapper(double val, std::string name, std::string description)		
	{
		this->mDouble		= val; 
		this->mType			= DOUBLE;
		this->mName			= name;
		this->mDescription	= description;	
	};
	ParameterWrapper(cv::Mat val, std::string name, std::string description)	
	{
		this->mMat			= val;
		this->mName			= name;
		this->mDescription	= description;
		this->mType			= MAT;
	};
	ParameterWrapper(cv::Scalar val, std::string name, std::string description)	
	{
		this->mScalar		= val;
		this->mName			= name;
		this->mDescription	= description;
		this->mType			= SCALAR;
	};
	ParameterWrapper(bool val, std::string name, std::string description)	
	{
		this->mBoolean		= val;
		this->mName			= name;
		this->mDescription	= description;
		this->mType			= BOOLEAN;
	};
	ParameterWrapper()					{};

	~ParameterWrapper(void){};

	std::string toString()
	{
		std::ostringstream strs;
		switch(this->mType)
		{
		case ParameterWrapper::DOUBLE:
			strs << this->mDouble;
			break;
		case ParameterWrapper::FLOAT:
			strs << this->mFloat;
			break;
		case ParameterWrapper::INT:
			strs << this->mInt;
			break;
		case ParameterWrapper::MAT:
			{
				int w = mMat.size().width;
				int h = mMat.size().height;
				for(int y = 0; y < h; y++)
				{
					strs << "[";
					for(int x = 0; x < w-1; x++)
					{
						strs << (int)mMat.at<uchar>(y,x) << ";";
					}
					strs << (int)mMat.at<uchar>(y,w-1);
					strs << "]";
				}
			}			
			break;
		case ParameterWrapper::SCALAR:
			{
				strs << mScalar[0];
				for(int i = 1; i < mScalar.channels; i++)
				{					
					strs << ";" << mScalar[i];					
				}				
			}			
			break;
		}
		return strs.str();
	}

	std::string mName;
	std::string mDescription;

	EDataType	mType;
	int			mInt;
	float		mFloat;
	double		mDouble;
	cv::Mat		mMat;
	cv::Scalar	mScalar;
	bool		mBoolean;
};