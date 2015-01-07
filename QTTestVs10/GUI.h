/**
 *  @file GUI.h
 *
 * @section DESCRIPTION
 * Main class of gui, provides functionality for using
 * the set of available trackers as well as advanced
 * features (console, notepad,...).
 *
 * SINGLETON
 */

#ifndef GUI_H
#define GUI_H
#include <iostream>
#include <vector>
#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QString>

#include "ui_qttestvs10.h"
#include "ui_QTAlgorithmRunning.h"
#include "ui_QTHelp.h"

#include "GUIConsole.h"
#include "ATracker.h"
#include "ATrackerObserver.h"
#include "PluginLoader.h"
#include "TrackerThread.h"
#include "ImportFromCSV.h"
#include "GUITracker.h"
#include "GUITabWidget.h"
#include "GUINotes.h"
#include "GUIAbout.h"
#include "GUISettings.h"
#include "GUIResultComparer.h"

#include "ecwin7.h"


namespace Gui
{
	/**
	 * Main class of gui, provides functionality for using
	 * the set of available trackers as well as advanced
	 * features (console, notepad,...).
	 */
	class GUI : public QMainWindow
	{
		Q_OBJECT

	public:
		/**
		* @return The one and only instance of GUI
		*/
		static GUI* Instance();
		~GUI();
	
	private:	
		GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
		/**
		* Singleton
		*/
		static GUI* mInstance;

		/**
		* Is called when qt window closes.
		*/
		void					closeEvent(QCloseEvent*);

		/**
		* Initialize all fields.
		*/
		void					initialize();

		/**
		* EcWin7 hook on windows messages
		*/
		bool					GUI::winEvent(MSG *message, long *result); 

		/**
		* Userinterface as created through QTCreator.
		*/
		Ui::QTTestVs10Class		mUi;

		/**
		*List of all trackers
		*/
		vector< GUITracker* >	mTracker;

		/**
		* Displays status information of all trackers
		*/
		GUIConsole	*			mConsole;	

		/**
		* Allows access to general options
		*/
		GUISettings	*			mSettings;	

		/**
		* Notepad
		*/
		GUINotes	*			mNotes;		

		/**
		* Contains Trackers and such
		*/
		QTabWidget	*			mTabs;		

		/**
		* About Dialog
		*/
		GUIAbout	*			mAbout;		

		/**
		* Help Dialog
		*/
		QDialog		*			mHelp;		

		/**
		* Win7 Progress bar features
		*/
		EcWin7					mWin7;		

		/**
		* Colors used for different trackers for better distinction in console
		*/
		vector< QColor >		mColors;	

	public slots:
		/**
		* Exit program
		*/
		void exit				();			
		
		/**
		* Add new result-comparer tab
		*/
		void addResultComparer	();

		/**
		* Add new tracker tab
		*/
		void addTracker			();			
		
		/**
		* Add new tracker tab with specific settings
		*/
		void addTrackerWithPreset();		
		
		/**
		* Display about message
		*/
		void showAbout			();		
		
		/**
		* Receive status updates
		*
		* @param _src		Tracker that sent the data.
		* @param _data		String data sent from tracker.
		*/
		void receiveTextData	(GUITracker* _src, const QString& _data);

		/**
		* Toggle display notepad-tab
		*
		* @param _visible	true, if notes tab shall be displayed. 
		*/
		void showNotesTab		(bool _visible);		
		
		/**
		* Display Help message
		*/
		void showHelp			();			
		
		/**
		* Import tracker results
		*/
		void startImport		();			
		
		/**
		* Tab changed, e.g. to other tracker; update of taskbar-progress is required
		*
		* @param _newIndex Index of active tab.
		*/
		void activeTabChanged	(const int _newIndex);	
	};
}
#endif // QTTESTVS10_H
