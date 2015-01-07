/**
 *  @file GUIResults.h
 *
 *  @section DESCRIPTION
 *	GUI Component for dealing with tracking results.
 */

#pragma once
#include <QWidget>
#include <QInputDialog>
#include "ui_QTResult.h"
#include "LarvaModel.h"
#include "GUIResultsLarva.h"
#include "TrackerGraphicsView.h"

namespace Gui
{
	/**
	 *	GUI Component for dealing with tracking results.
	 */
	class GUIResults : public QWidget
	{
		Q_OBJECT

	public:
		/**
		* Constructor
		*
		* @param	_results	Result-Dataset
		* @param	_scene		Scene that displays the results
		* @param	_maxtime	Maximal time index of any larva
		*/
		GUIResults	(
			vector< LarvaModel* >				*	_results, 
			Visualization::TrackerGraphicsView	*	_scene,
			int										_maxtime);
		~GUIResults	(void);	

	private:	
		/**
		* Add a gui component for a distinct larva
		*
		* @param	_larva	Larva for which a component will be created.
		*/
		void addLarvaGui	(LarvaModel * _larva);
		/**
		* Marks a larva-editor-tab, signalling that its larva was modified.
		*
		* @param	_tabIndex	Index of the tab to mark.
		*/
		void markChanged	(int _tabIndex);
		/**
		* Unmarks a larva-editor-tab. Changes have either been applied or reverted.
		*
		* @param	_tabIndex	Index of the tab to unmark.
		*/
		void markUnChanged	(int _tabIndex);

		Ui::ResultForm							mUi;	

		//Pointers to scene and result larvae for easy removal of unwanted larvae
		vector< LarvaModel* >				*	mLarvae;
		Visualization::TrackerGraphicsView	*	mScene;

		int										mTabCounter;

	private slots:
		void setChanged		(QWidget * _widget, bool _changed);
		void timeChanged	(int _time);
		void removeLarva	(GUIResultsLarva * _widget);
		void addLarva		(LarvaModel *);
		void uniteLarvae	(GUIResultsLarva * _widget);
		void centerSceneOn	(qreal, qreal);

	signals:
		void timeChangedSignal(int);
	};
}