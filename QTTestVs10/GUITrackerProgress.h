/**
 *  @file GUITrackerProgress.h
 *
 *  @section DESCRIPTION
 *	GUI Component for displaying the progress of an active tracking task.
 */
#pragma once
#include <qmovie.h>
#include "ui_QTAlgorithmRunning.h"
#include "GUISettings.h"
#include "Mail.h"
#include "Painter.h"

namespace Gui
{
	/**
	 *	GUI Component for displaying the progress of an active tracking task.
	 */
	class GUITrackerProgress
		:	public QWidget
	{
		Q_OBJECT

	public:
		GUITrackerProgress	(void);
		~GUITrackerProgress	(void);

		/**
		* Returns the complete progress of the trackings task
		* represented by this object.
		*
		* @return	Complete progress (0-100)
		*/
		int getCompleteProgress			();

	private:
		Ui::FormProcessing mUi;
		/**
		* Complete progress (0-100)
		*/
		int mCompleteProgress;
		/**
		* Working-animation
		*/
		QMovie *mMovie;

	signals:
		void	stop					();

	public slots:
		void	setCurrentProgress		(const int&);
		void	setCompleteProgress		(const int& p);	
		void	receiveTextData			(const QString&);
		void	setSendMail				(bool);
		void	setFinished				(vector<LarvaModel*>*);

	private slots:
		void	stopTracker				();
		void	updateMailSendableStatus();
	};
}

