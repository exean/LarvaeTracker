/**
 *  @file ABackgroundSubtractor.h
 *
 *	@section DESCRIPTION
 *  Specifies a background subtraction base class.
 */
#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

#include "APlugin.h"

namespace Dll
{
	/**
	 *  Specifies a background subtraction base class.
	 */
	class ABackgroundSubtractor : public APlugin
	{
	public:
		ABackgroundSubtractor(void) : mSampleRate(0.1f), mReady(false) {};
		~ABackgroundSubtractor(void){};
		
		/**
        * Sets the percentual amount of images to be used
		* for the computation of this background-subtraction
		* method (the complete dataset is provided whenever
		* create is called).
        *
        * @param	_sampleRate specifies the percentage of
		*			input images to be used.
        */
		void setSampleRate(
			const float	 _sampleRate)
			{this->mSampleRate = _sampleRate;this->mReady=false;};

		/**
        * Returns the percentual amount of images to be used
		* for the computation of this background-subtraction
		* method (the complete dataset is provided whenever
		* create is called).
        *
        * @return	Percentage of input images to be used.
        */
		float getSampleRate()
			{return this->mSampleRate;};

		/**
        * Called one time to setup the algorithm.
        *
		* @param	_files Complete list of all input-files
		*			the tracking algorithm works on.
		* @param	_roi Region of interest used by the
		*			tracking algorithm.
        * @return	Percentage of input images to be used.
        */
		virtual void create(
			const std::vector<std::string>	& _files,
			const cv::Rect					& _roi)
			= 0;

		/**
        * Applies this background subtraction method to
		* an image.
        *
		* @param	out_img Image to which the background
		*			subtraction shall be applied.
		* @return	true if background subtraction could
		*			be applied successful.
        */
		virtual bool apply (
			cv::Mat & out_img)
			= 0 ;

		/**
        * Applies this background subtraction method to
		* an image.
        *
		* @return	true if apply(..) may be called;
		*			false implies that create(..) has
		*			not yet finished.
        */
		virtual bool isReady()
			{return this->mReady;};

	protected:
		/**
		* Determines percentual amount of images 
		* used for background mask creation.
		*/
		float	mSampleRate;
		/**
		* Specifies wether algorithm is ready to run.
		*/
		bool	mReady;

	};
}