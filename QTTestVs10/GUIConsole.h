/**
 *  @file GUIConsole.h
 *
 * @section DESCRIPTION
 * Displays trackers text-output.
 */

#pragma once

#include <QtGui/QDialog>
#include <QTextBlock>
#include "ui_QTConsole.h"
#include <qscrollbar.h>
#include <QDateTime>
namespace Gui
{
	/**
	 * Displays trackers text-output.
	 */
	class GUIConsole
		: public QDialog
	{
		Q_OBJECT

	public:
		GUIConsole(QWidget *parent = 0);
		~GUIConsole(void);

		/**
		* Adds a line of text in specified color to the output.
		*
		* @param _s		Line to be added.
		* @param _color	Color of the text to be added.
		*/
		void addMessage(QString _s, const QColor & _color);

	private:	
		/**
		* QT Hide event
		*/
		void hideEvent(QHideEvent *);

		/**
		* Userinterface as created through QTCreator.
		*/
		Ui::Form	mUi;	

		/**
		* Scrollbar used to assure newly added lines are visible.
		*/
		QScrollBar* mScroll;

		/**
		* Maximal amount of lines to be displayed at one time.
		*/
		int			mMaxLineCount;

	signals:	
		/**
		* Dialog becomes visible or invisible.
		*/
		void visibilityChanged	(bool);
	};
}