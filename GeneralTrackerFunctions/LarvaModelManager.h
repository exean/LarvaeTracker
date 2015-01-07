/**
 *  @file LarvaModelManager.h
 *
 *	@section DESCRIPTION
 *	Manages sets of LarvaModels:
 *	-	Add new Models
 *	-	Find conflicts
 */
#pragma once
#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//LarvaModel.h"
#include "SpineFinder.h"
#include "LarvaModelController.h"
namespace GeneralTrackerFuncs
{
	/**
	 *	Manages sets of LarvaModels:
	 *	-	Add new Models
	 *	-	Find conflicts
	 */
	class LarvaModelManager
	{
	public:
		/**
        * Constructor
        *
        * @param	_ModelFinder Object for finding model-data
        */
		LarvaModelManager(ModelFinder * _ModelFinder);
		~LarvaModelManager(void);

		/**
        * Computes a new LarvaModel. The Model
		* is computed from the provided contour.
        *
        * @param	_currentTargetContour Contour on which
		*			the new LarvaModel shall be computed.
		* @param	_image Input image, used for e.g. intensities
		*			of head- and tail-region
		* @param	_timeIndex Timeindex of the new larva
		* @return	New LarvaModel based on given contour.
        */
		LarvaModel* addNewLarva(	
			Contour						*	_currentTargetContour,
			const Mat					&	_image,
			const int						_timeIndex);

		/**
        * Determines a list of LarvaModels which are
		* assigned to the same contour.
        *
        * @param	_index Index of the contour for which conflicts
		*			shall be found.
		* @param	mod_currentTargetContours Complete list of assigned
		*			target contours corresponding to mod_lastDetectedLarvaModels.
		* @param	mod_lastDetectedLarvaModels List of LarvaModels from last
		*			timestep.
		* @param	out_collidingLarvaModels List of LarvaModels assigned to
		*			the same contour.
        */
		void findConflicts(
			const int						_index,
			vector< Contour* >			&	mod_currentTargetContours,
			vector< LarvaModel* >		&	mod_lastDetectedLarvaModels,
			vector< LarvaModel* >		&	out_collidingLarvaModels);

		/**
        * Computes a successor-model to a given larva. Differs
		* to addNewLarva(..) mainly in cases of "splits" (more
		* than one model assigned to a predeccessor).
        *
        * @param	_larvaPredecessor Parent to the new larva.
        * @param	_targetContour Contour on which
		*			the new LarvaModel shall be computed.
		* @param	_image Input image, used for e.g. intensities
		*			of head- and tail-region
		* @param	mod_AllLarvaeBaseList List containing the base-
		*			models of any found LarvaModel-Timechain. Will be
		*			extended in case current model is part of a split.
		* @param	_useUnresolved Add an unresolved model if detection failed.
		* @return	New LarvaModel based on given contour.
		*/
		LarvaModel* addSuccessiveLarva(
			LarvaModel					*	_larvaPredecessor,
			Contour						*	_targetContour,
			const cv::Mat					_image,
			vector< LarvaModel* >		*	mod_AllLarvaeBaseList,
			const bool						_useUnresolved = true);
		
		/**
        * Sets the given larvas successor while making sure that
		* head and tail do not switch around (alignment from mod_larva
		* is used).
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_next Successor to mod_larva
        */
		void setNext					(LarvaModel *mod_larva, LarvaModel		*	_next		);

		/**
        * Inverts the spine of given larva (head becomes tail and vice versa)
        *
        * @param	mod_larva Model to be modified by this method
        */
		static void InvertSpine		(LarvaModel *mod_larva);

		/**
        * Searches for larvae in timesteps successive to a certain larvas
		* final detection and unites them.
        *
        * @param	mod_larva		Model for which temporal successors shall be found
		* @param	mod_allLarvae	Possible partners
		* @param	_timeGap		Maximal allowed temporal gap
		* @return	true, if model was united with other.
        */
		static bool UniteOverTime		(
			LarvaModel				* mod_larva, 
			vector< LarvaModel* >	* mod_allLarvae,
			unsigned int			  _timeGap = 0);

		/**
        * Corrects the assignment of head and tail by checking the
		* larvas movement over all frames.
        *
        * @param	mod_larva	Model to be modified by this method
		* @param	_stepWidth	Temporal distance to compare-model
        */
		static void CorrectHeadAssignmentByDirection(	
			LarvaModel			*	mod_larva,
			const unsigned int	_stepWidth = 2);

	private:
		/**
        * Object for computing model-information from a contour.
		*/
		ModelFinder *	mModelFinder;		
	};
};

