/**
 *  @file GUIResultComparer.h
 *
 *  @section DESCRIPTION
 *	GUI Component for ResultComparerThread.
 */

#pragma once
#include <iostream>
#include <vector>
#include <QWidget>
#include <QLayout>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <qmovie.h>

#include "LarvaModel.h"
#include "ResultComparerThread.h"

#include "ui_QTResultComparer.h"
namespace Gui
{
	/**
	 *	GUI Component for ResultComparerThread.
	 */
	class GUIResultComparer : public QWidget
	{
			Q_OBJECT
	public:
		GUIResultComparer(void);
		~GUIResultComparer(void);

	private:	
		/**
		*	Checks wether all results have been computed
		*	and updates the GUI accordingly.
		*/
		void computationFinished();

		ResultComparerThread *	mThread;
		Ui::FormResultComparer	mUi;		
		QMovie *				mMovie;
		
	private slots:
		void selectTruthFile	();
		void selectCompareFile	();
		void startComparison	();

		void receiveBadFileNotification(
			const QString);

		void receiveCenterComparisonResuls(
			const double _min, 
			const double _max, 
			const double _mid, 
			const double _med);
		void receiveSpinepointComparisonResuls(
			const double _min, 
			const double _max, 
			const double _mid, 
			const double _med);
		void receiveWidthsComparisonResuls(
			const double _minAbs, const double _minGen, const double _minSmaller, const double _minBigger, 
			const double _maxAbs, const double _maxGen, const double _maxSmaller, const double _maxBigger, 
			const double _midAbs, const double _midGen, const double _midSmaller, const double _midBigger,  
			const double _medAbs, const double _medGen, const double _medSmaller, const double _medBigger);
	};
}

