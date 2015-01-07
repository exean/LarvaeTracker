#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "Toolbox.h"

namespace BackgroundSubtraction
{
	class ABackgroundSubtractor
	{
	public:
		ABackgroundSubtractor(void) : mSampleRate(0.05f) {};
		~ABackgroundSubtractor(void){};
		
		void setSampleRate(
			const float	 sampleRate)
			{this->mSampleRate = sampleRate;};

		virtual void create(
			const std::vector<std::string>	& files,
			const cv::Rect					& roi)
			= 0;

		virtual void apply(
			cv::Mat & img)
			= 0;

	protected:
		float mSampleRate;//Determines amount of images used for background mask creation
	};
}