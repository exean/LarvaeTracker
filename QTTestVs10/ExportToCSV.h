/**
 *  @file ExportToCSV.h
 *
 *  @section DESCRIPTION
 *	Export tracking-results to a CSV-file
 */

#pragma once

#include <fstream>
#include <iomanip>
#include <vector>
#include "LarvaModel.h"
#include "..//GeneralTrackerFunctions//Toolbox.h"

namespace InOut
{
	/**
	 *	Export tracking-results to a CSV-file
	 */
	class ExportToCSV
	{
	public:
		ExportToCSV(void);
		~ExportToCSV(void);
				
		/**
		* Export tracking results to CSV-file.
		* 
		* @param	_rowSeperator	Seperator-string for rows.
		* @param	_colSeperator	Seperator-string for columns.
		* @param	_resultsInMM	If true, results will be provided 
		*							in mm; else in pixels.
		* @param	_file			File to which results will be written.
		* @param	_larvae			Results of tracking as list of base
		*							larvae.
		* @param	_allXY			Export of all spine-points.
		* @param	_massXY			Export all larvas contour-CenterOfMass.
		* @param	_allRadi		Export radi of spine-point circles.
		* @param	_contourArea	Export the area of the larvas contour.
		* @param	_collisionFlag	Export 1 for each larva during a 
		*							collision, 0 otherwise.
		* @param	_nnDistance		Export distance to nearest other larva.
		* @param	_massDistance	Export distance to previous center of mass.
		* @param	_landmarks		Export distance to landmarks set by user.
		* @param	_velocity		Export the larvas velocity.
		* @param	_acceleration	Export the larvas acceleration.
		* @param	_originDistance	Export distance to the larvas first detection.
		* @param	_accDistance	Export distance larva has travelled.
		* @param	_direction		Export direction the larva travels in.
		* @param	_timeSteps		Amount of time-steps to be exported.
		* @param	_pixelPerMM		Pixel per mm on input data.
		* @param	_fps			Frames per second of input data.
		* @param	_p1				Export angle between 3 spine-points; 
		*							index of 1st point.
		* @param	_p2				Export angle between 3 spine-points; 
		*							index of 2nd point.
		* @param	_p3				Export angle between 3 spine-points; 
		*							index of 3rd point.
		* @return	true, if export successful.
		*/
		bool exportLarvae(
			const string				&	_rowSeperator,
			const string				&	_colSeperator,
			const bool						_resultsInMM,
			const string				&	_file, 
			const vector<LarvaModel*>	&	_larvae,
			const bool						_allXY,
			const bool 					_massXY,
			const bool 					_allRadi,
			const bool 					_contourArea,
			const bool 					_collisionFlag,
			const bool 					_nnDistance,
			const bool 					_massDistance,
			const vector<Point>			&	_landmarks,//Empty, if no information shall be exported
			const bool 					_velocity,
			const bool 					_acceleration,
			const bool 					_originDistance,
			const bool 					_accDistance,
			const bool 					_direction,
			const unsigned int			_timeSteps,
			const double					_pixelPerMM = 1,
			const double 					_fps = 1,
			const int _p1 = -1, const int _p2 = -1, const int _p3 = -1//Angle
			);

	private:
#pragma region FIRST-ORDER	
		/**
		* Setup lists for exporting x-,y-coordinates of spine-points.
		* 
		* @param	_larvae			Results of tracking as list of base
		*							larvae.
		* @param	_timeSteps		Amount of time-steps to be exported.
		* @param	out_x			List of x-coordinate-export-strings.
		* @param	out_y			List of y-coordinate-export-strings.
		* @param	out_spineSize	Measured length of spines.
		*/
		void setupAllXY	(
			const vector< LarvaModel* >	&	_larvae, 
			const unsigned int			_timeSteps,
			vector<vector<string>>		&	out_x,
			vector<vector<string>>		&	out_y,
			int							&	out_spineSize);

		/**
		* Gather x-,y-coordinates of spine-points.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getAllXY	(
			const vector< LarvaModel* >	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Gather center of masses.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getMassXY	(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Setup lists for exporting radi of spine-points.
		* 
		* @param	_larvae			Results of tracking as list of base
		*							larvae.
		* @param	_timeSteps		Amount of time-steps to be exported.
		* @param	out_radi		List of radius-export-strings.
		* @param	out_spineSize	Measured length of spines.
		*/
		void setupRadi (
			const vector< LarvaModel* >	&	_larvae, 
			const unsigned int			_timeSteps,
			vector<vector<string>>		&	out_radi,
			int							&	out_spineSize);

