/**
 *  @file Painter.h
 *
 *	@section DESCRIPTION
 *	Static graphics functions using open cv. Mainly useful for
 *	debugging as the GUI uses QT for visualization.
 *	Also useful for creating masks of larvae for samling
 *	from specific regions of input data.
 */

#pragma once
#include <opencv2/opencv.hpp>
#include "..//QTTestVs10//LarvaModel.h"

namespace GeneralTrackerFuncs
{
	/**
	 *	Static graphics functions using open cv. Mainly useful for
	 *	debugging as the GUI uses QT for visualization.
	 *	Also useful for creating masks of larvae for samling
	 *	from specific regions of input data.
	 */
	class Painter
	{
	public:
		/**
        * Paints a polyline with specified width and color.
		*
		* @param	_polyline Polyline to be painted.
		* @param	_image Image on which the line shall be painted.
		* @param	_color Color to be used. Should match channelcount
		*			of _image.
		* @param	_lineWidth Thickness of the polyline on the image.
        */
		static void PaintPolyline(
			const cv::vector<cv::Point>*	_polyline, 
			cv::Mat						&	_image, 
			const cv::Scalar			&	_color		= cv::Scalar(255), 
			const int						_lineWidth	= 1);
		static void PaintPolyline(
			const cv::vector<Vector2D>	*	_polyline, 
			cv::Mat						&	_image, 
			const cv::Scalar			&	_color		= cv::Scalar(255), 
			const int						_lineWidth	= 1);

		/**
        * Paints the path of a LarvaModel as polyline of the moment
		* of each timestep.
		*
		* @param	_rootmodel Model of timeindex at which the path
		*			shall begin.
		* @param	mod_image Image on which the line shall be painted.
		* @param	_color Color to be used. Should match channelcount
		*			of _image.
        */
		static void PaintModelPath(
			const LarvaModel			&	_rootmodel, 
			cv::Mat						&	mod_image, 
			const cv::Scalar			&	_color	= cv::Scalar(255));

		/**
        * Same as cv::putText, but with reduced parameter-count.
		*
		* @param	_text Text to be painted.
		* @param	mod_image Image on which the line shall be painted.
		* @param	_point Position of text on image.
		* @param	_color Color to be used. Should match channelcount
		*			of _image.
        */
		static void PaintText(
			const char					*	_text, 
			cv::Mat						&	mod_image, 
			const cv::Point				&	_point, 
			const cv::Scalar			&	_color = Scalar(255));
		
		/**
        * Paints a LarvaModel as set of circles representing
		* spinepoints with widths. This can be used as well for
		* debugging purposes as for generating a mask that allows
		* sampling image data.
		*
		* @param	_larva Model to be painted.
		* @param	mod_image Image on which the line shall be painted.
		* @param	_color Color to be used. Should match channelcount
		*			of _image.
		* @param	_markHead Head will be marked.
		* @param	_fill Larva will be filled solid (useful as mask).
		* @param	_drawSpine Spine will be drawn inside larva.
        */
		static void PaintLarva(
			LarvaModel					&	_larva, 
			cv::Mat						&	mod_image, 
			const cv::Scalar			&	_color, 
			const bool						_markHead	= true, 
			const bool						_fill		= false, 
			const bool						_drawSpine	= true);

		/**
        * Paints a LarvaModel. Useful for generating a mask that allows
		* sampling specific image data.
		*
		* @param	_larva Model to be painted.
		* @param	mod_image Image on which the line shall be painted.
		* @param	_color Color to be used. Should match channelcount
		*			of _image.
		* @param	_sizeFactor Allows scaling of width of LarvaModel for painting.
		* @param	_translate Allows translation of LarvaModel for painting
        */
		static void PaintLarvaSilhouette(
			LarvaModel					&	_larva, 
			cv::Mat						&	mod_image, 
			const cv::Scalar			&	_color, 
			const float					_sizeFactor	= 1.0f,
			const Vector2D					_translate = Vector2D(0,0));

		/**
        * Creates an image representing tracking results.
		*
		* @param	_larvae A set of tracked larvae
		* @return	Image of the tracked larvae
        */
		static cv::Mat PaintDataset(
			vector<LarvaModel*>					&	_larvae);

	private:
		Painter(void){};
		~Painter(void){};
	};
}