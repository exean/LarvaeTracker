/**
 *  @file GUIResultsLarva.h
 *
 *  @section DESCRIPTION
 *	GUI Component for a tracked larva.
 */
#pragma once
#include <QtGui/QDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QTimer>
#include "ui_QTLarvaRes.h"
#include "LarvaModel.h"
#include "TrackerSceneLarva.h"
#include "AObserver.h"
#include "Contour.h"
#include "LarvaModelManager.h"
namespace Gui
{
	/**
	 *	GUI Component for a tracked larva.
	 */
	class GUIResultsLarva 
		: public QWidget, public AObserver
	{
		Q_OBJECT

	public:
		/**
		* Constructor
		*
		* @param	_model			Model which this component should handle.
		* @param	_sceneLarva		Visualization of _model.
		*/
		GUIResultsLarva		(
			LarvaModel							* _model, 
			Visualization::TrackerSceneLarva	* _sceneLarva);
		~GUIResultsLarva	(void);		
	
		/**
		* Checks wether the timeindex for this model may be changed.
		* Might nit be allowed in case of unsaved changes to this model.
		*
		* @return	True, if timechange is allowed.
		*/
		bool									canChangeTime();

		//Observer
		void									observedObjectChanged(Observable * _source);		

		/**
		* Getter for base model of this components larva
		*
		* @return	Base model
		*/
		LarvaModel							*	getBaseLarva(){return this->mBaseLarva;};
		/**
		* Getter for visualization of this components larva
		*
		* @return	Visualization object
		*/
		Visualization::TrackerSceneLarva	*	getSceneLarva(){return this->mSceneLarva;};

		/**
		* Update min/max-Time
		*/
		void									updateTimes();

	private:
		/**
		* Changed this larvas color in the scene.
		*
		* @param	_color	Color the visualization of this model shall have.
		*/
		void									setColor(QColor _color);

		Ui::LarvaResForm						mUi;
		LarvaModel*								mBaseLarva;
		LarvaModel*								mCurrentLarva;
		QColor									mColor;

		Visualization::TrackerSceneLarva	*	mSceneLarva;
		unsigned int							mCurrentTime;
		

		/**
		* Timer allows flashing of this Larva.
		*/
		QTimer								* 	mFlashTimer;

		/**
		* Number of times the scene larva will flash.
		*/
		unsigned int							mFlashCount;

		/**
		* True, if larva was modified by user.
		*/
		bool									mModified;

	signals:
		void changed				(QWidget		*	_me,	bool _changed);
		void deleteResultsLarva		(GUIResultsLarva*	_me);
		void addResultsLarva		(LarvaModel		*	_larva);
		void requestUnion			(GUIResultsLarva*	_me);
		void requestCenterOn		(qreal				_x,		qreal _y);

	public slots:
		void setTimeIndex			(int		_time);

	private slots:
		void centerSceneOnLarva		();
		void update					(int		_time);	
		void updateColor			();	
		void setTimeToPrevCollsion	();
		void setTimeToNextCollsion	();

		/**
		*	Radius modification via left-hand-gui
		**/
		void updateDisplayedRadius	(int		_index);
		void applyRadiusToSceneLarva(double	_radius);

		/**
		*	Unite model with different model from results (add before/after all timesteps of this model)
		**/
		void uniteWithOtherModel	();

		/**
		*	Cut off successive timesteps and add them as a new model to the results
		**/
		void divideAfterCurrentTimestep();

		/**
		*	Delete Larva from results
		**/
		void deleteLarva			();
		
		/**
		*	Delete Model of current TimeStep from results
		**/
		void deleteCurrentModel		();

		/**
		*	Reset larva after manual changes in scene
		**/
		void reset					();

		/**
		*	Apply manual changes in scene
		**/
		void apply					();
		
		/**
		*	Apply manual changes in scene
		**/
		void invertLarva			();

		/**
		*	copy current larva-data to previous/next timestep
		**/
		void copyToNextTimestep		();
		void copyToPrevTimestep		();

		/**
		*	Visualization visibilities
		**/
		void displaySceneLarva					(bool _visible);
		void displaySceneLarvaPath				(bool _visible);
		void displaySceneLarvaDistance2Origin	(bool _visible);

		/**
		*	flashing of visualization for easier recognition.
		**/
		void flash								();
		void flashEvent							();
	};
}