		/**
		* Gather spine-point-radii.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getAllRadi(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor =	1);

		/**
		* Gather contour area.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getContourArea		(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Gather collision flags.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		*/
		void getCollisionFlag		(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps);
#pragma endregion

#pragma region SECOND-ORDER	
		/**
		* Gather nearest neighbor distance at specific timepoint.
		* 
		* @param	_larva				Larva for which the nearest neighbor
		*								shall be examined.
		* @param	_neighbors			Larva for which the nearest neighbor
		*								shall be examined.
		* @param	_time				Timepoint at which nereast neighbor 
		*								shall be found.
		* @return	Distance to nearest neighbor.
		*/
		double getNNDistanceAtTime(
			const LarvaModel			*	_larva,
			const vector<LarvaModel*>	&	_neighbors,
			const unsigned int			_time,
			const double					_pixelToMetricFactor);

		/**
		* Gather nearest neighbor distances.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getNNDistance			(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps, 
			const double					_pixelToMetricFactor = 1);

		/**
		* Gather distance to center of mass in last timestep.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getMassDistance		(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Distance of each larva to each landmark.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_landmarks				List of landmarks set by user.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getDistanceToLandmarks	(
			const vector<LarvaModel*>	&	_larvae, 
			const vector<Point>			&	_landmarks, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);
		
		/**
		* Distance of each larva to its origin.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getDistanceToOrigin	(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Accumulated distance a larva has travelled up to the current timestep.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getAccumulatedDistance	(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor = 1);

		/**
		* Gather directions the larvae move in.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		*/
		void getMovementDirection	(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps);

		/**
		* Gather angle of spine between specified spinepoints.
		* 
		* @param	_p1					Index of first spinepoint.
		* @param	_p2					Index of second spinepoint.
		* @param	_p3					Index of third spinepoint.
		* @param	_larvae				Results of tracking as list of base
		*								larvae.
		* @param	_file				Output stream to save results.
		* @param	_timeSteps			Amount of time-steps to be exported.
		*/
		void getCurvature			(
			const int						_p1, 
			const int						_p2, 
			const int						_p3, 
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file,
			const unsigned int			_timeSteps);

		/**
		* Gather velocities of larvae.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_fps					Frames per second on input data.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getVelocity			(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps, 
			const double					_fps,
			const double					_pixelToMetricFactor = 1);
		
		/**
		* Gather acceleration of larvae.
		* 
		* @param	_larvae					Results of tracking as list of base
		*									larvae.
		* @param	_file					Output stream to save results.
		* @param	_timeSteps				Amount of time-steps to be exported.
		* @param	_fps					Frames per second on input data.
		* @param	_pixelToMetricFactor	Factor to convert pixel to mm.
		*/
		void getAcceleration		(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps, 
			const double					_fps, 
			const double					_pixelToMetricFactor = 1);
#pragma endregion

#pragma region convenience-methods
		/**
		* Setup lists for exporting larva data.
		* 
		* @param	_timeSteps		Amount of time-steps to be exported.
		* @param	_name			Name of data to be exported.
		* @param	out_strings		List of export-strings.
		*/
		void setupStrings	(
			const unsigned int			_timeSteps,
			const string				&	_name,
			vector< string >			&	out_strings);

		/**
		* Insert a set of empty fields.
		* 
		* @param	_begin			Timeindex at which the gap begins.
		* @param	_end			Timeindex at which the gap ends.
		* @param	mod_strings		List of export-strings in which the
		*							gaps will be inserted.
		*/
		void insertGap	(
			const int						_begin,
			const int						_end,
			vector< string >			&	mod_strings);

		/**
		* Write a list of export-strings to a file.
		* 
		* @param	_data			List of export-strings.
		* @param	_file			Stream for writing to file.
		*/
		void write					(
			const vector<string>		&	_data, 
			fstream						&	_file);

		/**
		* Seperator-string for rows.
		*/
		string mColSeperator;

		/**
		* Seperator-string for columns.
		*/
		string mRowSeperator;
#pragma endregion
	};
}