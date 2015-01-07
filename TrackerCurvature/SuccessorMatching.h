/**
 *  @file SuccessorMatching.h
 *
 *	@section DESCRIPTION
 *	Matches LarvaModels and Contours via distance.
 */
#pragma once
#include "LarvaModelManager.h"
#include "Contour.h"
#include "Toolbox.h"
#include "..//QTTestVs10//LarvaModel.h"
#include "Painter.h"
namespace CurvatureTrackerFuncs
{
	/**
	*	Matches LarvaModels and Contours via distance.
	*/
	class SuccessorMatching
	{
	public:
		SuccessorMatching(void);
		~SuccessorMatching(void);
	
		/**
        * Matches known LarvaModels and newly found contours via distance.
        *
        * @param	_modelsBegin Iterator: First model to be checked.
        * @param	_modelsEnd End-Iterator of models to be checked.
        * @param	_contours List of all contours to be checked
        * @param	mod_lastDetectedLarvaModels LarvaModels which have 
		*				been detected during previous timestep and 
		*				belonging to Newly detected matches  will 
		*				be pushed to this list.
        * @param	mod_currentTargetContours Contours belonging to 
		*				newly detected matches will be pushed to 
		*				this list.
        */
		void assignContoursToModels(
			const	vector< LarvaModel * >::const_iterator	& _modelsBegin,
			const	vector< LarvaModel * >::const_iterator	& _modelsEnd,
			const	vector< GeneralTrackerFuncs::Contour* >	& _contours,
					vector< LarvaModel* >					& mod_lastDetectedLarvaModels,
					vector< GeneralTrackerFuncs::Contour* >	& mod_currentTargetContours);

		/**
        * Matches known larva models and newly found contours via distance.
        *
        * @param	_modelsBegin Iterator: First model to be checked.
        * @param	_modelsEnd End-Iterator of models to be checked.
        * @param	_contours List of all contours to be checked
        * @param	mod_lastDetectedLarvaModels LarvaModels which have 
		*				been detected during previous timestep and 
		*				belonging to Newly detected matches  will 
		*				be pushed to this list.
        * @param	mod_currentTargetContours Contours belonging to 
		*				newly detected matches will be pushed to 
		*				this list.
        */
		void assignModelsToContours(
			const	vector< LarvaModel * >::const_iterator	& _modelsBegin,
			const	vector< LarvaModel * >::const_iterator	& _modelsEnd,
			const	vector< GeneralTrackerFuncs::Contour* >	& _contours,
					vector< LarvaModel* >					& mod_lastDetectedLarvaModels,
					vector< GeneralTrackerFuncs::Contour* >	& mod_currentTargetContours);

		/**
		* Estimates progression of a larva from progression of its predecessors.
		*
		* @param	_model LarvaModel whose progression shall be estimated.
		* @return	Esitimated progression (dx, dy)
		*/
		Vector2D estimateProgression(
			const LarvaModel				* _model);
		
	private:

		/**
		* Sets double median times mContourDistanceLimitFactor as new mMaxContourDistance
		*
		* @param	mod_distances List of distances between models in adjacent timesteps;
		*			will be sorted by this method.
		* @param	mod_spineLengths List of lengths of detected spines;
		*			will be sorted by this method.
		*/
		void updateMaxContourDistance(
			vector<double>					& mod_distances,
			vector<double>					& mod_spineLengths);//will be sorted by this method!

		/**
        * Maximal distance to contours may be apart in adjacent timesteps
		* in order to still be part of the same larva.
		*/
		double	mMaxContourDistance;

		/**
        * This factor will be applied to measured contour-distances to 
		* update mMaxContourDistance.
		*/
		double	mContourDistanceLimitFactor;

		/**
		* Amount of predecessors used to estimate progression
		*/
		int		mSpeedSamplingCount;
	};
}
