/**
 *  @file CurvatureTracker.h
 *
 *  @section DESCRIPTION
 *	Base class of this plugin. Provides a KonZert based
 *	tracking-approach.
 */

#pragma once

#include <windows.h>
#include <map>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//ATracker.h"
#include "..//QTTestVs10//LarvaModel.h"
#include "..//QTTestVs10//ParameterWrapper.h"

#include "Preprocessing.h"
#include "Toolbox.h"
#include "Painter.h"
#include "Contour.h"
#include "LarvaModelController.h"

#include "ModelFinder.h"
#include "KonZertCore.h"

using namespace cv;

#define DllExport   __declspec( dllexport )

namespace CurvatureTrackerFuncs
{
	/**
	 *	Base class of this plugin. Provides a KonZert based
	 *	tracking-approach.
	 */
	class CurvatureTracker : public Dll::ATracker	
	{
	public:
		CurvatureTracker(void);
		~CurvatureTracker(void);	

	#pragma region ATracker
		void				perform(
			PreprocessingFuncs::ImageLoader _imgLoader);
		void				stop		();	
		vector< ParameterWrapper > getSettings();	
		void				setSettings(
			vector< ParameterWrapper > _settings);
	#pragma region

	private:	
			/**
		*	Does the last thing left to do: Send final information
		*
		*	@param	_duration	Amount of time the tracking process took
		*	@param	_models		Results of tracking.
		**/
		void finish(
			double duration,
			vector<LarvaModel*> * models);

		/**
		*	Amount of segments of each model
		*/
		int		mSegmentCount;
		/**
		*	Curvature size parameter: length of larva head
		*/
		double	mCurvature_HeadLength;
		/**
		*	Curvature strength parameter: width of larva
		*/
		double	mCurvature_Width;

		/**
		*	Stop-processing-Flag
		*/
		bool	mStop;
	};
}
extern "C"
{
	DllExport Dll::ATracker*	CreateTracker()
	{
		return new CurvatureTrackerFuncs::CurvatureTracker();
	}	
}