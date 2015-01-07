/**
 *  @file GUITracker.h
 *
 *  @section DESCRIPTION
 *	GUI Component for interation with a
 *	tracker-plugin.
 */

#pragma once

#include <QString>
#include <QtGui/QDialog>
#include <QFileDialog>
#include <QPixmap>

#include "ui_QTTrackerForm.h"
#include "ui_QTResultParameterChooser.h"

#include "ATracker.h"
#include "ABackgroundSubtractor.h"

#include "TrackerThread.h"
#include "PluginLoader.h"
#include "GUIResults.h"
#include "ExportToCSV.h"
#include "TrackerGraphicsView.h"
#include "GUIParameterLayout.h"
#include "QListWidgetItemWithData.h"
#include "SettingsIO.h"
#include "GUITrackerProgress.h"

#include "Preprocessing.h"

namespace Gui
{
	/**
	 *	GUI Component for interation with a
	 *	tracker-plugin.
	 */
	class GUITracker
		:	public QWidget
	{
		Q_OBJECT

	public:
		GUITracker		(void);

		/**
		* Constructor accepting a settings-file
		*
		* @param	_presetFileName	Path to a settings file
		*/
		GUITracker		(const QString & _presetFileName);
		~GUITracker		(void);

		/**
		* Returns the complete progress of the tracker
		* represented by this object.
		*
		* @return	Complete progress (0-100)
		*/
		int											getCompleteProgress		();
		
		/**
		* Returns the ROI set for this tracker
		*
		* @return	Region of interest
		*/
		cv::Rect									getROIRect				();

		/**
		* Set the ROI set for this tracker
		*
		* @param	_roi	Region of interest
		*/
		void										setROIRect				(const cv::Rect & _roi);
			
	private:
		void										setup					();
		void										setupFilesPage			();
		void										setupTrackerPage		();
		void										setupPreprocessingPage	();			
		void										setupPreprocessingForm	();
		void										setupBGSubtractionForm	();
		void										setupAvailableAlgos		();	

		void										cleanThread				(TrackerConnection::TrackerThread* _thread);

		void										finishAlgo				();
		
		/**
		* Load settings from a file
		*
		* @param	_path	Path to the file containig tracking-settings
		*/
		void										loadPreset				(const QString & _path);

		/**
		* Returns the list of selected input images
		*
		* @return	Paths to selected input images
		*/
		vector<string>								getFileList				();

		/**
		* @return	True, if conditions for background-subtraction are satisfied (e.g. enough input images for selected percentage).
		*/
		bool										checkBackgroundSubtractorSettings();

		Ui::TrackerForm								mUi;	
		Ui::ResultParameterForm						mResultForm;	
		TrackerConnection::TrackerThread*			mThread;
		map<QString, Dll::ATracker* >				mAvailableAlgos;	
		map<QString, Dll::ABackgroundSubtractor* >	mAvailableBackgroundSubtractors;
		vector<LarvaModel*>					*		mReceivedTrackerData;
		GUITrackerProgress					*		mProgressWidget;

		Visualization::TrackerGraphicsView	*		mCurrentImage;

		PreprocessingFuncs::Preprocessing	*		mPreprocessor;
		InOut::SettingsIO							mSettingsIO;

		GUIParameterLayout					*		mAlgorithmSettingsLayout;
		vector< ParameterWrapper >					mAlgorithmSettings;

		unsigned int								mMaximalTimeIndex;

	public slots:
		void										receiveTrackerResults	(vector<LarvaModel*>* _data);		
		void 										loadPreset				();
		void										stopTracker				();

	private slots:
		//In/Out
		void 										startExport				();
		void 										savePreset				();
		void 										setDefaultPreprocessing	();
		void 										loadDefaultPreprocessing();
		void 										setDefaultBGSubtraction	();
		void 										loadDefaultBGSubtraction();
		void 										setDefaultAlgo			();
		void 										loadDefaultAlgo			();
		void 										setDefaultFPSPixPerMM	();
		void 										loadDefaultFPSPixPerMM	();
		void 										setDefaultROI			();
		void 										loadDefaultROI			();

		//Image files
		void 										addFiles				();
		void 										resetFiles				();
		void 										removeFiles				();
		void 										selectImage				(QListWidgetItem * _item,  QListWidgetItem * _previous);		
		void 										showImage				(int _index);

		void 										roiChanged				();
		void 										drawROI					(const int _x, const int _y, const int _width, const int _height);
		void										scaleChanged			(const qreal, const qreal);
		void										changeScale				(const int _scale);

		//Algo
		void 										displaySettings			(const QString &);
		void 										startAlgo				();	

		//Communication with tracker thread
		void 										receiveImage			(const QImage&);	
		void										receiveTextData			(const QString&);

		//preprocessing methods
		void										addPreprocessingMethod	();
		void 										removePreprocessingMethod();	
		void 										movePreprocessingMethodDown();	
		void 										movePreprocessingMethodUp();
		void 										editPreprocessingMethod	(QListWidgetItem * _item = NULL);
		void 										doPreprocessingTest		();

		//Background subtraction
		void										changeBGSubMethod		(const QString &);
		void 										doBGSubtractionTest		();
		void										setBGSubSampleRate		( double );
		Dll::ABackgroundSubtractor*					getSelectedBackgroundSubtractor();

	signals:	
		void 										text2GUI				(GUITracker*, const QString&); 	
	};
}