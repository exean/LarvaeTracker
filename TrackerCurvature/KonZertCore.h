/**
 *  @file KonZertCore.h
 *
 *	@section DESCRIPTION
 *	Main computations of KonZert tracker take
 *	place in this class.
 */

#pragma once

#include <map>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//ATracker.h"
#include "..//QTTestVs10//LarvaModel.h"

#include "ImageLoader.h"
#include "Toolbox.h"
#include "Painter.h"
#include "Contour.h"
#include "LarvaModelController.h"
#include "LarvaModelManager.h"

#include "ModelFinder.h"
#include "SuccessorMatching.h"

namespace CurvatureTrackerFuncs
{
	/**
	 *	Main computations of KonZert tracker take
	 *	place in this class.
	 */
	class KonZertCore
	{
	public:
		/**
		* Constructor
		*
		* @param	_tracker				This plugins main object, used for sending information to gui.
		* @param	_segmentCount			Amount of segments the models spines shall have.
		* @param	_curvature_distance		IPAN: Distance of neighboring points along spine to check.
		* @param	_curvature_spanThresh	IPAN: Maximal Distance between neighborpoints.
		*/
		KonZertCore(
			Dll::ATracker							*	_tracker,
			int											_segmentCount,
			double										_curvature_distance, 
			double										_curvature_spanThresh);
		~KonZertCore(void);

		void processImage(
			cv::Mat									&	_image,
			cv::Mat									&	_FBimage,
			vector< LarvaModel* >					*	_models,
			const int									_timeIndex);

	private:	
		/**
		* High-Information-Points of a larva
		*/
		struct LarvaPoints { LarvaModel* larva; Point head; Point tail; };

		/**
		* Checks wether a split is acceptable and prepares the
		* models accordingly. Deletes inferior model, if split
		* is not acceptable.
		*
		* @param	mod_currentSuccessor	First possible successor.
		* @param	mod_additionalSuccessor	Second possible successor.
		* @param	mod_allLarvaeBaseList	List containing all base models.
		* @return	true, if split is acceptable.
		*/
		bool prepareSplit(						
			LarvaModel								*	mod_currentSuccessor,
			LarvaModel								*	mod_additionalSuccessor,
			vector<LarvaModel*>						*	mod_allLarvaeBaseList);

		/**
		* Checks wether contours are found that don't seem to belong to any 
		* known larva.
		*
		* @param	_currentAssignmentTargets	List of models that are assigned to contours
		* @param	_currentTargetContours		List of contours that are assigned to larvae
		* @param	_image						Input image
		* @return	_timeIndex					Current time
		* @return	_models						List of all larvae
		*/
		void checkForNewContours(
			vector< LarvaModel* >					&	_currentAssignmentTargets,
			vector< GeneralTrackerFuncs::Contour* >	&	_currentTargetContours,
			const cv::Mat							&	_image,
			const int									_timeIndex,
			vector< LarvaModel* >					*	_models);

		/**
		* Corrects a model
		*
		* @param	mod_model		Model to be modified
		* @param	_FBimage		Foreground-Background-Image
		* @param	_maxDifference	Maximal allowed difference to predecessor
		*/
		void correctSpineWidths(
			LarvaModel								*	mod_model,
			const cv::Mat							&	_FBimage,
			const double								_maxDifference);

		/**
		* Checks for conflicts (multiple models assigned to one contour, vice versa)
		* and assigns contours to models accordingly.
		*
		* @param	_currentTargetContours		Available contours
		* @param	_currentAssignmentTargets	Current models.
		* @param	_image						Input image
		* @param	mod_allLarvaeBaseList		List containing the first occurence of all larvae
		*/
		void checkForConflictsAndAssignModels(
			vector< GeneralTrackerFuncs::Contour* >		&	_currentTargetContours,
			vector< LarvaModel* >						&	_currentAssignmentTargets,
			const Mat									&	_image,
			vector< LarvaModel* >						*	mod_allLarvaeBaseList);

		/**
		* Resolves conflicts between specified models assigned to the same contour
		* and assignes their successors to partial contours accordingly
		*
		* @param	_contour				Contour to which multiple larvae are assigneds
		* @param	_image					Current input image.
		* @param	mod_currentConflicts	Conflicting larva-models
		* @param	mod_allLarvaeBaseList	List containing the first occurence of all larvae
		*/
		void resolveConflictsAndAssignModels(
			vector< Point >								*	_contour,
			const Mat									&	_image,
			vector< LarvaModel* >						&	mod_currentConflicts,			
			vector< LarvaModel* >						*	mod_allLarvaeBaseList);

		void resolveBackwards(
			vector< LarvaModel * >							_splitModels,
			const cv::Mat								&	_image,	
			vector< LarvaModel* >						*	mod_allLarvaeBaseList);

		/**
		* Partitions a contour between a set of larvae.
		*
		* @param	_contour				Contour to be cut into pieces like the girl from hostel
		* @param	_currentConflicts		List of larvae that want a piece of _contour.
		* @return	Set of contours ordered to match _currentConflicts
		*/
		vector< vector< Point > > getSeperatedContours(
			const vector< Point >						*	_contour,
			const vector< LarvaModel* >					&	_currentConflicts);

		/**
		* Finds gap within a contour and creates a new contour
		* without those gaps.
		*
		* @param	_contour	Contour with gaps.
		* @param	_density	Allowed distance between two points of contour
		* @return	Contour without gaps.
		*/
		vector< Point > getClosedContour(
			const	vector< Point >						&	_contour,
					LarvaModel							*	_prev,
			const	int										_density = 2);

		/**
		* Closes a single gap within a contour which begins at the end of
		* supplied point-vector and ends at specified end-Point.
		*		
		* @param	_gapEnd			Index of end-point of gap
		* @param	_density		Distance between each point that fills the gap.
		* @param	_contourWGaps	Complete Contour with gaps
		* @param	mod_contour		Partial contour up to the beginning of the gap.
		*/
		void getClosedSingleContourGap(			
			const	unsigned int						&	_gapEndIndex,
					LarvaModel							*	_prev,//Only required for outcommented solutions
			const	int										_density,
			const	vector< Point >						&	_contourWGaps,
					vector< Point >						&	mod_contour);
		
		Dll::ATracker									*	mTracker;
		int													mSegmentCount;
		double												mCurvature_distance;
		double												mCurvature_spanThresh;
		vector< LarvaModel* >								mLastTimeStepLarvae;
		vector< LarvaModel* >								mCurrentTimeStepLarvae;
		/**
		*	First time index of thingy which may or my not be one or more larvae
		*/
		vector< LarvaModel* >								mUnresolvedContours;
		/**
		*	Model data finder
		*/
		GeneralTrackerFuncs::SpineFinder				*	mSpineFinder;
		GeneralTrackerFuncs::LarvaModelManager			*	mModelManager;
		/**
		*	Model computer
		*/
		GeneralTrackerFuncs::ModelFinder				*	mModelFinder;
		/**
		*	Controller for modifying Larva-Models
		*/
		GeneralTrackerFuncs::LarvaModelController			mLarvaController;	
		/**
		*	Image preprocessor
		*/
		PreprocessingFuncs::Preprocessing				*	mPreprocessing;		
		cv::Mat												mObserverImage;
		/**
		*	Matches contours and models by distance
		*/
		SuccessorMatching									mMatching;			
	};
}