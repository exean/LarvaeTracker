#include "SettingsIO.h"

namespace InOut
{
	SettingsIO::SettingsIO(void)
	{
	}


	SettingsIO::~SettingsIO(void)
	{
	}

#pragma region Settings
	bool SettingsIO::LoadSettings(
		QString							&out_mailSenderAddress,
		QString							&out_mailDefaultRecipient,
		QString							&out_mailServer,
		QString							&out_mailUsername,
		QString							&out_mailPassword,
		int								&out_mailPort,
		QString							&out_csvCol,
		QString							&out_csvRow,
		const QString					&_filename,
		const QIODevice::OpenMode		 _fd)
	{
		QFile file(_filename); 
		if(file.open(_fd))
		{
			QTextStream in(&file);
			QString line = in.readLine();
			if (!line.isNull()) 
			{
				QStringList parts = line.split("_|_");
				
				int i = 0;
				if(parts.size() >= 6)
				{
					out_mailSenderAddress		= parts[i++];
					out_mailDefaultRecipient	= parts[i++];
					out_mailServer				= parts[i++];
					out_mailUsername			= parts[i++];
					out_mailPassword			= QString::fromLocal8Bit(QByteArray::fromBase64(parts[i++].toAscii()));
					out_mailPort				= parts[i++].toInt();
				}
				if(parts.size() >= 8)
				{
					out_csvCol					= parts[i++];
					out_csvRow					= parts[i++];
				}
			}
			return true;
		}
		return false;
	}

	void SettingsIO::SaveSettings(
		const QString					&_mailSenderAddress,
		const QString					&_mailDefaultRecipient,
		const QString					&_mailServer,
		const QString					&_mailUsername,
		const QString					&_mailPassword,
		const int						&_mailPort,
		const QString					&_csvCol,
		const QString					&_csvRow,
		const QString					&_filename,
		const QIODevice::OpenMode		 _fd)
	{
		QFile file(_filename);
		if(file.open(_fd))
		{		
			QTextStream out(&file);

			out << _mailSenderAddress
				<< "_|_" 
				<< _mailDefaultRecipient 
				<< "_|_" 
				<< _mailServer
				<< "_|_" 
				<< _mailUsername
				<< "_|_" 
				<< _mailPassword.toLocal8Bit().toBase64()//avoid plainly printing the pw... real encryption would be better
				<< "_|_" 
				<< _mailPort
				<< "_|_" 
				<< _csvCol
				<< "_|_" 
				<< _csvRow;

			file.close();
		}
	}
#pragma endregion

#pragma region Preprocessing
	bool SettingsIO::loadPreprocessing(
		std::vector< std::string >						& out_methodNames, 
		std::vector< std::vector< ParameterWrapper >* >	& out_methodParams,
		const QString									& _filename,
		const QIODevice::OpenMode						  _fd)
	{
		QFile file(_filename); 
		if(file.open(_fd))
		{
			QTextStream in(&file);
			QString line = in.readLine();
			while (!line.isNull()) 
			{
				QStringList parts = line.split('_');

				vector< ParameterWrapper >* params = 
					new vector< ParameterWrapper >(
						PreprocessingFuncs::PreprocessingMethods::GetAvailablePreprocessingMethods()->at(
							PreprocessingFuncs::PreprocessingMethods::StringToEMethods(parts[0].toStdString())));

				int i = 1;
				for(vector< ParameterWrapper >::iterator it = params->begin();
					it != params->end();
					it++, i++)
				{
					Gui::GUIParameterLayout::ParameterFromString(parts[i], &*it);
				}

				out_methodNames.push_back(parts[0].toStdString());
				out_methodParams.push_back(params);

				line = in.readLine();
			}
			return true;
		}
		return false;
	}
	

