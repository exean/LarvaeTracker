/**
* @file TrackerScene.h
*
* @section DESCRIPTION
* GraphicsScene for displaying input data
* and visualizing as well as editing tracking
* results.
*/
#pragma once
#include <QGraphicsScene>
#include <QPixmap>
#include <QRect>
#include <QGraphicsRectItem>
#include "LarvaModel.h"
#include "TrackerSceneLarva.h"
namespace Visualization
{
	/**
	* GraphicsScene for displaying input data
	* and visualizing as well as editing tracking
	* results.
	*/
	class TrackerScene :
		public QGraphicsScene
	{
	public:
		TrackerScene	(void);
		~TrackerScene	(void);

		/**
		* Add a LarvaModel to the scene
		*
		* @param _larva	LarvaModel to be added.
		* @param _name	Name of the LarvaModel.
		* @return	Visualization-component of the model.
		*/
		TrackerSceneLarva* addLarva	(LarvaModel* _larva, const QString & _name);
			
		/**
		* Remove a LarvaModel represantation from
		* the scene.
		*
		* @param _larva	Larva-representation to be removed.
		*/
		void deleteLarva			(TrackerSceneLarva* _larva);
			
		/**
		* Sets the roi
		*
		* @param _x x-Coordinate of top left point
		* @param _y y-Coordinate of top left point
		* @param _w width
		* @param _h height
		*/
		void setRect				(int _x, int _y, int _w, int _h);
			
		/**
		* Sets the image to be displayed
		*
		* @param _img
		*/
		void setImage				(const QImage _img);
			
		/**
		* Returns the currently displayed image.
		*
		* @return Currently displayed image
		*/
		QImage getImage				();

	private:
		/**
		* Update GraphicsItems belonging to roi
		* and repaint.
		*/
		void updateRoi				();

		/**
		* Set wether roi is displayed
		*
		* @param _visible true->display roi
		*/
		void setROIvisible			(bool _visible);

		/**
		* Region of interest that will be vizualized
		* in this scene.
		*/
		QRect						mRectRoi;

		/**
		* List of LarvaModel-representations
		*/
		vector<TrackerSceneLarva*>	mLarvae;
		
		/**
		* List of QGraphicsItem that visualize
		* the region of interest.
		*/
		vector<QGraphicsRectItem*>	mRoi;

		/**
		* Image to be displayed in  this scene.
		*/
		QGraphicsPixmapItem*		mImage;	

	};
}

