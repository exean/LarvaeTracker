/**
 *  @file TrackerThread.h
 *
 * @section DESCRIPTION
 * Allows parallelization of tracking algorithms and
 * provides signal/slot-based communication with the
 * algorithm.
 */

#pragma once

#include <QThread>
#include <qimage.h>
#include <qdebug.h>
#include <opencv2/opencv.hpp>
#include "ATrackerObserver.h"
#include "ATracker.h"
#include "Preprocessing.h"

namespace TrackerConnection
{
	/**
	 * Allows parallelization of tracking algorithms and
	 * provides signal/slot-based communication with the
	 * algorithm.
	 */
	class TrackerThread : public QThread, ATrackerObserver
	{
		Q_OBJECT
	public:
		/**
		* Constructor
		*
		* @param _tracker	Tracking algorithm
		* @param _files		Input data
		* @param _roi		Region of interest
		* @param _prepoc	Image preprocessor
		* @param _bgSub		Background subtractor
		*/
		TrackerThread(
			Dll::ATracker						*	_tracker, 
			vector<string>							_files, 
			cv::Rect								_roi, 
			PreprocessingFuncs::Preprocessing	*	_prepoc,
			Dll::ABackgroundSubtractor			*	_bgSub)
		{ 
			this->mTracker		= _tracker; 
			this->mFiles		= _files; 
			this->mRoi			= _roi;
			this->mPreprocessor	= _prepoc;
			this->mBgSub		= _bgSub;
			this->mTracker->addObserver(this);
		};
		~TrackerThread(void) 
		{ 
			if(this->mBgSub) delete this->mBgSub;
			if(this->mPreprocessor) delete this->mPreprocessor; 
		};
	
		/**
		* Send results of tracking to this class which will
		* provide it to its observers.
		*
		* @param _data		Tracking results
		*/
		void receiveResult		(vector<LarvaModel*>* _data){ emit result2GUI	(_data);		};
	
		/**
		* Send text-data of tracking to this class which will
		* provide it to its observers.
		*
		* @param _data		Text data
		*/
		void receiveData		(const string	&_data)		{ emit text2GUI	(_data.c_str());	};		
	
		/**
		* Send current progress of tracking to this class which 
		* will provide it to its observers.
		*
		* @param _data		Current progress
		*/
		void setCurrentProgress	(const int		&_data)		{ emit currentProgress2GUI	(_data); };
	
		/**
		* Send complete progress of tracking to this class which 
		* will provide it to its observers.
		*
		* @param _data		Complete progress
		*/
		void setCompleteProgress(const int		&_data)		{ emit completeProgress2GUI	(_data); };
	
		/**
		* Send image data of tracking to this class which will
		* provide it to its observers.
		*
		* @param _data		Image data
		*/
		void receiveData		(const cv::Mat	&_data)	
		{ 
			QImage img(_data.data, _data.cols, _data.rows, _data.step, QImage::Format_RGB888);
			emit image2GUI(img);
		};

signals:
		/**
		* Emitted whenever tracker sent image data.
		*/ 
		void image2GUI				(const QImage&);		

		/**
		* Emitted whenever tracker sent text data.
		*/ 
		void text2GUI				(const QString&); 

		/**
		* Emitted whenever a progress update occurs.
		*/ 
		void currentProgress2GUI	(const int&);		

		/**
		* Emitted whenever a progress update occurs.
		*/ 
		void completeProgress2GUI	(const int&); 

		/**
		* Emitted after tracker finished.
		*/ 
		void result2GUI				(vector<LarvaModel*>*);

	public slots:
		/**
		* Request the tracker to stop tracking
		*/
		void stop					() { this->mTracker->stop(); };

	private:
		/**
		* Start a QThread with the selected tracker
		*/
		virtual void run()
		{ /*QThread::exec();*/ 	
			this->mTracker->sendCompleteProgressToObservers(0);
			this->mTracker->sendCurrentProgressToObservers("Initialisiere System...", 0);
			PreprocessingFuncs::ImageLoader loader(//Image loader
				this->mFiles,
				this->mBgSub, 
				this->mRoi, 
				this->mPreprocessor);
			loader.initialize();				
			this->mTracker->perform(loader);
		}
		
		/**
		* Image preprocessor
		*/
		PreprocessingFuncs::Preprocessing	*	mPreprocessor;

		/**
		* Tracking algorithm
		*/
		Dll::ATracker						*	mTracker;

		/**
		* Input data
		*/
		vector<string>							mFiles;

		/**
		* Region of interest
		*/
		cv::Rect								mRoi;

		/**
		* Background subtractor
		*/
		Dll::ABackgroundSubtractor			*	mBgSub;
	};
}