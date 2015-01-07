/**
 *  @file TrackerSceneLarvaCircle.h
 *
 * @section DESCRIPTION
 * Observable circle used to visualize and
 * edit LarvaModels.
 */

#pragma once
#include <QtGui/QDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include "Observable.h"

namespace Visualization
{
	/**
	 * Observable circle used to visualize and
	 * edit LarvaModels.
	 */
	class TrackerSceneLarvaCircle
		: public QGraphicsEllipseItem, public Observable
	{
	public:
		/**
		 * Constructor
		 *
		 * @param _name			Tooltip-Text
		 * @param _resizable	true, if circle may be resized.
		 */
		TrackerSceneLarvaCircle	(
			const QString &	_name, 
			bool			_resizable = true);
		~TrackerSceneLarvaCircle(void);


	protected:
		 void mousePressEvent	(QGraphicsSceneMouseEvent *event);
		 void mouseMoveEvent	(QGraphicsSceneMouseEvent *event);
		 void mouseReleaseEvent	(QGraphicsSceneMouseEvent *event);

	private:	
		/**
		 * true, if circle may be resized.
		 */
		bool mResizable;
	};
}