	void SettingsIO::savePreprocessing(
		const std::vector< std::string >						& _methodNames, 
		const std::vector< std::vector< ParameterWrapper >* >	& _methodParams,
		const QString											& _filename,
		const QIODevice::OpenMode								  _fd)
	{
		QFile file(_filename);
		if(file.open(_fd))
		{		
			QTextStream out(&file);
			std::vector< std::string >::const_iterator nameIt = 
				_methodNames.begin();
			std::vector< std::vector< ParameterWrapper >* >::const_iterator paramsIt = 
				_methodParams.begin();

			for(;
				nameIt != _methodNames.end();
				nameIt++, paramsIt++)
			{
				out << nameIt->c_str() << "_";

				for(std::vector< ParameterWrapper >::iterator paramIt = (*paramsIt)->begin();
					paramIt != (*paramsIt)->end();
					paramIt++)
				{
					out << (*paramIt).toString().c_str() << "_";
				}

				out << "\n";
			}

			file.close();
		}
	}
#pragma endregion

#pragma region BGSubtraction
	bool SettingsIO::loadBGSubtraction(
		QString													& out_methodName, 
		double													& out_percentage,
		const QString											& _filename,
		const QIODevice::OpenMode								  _fd)
	{
		QFile file(_filename); 
		if(file.open(_fd))
		{
			QTextStream in(&file);
			this->parseBGSubtraction(out_methodName, out_percentage, in.readLine());
			return true;
		}
		return false;
	}

	void SettingsIO::saveBGSubtraction(
		const QString											& _methodName, 
		const double											& _percentage,
		const QString											& _filename,
		const QIODevice::OpenMode								  _fd)
	{
		QFile file(_filename);
		if(file.open(_fd))
		{	
			QTextStream out(&file);
			out << _methodName << "_" << QString::number(_percentage);
			file.close();
		}
	}

	void SettingsIO::parseBGSubtraction(
		QString													& out_methodName, 
		double													& out_percentage,
		const QString											& _bgSubtractionString)
	{
		QStringList parts = _bgSubtractionString.split('_');
		out_methodName = parts[0];
		out_percentage = parts[1].toDouble();
	}
#pragma endregion

#pragma region input
		bool SettingsIO::loadFPSPixPerMM(
			double												& out_fps,
			double												& out_pixelPerMm,
			const QString										& _filename,//Default/Standard file
			const QIODevice::OpenMode							  _fd)
		{
			QFile file(_filename); 
			if(file.open(_fd))
			{
				QTextStream in(&file);
				QString line = in.readLine();
				if (!line.isNull()) 
				{
					this->parseFPSPixPerMM(out_fps, out_pixelPerMm, line);
				}
				return true;
			}
			return false;
		}

		bool SettingsIO::loadROI(
			cv::Rect											& out_roi,
			const QString										& _filename,//Default/Standard file
			const QIODevice::OpenMode							  _fd)
		{
			QFile file(_filename); 
			if(file.open(_fd))
			{
				QTextStream in(&file);
				QString line = in.readLine();
				if (!line.isNull()) 
				{
					this->parseROI(out_roi, line);
				}
				return true;
			}
			return false;
		}

		void SettingsIO::parseFPSPixPerMM(
			double												& out_fps,
			double												& out_pixelPerMm,
			const QString										& _parseString)
		{
			QStringList parts	= _parseString.split("_");
			int i				= 0;
			out_fps				= parts[i++].toDouble();
			out_pixelPerMm		= parts[i].toDouble();
		}

		void SettingsIO::parseROI(
			cv::Rect											& out_roi,
			const QString										& _roiString)
		{
			QStringList parts = _roiString.split("_");
			int i = 0;
			out_roi.x		= parts[i++].toInt();
			out_roi.y		= parts[i++].toInt();
			out_roi.width	= parts[i++].toInt();
			out_roi.height	= parts[i++].toInt();
		}

		void SettingsIO::saveFPSPixPerMM(
			const double										& _fps, 
			const double										& _pixelPerMm,
			const QString										& _filename,
			const QIODevice::OpenMode							  _fd)
		{
			QFile file(_filename);
			if(file.open(_fd))
			{		
				QTextStream out(&file);

				out << _fps
					<< "_" 
					<< _pixelPerMm;

				file.close();
			}
		}

		void SettingsIO::saveROI(
			const cv::Rect										& _roi,
			const QString										& _filename,//Default/Standard file
			const QIODevice::OpenMode							  _fd)
		{
			QFile file(_filename);
			if(file.open(_fd))
			{		
				QTextStream out(&file);

				out << _roi.x
					<< "_" 
					<< _roi.y 
					<< "_" 
					<< _roi.width
					<< "_" 
					<< _roi.height;

				file.close();
			}
		}
#pragma endregion

#pragma region Algorithm
	bool SettingsIO::loadAlgo(
		QString													& out_algoName,
		std::vector< ParameterWrapper >							& out_algoParams,
		const map<QString, Dll::ATracker*>						* _availablAlgos,
		const QString											& _filename,
		const QIODevice::OpenMode								  _fd)
	{
		QFile file(_filename); 		
		if(file.open(_fd))
		{
			QTextStream in(&file);
			this->parseAlgo(
				out_algoName, 
				out_algoParams, 
				_availablAlgos, 
				in.readLine());
			return true;
		}
		return false;
	}

