/**
 *  @file SpineFinder.h
 *
 *	@section DESCRIPTION
 *	Lays a spine into a contour.
 *  Uses an ipan-based algorithm to determine potential
 *  head- and tail-regions.
 */
#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <limits>
#include "..//QTTestVs10//LarvaModel.h"
#include "Toolbox.h"
#include "Painter.h"
using namespace cv;

namespace GeneralTrackerFuncs
{
	/**
	 *	Lays a spine into a contour.
	 *  Uses an ipan-based algorithm to determine potential
	 *  head- and tail-regions.
	 */
	class SpineFinder
	{
	public:
		/**
		* Constructor
		*
		* @param	_headSize		Length of head-/tail-region
		* @param	_spanThresh		Maximal Distance between neighborpoints.
		* @param	_allowedGap		Allowed "non-high-curvature-points" 
		*							within a high-curvature-segment.
		* @param	_minCurvePoints	Minimal amount of points a curve must have.
		*/
		SpineFinder(			
			const double						_headSize		= 10,//small 10 (20) 
			const double						_spanThresh		= 15,//small 15 (25)
			const int							_allowedGap		= 1,
			const int							_minCurvePoints = 5);//small 5 (10))
		~SpineFinder(void);

		/**
		* Determines endpoints of spine with corresponding points along contour.
		*
		* @param	_contour						Contour to be examined.
		* @param	_image							Input image required to detemine
		*											intensity values for head/tail.
		* @param	out_potentialEndpoints			List of potential endpoints on contour.
		* @param	out_correspondingBorderPoints0	Corresponding points of contour on 
		*											one side.
		* @param	out_correspondingBorderPoints1	Corresponding points of contour on 
		*											other side.
		* @param	out_endPointIntensityValues		Intensity values of found endpoints.
		* @param	out_endPointCorrespondingSpinePoints	Potential head/tail-points.
		* @param	out_correspondingSpineWidths	Widths of potential head/tail regions.
		*/
		void findEndPoints(
			vector<Point>					*	_contour, 
			const Mat						&	_image, 
		//	const Mat						&	_boolImage,
			vector< Point >					*	out_potentialEndpoints, 
			vector<Point>					*	out_correspondingBorderPoints0, 
			vector<Point>					*	out_correspondingBorderPoints1, 
			vector<double>					*	out_endPointIntensityValues,
			vector<Vector2D>				*	out_endPointCorrespondingSpinePoints,
			vector<float>					*	out_correspondingSpineWidths);

		/**
		* Determines regions of high curvatute of a given polygon (spine).
		*
		* @param	_contour	Contour to be examined.
		* @return	Regions of high curvature
		*/
		vector< vector<Point> > getHighCurvatureRegions(
			const vector<Point>				*	_contour);

		Point getPointAlongContour(
			const vector<Point>::const_iterator		&	_start,
			const vector<Point>						*	_contour,
			const double								_distance,
			const char									_direction = 1);

		Point getPointOfDistance(
			const vector<Point>::const_iterator		&	_start,
			const vector<Point>						*	_contour,
			const double								_distance,
			const char									_direction = 1,
			const bool									_exact = false);

		// Fills "potentialEndpoints" with points of highest curvature	
		//void findEndPoints_highestCurvature(
		//	vector<Point>					*	_contour,
		//	vector<vector<Point>::iterator> *	_potentialEndpoints, 
		//	vector<double>					*	_endPointIntensityValues, 
		//	const Mat						&	_image, 
		//	double								_distance,
		//	double								_spanThresh);

