/**
 *  @file APlugin.h
 *
 *  @section DESCRIPTION
 *	General functions plugins may implement to
 *	provide information about themselves
 */

#pragma once
#include <string>

namespace Dll
{
	/**
	 *	General functions plugins may implement to
	 *	provide information about themselves
	 */
	class APlugin
	{
	public:	
		APlugin(void) :
			mName("Untitled"), 
			mAuthor("Anonymous"),
			mDescription("-"),
			mVersion(0),
			mAllowMultipleInctances(true){};

		/**
		* Provides the name of the algorithm.
		* 
		* @return Name of the algorithm
		*/
		virtual std::string		getName()		{ return this->mName; };

		/**
		* Provides the name of author of the algorithm.
		*
		* @return Author of the algorithm
		*/
		virtual std::string		getAuthor()		{ return this->mAuthor; };

		/**
		* Provides the version of the algorithm.
		* 
		* @return Version of the algorithm
		*/
		virtual float			getVersion()	{ return this->mVersion; };

		/**
		* Provides a short description about the algorithm.
		* 
		* @return Description of the algorithm
		*/
		virtual std::string		getDescription(){ return this->mDescription; };

		/**
		* Some Plugins, e.g. those that change the GUI may only be loaded once..
		* 
		* @return true, if multiple instances are allowed
		*/
		virtual bool			isMultiInstancingAllowed(){ return this->mAllowMultipleInctances; };

	protected:
		/**
		* Name of the algorithm
		*/
		std::string mName;
		
		/**
		* Author of the algorithm
		*/
		std::string mAuthor;
		
		/**
		* Description of the algorithm
		*/
		std::string mDescription;
		
		/**
		* Version of the algorithm
		*/
		float mVersion;

		/**
		* true, if multiple instances are allowed
		*/
		bool mAllowMultipleInctances;
	};
}