	void SettingsIO::saveAlgo(
		const QString											& _algoName,
		const std::vector< ParameterWrapper >					& _algoParams,
		const QString											& _filename,
		const QIODevice::OpenMode								  _fd)
	{
		QFile file(_filename);
		if(file.open(_fd))
		{	
			QTextStream out(&file);
			out << _algoName << "_|_";

			for(std::vector< ParameterWrapper >::const_iterator paramIt = 
					_algoParams.begin();
				paramIt != _algoParams.end();
				paramIt++)
			{
				out << Gui::GUIParameterLayout::ParameterToString(*paramIt) << "_|_";
			}
			file.close();
		}
	}

	void SettingsIO::parseAlgo(
		QString													& out_algoName,
		std::vector< ParameterWrapper >							& out_algoParams,
		const map<QString, Dll::ATracker*>						* _availablAlgos,
		const QString											& _algoString)
	{
		QStringList parts = _algoString.split("_|_");
		out_algoName = parts[0];
		map<QString, Dll::ATracker*>::const_iterator algIt = 
			_availablAlgos->find(out_algoName); 
		if(algIt != _availablAlgos->end())
		{
			out_algoParams = algIt->second->getSettings();
			int i = 1;
			for(vector< ParameterWrapper >::iterator it = out_algoParams.begin();
				it != out_algoParams.end();
				it++, i++)
			{
				Gui::GUIParameterLayout::ParameterFromString(parts[i], &*it);
			}
		}
	}
#pragma endregion

#pragma region Preset
	void SettingsIO::savePreset(
		const double											  _fps,
		const double											  _pixelPerMm,
		const cv::Rect											& _roi,
		const std::vector< std::string >						& _methodNames, 
		const std::vector< std::vector< ParameterWrapper >* >	& _methodParams,
		const QString											& _bgSubMethodName, 
		const double											& _bgSubPercentage,
		const QString											& _algoName,
		const std::vector< ParameterWrapper >					& _algoParams,
		const QString											& _filename)
	{
		this->saveAlgo(_algoName, _algoParams, _filename);
		this->saveFPSPixPerMM(_fps, _pixelPerMm, _filename, QIODevice::Append | QIODevice::Text); 
		this->saveROI(_roi, _filename, QIODevice::Append | QIODevice::Text); 
		this->saveBGSubtraction(_bgSubMethodName, _bgSubPercentage, _filename, QIODevice::Append | QIODevice::Text);
		this->savePreprocessing(_methodNames, _methodParams, _filename, QIODevice::Append | QIODevice::Text);
	}

	bool SettingsIO::loadPreset(
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
		const QString											& _filename)
	{
		QFile file(_filename); 
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			QString line;
			//First line = algo
			this->parseAlgo(out_algoName, out_algoParams, _availablAlgos, in.readLine());
			//Second line = FPS and pix/mm
			this->parseFPSPixPerMM(out_fps, out_pixelPerMm, in.readLine());
			//third line = ROI
			this->parseROI(out_roi, in.readLine());
			//Fourth line = bgsub
			this->parseBGSubtraction(out_bgSubMethodName, out_bgSubPercentage, in.readLine());
			//Remaining lines = preprocessing
			while (!(line = in.readLine()).isNull()) 
			{
				QStringList parts = line.split('_');
				vector< ParameterWrapper >* params = 
					new vector< ParameterWrapper >(
						PreprocessingFuncs::PreprocessingMethods::GetAvailablePreprocessingMethods()->at(
							PreprocessingFuncs::PreprocessingMethods::StringToEMethods(parts[0].toStdString())));

				int i = 1;
				for(vector< ParameterWrapper >::iterator it = params->begin();
					it != params->end();
					it++, i++)
				{
					Gui::GUIParameterLayout::ParameterFromString(parts[i], &*it);
				}

				out_methodNames.push_back(parts[0].toStdString());
				out_methodParams.push_back(params);
			}
			return true;
		}
		return false;
	}
#pragma endregion
}