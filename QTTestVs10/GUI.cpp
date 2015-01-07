#include "GUI.h"

namespace Gui
{
	#pragma region Singleton
	// Global static pointer used to ensure a single instance of the class.
	GUI* GUI::mInstance = NULL; 

	GUI* GUI::Instance()
	{
		if (!mInstance)   // Only allow one instance of class to be generated.
			mInstance = new GUI;
		return mInstance;
	}
	#pragma endregion

	GUI::GUI(QWidget *parent, Qt::WFlags flags)
		: QMainWindow(parent, flags)
	{
		// QT initialization
		this->mUi.setupUi(this);
		
		// EcWin7 initialization (mWin7 taskbar features)
		this->mWin7.init(this->winId());
		
		// Own components initialization
		this->initialize();
	}

	GUI::~GUI()
	{
		//if(thread)	
		//{
		//	this->thread->terminate();
		//	delete this->thread;
		//}
		//for(int i = this->receivedTrackerData.size()-1; i >= 0; i--)
		//{
		//	delete this->receivedTrackerData[i];
		//}
		delete this->mNotes;
		delete this->mTabs;
		delete this->mConsole;	
		delete this->mSettings;	
		if(this->mAbout) delete this->mAbout;
		if(this->mHelp) delete this->mHelp;
		//qDeleteAll(this->mUi.listFiles->selectedItems());
		/*for(map<QString, ATracker*>::iterator it = this->availableAlgos.end(); it != this->availableAlgos.begin(); )
		{
			delete (*--it).second;
		}*/
	}

	void GUI::closeEvent(QCloseEvent* evt)
	{
		evt->ignore();
		this->exit();
	}

	#pragma region primary GUI methods
	void GUI::initialize()
	{
		//Console (Output)
		this->mConsole = new GUIConsole(this);

		//Notes
		this->mNotes = GUINotes::Instance();

		//Notes
		this->mSettings = GUISettings::Instance();

		//Tab-Widget
		this->mTabs = new GUITabWidget();
		this->mUi.centralWidget->layout()->addWidget(this->mTabs);
		connect(this->mTabs,					SIGNAL(currentChanged(int)),	this,					SLOT(activeTabChanged(int)));

		this->mAbout	= 0;
		this->mHelp		= 0;

		//Menubar
		connect(this->mUi.actionNotes,		SIGNAL(triggered(bool)),		this,						SLOT(showNotesTab(bool)));
		connect(this->mUi.actionImportCSV,	SIGNAL(triggered()),			this,						SLOT(startImport()));
		connect(this->mUi.actionExit,		SIGNAL(triggered()),			this,						SLOT(exit()));
		connect(this->mUi.actionAusgabe,	SIGNAL(triggered(bool)),		this->mConsole,				SLOT(setVisible(bool)));
		connect(this->mConsole,				SIGNAL(visibilityChanged(bool)),this->mUi.actionAusgabe,	SLOT(setChecked(bool)));
		connect(this->mUi.actionHelp,		SIGNAL(triggered()),			this,						SLOT(showHelp()));
		connect(this->mUi.actionAbout,		SIGNAL(triggered()),			this,						SLOT(showAbout()));
		connect(this->mUi.actionLoadPreset,	SIGNAL(triggered()),			this,						SLOT(addTrackerWithPreset()));
		connect(this->mUi.actionResultComparer,	SIGNAL(triggered()),		this,						SLOT(addResultComparer()));
		connect(this->mUi.actionSettings,	SIGNAL(triggered(bool)), 		this->mSettings,			SLOT(setVisible(bool)));
		connect(this->mSettings,			SIGNAL(visibilityChanged(bool)),this->mUi.actionSettings,	SLOT(setChecked(bool)));
		//Menubar - EOF

		//Toolbar
		connect(this->mUi.actionAddTracker, SIGNAL(triggered()), this, SLOT(addTracker()));
		//Toolbar - EOF	

		//Colors for better distintion of trackers
		this->mColors.push_back(QColor(0,100,150));
		this->mColors.push_back(QColor(100,100,100));
		this->mColors.push_back(QColor(0,155,100));
		this->mColors.push_back(QColor(55,155,0));
		this->mColors.push_back(QColor(0,55,155));
		this->mColors.push_back(QColor(100,55,0));
		this->mColors.push_back(QColor(100,0,55));
		this->mColors.push_back(QColor(150,55,150));
	}

	void GUI::receiveTextData(
		GUITracker* src, 
		const QString& data)
	{	
		int id = 1+ std::find(this->mTracker.begin(), this->mTracker.end(), src) - this->mTracker.begin();
		QString postData = "[Tracker:"+QString::number(id)+"] "+data;
		this->mUi.statusBar->showMessage(postData, 2000);
	
		QColor color(255,0,0);
		if(!data.contains("ERROR", Qt::CaseInsensitive))
		{
			//QStringList mColors = QColor::colorNames();		
			//color = QColor(mColors[(id+7)%mColors.size()]).darker())
			color = this->mColors[id % this->mColors.size()];		
		}
		else
		{
			this->mWin7.setProgressState(EcWin7::Error);
		}

		this->mConsole->addMessage(postData, color);

		//If an update was received it's also time to update the progress in mWin7 taskbar
		if(src == this->mTabs->currentWidget())
		{
			this->mWin7.setProgressValue(src->getCompleteProgress(),100);
		}
	}

