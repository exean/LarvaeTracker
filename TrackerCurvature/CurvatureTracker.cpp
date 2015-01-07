/*
 *  CurvatureTracker.cpp
 *  OpenCVTest
 *
 *  Created by Tobias on 5/5/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CurvatureTracker.h"
namespace CurvatureTrackerFuncs
{
	CurvatureTracker::CurvatureTracker(void)
	{
		//Dafault parameters
		this->mSegmentCount			= 6;		
		this->mCurvature_HeadLength	= 10; 
		this->mCurvature_Width		= 15;
		//Dafault parameters - EOF

		//Plugin information
		this->mName = "KonZert";
		this->mAuthor = "Tobias Ries (tobias@ries.asia)";
		this->mDescription = "Tracking of larvae based on regions of high curvature.";
		this->mVersion = 1;
		//Plugin information - EOF
	};

	vector< ParameterWrapper > CurvatureTracker::getSettings()
	{ 
		vector< ParameterWrapper > res; 
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
	}	

	void CurvatureTracker::setSettings (
		vector< ParameterWrapper > _settings)
	{ 
		for each(ParameterWrapper param in _settings)
		{
			if(!param.mName.compare("#Segments"))
			{
				this->mSegmentCount = param.mInt; 
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

	void CurvatureTracker::stop()
	{
		this->mStop = true;
	};

	void CurvatureTracker::perform(
		PreprocessingFuncs::ImageLoader _imgLoader)
	{
		//----------------------------------------------------------------Setup	
		//Duration measurement
		double duration = static_cast<double>(cv::getTickCount());	

		this->sendCompleteProgressToObservers(0);

		float progressPerImage			= 100.f/_imgLoader.getFileCount();
		float currentCompleteProgress	= 0.f;
		this->mStop						= false;
	
		//Computed models (->paths)
		vector< LarvaModel* >* models	= new vector< LarvaModel* >();
		vector< int > invertModelCounter;
	
		KonZertCore core(
			this,
			this->mSegmentCount,
			this->mCurvature_HeadLength, 
			this->mCurvature_Width);
		//----------------------------------------------------------------Setup - DONE
	

		//----------------------------------------------------------------Compute on images
		int timeIndex = 0;
		Mat image, processedInput, outputImage;
		while(!this->mStop && _imgLoader.getNext(image, processedInput))//Load image
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
				models,
				timeIndex++);

			image.release();
			processedInput.release();
			this->sendCompleteProgressToObservers((int)(currentCompleteProgress += progressPerImage));
		}
		//----------------------------------------------------------------Compute on images - DONE

		//Remove completely (in every timestep) unresolved models (=miss-detections)
		for(unsigned int i = 0; i < models->size();)
		{
			LarvaModel *baseModel	= models->at(i);
			for(LarvaModel *m			= baseModel;
				; 
				m = m->getNext())
			{
				if(m->isResolved())
				{
					if(m != baseModel)//Delete unresolved models at beginning
					{
						m->getPrev()->setNext(0);
						baseModel->deleteChildren();
						delete baseModel;
						m->rmPointerToPrev();
						models->at(i) = m;
					}
					LarvaModel* last = m->getLast();
					for(;//Delete unresolved models at end
						!last->isResolved();
						last = m->getLast())
					{
						last->getPrev()->deleteChildren();
					}
					i++;
					break;
				}
				else if(!m->hasNext())
				{
					//completely unresolved
					baseModel->deleteChildren();
					delete baseModel;
					models->erase(
						models->begin() + i);
					break;
				}	
			};
		}

		//Close temporal gaps
		for(unsigned int i = 0; i < models->size(); i++)
		{
			while(GeneralTrackerFuncs::LarvaModelManager::UniteOverTime(models->at(i), models, 1));			
		}		

		//Correct head-tail-assignments
		for each(LarvaModel* larva in *models)
		{
			GeneralTrackerFuncs::LarvaModelManager::CorrectHeadAssignmentByDirection(larva);
		}			

		//All done, send final information to Observers
		this->finish(
			duration, 
			models);
	}

	//Does the last thing left to do: Send final information
	void CurvatureTracker::finish(
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
}