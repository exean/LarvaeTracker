/**
 *  @file CondensationTracker.h
 *
 *  @section DESCRIPTION
 *	Base class of this plugin. Provides a condensation based
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

#include "CondensationCore.h"

using namespace cv;

#define DllExport   __declspec( dllexport )
/**
 *	Base class of this plugin. Provides a condensation based
 *	tracking-approach.
 */
class CondensationTracker : public Dll::ATracker
{
public:
	CondensationTracker	(void);
	~CondensationTracker(void){};	

#pragma region ATracker
	
	/**
	* Starts the algorithm; updates and results will be send using signals.
	* 
	* @param	_imgLoader	Object for loading images.
	*/
	void				perform		(
		PreprocessingFuncs::ImageLoader _imgLoader);

	/**
	* Stops the algorithm
	*/
	void				stop		();	

	/**
	* Provides a map of default parameters.
	* Does not need to be implemented if no parameters are required.
	* 
	* @return	A map of default parameters
	*/
	vector< ParameterWrapper >getSettings	();	

	/**
	* Set parameters for this algorithm. The maps keys are
	* defined in corresponding "getSettings" method.
	* Does not need to be implemented if no parameters are required.
	* 
	* @param	_settings	A map of parameters
	*/
	void				setSettings	(
		vector< ParameterWrapper > _settings);

#pragma endregion

private:
	/**
	*	Unite models over time, correct head-assignment
	*
	*	@param	_allLarvae	List of models to be processed
	**/
	void postprocess(
		vector< LarvaModel* >	* _allLarvae);

	/**
	*	Does the last thing left to do: Send final information
	*
	*	@param	_duration	Amount of time the tracking process took
	*	@param	_models		Results of tracking.
	**/
	void finish(
		double					_duration,
		vector<LarvaModel*> *	_models);

	/**
	*	Amount of samples to be created for each larva at each timestep
	*/
	int		mSampleCount;
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


//Class will be used as plugin, this is the creation-method to be used.
extern "C"
{
	DllExport Dll::ATracker*	CreateTracker()
	{
		return new CondensationTracker();
	}	
}