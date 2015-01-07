/**
 *  @file SettingsIO.h
 *
 *	@section DESCRIPTION
 *	Handles reading and writing settings from/to files.
 */
#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include "ParameterWrapper.h"
#include "GUIParameterLayout.h"

#include "Preprocessing.h"
#include "PreprocessingMethods.h"
#include "ATracker.h"

namespace InOut
{
	/**
	 *	Handles reading and writing settings from/to files.
	 */
	class SettingsIO
	{
	public:
		SettingsIO(void);
		~SettingsIO(void);

#pragma region Settings
		/**
		* Load settings settable through settings-dialog by user.
		*
		* @param	out_mailSenderAddress		Sender-Adress for e-mails
		* @param	out_mailDefaultRecipient	Default e-mail recipient
		* @param	out_mailServer				SMTP-server-adress 
		* @param	out_mailUsername			SMTP username
		* @param	out_mailPassword			SMTP password
		* @param	out_mailPort				SMTP port
		* @param	out_csvCol					Column seperator for CSV export
		* @param	out_csvRow					Row seperator for CSV export
		* @param	_filename					Filename of the file that stores the settings.
		* @param	_fd							File open mode
		*/
		static bool LoadSettings(
			QString							&out_mailSenderAddress,
			QString							&out_mailDefaultRecipient,
			QString							&out_mailServer,
			QString							&out_mailUsername,
			QString							&out_mailPassword,
			int								&out_mailPort,
			QString							&out_csvCol,
			QString							&out_csvRow,
			const QString					&_filename = QString("default_settings"),//Default/Standard file
			const QIODevice::OpenMode		 _fd = QIODevice::ReadOnly | QIODevice::Text);
			