	// *** EcWin7 hook on windows messages ***
	// Override this method to intercept needed messages
	bool GUI::winEvent(MSG *message, long *result)
	{
		return this->mWin7.winEvent(message, result);
	}

	void GUI::exit()
	{
		QMessageBox msgBox;
		msgBox.setText("Wirklich beenden?");
		msgBox.setInformativeText("Möchten Sie das Programm wirklich beenden? Nicht gespeicherte Ergebnisse gehen dabei verloren.");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIcon(QMessageBox::Warning);
		switch (msgBox.exec()) 
		{
		   case QMessageBox::Ok:
			   this->mNotes->saveNotes();
				qApp->quit();
				break;
		   default:
				break;
		 }
	}

	void GUI::activeTabChanged(int)
	{
		GUITracker* t = dynamic_cast<GUITracker*>(this->mTabs->currentWidget());//Ugly... better way?
		if(t != 0) 
		{
		    // old was safely casted to NewType
			this->mWin7.setProgressValue(t->getCompleteProgress(),100);
			this->mWin7.setProgressState(EcWin7::Normal);
		}
		else
		{
			mWin7.setProgressState(mWin7.NoProgress);		
		}
	}
	#pragma endregion

	#pragma region display additional GUI elements
	void GUI::showHelp()
	{
		if(!this->mHelp)
		{
			this->mHelp = new QDialog(this);
			Ui::FormHelp h;
			h.setupUi(this->mHelp);
		}
		this->mHelp->setVisible(true);
	}

	void GUI::showAbout()
	{
		if(!this->mAbout)
		{
			this->mAbout = new GUIAbout(this);
		}
		this->mAbout->setVisible(true);
	}

	void GUI::showNotesTab(bool show)
	{	
		if(show)
		{
			this->mTabs->setCurrentIndex(
				this->mTabs->addTab(this->mNotes, QIcon(":/QTTestVs10/mNotes.png"), "Notizen"));
		}
		else
		{
			this->mTabs->removeTab(this->mTabs->indexOf(this->mNotes));
		}
	}

	void GUI::addResultComparer	()
	{
		GUIResultComparer *w = new GUIResultComparer();
		
		this->mTabs->setCurrentIndex(
			this->mTabs->addTab(w, "Ergebnis-Vergleich"));
	}
	#pragma endregion

	#pragma region Adding mTracker mTabs
	void GUI::addTracker()
	{
		GUITracker *w = new GUITracker();

		this->mTracker.push_back(w);

		this->mTabs->setCurrentIndex(
			this->mTabs->addTab(w, "Tracker " + QString::number(this->mTracker.size())));
	
		QObject::connect(w, SIGNAL(text2GUI(GUITracker*, const QString&)),	this, SLOT(receiveTextData(GUITracker*, const QString&)));
	}

	void GUI::addTrackerWithPreset()
	{
		QString path = QFileDialog::getOpenFileName (this, tr("Select preset"));
		if ( !path.isNull() )
		{
			GUITracker *w = new GUITracker(path);

			this->mTracker.push_back(w);

			this->mTabs->setCurrentIndex(
				this->mTabs->addTab(w, "Tracker " + QString::number(this->mTracker.size())));
	
			QObject::connect(w, SIGNAL(text2GUI(GUITracker*, const QString&)),	this, SLOT(receiveTextData(GUITracker*, const QString&)));
		}
	}

	void GUI::startImport()
	{
		QFileDialog dialog(this);
		dialog.setNameFilter(tr("Tracker-Results (*.csv)"));	
		dialog.setFileMode(QFileDialog::ExistingFiles);
		QStringList fileNames;
		if (dialog.exec())
		{
			 InOut::ImportFromCSV importer;
			 fileNames = dialog.selectedFiles();
			 while(!fileNames.isEmpty())
			 {
				 QString file = fileNames.takeFirst();			 

				 QString rowSeperator, colSeperator;
				 GUISettings::Instance()->getCSVValues(
					 colSeperator, rowSeperator);
				 vector< LarvaModel*> * larvae = importer.importLarvae(
					 file, rowSeperator, colSeperator);

				 if(larvae)
				 {
					 GUITracker *t = new GUITracker();
					 t->setROIRect(cv::Rect(0,0,0,0));
					 t->receiveTrackerResults(larvae);
					 this->mTabs->setCurrentIndex(
						 this->mTabs->addTab(t, QFileInfo(file).fileName()));
				 }
				 else
				 {
					 QMessageBox::information(0, "Fehler", "Datei ["+file+"] konnte nicht geladen werden. Stimmen die CSV-Parameter (Datei/Einstellungen)?");
				 }
			 }		 
		}	
	}
	#pragma endregion
}