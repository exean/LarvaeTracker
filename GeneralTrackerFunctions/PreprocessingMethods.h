/**
 *  @file PreprocessingMethods.h
 *
 *	@section DESCRIPTION
 *	Declaration of openCV image processing methods that
 *	may be used to process input images.
 */

#pragma once

#include "..//QTTestVs10//ParameterWrapper.h"

namespace PreprocessingFuncs
{
	/**
	 *	Declaration of openCV image processing methods that
	 *	may be used to process input images.
	 */
	class PreprocessingMethods
	{
	public:
		/**
        * Refers to available preprocessing methods.
        */
		enum EMethods 
		{
			MedianBlur, GaussianBlur, Threshold, Dilate, Erode, CvtColor
		};

		PreprocessingMethods(void);
		~PreprocessingMethods(void);
		
		/**
        * Provides all available method-descriptions.
		*
		* @return	Map containing parametersets for all availavle methods.
        */
		static std::map< EMethods, std::vector< ParameterWrapper > > *	GetAvailablePreprocessingMethods();	

		/**
        * Provides a user-friendly name for any available method.
		*
		* @param	_m Preprocessing method specifier
		* @return	user-friendly name for requested method
        */
		static std::string												EMethodsToString(const EMethods _m);

		/**
        * Returns the correct specifier for a method-name.
		*
		* @param	_m User-friendly name of method
		* @return	Preprocessing method specifier
        */
		static EMethods													StringToEMethods(const std::string _m);

	private:		
		/**
        * Lists of parameters required by available 
		* preprocessing functions.
        */
		static std::map< EMethods, std::vector< ParameterWrapper > > * mAvailablePreprocessingMethods;	

		/**
        * Human-friendly method names (for gui).
        */
		static std::string mMethodNames[];
	};
}