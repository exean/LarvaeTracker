/**
 *  @file Preprocessing.h
 *
 *	@section DESCRIPTION
 *	This class offers functionality for applying
 *	a user-defined set of openCV-image-processing
 *	methods to a given input image.
 */

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "..//QTTestVs10//ParameterWrapper.h"

#include "Toolbox.h"
#include "Contour.h"
#include "PreprocessingMethods.h"

namespace PreprocessingFuncs
{
	/**
	 *	This class offers functionality for applying
	 *	a user-defined set of openCV-image-processing
	 *	methods to a given input image.
	 */
	class Preprocessing
	{
	public:
		Preprocessing	(void);
		~Preprocessing	(void);

		/**
        * Userdefined preprocessor
		*
		* @param	_methodNames List of names of openCV methods to be applied.
		* @param	_methodParams Parameters for aforementioned methods.
        */
		void setPreprocessingMethods(
			const vector< PreprocessingMethods::EMethods >	&	_methodNames, 
			const vector< vector< ParameterWrapper >* >		&	_methodParams);

		/**
        * Computes an image to which the methods specified through
		* setPreprocessingMethods(..) have been applied.		
		*
		* @param	_source Image which shall be processed.
		* @return	_source with applied methods.
        */
		cv::Mat getBinaryFBImage(
			const cv::Mat									&	_source);

		/**
        * Extract contours from a binary image.
		*
		* @param	mod_source Binary input image.
		* @param	_minSize Minimal contour size to be returned.
		* @return	List of found contours.
        */
		vector< vector<Point> > getContours(
					cv::Mat									&	mod_source, 
			const	unsigned int								_minSize =  50);

		/**
        * Computes an image to which the specified methods
		* have been applied.	
		*
		* @param	_source Image which shall be processed.
		* @param	_methods List of enums referring to openCV methods to be applied.
		* @param	_methodParams Parameters for aforementioned methods.
		* @return	_source with applied methods.
        */
		cv::Mat getProcessedImage(
			const cv::Mat									&	_source, 
			const vector< PreprocessingMethods::EMethods >	&	_methods, 
			const vector<vector< ParameterWrapper >* >		&	_methodParams);

		/**
        * Parses and applies a method specified through parameters.	
		*
		* @param	mod_image Image on which the method shall be applied. 
		* @param	_method Refers to specific openCV method.
		* @param	_methodParams Parameters for aforementioned method.
        */
		void applyMethod(
			cv::Mat											&	mod_image, 
			const PreprocessingMethods::EMethods 			&	_method, 
			const vector< ParameterWrapper >				*	_methodParams);

	private:			
		/**
        * Methods chosen by user
		*/
		vector< PreprocessingMethods::EMethods >				mAssignedMethods; 
		/**
        *  Parameters for methods specified in mAssignedMethods.
		*/
		vector< vector< ParameterWrapper >* >					mAssignedMethodParams;
	};
}