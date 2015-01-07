/**
 *  @file TrackerSceneLandmark.h
 *
 * @section DESCRIPTION
 * QGraphicsItemGroup that the user may
 * set, move and remove from scene in order
 * to mark special points to which the 
 * distance of each larva can be computed
 * during CSV-export.
 */

#pragma once
#include <qstring.h>
#include <QGraphicsScene>
#include <QColor>
#include "TrackerSceneLarvaCircle.h"

namespace Visualization
{
	/**
	 * QGraphicsItemGroup that the user may
	 * set, move and remove from scene in order
	 * to mark special points to which the 
	 * distance of each larva can be computed
	 * during CSV-export.
	 */
	class TrackerSceneLandmark
		: public QGraphicsItemGroup
	{
	public:
		/**
		* Constructor
		*
		* @param _parent	Scene in which this Landmark is displayed.
		* @param _name		Name of the Landmark represented by this object.
		* @param _x			x-Position of landmark in scene
		* @param _y			y-Position of landmark in scene
		*/
		TrackerSceneLandmark	(QGraphicsScene *	_parent, 
								const QString	&	_name, 
								unsigned int		_x, 
								unsigned int		_y);

		~TrackerSceneLandmark	(void);
	
		/**
		* Checks wether a point is inside any of the
		* QGraphicsItems of this group. Used for
		* user-interaction.
		*
		* @param _point	Position of landmark in scene
		* @return	true, if _point is inside the
		*			representation of this landmark.
		*/
		bool contains			(const QPointF&		_point);

	protected:	
		void mousePressEvent	(QGraphicsSceneMouseEvent *event);
		void mouseMoveEvent		(QGraphicsSceneMouseEvent *event);
		void mouseReleaseEvent	(QGraphicsSceneMouseEvent *event);
		QPointF lastMousePos;

	private:
		/**
		* Initializes the Landmark. Instantiates required
		* QGraphicsItems.
		*
		* @param _x	x-Position of landmark in scene
		* @param _y	y-Position of landmark in scene
		*/
		void initialize			(unsigned int		_x, 
								unsigned int		_y);

		/**
		* Scene in which this Landmark is displayed.
		*/
		QGraphicsScene*			mParent;	
			
		/**
		 * Name of the Landmark represented by this object.
		 * Also used for tooltip.
		 */
		QString					mName;

		/**
		* Editable movable circle that allows user interaction.
		*/
		QGraphicsEllipseItem*	mCircle;

		/**
		* Additional visualization features to make
		* landmarks more discernable from LarvaModel-circles.
		*/
		QGraphicsPathItem*		mPath;
	};
}