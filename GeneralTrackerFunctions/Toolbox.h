/**
 *  @file Toolbox.h
 *
 *	@section DESCRIPTION
 *	General methods for frequent tasks.
 */

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include "..//QTTestVs10//Vector2D.h"
#include "Contour.h"
using namespace cv;

namespace GeneralTrackerFuncs
{
	/**
	 *	General methods for frequent tasks.
	 */
	class Toolbox
	{
	public:	
		/**
        * Computes the distance between two points.
		*
		* @param	_p1 First Point
		* @param	_p2 Second Point
		* @return	Distance between _p1 and _p2
        */
		static double	GetDistance			(
			const cv::Point								&	_p1, 
			const cv::Point								&	_p2);

		/**
        * Computes the distance between a point and a line.
		*
		* @param	_p Point
		* @param	_lineStart Start of line
		* @param	_lineEnd End of line
		* @return	Distance
        */
		static double	GetDistance(
			const Vector2D								&	_p, 
			const Vector2D								&	_lineStart,
			const Vector2D								&	_lineEnd);

		/**
        * Computes the minimal distance between a point
		* and a polyline.
		*
		* @param	_p Point
		* @param	_polyline Polyline
		* @return	Minimal distance between _p and _polyline
        */
		static double	GetMinimalDistance	(
			const Vector2D								&	_p, 
			const vector< Point >						&	_polyline);
		static double	GetMinimalDistance	(
			const Vector2D								&	_p, 
			const vector< Vector2D >					&	_polyline);

		/**
        * Computes the wrapped distance of two iterators on a point-list.
		*
		* @param	_i0 First iterator
		* @param	_i1 Second iterator
		* @param	_contourSize Size of the point-list
		* @return	Wrapped distance between _i0 and _i1
        */
		static int		GetIteratorDistance	(
			const std::vector<Point>::const_iterator	&	_i0, 
			const std::vector<Point>::const_iterator	&	_i1,
			const int									& 	_contourSize);

		/**
        * Computes the moment of a contour.
		*
		* @param	_contour Contour whose moment will be computed.
		* @return	Center of mass of the provided contour.
        */
		static Vector2D	GetMoment(
			const vector<Point>							&	_contour);

		/**
        * Computes mean intensity along a line on an image.
		*
		* @param	_image Image on which will be sampled.
		* @param	_p1 First point of sampling-Line.
		* @param	_p2 Final point of sampling-Line.
		* @return	Mean value of sampling along specified line.
        */
		static double	GetMeanIntensity(
			const cv::Mat								&	_image, 
			const cv::Point								&	_p1, 
			const cv::Point								&	_p2);

		/**
        * Computes length of a polyline.
		*
		* @param	_polyline 
		* @return	Length of _polyline.
        */
		static double	GetLength(
			const vector< Vector2D >						*	_polyline);	

		/**
        * Computes which contour is nearest to a given point.
		*
		* @param	_p Point to which the contours distances will be measured.
		* @param	_contours Set of contours.
		* @return	Minimal distanced contour to p.
        */
		static Contour *GetNearestContour(
			const Vector2D								&	_p, 
			const vector<Contour*>						&	_contours);
		
		/**
		* Retrieves the point on a contour with minimal distance to a given point.
		*
		* @param	_contour	Contour to be examined.
		* @param	_point		Distance to this point will be examined.
		* @return	Iterator: Nearest point on contour.
		*/
		static vector<Point>::const_iterator GetNearestPointOnContour(
			const vector<Point>				*	_contour,
			const Vector2D					&	_point);

#pragma region inline-decl
		/**
        * Returns the value of the mat at given position or 0 if outside bounds.
		* Inline b/c template.
		*
		* @param	_boolImage Mat on which the value shall be retrieved.
		* @param	_pos Position to be retrieved.
		* @return	Value at _pos or 0 if outside bounds.
        */
		template <typename T>
		static T GetValueAt(
			const	Mat									&	_boolImage,
			const 	Point								&	_pos)
		{	
			if(_pos.x < 0 || _pos.y < 0 || _boolImage.cols <= _pos.x || _boolImage.rows <= _pos.y)
			{	//Not inside boolImage
				return 0;
			}
			else
			{
				return _boolImage.at<T>(_pos);
			}
		}

		/**
        * @return	Maximal value of double.
        */
		static double	GetMaxDouble	()						{	return std::numeric_limits<double>::max();	};
		/**
		* @param	_number Value to be rounded
        * @return	Rounded value.
        */
		static double	Round			(double _number)		{	return floor(_number+0.5); };
		/**
		* Oonverts Rad to Grad
		* @param	_rad Value in radians
        * @return	Gradient value
        */ 
		static double	Rad2Grad		(double _rad)			{	return _rad*(180/(4. * atan(1.)));};
#pragma endregion

	private:
		Toolbox(void);
		~Toolbox(void);	
	};
}