		/**
		* Determines spine with requested amounts of segments by
		* first generating a high-information spine and reducing
		* it subsequently.
		*
		* @param	_contour				Contour to be examined.
		* @param	_head					Head of larva (first spine point).
		* @param	_head_border0			Point on contour that corresponds to _head.
		* @param	_head_border1			Point on contour that corresponds to _head (other side).
		* @param	_head_width				Width of head-region.
		* @param	_tail					Tail of larva (last spine point).
		* @param	_tail_border0			Point on contour that corresponds to _tail.
		* @param	_tail_border1			Point on contour that corresponds to _tail (other side).
		* @param	_tail_width				Width of tail-region.
		* @param	_maxWidth				Maximal width the spine is allowed to have.
		* @param	out_spine				Computed spine.
		* @param	out_segWidths			Widths corresponding to spine.
		* @param	_requestedSegmentCount	Amount of segments the spine should have.
		*/
		void findSpine(
			vector<Point>					*	_contour, 
			const Vector2D					&	_head,
			const Point						&	_head_border0,
			const Point						&	_head_border1,
			const double					&	_head_width,
			const Vector2D					&	_tail,
			const Point						&	_tail_border0,
			const Point						&	_tail_border1,
			const double					&	_tail_width,
			const double					&	_maxWidth,
			vector<Vector2D>				*	out_spine, 
			vector<double>					*	out_segWidths,
			const int							_requestedSegmentCount = 10);
	
		/**
		* Determines head-/tail-point as center of circle fitted into head/tail 
		* region. This results in head/tail having widths > 0.
		*
		* @param	_contour			Contour to be examined.
		* @param	_curvaturePoints	Point with high curvature values.
		* @param	_endpoint			Endpoint of larva (in curvature region).
		* @param	out_radius			Determined radius of head/tail region.
		* @param	out_spinePoint		Center of head/tail region.
		* @param	out_border0			Point on contour corresponding to head/tail.
		* @param	out_border1			Point on contour corresponding to head/tail (other side).
		*/
		void layInCircle(
			const	vector<Point>			*	_contour,
			vector<Point>						_curvaturePoints,
			const Point						&	_endpoint,
			float							&	out_radius,
			Vector2D						&	out_spinePoint,
			Point							&	out_border0,
			Point							&	out_border1);

	protected:	/*cv::Mat tmp;*/
		/**
		* Reduce the amount of segments of a spine.
		*
		* @param	_spine					Spine to be reduced.
		* @param	_segWidths				Corresponding input widths.
		* @param	_segmentCount			Required amount of segments.
		* @param	out_reducedSpine		Result spine.
		* @param	out_reducedSegWidths	Result spine widths.
		*/
		void reduceSpine(
			vector<Vector2D>				*	_spine,
			vector<double>					*	_segWidths,
			int									_segmentCount, 
			vector<Vector2D>				*	out_reducedSpine, 
			vector<double>					*	out_reducedSegWidths);
	
		/**
		* Determines center and intensity of a high curvature region.
		*
		* @param	_currentCurveStartPoint	First point of current curve.
		* @param	_lastDetectedCurvePoint	Last point of current curve.
		* @param	_smoothedIntensityImage	Image for intensity-sampling.
		* @param	_contour				Contour of the larva.
		* @param	out_endpoint			Determined center.
		* @param	out_intensity			Corresponding intensity.
		*/
		void findHighCurvatureRegionCenter(
			vector<Point>::iterator				_currentCurveStartPoint,
			vector<Point>::iterator				_lastDetectedCurvePoint,
			const Mat						&	_smoothedIntensityImage,
			const vector<Point>				*	_contour,
			vector<Point>::iterator			&	out_endpoint,
			double							&	out_intensity);

	private:
		/**
		* Distance of neighboring points along spine to check.
		*/
				double						mDistance;//small 10 (20) 
		
		/**							
		* Maximal Distance between neighborpoints.
		*/
		const	double						mSpanThresh;//small 15 (25)

		/**
		* Allowed "non-high-curvature-points" within a high-curvature-segment.
		*/
		const unsigned int				mAllowedGap;//1

		/**							
		* Minimal amount of points a curve must have.
		*/
		const unsigned int				mMinCurvePoints;//small 5 (10)

		/**
		* Maximal opening angle of curvature triangle.
		*/
				double						mOpeningAngle;
	};
}