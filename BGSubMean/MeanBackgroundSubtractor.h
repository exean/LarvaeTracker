/**
 *  @file MeanBackgroundSubtractor.h
 *
 *	@section DESCRIPTION
 *  Generates a matrix of mean pixel values and subtracts 
 *	these from input images.
 */
#pragma once

#include <windows.h>
#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//ABackgroundSubtractor.h"

#define DllExport   __declspec( dllexport )
/**
 *  Generates a matrix of mean pixel values and subtracts 
 *	these from input images.
 */
class MeanBackgroundSubtractor 
	: public Dll::ABackgroundSubtractor
{
public:
	MeanBackgroundSubtractor(void);
	~MeanBackgroundSubtractor(void);

	/**
    * Subtracts the mask from the image.
    *
	* @param	mod_img		Image on which the background 
	*						subtraction will be applied.
    * @return	true, if subtraction could be applied.
    */
	bool apply(
		cv::Mat							& mod_img);
	
	/**
    * Creates a mask from the median of provided images.
    *
	* @param	_files		Images to be used.
	* @param	_roi		Region to be used.
    */
	void create(
		const std::vector<std::string>	& _files,
		const cv::Rect					& _roi);

private:
	/**
	* Mask that will be subtracted from input images.
	*/
	cv::Mat * mMask;

};

extern "C"
{
	/**
    * Creates an instance of this background subtractor.
    *
	* @return	Instance of this class.
    */
	DllExport Dll::ABackgroundSubtractor*	CreateBackgroundSubtractor()
	{
		return new MeanBackgroundSubtractor();
	}	
}