/**
 *  @file Contour.h
 *
 *	@section DESCRIPTION
 *	Convinience class for storing contours with corresponding centerOfMasss
 */

#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

namespace GeneralTrackerFuncs
{
	/**
	 *	Convinience class for storing contours with corresponding centerOfMasss
	 */
	class Contour
	{
	public:
		Contour(vector<Point> _data)
		{
			this->mData			= _data;
			this->mCenterOfMass	= this->getCenterOfMass(this->mData);
		};
		~Contour(void){};
	
		/**
        * Get the contour-data
        *
		* @return	Point-list forming the contour
        */
		vector<Point>	*getContour				()			{return &this->mData;};
		/**
        * Get the contour-centerOfMass.
        *
		* @return	Contour-centerOfMass of this contour
        */
		Vector2D		*getCenterOfMass				()			{return &this->mCenterOfMass;};
		/**
        * Get list of points of high curvature
        *
		* @return	List of points with high curvature values
        */
		vector<Point>	*getHighCurvaturePoints	()			{return &this->mHighCurvature;};
		/**
        * Add a point of high curvature belonging to this contour
        *
		* @param	_p Point of high curvature
        */
		void			 addHighCurvaturePoint	(Point _p)	{this->mHighCurvature.push_back(_p);};
	
	private:
		/**
        * Computes the center of mass of the contour.
        *
		* @param	_contour Contour of which the centerOfMass
		*			shall be computed.
		* @return	Contour's center of mass
        */
		Vector2D getCenterOfMass(const vector<Point> & _contour)
		{
			Moments mom = moments(cv::Mat(_contour));
			return Vector2D(
				(mom.m10/mom.m00),
				(mom.m01/mom.m00));
		}
	
		/**
		* List of points forming the contour
		*/
		vector<Point>	mData;

		/**
		* Contour-CenterOfMass
		*/
		Vector2D		mCenterOfMass;

		/**
		* List of contour-points of high
		* curvature values
		*/
		vector<Point>	mHighCurvature;
	};
}