		/**
		* Save settings settable through settings-dialog by user.
		*
		* @param	_mailSenderAddress		Sender-Adress for e-mails
		* @param	_mailDefaultRecipient	Default e-mail recipient
		* @param	_mailServer				SMTP-server-adress 
		* @param	_mailUsername			SMTP username
		* @param	_mailPassword			SMTP password
		* @param	_mailPort				SMTP port
		* @param	_csvCol					Column seperator for CSV export
		* @param	_csvRow					Row seperator for CSV export
		* @param	_filename				Filename of the file that will store the settings.
		* @param	_fd						File open mode
		*/
		static void SaveSettings(			
			const QString					&_mailSenderAddress,
			const QString					&_mailDefaultRecipient,
			const QString					&_mailServer,
			const QString					&_mailUsername,
			const QString					&_mailPassword,
			const int						&_mailPort,
			const QString					&_csvCol,
			const QString					&_csvRow,
			const QString					&_filename = QString("default_settings"),//Default/Standard file
			const QIODevice::OpenMode		 _fd = QIODevice::WriteOnly | QIODevice::Text);
#pragma endregion

#pragma region Preprocessing
		/**
		* Load preprocessing settings
		*
		* @param	_methodNames		Names of preprocessing methods.
		* @param	_methodParams		Parameters of preprocessing method.
		* @param	_filename			Filename of the file that stores the settings.
		* @param	_fd					File open mode
		*/
		bool loadPreprocessing(
			std::vector< std::string >								& _methodNames, 
			std::vector< std::vector< ParameterWrapper >* >			& _methodParams,
			const QString											& _filename = QString("default_preprocessing"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::ReadOnly | QIODevice::Text);

		/**
		* Save preprocessing settings
		*
		* @param	_methodNames		Names of preprocessing methods.
		* @param	_methodParams		Parameters of preprocessing method.
		* @param	_filename			Filename of the file that will store the settings.
		* @param	_fd					File open mode
		*/
		void savePreprocessing(
			const std::vector< std::string >						& _methodNames, 
			const std::vector< std::vector< ParameterWrapper >* >	& _methodParams,
			const QString											& _filename = QString("default_preprocessing"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::WriteOnly | QIODevice::Text);
#pragma endregion

#pragma region Algorithm
		/**
		* Load tracking algorithm settings
		*
		* @param	out_algoName		Name of the tracking algorithm.
		* @param	out_algoParams		Parameters of tracking algorithm.
		* @param	_availablAlgos		List of all currently available tracking algorithms.
		* @param	_filename			Filename of the file that stores the settings.
		* @param	_fd					File open mode
		*/
		bool loadAlgo(
			QString													& out_algoName,
			std::vector< ParameterWrapper >							& out_algoParams,
			const map<QString, Dll::ATracker*>						* _availablAlgos,
			const QString											& _filename = QString("default_algorithm"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::ReadOnly | QIODevice::Text);

		/**
		* Save algorithm settings
		*
		* @param	_algoName			Name of tracking algorithm.
		* @param	_algoParams			Parameters of tracking algorithm.
		* @param	_filename			Filename of the file that will store the settings.
		* @param	_fd					File open mode
		*/
		void saveAlgo(
			const QString											& _algoName,
			const std::vector< ParameterWrapper >					& _algoParams,
			const QString											& _filename = QString("default_algorithm"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::WriteOnly | QIODevice::Text);
		
		/**
		* Parse algorithm data from a string based on available algorithms.
		*
		* @param	out_algoName		Name of tracking algorithm.
		* @param	out_algoParams		Parameters of  tracking algorithm.
		* @param	_availablAlgos		List of all currently available tracking algorithms.
		* @param	_algoString			String to be parsed.
		*/
		void parseAlgo(
			QString													& out_algoName,
			std::vector< ParameterWrapper >							& out_algoParams,
			const map<QString, Dll::ATracker*>						* _availablAlgos,
			const QString											& _algoString);
#pragma endregion

#pragma region BGSubtraction
		/**
		* Load background subtraction settings
		*
		* @param	out_methodName		Name of the bgsub method.
		* @param	out_percentage		Precentage of input images to be used.
		* @param	_filename			Filename of the file that stores the settings.
		* @param	_fd					File open mode
		*/
		bool loadBGSubtraction(
			QString													& out_methodName, 
			double													& out_percentage,
			const QString											& _filename = QString("default_bgsub"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::ReadOnly | QIODevice::Text);

		/**
		* Parses information about a background subtraction method.
		*
		* @param	out_methodName			Name of the bgsub method.
		* @param	out_percentage			Precentage of input images to be used.
		* @param	_bgSubtractionString	String to be parsed.
		*/
		void parseBGSubtraction(
			QString													& out_methodName, 
			double													& out_percentage,
			const QString											& _bgSubtractionString);
		
		/**
		* Save algorithm settings
		*
		* @param	_methodName			Name of the bgsub method.
		* @param	_percentage			Precentage of input images to be used.
		* @param	_filename			Filename of the file that will store the settings.
		* @param	_fd					File open mode
		*/
		void saveBGSubtraction(
			const QString											& _methodName, 
			const double											& _percentage,
			const QString											& _filename = QString("default_bgsub"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::WriteOnly | QIODevice::Text);
#pragma endregion

#pragma region input
		/**
		* Loads "frames per second" and "pixel per mm" values.
		*
		* @param	out_fps				Frames per second
		* @param	out_pixelPerMm		Pixel per mm
		* @param	_filename			Filename of the file that stores the settings.
		* @param	_fd					File open mode
		*/
		bool loadFPSPixPerMM(
			double													& out_fps,
			double													& out_pixelPerMm,
			const QString											& _filename = QString("default_fps"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::ReadOnly | QIODevice::Text);

		/**
		* Load region of interest
		*
		* @param	out_roi				Region of interest
		* @param	_filename			Filename of the file that stores the settings.
		* @param	_fd					File open mode
		*/
		bool loadROI(
			cv::Rect												& out_roi,
			const QString											& _filename = QString("default_roi"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::ReadOnly | QIODevice::Text);

		/**
		* Parses "frames per second" and "pixel per mm" values.
		*
		* @param	out_fps				Frames per second
		* @param	out_pixelPerMm		Pixel per mm
		* @param	_parseString		String to be parsed.
		*/
		void parseFPSPixPerMM(
			double													& out_fps,
			double													& out_pixelPerMm,
			const QString											& _parseString);

		/**
		* Parses region of interest
		*
		* @param	out_roi				Region of interest
		* @param	_roiString			String to be parsed.
		*/
		void parseROI(
			cv::Rect												& out_roi,
			const QString											& _roiString);
			
		/**
		* Save "frames per second" and "pixel per mm" values.
		*
		* @param	_fps				Frames per second
		* @param	_pixelPerMm			Pixel per mm
		* @param	_filename			Filename of the file that will store the settings.
		* @param	_fd					File open mode
		*/
		void saveFPSPixPerMM(
			const double											& _fps, 
			const double											& _pixelPerMm,
			const QString											& _filename = QString("default_fps"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::WriteOnly | QIODevice::Text);
			
		/**
		* Save region of interest
		*
		* @param	_roi				Region of interest
		* @param	_filename			Filename of the file that will store the settings.
		* @param	_fd					File open mode
		*/
		void saveROI(
			const cv::Rect											& _roi,
			const QString											& _filename = QString("default_roi"),//Default/Standard file
			const QIODevice::OpenMode								  _fd = QIODevice::WriteOnly | QIODevice::Text);
#pragma endregion
		
#pragma region Preset
		/**
		* Load preset 
		*
		* @param	out_fps				Frames per second
		* @param	out_pixelPerMm		Pixel per mm
		* @param	out_roi				Region of interest
		* @param	out_methodNames		Names of preprocessing methods
		* @param	out_methodParams	Parameters of preprocessing methods
		* @param	out_bgSubMethodName	Name of background-subtraction-method
		* @param	out_bgSubPercentage	Percentage of background-subtraction
		*								input sampling rate.
		* @param	out_algoName		Name of tracking algorithm
		* @param	out_algoParams		Parameters of tracking algorithm
		* @param	_availablAlgos		List of available tracking algorithms
		* @param	_fileName			Filename of the file that stores the settings.
		* @return	True, if loading was successful
		*/
		bool loadPreset(
			double													& out_fps,
			double													& out_pixelPerMm,
			cv::Rect												& out_roi,
			std::vector< std::string >								& out_methodNames, 
			std::vector< std::vector< ParameterWrapper >* >			& out_methodParams,
			QString													& out_bgSubMethodName, 
			double													& out_bgSubPercentage,
			QString													& out_algoName,
			std::vector< ParameterWrapper >							& out_algoParams,
			const map<QString, Dll::ATracker*>						* _availablAlgos,
			const QString											& _fileName = QString("latest_preset"));//Default/Standard file

		/**
		* Save preset 
		*
		* @param	_fps				Frames per second
		* @param	_pixelPerMm			Pixel per mm
		* @param	_roi				Region of interest
		* @param	_methodNames		Names of preprocessing methods
		* @param	_methodParams		Parameters of preprocessing methods
		* @param	_bgSubMethodName	Name of background-subtraction-method
		* @param	_bgSubPercentage	Percentage of background-subtraction
		*								input sampling rate.
		* @param	_algoName			Name of tracking algorithm
		* @param	_algoParams			Parameters of tracking algorithm
		* @param	_fileName			Filename of the file that will store the settings.
		*/
		void savePreset(
			const double											  _fps,
			const double											  _pixelPerMm,
			const cv::Rect											& _roi,
			const std::vector< std::string >						& _methodNames, 
			const std::vector< std::vector< ParameterWrapper >* >	& _methodParams,
			const QString											& _bgSubMethodName, 
			const double											& _bgSubPercentage,
			const QString											& _algoName,
			const std::vector< ParameterWrapper >					& _algoParams,
			const QString											& _fileName = QString("latest_preset"));//Default/Standard file
#pragma endregion
	};
}