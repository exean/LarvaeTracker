#include "PreprocessingMethods.h"

namespace PreprocessingFuncs
{
PreprocessingMethods::PreprocessingMethods(void)
{
}


PreprocessingMethods::~PreprocessingMethods(void)
{
}

std::string PreprocessingMethods::mMethodNames[] = 
{
	"medianBlur", "GaussianBlur", "threshold", "dilate", "erode", "cvtColor"
};

std::string	PreprocessingMethods::EMethodsToString(const EMethods m)
{
	return mMethodNames[m];
}

PreprocessingMethods::EMethods PreprocessingMethods::StringToEMethods(const std::string m)
{
	for(unsigned int i = 0; 
		i < mMethodNames->size();
		i++)
	{
		if(mMethodNames[i].compare( m ) == 0)
		{
			return (EMethods)i;
		}
	}
	throw std::exception("No such method");
}

#pragma region PreprocessingMethods
	std::map< PreprocessingMethods::EMethods, std::vector< ParameterWrapper > >* PreprocessingMethods::mAvailablePreprocessingMethods = NULL; 
	std::map< PreprocessingMethods::EMethods, std::vector< ParameterWrapper > >* PreprocessingMethods::GetAvailablePreprocessingMethods()
	{
		if(!mAvailablePreprocessingMethods)
		{
			mAvailablePreprocessingMethods = new std::map< EMethods, std::vector< ParameterWrapper > >();


			std::vector< ParameterWrapper > medianBlur;
			medianBlur.push_back(
				ParameterWrapper(
					3,//ParameterWrapper::EDataType::_INT, 
					"ksize", 
					"The aperture linear size. It must be odd and more than 1, i.e. 3, 5, 7 ..."));		
			(*mAvailablePreprocessingMethods)[MedianBlur] = medianBlur;


			std::vector< ParameterWrapper > gaussianBlur;		
			gaussianBlur.push_back(
				ParameterWrapper(
					3,//ParameterWrapper::EDataType::_INT,
					"ksize.width",
					"The Gaussian kernel size; ksize.width and ksize.height can differ, but they both must be positive and odd. Or, they can be zero’s, then they are computed from sigma*"));
			gaussianBlur.push_back(
				ParameterWrapper(
					3,//ParameterWrapper::EDataType::_INT,
					"ksize.height",
					"The Gaussian kernel size; ksize.width and ksize.height can differ, but they both must be positive and odd. Or, they can be zero’s, then they are computed from sigma*"));
			gaussianBlur.push_back(
				ParameterWrapper(
					0.0,//ParameterWrapper::EDataType::_DOUBLE,
					"sigmaX",
					"The Gaussian kernel standard deviation in X direction. If sigmaX and sigmaY are both zeros, they are computed from ksize.width and ksize.height, respectively."));
			gaussianBlur.push_back(
				ParameterWrapper(
					0.0,//ParameterWrapper::EDataType::_DOUBLE,
					"sigmaY",
					"The Gaussian kernel standard deviation in Y direction. If sigmaY is zero, it is set to be equal to sigmaX. If sigmaX and sigmaY are both zeros, they are computed from ksize.width and ksize.height, respectively."));
			gaussianBlur.push_back(
				ParameterWrapper(
					4,//ParameterWrapper::EDataType::_INT,
					"borderType",
					"Pixel extrapolation method [BORDER_CONSTANT=0, BORDER_REPLICATE=1, BORDER_REFLECT=2, BORDER_WRAP=3, BORDER_REFLECT_101=4]"));
			(*mAvailablePreprocessingMethods)[GaussianBlur] = gaussianBlur;


			std::vector< ParameterWrapper > threshold;		
			threshold.push_back(
				ParameterWrapper(
					40.0,//ParameterWrapper::EDataType::_DOUBLE,
					"thresh",
					"Threshold value"));
			threshold.push_back(
				ParameterWrapper(
					255.0,//ParameterWrapper::EDataType::_DOUBLE,
					"maxVal",
					"Maximum value to use with THRESH_BINARY and THRESH_BINARY_INV thresholding types"));
			threshold.push_back(
				ParameterWrapper(
					0,//ParameterWrapper::EDataType::_INT,
					"thresholdType",
					"Thresholding type [THRESH_BINARY=0, THRESH_BINARY_INV=1, THRESH_TRUNC=2, THRESH_TOZERO=3, THRESH_TOZERO_INV=4]"));
			(*mAvailablePreprocessingMethods)[Threshold] = threshold;


			std::vector< ParameterWrapper > dilate;	
			dilate.push_back(
				ParameterWrapper(
					cv::Mat(5,5,CV_8U,cv::Scalar(1)), //ParameterWrapper::EDataType::_MAT,
					"element",
					"Structuring element used for dilation."));
			dilate.push_back(
				ParameterWrapper(
					-1,//ParameterWrapper::EDataType::_INT,
					"anchor.x",
					"Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center."));
			dilate.push_back(
				ParameterWrapper(
					-1,//ParameterWrapper::EDataType::_INT,
					"anchor.y",
					"Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center."));
			dilate.push_back(
				ParameterWrapper(
					1,//ParameterWrapper::EDataType::_INT,
					"iterations",
					"Number of times dilation is applied."));
			dilate.push_back(
				ParameterWrapper(
					0,//ParameterWrapper::EDataType::_INT,
					"borderType",
					"Pixel extrapolation method [BORDER_CONSTANT=0, BORDER_REPLICATE=1, BORDER_REFLECT=2, BORDER_WRAP=3, BORDER_REFLECT_101=4]"));
			dilate.push_back(
				ParameterWrapper(
					cv::Scalar(0),//ParameterWrapper::EDataType::_SCALAR,
					"borderValue",
					"Border value in case of a constant border"));
			(*mAvailablePreprocessingMethods)[Dilate] = dilate;


			std::vector< ParameterWrapper > erode;		
			erode.push_back(
				ParameterWrapper(
					cv::Mat(5,5,CV_8U,cv::Scalar(1)),//ParameterWrapper::EDataType::_MAT,
					"element",
					"Structuring element used for erosion."));
			erode.push_back(
				ParameterWrapper(
					-1,//ParameterWrapper::EDataType::_INT,
					"anchor.x",
					"Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center."));
			erode.push_back(
				ParameterWrapper(
					-1,//ParameterWrapper::EDataType::_INT,
					"anchor.y",
					"Position of the anchor within the element; default value (-1, -1) means that the anchor is at the element center."));
			erode.push_back(
				ParameterWrapper(
					1,//ParameterWrapper::EDataType::_INT,
					"iterations",
					"Number of times erosion is applied."));
			erode.push_back(
				ParameterWrapper(
					0,//ParameterWrapper::EDataType::_INT,
					"borderType",
					"Pixel extrapolation method [BORDER_CONSTANT=0, BORDER_REPLICATE=1, BORDER_REFLECT=2, BORDER_WRAP=3, BORDER_REFLECT_101=4]"));
			erode.push_back(
				ParameterWrapper(
					cv::Scalar(0),//ParameterWrapper::EDataType::_SCALAR,
					"borderValue",
					"Border value in case of a constant border"));
			(*mAvailablePreprocessingMethods)[Erode] = erode;


			std::vector< ParameterWrapper > cvtColor;		
			cvtColor.push_back(
				ParameterWrapper(
					0,
					"code",
					"Color space conversion code. [CV_BGR2GRAY=6, ]¸"));
			(*mAvailablePreprocessingMethods)[CvtColor] = cvtColor;
		}

		return mAvailablePreprocessingMethods;
	}
#pragma endregion
}