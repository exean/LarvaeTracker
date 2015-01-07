/**
 *  @file ModelFinder.h
 *
 *	@section DESCRIPTION
 *	Computes LarvaModels based on given contours.
 */

#pragma once

#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//LarvaModel.h"
#include "SpineFinder.h"

namespace GeneralTrackerFuncs
{
	/**
	 *	Computes LarvaModels based on given contours.
	 */
	class ModelFinder
	{
	public:
		/**
        * Constructor
		*
		* @param	_spineFinder Object for fitting a spine into a contour.
		* @param	_segmentCount Amount of spine segments required.
        */
		ModelFinder(
			GeneralTrackerFuncs::SpineFinder *	_spineFinder, 
			const int							_segmentCount);
		~ModelFinder(void);

		/**
        * Computes a LarvaModel based on given contour.
		*
		* @param	_contour Contour on which LarvaModel shall be computed.
		* @param	_image Input image, used for intensities of head and tail.
		* @param	_predecessor Parent of newly computed model.
		* @param	mod_larva If not null, this model will be modified to
		*			contain the newly computed data.
		* @return	Computed LarvaModel
        */
		LarvaModel* getModelFromContour(
			Contour						*	_contour, 
			const Mat					&	_image, 
		//	const Mat					&	_boolImage, 		
			LarvaModel					*	_predecessor	= 0,
			LarvaModel					*	mod_larva		= 0);	
		

	private:
		/**
        * Finds the index of the highest value in provided list.
        *
        * @param	_high_intensities List of intensity-values.
		* @return	Index of the highest value.
        */
		int getHighestIntensityIndex(
			const vector<double>		&	_high_intensities);
		/**
        * Finds the index of the smallest value in provided list.
        *
        * @param	_high_intensities List of intensity-values.
		* @return	Index of the lowest value.
        */
		int getLowestIntensityIndex(
			const vector<double>		&	_high_intensities);

		
		SpineFinder						*	mSpineFinder;
		unsigned const int				mSegmentCount;
	};
}
