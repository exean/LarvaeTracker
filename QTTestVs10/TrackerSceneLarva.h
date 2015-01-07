/**
 *  @file TrackerSceneLarva.h
 *
 * @section DESCRIPTION
 * Observable vizualization of a LarvaModel
 * as a set of QGraphicsItems which may be 
 * manipulated by user.
 */

#pragma once
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsPolygonItem>
#include <QColorDialog>
#include "TrackerSceneLarvaCircle.h"
#include "LarvaModel.h"
#include "Observable.h"
namespace Visualization
{
	/**
	 * Observable vizualization of a LarvaModel
	 * as a set of QGraphicsItems which may be 
	 * manipulated by user.
	 */
	class TrackerSceneLarva
		: public Observable, public AObserver
	{
	public:
		/**
		 * Constructor
		 *
		 * @param _parent	Scene in which this larva visualization should be display in.
		 * @param _larva	Model which will be viszualized by this object.
		 * @param _name		Name of the larve
		 */
		TrackerSceneLarva					(QGraphicsScene * _parent, LarvaModel* _larva, const QString & _name);
		~TrackerSceneLarva					(void);

		/**
		 * Changes model to be displayed
		 *
		 * @param _larva	New model
		 */
		void setLarva						(LarvaModel* _larva);

		/**
		 * Changes the color
		 *
		 * @param _color	New color of this representations components.
		 */
		void setColor						(const QColor &_color);	

		/**
		 * Change visibility
		 *
		 * @param _visible	true->visible
		 */
		void setVisible						(bool _visible);	

		/**
		 * Change visibility of path travelled.
		 *
		 * @param _visible	true->visible
		 */
		void setPathVisible					(bool _visible);

		/**
		 * Change visibility of line to origin.
		 *
		 * @param _visible	true->visible
		 */
		void setDistance2OriginVisible		(bool _visible);

		/**
		 * Applies values obtained from QGraphicsItems
		 * to LarvaModel. Those values could have been
		 * manipulated by user.
		 *
		 * @param mod_target LarvaModel to which the values will be assigned.
		 */
		void applyCurrentValues				(LarvaModel* mod_target);

		/**
		 * Notification that an observed object, i.e.
		 * a TrackerSceneLarvaCircle, was manipulated.
		 *
		 * @param _source Changed object.
		 */
		void observedObjectChanged			(Observable * _source);

		/**
		 * Radius of a specified TrackerSceneLarvaCircle
		 *
		 * @param	_index	Index of the circle
		 * @return	Radius of specified circle
		 */
		qreal	getRadius						(int _index);

		/**
		 * Set radius of a specified TrackerSceneLarvaCircle
		 *
		 * @param	_radius	New radius of the circle
		 * @param	_index	Index of the circle
		 */
		void	setRadius						(qreal _radius, int _index);

		/**
		 * Name of the LarvaModel represented by this object.
		 */
		QString								mName;

	private:
		/**
		 * Updates the silhouette used to visualize the LarvaModel.
		 */
		void updateSilhouette();

		/**
		 * Scene in which this LarvaModel is displayed.
		 */
		QGraphicsScene*						mParent;	

		/**
		 * Set of circles representing spine points and sizes
		 * of LarvaModel. May be edited by user.
		 */
		vector<TrackerSceneLarvaCircle*>	mCircles;

		/**
		 * Visualization of LarvaModel spine
		 */
		QGraphicsPathItem*					mSpine;

		/**
		 * Path the LarvaModel has travelled
		 */
		QGraphicsPathItem*					mPath;

		/**
		 * Line to origin
		 */
		QGraphicsPathItem*					mDistance2Origin;

		/**
		 * Polygon visualizing the larvas silhouette.
		 */
		QGraphicsPolygonItem*				mSilhouette;	
	};
}