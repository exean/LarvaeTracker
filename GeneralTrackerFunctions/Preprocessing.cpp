#include "Preprocessing.h"
namespace PreprocessingFuncs
{
	Preprocessing::Preprocessing(void)
	{
	}

	Preprocessing::~Preprocessing(void)
	{
	}

	void Preprocessing::setPreprocessingMethods	(
		const vector< PreprocessingMethods::EMethods >	& _methodNames, 
		const vector< vector< ParameterWrapper >* >		& _methodParams)
	{
		this->mAssignedMethods		 = _methodNames;
		this->mAssignedMethodParams	 = _methodParams;
	}

	vector< vector<Point> > Preprocessing::getContours(
				cv::Mat									& mod_source, 
		const	unsigned int							  _minSize)
	{	
		vector< vector<Point> > contours;

		 /**
		 * OPENCV-NOTE: Source image is modified by this function. 
		 * Also, the function does not take into account 1-pixel 
		 * border of the image (it’s filled with 0’s and used for 
		 * neighbor analysis in the algorithm), therefore the 
		 * contours touching the image border will be clipped.
		 */
		cv::findContours(
			mod_source,				// Binary image
			contours,				// a vector of contours
			CV_RETR_EXTERNAL,		// retrieve the external contours
			CV_CHAIN_APPROX_NONE);	// all pixels of each contours	
	
		//remove small contours
		for (unsigned int i = 0; 
			i < contours.size();)
		{
			if (contours[i].size() < _minSize)
			{
				contours.erase(contours.begin() + i);
			}
			else
			{
				i++;
			}
		}
		return contours;
	}

#pragma region UserdefinedPreprocessing
	Mat Preprocessing::getBinaryFBImage(const Mat & _source)
	{
		return this->getProcessedImage(
			_source,
			this->mAssignedMethods, 
			this->mAssignedMethodParams);
	}

	cv::Mat Preprocessing::getProcessedImage(
		const Mat										& _source, 
		const vector< PreprocessingMethods::EMethods >	& _methodNames, 
		const vector< vector< ParameterWrapper >* >		& _methodParams)
	{
		cv::Mat res = _source.clone();
		
		vector< PreprocessingMethods::EMethods >::const_iterator	methodNameIt	= _methodNames.begin();
		vector< vector< ParameterWrapper >* >::const_iterator		methodParamIt	= _methodParams.begin();

		//Apply all image operations in turn
		for(;
			methodNameIt != _methodNames.end();
			methodNameIt++, methodParamIt++)
		{
			this->applyMethod(res, *methodNameIt, *methodParamIt);
		}

		return res;
	}

	void Preprocessing::applyMethod(
		Mat										& mod_image, 
		const PreprocessingMethods::EMethods	& _method, 
		const vector< ParameterWrapper >		* _methodParams)
	{
		switch(_method)
		{
		case PreprocessingMethods::MedianBlur:
			cv::medianBlur(
				mod_image, 
				mod_image,
				(*_methodParams)[0].mInt);
			break;
		case PreprocessingMethods::GaussianBlur:
			cv::GaussianBlur(
				mod_image, 
				mod_image, 
				cv::Size((*_methodParams)[0].mInt, (*_methodParams)[1].mInt),
				(*_methodParams)[2].mDouble,
				(*_methodParams)[3].mDouble,
				(*_methodParams)[4].mInt);
			break;
		case PreprocessingMethods::Threshold:
			cv::threshold(
				mod_image,
				mod_image,
				(*_methodParams)[0].mDouble,
				(*_methodParams)[1].mDouble,
				(*_methodParams)[2].mInt);
			break;
		case PreprocessingMethods::Dilate:
			cv::dilate(
				mod_image,
				mod_image, 
				(*_methodParams)[0].mMat,
				cv::Point((*_methodParams)[1].mInt, (*_methodParams)[2].mInt),
				(*_methodParams)[3].mInt,
				(*_methodParams)[4].mInt,
				(*_methodParams)[5].mScalar);
			break;
		case PreprocessingMethods::Erode:
			cv::erode(
				mod_image,
				mod_image, 
				(*_methodParams)[0].mMat,
				cv::Point((*_methodParams)[1].mInt, (*_methodParams)[2].mInt),
				(*_methodParams)[3].mInt,
				(*_methodParams)[4].mInt,
				(*_methodParams)[5].mScalar);
			break;
		case PreprocessingMethods::CvtColor:
			cv::cvtColor(
				mod_image,
				mod_image,
				(*_methodParams)[0].mInt);
			break;
		}
	}
#pragma endregion

}