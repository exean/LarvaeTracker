/**
 *  @file GUITabWidget.h
 *
 *	@section DESCRIPTION
 *	Extends QTabWidget in order to display a welcome-message
 *	when no tab is opened.
 */
#pragma once

#include <QTabWidget>
#include <QLabel>
namespace Gui
{
	/**
	 *	Extends QTabWidget in order to display a welcome-message
	 *	when no tab is opened.
	 */
	class GUITabWidget: public QTabWidget
	{
	public:
		GUITabWidget(QWidget* parent = 0);
		~GUITabWidget(void);
		virtual void tabInserted( int aIndex );
		virtual void tabRemoved( int aIndex );
		virtual void resizeEvent(QResizeEvent* aEvent );
	protected:
		void updateLabelPos();
	private:
		QLabel* labelNoTabs;
	};
}