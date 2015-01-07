/**
 *  @file CondensationCore.h
 *
 *	@section DESCRIPTION
 *	Main computations of condensation tracker take
 *	place in this class.
 */

#pragma once
#include <map>
#include <vector>
#include <string>

#include "..//QTTestVs10//ATracker.h"

#include "LarvaModelController.h"
#include "SpineFinder.h"
#include "Toolbox.h"
#include "Contour.h"
#include "Painter.h"
#include "Preprocessing.h"
#include "ModelFinder.h"
#include "LarvaModelManager.h"

#include "Condensation.h"
/**
 *	Main computations of condensation tracker take
 *	place in this class.
 */
class CondensationCore
{
public:
	/**
	* Constructor
	*
	* @param	_tracker				This plugins main object, used for sending information to gui.
	* @param	_sampleCount			Amount of samples used for condensation.	
	* @param	_segmentCount			Amount of segments the models spines shall have.
	* @param	_curvature_distance		IPAN: Distance of neighboring points along spine to check.
	* @param	_curvature_spanThresh	IPAN: Maximal Distance between neighborpoints.
	*/
	CondensationCore(
		Dll::ATracker*	_tracker,		
		int				_sampleCount,
		int				_segmentCount,
		double			_curvature_distance,
		double			_curvature_spanThresh);
	~CondensationCore(void);

public:
	/**
	* Find larvae on image.
	*
	* @param _image		Input image
	* @param _FBimage	Foreground/Background-image
	* @param _preprocessing	Object for preprocessing loaded images
	* @param out_models	Found LarvaModels
	* @param _timeIndex	Current time index
	*/
	void processImage(		
		cv::Mat									&	_image, 
		cv::Mat									&	_FBimage,
		PreprocessingFuncs::Preprocessing		&	_preprocessing,
		vector< LarvaModel* >					*	out_models,
		const int									_timeIndex);

private:
	//Settings
	int												mSampleCount;
	int												mSegmentCount;
	double											mCurvature_distance;
	double											mCurvature_spanThresh;
	double											mMinimalFitness;
	double											mMinContourArea;
	GeneralTrackerFuncs::SpineFinder			*	mSpineFinder;//Model data finder
	GeneralTrackerFuncs::LarvaModelManager		*	mModelManager;
	GeneralTrackerFuncs::ModelFinder			*	mModelFinder;//Model computer

	GeneralTrackerFuncs::LarvaModelController		mModelController;
	map<LarvaModel*, vector<LarvaModel> >			mCurrentDescriptors;
	map<LarvaModel*, double>						mLargest_cumulative_prob;
	map<LarvaModel*, vector<double> >				mCumul_prob_array;	
	Condensation									mCondensation;
	Dll::ATracker*									mTracker;

	vector< LarvaModel* >							mLastTimeStepLarvae;
	vector< LarvaModel* >							mCurrentTimeStepLarvae;
	vector< LarvaModel* >							mUnresolvedContours;//First time index of thingy which may or my not be one or more larvae
};

