/**
 *  @file ResultComparerThread.h
 *
 *  @section DESCRIPTION
 *	Compares results from two CSV files
 *	containing tracking result.
 */

#pragma once

#include <vector>
#include <qthread.h>

#include "GUISettings.h"
#include "LarvaModel.h"
#include "ImportFromCSV.h"
#include "Toolbox.h"
/**
 *	Compares results from two CSV files
 *	containing tracking result.
 */
class ResultComparerThread : public QThread
{
		Q_OBJECT
public:	
	/**
    * Constructs the comparer for the specified datasets.
    *
    * @param	_larvaeGT		Base data set
	* @param	_larvaeCompare	Compare data set
    */
	ResultComparerThread(
		const QString & _larvaeGT, 
		const QString & _larvaeCompare);
	~ResultComparerThread(void);

	/**
    * Forrest
    */
	void run();

private:
	/**
    * Gets the nearest model to specified position
	* at given time index.
    *
    * @param	_models				List of base models.
	* @param	_requiredTimeIndex	Required time index of target.
	* @param	_position			Position target should be near to.
	* @return	Best matching model.
    */
	LarvaModel* getBestMatch(		
		const std::vector<LarvaModel*> *	_models,
		const unsigned int				_requiredTimeIndex,
		const Vector2D						_position);

	/**
    * Compares the center-position of two sets of larvae,
	* Returns result via signal.
    *
    * @param	_truth		Base data set
	* @param	_compare	Compare data set
    */
	void compareCenter(
		const std::vector<LarvaModel*> * _truth,
		const std::vector<LarvaModel*> * _compare);
	
	/**
    * Compares the position of each spine point from 
	* two sets of larvae, Returns result via signal.
    *
    * @param	_truth		Base data set
	* @param	_compare	Compare data set
    */
	void compareEachSpinePoint(
		const std::vector<LarvaModel*> * _truth,
		const std::vector<LarvaModel*> * _compare);

	/**
    * Compares the widths of each spine point from 
	* two sets of larvae, Returns result via signal.
    *
    * @param	_truth		Base data set
	* @param	_compare	Compare data set
    */
	void compareSpineWidths(
		const std::vector<LarvaModel*> * _truth,
		const std::vector<LarvaModel*> * _compare);
		
	QString mLarvaeGTFile;		//path of base data set
	QString mLarvaeCompareFile;	//path of compare data set

signals:
	/**
	* Emitted if at least one file could not be loaded
	* @param	_file	Path of the unreadable file
	*/ 
	void badFile	(
		const QString _file);

	/**
	* Emitted when center comparison finished
	* @param	_min	Minimum value
	* @param	_max	Maximum value
	* @param	_mid	Middle value
	* @param	_med	Medium value
	*/ 
	void centerComparisonFinished	(
		const double _min, 
		const double _max, 
		const double _mid, 
		const double _med);

	/**
	* Emitted when spinepoint comparison finished
	* @param	_min	Minimum value
	* @param	_max	Maximum value
	* @param	_mid	Middle value
	* @param	_med	Medium value
	*/ 
	void spinepointComparisonFinished	(
		const double _min, 
		const double _max, 
		const double _mid, 
		const double _med);

	/**
	* Emitted when width comparison finished
	* @param	_minAbs	Minimum value [Abs]
	* @param	_minGen	Minimum value [gen]
	* @param	_minSmaller	Minimum value [smaller]
	* @param	_minBigger	Minimum value [bigger]
	* @param	_maxAbs	Maximum value [Abs]
	* @param	_maxGen	Maximum value [gen]
	* @param	_maxSmaller	Maximum value [smaller]
	* @param	_maxBigger	Maximum value [bigger]
	* @param	_midAbs	Middle value [Abs]
	* @param	_midGen	Middle value [gen]
	* @param	_midSmaller	Middle value [smaller]
	* @param	_midBigger	Middle value [bigger]
	* @param	_medAbs	Medium value [Abs]
	* @param	_medGen	Medium value [gen]
	* @param	_medSmaller	Medium value [smaller]
	* @param	_medBigger	Medium value [bigger]
	*/ 
	void widthsComparisonFinished	(
		const double _minAbs, const double _minGen, const double _minSmaller, const double _minBigger, 
		const double _maxAbs, const double _maxGen, const double _maxSmaller, const double _maxBigger, 
		const double _midAbs, const double _midGen, const double _midSmaller, const double _midBigger,  
		const double _medAbs, const double _medGen, const double _medSmaller, const double _medBigger);
};

