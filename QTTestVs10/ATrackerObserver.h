/**
 *  @file ATrackerObserver.h
 *
 *  @section DESCRIPTION
 *	Interface for observer for ATrackers.
 */

#pragma once
#include "LarvaModel.h"
using namespace std;
namespace TrackerConnection
{
	/**
	 *	Interface for observer for ATrackers.
	 */
	class ATrackerObserver
	{
	public:	
		/**
		* Receive image data from the tracker.
		* 
		* @param	_data	Image data.
		*/
		virtual void receiveData			(const cv::Mat		& _data)= 0;
		
		/**
		* Receive text data from the tracker.
		* 
		* @param	_data	Text data.
		*/
		virtual void receiveData			(const string		& _data)= 0;
	
		/**
		* Provides the percentual progression of the current action.
		* 
		* @param	_data	Current progress.
		*/
		virtual void setCurrentProgress	(const int			& _data)= 0;
		
		/**
		* Provides the percentual progression of the total algorithm.
		* 
		* @param	_data	Total progress.
		*/
		virtual void setCompleteProgress	(const int			& _data)= 0;
			
		/**
		* Provides the results of the algorithms computations.
		* 
		* @param	_data	Tracking results.
		*/
		virtual void receiveResult			(vector<LarvaModel*>* _data)= 0;
	};
}