/*
 *  @file ATracker.h
 *
 *  @section DESCRIPTION
 *  Observable interface for Tracker-Plugins
 *
 */

#pragma once

#include <map>
#include <vector>

#include "APlugin.h"
#include "LarvaModel.h"
#include "ATrackerObserver.h"

#include "ImageLoader.h"

using namespace std;

namespace Dll
{
	/*
	 *  Observable interface for Tracker-Plugins
	 */
	class ATracker : public APlugin
	{
	public:	
		/**
		* Starts the algorithm; updates and results will be send using signals.
		* 
		* @param	_imgLoader	Object for loading images.
		*/
		virtual void perform(
			PreprocessingFuncs::ImageLoader _imgLoader) = 0;	

		/**
		* Stops the algorithm
		*/
		virtual void stop() = 0;	

		/**
		* Set parameters for this algorithm. The maps keys are
		* defined in corresponding "getSettings" method.
		* Does not need to be implemented if no parameters are required.
		* 
		* @param	_settings	A map of parameters
		*/
		virtual void setSettings(vector< ParameterWrapper > _settings){};

		/**
		* Provides a map of default parameters.
		* Does not need to be implemented if no parameters are required.
		* 
		* @return	A map of default parameters
		*/
		virtual vector< ParameterWrapper > getSettings(){ return vector< ParameterWrapper >(); };	

		/**
		* Allows observers to be added which will be
		* notified when the algorithm progresses and
		* will also be receiving the algorithms results.
		* 
		* @param	_obs	An observer
		*/
		void addObserver(TrackerConnection::ATrackerObserver* _obs){  this->mObserver.push_back(_obs); };	

		/**
		* Send image data to observers
		* 
		* @param	_data	cv-Image
		*/
		void sendUpdateToObservers(const cv::Mat &_data)
		{ 
			for each(TrackerConnection::ATrackerObserver* ob in this->mObserver) ob->receiveData(_data);	
		};

		/**
		* Send text data to observers to be displayed in statusbar
		* 
		* @param	_data	text data
		*/
		void sendUpdateToObservers(const string &_data)
		{ 
			for each(TrackerConnection::ATrackerObserver* ob in this->mObserver) ob->receiveData(_data);		 
		};

		/**
		* Send progress information concerning current image
		* 
		* @param _data	Current progress
		*/
		void sendCurrentProgressToObservers(const int &_data)
		{ 
			for each(TrackerConnection::ATrackerObserver* ob in this->mObserver) ob->setCurrentProgress(_data);		 
		};

		/**
		* Send progress information concerning current image
		* 
		* @param _info				information text
		* @param _currentprogress	Current progress (0-100)
		*/
		void sendCurrentProgressToObservers(const string &_info, const int &_currentprogress)
		{ 
			this->sendCurrentProgressToObservers(_currentprogress);					
			this->sendUpdateToObservers(_info);
		};

		/**
		* Send progress information concerning complete algorithm (all images)
		* 
		* @param _data	Overall progress
		*/
		void sendCompleteProgressToObservers(const int &_data)
		{ 
			for each(TrackerConnection::ATrackerObserver* ob in this->mObserver) ob->setCompleteProgress(_data);		 
		};

		/**
		* Send results to observers. Since this will be rather large
		* a pointer to the data is sent; clean-up needs to be performed by
		* receiver after the data has been used.
		* 
		* @param _data	Result of the algorithm
		*/
		void sendResultToObservers(vector<LarvaModel*>* _data)
		{ 
			for each(TrackerConnection::ATrackerObserver* ob in this->mObserver) ob->receiveResult(_data);		
		};

		/**
		* List of all observers
		*/
		vector<TrackerConnection::ATrackerObserver*> mObserver;
	};
}