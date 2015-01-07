/**
 *  @file PluginLoader.h
 *
 *	@section DESCRIPTION
 *	Class for loading and managing plugins.
 *	SINGLETON
 */
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <QString>
#include <QApplication>
#include <QDir>

#include <windows.h>
#include "ATracker.h"
#include "APlugin.h"

using namespace std;

namespace Dll
{
	/**
	* Creator-Method of tracker-plugins
	*/
	typedef ATracker							* (*CreateTrackerFunc)();

	/**
	* Creator-Method of background-subtractor-plugins
	*/
	typedef ABackgroundSubtractor				* (*CreateBackgroundSubtractorFunc)();

	/**
	* Creator-Method of general-plugins
	*/
	typedef APlugin								* (*CreatePluginFunc)();

	/**
	 *	Class for loading and managing plugins.
	 */
	class PluginLoader
	{
	public:
		/**
		* @return Singleton Instance
		*/
		static PluginLoader						*	Instance();
		~PluginLoader(void);

		/**
		* Reloads plugins
		*/
		void										reloadPlugins();
	
		/**
		* @return List of all tracker plugins
		*/
		vector< ATracker* >							getTrackers();
			
		/**
		* @return List of all background subtraction plugins
		*/
		vector< ABackgroundSubtractor* >			getBackgroundSubtractors();
			
		/**
		* @return List of all available plugins
		*/
		vector< APlugin* >							getAll();
	
	private:
		PluginLoader(void);
		/**
		* Singleton instance
		*/
		static PluginLoader* mInstance;

		/**
		* List of available tracker-plugins.
		*/
		vector < CreateTrackerFunc >				mTracker;

		/**
		* List of available background-subtractor-plugins.
		*/
		vector < CreateBackgroundSubtractorFunc >	mBackgroundSubtractor;

		/**
		* List of available plugins without further specification.
		*/
		vector < CreatePluginFunc >					mUnspecifiedPlugins;

		/**
		* Plugins that will only be initialized once, eg. those that change the GUI
		*/
		vector < APlugin* >							mSingeInstancePlugins;
		
		/**
		* Filenames of already loaded single instance plugins
		*/
		vector < QString >							mSingeInstancePluginFiles;
	};
}