/**
 *  @file TrackerGraphicsView.h
 *
 * @section DESCRIPTION
 * View for displaying a TrackerScene.
 * Extends QGraphicsView in order to provide
 * a target sensitive contect menu as well as
 * offer scaling functionality.
 */

#pragma once
#include <QGraphicsView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QFileDialog>
#include <QTimeLine>
#include "GUINotes.h"
#include "TrackerScene.h"
#include "TrackerSceneLandmark.h"

namespace Visualization
{
	/**
	 * View for displaying a TrackerScene.
	 * Extends QGraphicsView in order to provide
	 * a target sensitive contect menu as well as
	 * offer scaling functionality.
	 */
	class TrackerGraphicsView
		:	public QGraphicsView
	{
		Q_OBJECT

	public:
		TrackerGraphicsView(void);
		~TrackerGraphicsView(void);

		/**
		* @return Scene displayed in this view.
		*/
		TrackerScene* getScene();

		/**
		* @return	List of landmarks created in this 
		*			scene (normally by user).
		*/
		vector< Point > getLandmarkPoints();

		/**
		* Sets the scale (zooming factor) for the scene.
		*
		* @param _scale	New scale this scene shall be
		*				scaled to.
		*/
		void			setScale(const qreal _scale);

	private:	
		/**
		* Initialized context menu.
		*/
		void	setupContextMenu ();

		/**
		* Event for generating a context menu.
		*/
		void	contextMenuEvent ( QContextMenuEvent * event );

		/**
		* Event for scrolling/scaling.
		*/
		void	wheelEvent ( QWheelEvent * event );

		QAction		*	mActionSaveImageForNotes;
		QAction		*	mActionSaveImage;	
		QAction		*	mActionSetLandmark;
		QAction		*	mActionRemoveLandmark;

		/**
		* Scene displayed by this view.
		*/
		TrackerScene*	mScene;

		/**
		* Contextmenu of this view.
		*/
		QMenu			mContextMenu;

		/**
		* Last known position at which a
		* contextmenu was displayed.
		*/
		QPointF			mLastContextMenuPosition;

		/**
		* List of landmarks within the scene.
		*/
		vector< TrackerSceneLandmark* > mLandmarks;

	private slots:
		/**
		* An image of the current scene will be saved and
		* inserted into the notes.
		*/
		void saveImageForNotes	();

		/**
		* Saves an image of the current scene.
		*/
		void saveImage			();

		/**
		* Adds a new landmark to the scene.
		*/
		void setLandmark		();

		/**
		* Removec a landmark from the scene.
		*/
		void removeLandmark		();

	signals:
		/**
		* Emitted whenever the scenes scale is changed,
		* used e.g. in GUI.
		*/
		void scaleChanged(qreal, qreal);
	};
}