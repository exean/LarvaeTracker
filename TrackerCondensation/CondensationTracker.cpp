/*
 *  CondensationTracker.cpp
 *  OpenCVTest
 *
 *  Created by Tobias on 6/4/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CondensationTracker.h"

CondensationTracker::CondensationTracker()
{
	//default parameters
	this->mSampleCount			=	1000;
	this->mSegmentCount			=	6;
	this->mCurvature_HeadLength	=	10; 
	this->mCurvature_Width		=	15; 
	//default parameters - EOF

	//Plugin information
	this->mName			= "Condensation-based";
	this->mAuthor		= "Tobias Ries (tobias@ries.asia)";
	this->mDescription	= "Tracking of larvae using the condensation algorithm.";
	this->mVersion		= 1;
	//Plugin information - EOF
};

vector< ParameterWrapper > CondensationTracker::getSettings ()
{ 
	//Setup parameter-list
	vector< ParameterWrapper > res; 
	res.push_back(
		ParameterWrapper(
			this->mSampleCount,
			"#Samples", 
			"Anzahl der Samples, die in einem Condensation-Schritt ermittelt werden."));
	res.push_back(
		ParameterWrapper(
			this->mSegmentCount,
			"#Segments", 
			"Anzahl der Segmente des Spines eines LarvenModells."));
	res.push_back(
		ParameterWrapper(
			this->mCurvature_HeadLength,
			"Kopf-/Schwanzgröße", 
			"Durchschnittliche Länge des direkten Kopf-/Schwanzbereichs in Pixel."));
	res.push_back(
		ParameterWrapper(
			this->mCurvature_Width,
			"Larvenbreite", 
			"Durchschnittliche Breite der Larven in Pixel."));
	return res; 
};	

void CondensationTracker::setSettings (
	vector< ParameterWrapper > _settings)
{ 
	//Extract settings from parameter-list
	for each(ParameterWrapper param in _settings)
	{
		if(!param.mName.compare("#Segments"))
		{
			this->mSegmentCount = param.mInt; 
		}
		else if(!param.mName.compare("#Samples"))
		{
			this->mSampleCount = param.mInt; 
		}
		else if(!param.mName.compare("Kopf-/Schwanzgröße"))
		{
			this->mCurvature_HeadLength = param.mDouble; 
		}
		else if(!param.mName.compare("Larvenbreite"))
		{
			this->mCurvature_Width = param.mDouble; 
		}
	} 
};

void CondensationTracker::stop()
{
	//Set flag for stopping thread at next possible time
	this->mStop = true;
};

void CondensationTracker::perform(
	PreprocessingFuncs::ImageLoader _imgLoader)
{
	//"cancel"-flag
	this->mStop	= false;

	//------------------------------------------------------------------SETUP
	//Setup progress information for GUI
	double perFileProgress = 100./_imgLoader.getFileCount();
	this->sendCompleteProgressToObservers(0);
	this->sendCurrentProgressToObservers(0);	

	//Condensation Core
	CondensationCore core(
		this,//ATracker* for sending progress information to Observer
		this->mSampleCount,
		this->mSegmentCount,
		this->mCurvature_HeadLength,
		this->mCurvature_Width);

	//Image preprocessor
	PreprocessingFuncs::Preprocessing preprocessing;
	
	//Computed models (->paths)
	vector< LarvaModel* >* models = new vector< LarvaModel* >;
	
	//Duration measurement
	double duration = static_cast<double>(cv::getTickCount());


	//------------------------------------------------------------------SETUP - DONE

	//------------------------------------------------------------------COMPUTATION
	//Compute on remaining images	
	int time = 0;
	Mat image, processedInput, outputImage;
	while(!this->mStop 
		&& _imgLoader.getNext(image, processedInput))//Load image
	{
		//Send progress-Information
		std::string filename = _imgLoader.getCurrentFileName();
		this->sendCurrentProgressToObservers(
			"Bearbeite: \"..." + filename.substr(filename.find_last_of('/')) + "\"", 0);
		outputImage = cv::Mat(processedInput.size(), processedInput.type());
		cvtColor(processedInput,outputImage ,CV_GRAY2RGB);
		this->sendUpdateToObservers(outputImage);
		this->sendCurrentProgressToObservers(5);	

		//Process image
		core.processImage(
			image, 
			processedInput,
			preprocessing,
			models,
			time++);
		
		image.release();
		processedInput.release();

		//Progress
		this->sendCompleteProgressToObservers(
			(int)(time*perFileProgress));
	}
	//------------------------------------------------------------------COMPUTATION - DONE

	//Unite temporally successive models of 
	//minimal distance
	this->postprocess(models);

	//All done, send final information to Observers
	this->finish(
		duration,
		models);
}

void CondensationTracker::postprocess(vector< LarvaModel* >	* mod_allLarvae)
{
	//unify head-tail-alignment within all timesteps of each model
	for each(LarvaModel* larva in *mod_allLarvae)
	{
		LarvaModel* first = NULL;
		LarvaModel* second = larva;
		while(second->hasNext())
		{
			first = second;
			second = second->getNext();

			if((first->getHead()-second->getHead()).length() > 
				(first->getTail()-second->getHead()).length())
			{
				//Invert match
				GeneralTrackerFuncs::LarvaModelManager::InvertSpine(second);
			}
		}
	}


	//unite models over time
	for(unsigned int i = 0; i < mod_allLarvae->size(); i++)
	{
		while(GeneralTrackerFuncs::LarvaModelManager::UniteOverTime(mod_allLarvae->at(i), mod_allLarvae, 1));
	}	

	//correct head/tail assignment
	for each(LarvaModel* larva in *mod_allLarvae)
	{
		GeneralTrackerFuncs::LarvaModelManager::CorrectHeadAssignmentByDirection(larva);
	}
}


//Does the last thing left to do: Send final information
void CondensationTracker::finish(
	double duration,
	vector<LarvaModel*> * models)
{
	this->sendCurrentProgressToObservers(100);
	this->sendCompleteProgressToObservers(100);
	duration = static_cast<double>(getTickCount())-duration;
	duration /= getTickFrequency(); // the elapsed time in ms	
	stringstream oss;
	oss << "Bearbeitung abgeschlossen (" << duration << "s).";	
	this->sendUpdateToObservers(oss.str());
	this->sendResultToObservers(models);
}