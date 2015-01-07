/**
 *  @file ImportFromCSV.h
 *
 *  @section DESCRIPTION
 *	Import tracking-results from a csv file
 */


#pragma once

#include <QFile>
#include <QString>
#include <vector>
#include <QTextStream>
#include <QMessageBox>
#include "LarvaModel.h"

namespace InOut
{
	/**
	 *	Import tracking-results from a csv file
	 */
	class ImportFromCSV
	{
	public:
		ImportFromCSV(void);
		~ImportFromCSV(void);
	
		/**
        * Import data from a csv file using the specified format.
        *
        * @param	_file			Path of the file
		* @param	_rowSeperator	CSV row-seperator
		* @param	_colSeperator	CSV col-seperator
		* @return	tracking data
        */
		vector<LarvaModel*>* importLarvae(
			const QString				&	_file,
			const QString				&	_rowSeperator,
			const QString				&	_colSeperator);

	private:
		/**
        * Reads tracking information from a line of text.
        *
        * @param	_line			Text to parse
		* @param	_seperator		CSV col-seperator
		* @param	mod_larvae		Data will be appended to this list.
		* @return	true if parsing was successful
        */
		bool					parseLine(
			const QString					&	_line,
			const QString					&	_seperator,
			vector<LarvaModel*>				*	mod_larvae);

		/**
        * Retrieves the model for the specified time index.
        *
        * @param	_timeIndex		Requested time index
		* @param	_larva			Base larva
		* @return	Pointer to the requested model.
        */
		LarvaModel*				getLarvaTimeFrame(
			const unsigned int			_timeIndex, 
			LarvaModel					*	_larva);

		/**
        * Builds a spine of the specified size
        *
        * @param	_finalSpineIndex	Requested spine size-1
		* @param	_larva				Larva for which the spine is built
        */
		void					setupLarvaSpine(
			const unsigned int			_finalSpineIndex, 
			LarvaModel					*	_larva);
	};
}