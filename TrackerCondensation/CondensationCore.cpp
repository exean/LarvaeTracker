#include "CondensationCore.h"


CondensationCore::CondensationCore(
	Dll::ATracker				*	_tracker,		
	int								_sampleCount,
	int								_segmentCount,
	double							_curvature_distance,
	double							_curvature_spanThresh)
{
	this->mTracker				= _tracker;
	this->mSampleCount			= _sampleCount;
	this->mSegmentCount			= _segmentCount;
	this->mCurvature_distance	= _curvature_distance;
	this->mCurvature_spanThresh	= _curvature_spanThresh;	

	this->mSpineFinder			= new GeneralTrackerFuncs::SpineFinder(
									_curvature_distance, 
									_curvature_spanThresh);
	this->mModelFinder			= new GeneralTrackerFuncs::ModelFinder(
									this->mSpineFinder, 
									_segmentCount);
	this->mModelManager			= new GeneralTrackerFuncs::LarvaModelManager(
									this->mModelFinder);	

	this->mMinimalFitness		= 0.8;

	this->mMinContourArea		= 0;
}


CondensationCore::~CondensationCore(void)
{
	delete this->mModelManager;
	delete this->mModelFinder;	
	delete this->mSpineFinder;
}

void CondensationCore::processImage(
	cv::Mat								& _image, 
	cv::Mat								& _FBimage,
	PreprocessingFuncs::Preprocessing	& _preprocessing,
	vector< LarvaModel* >				* out_models,
	const int							  _timeIndex)
{
	this->mCurrentTimeStepLarvae.clear();
	std::ostringstream sstream; // stream for number-appendation in progress notifications
	//---------Condensation-Part

	//Calculate position energies
	int lastLarvaeCount = this->mLastTimeStepLarvae.size();
	if(lastLarvaeCount > 0)
	{	
		sstream << "Berechne Energien für " << lastLarvaeCount << " Modelle.";
		this->mTracker->sendUpdateToObservers(sstream.str());
		double progressPerModell = 10. / lastLarvaeCount;
		int currentProgressCounter = 0;

		vector<cv::Mat> larvaEnergies;
		for each(LarvaModel* larva in this->mLastTimeStepLarvae)
		{
			larvaEnergies.push_back(
				this->mCondensation.getEnergy(larva,_image,this->mSampleCount));
			this->mTracker->sendCurrentProgressToObservers((int)(5+ ++currentProgressCounter*progressPerModell));
		}

	
		this->mTracker->sendUpdateToObservers("Bestimme Folgemodelle...");
		progressPerModell = 20. / lastLarvaeCount;		
		currentProgressCounter = 0;

		//Estimate larva positions
		int lastTimeStepLarvaeCount = this->mLastTimeStepLarvae.size();
		for(int i = 0; i < lastTimeStepLarvaeCount; i++)
		{
			this->mTracker->sendCurrentProgressToObservers((int)(15+currentProgressCounter++*progressPerModell));
			cv::Mat samplingMat = cv::Mat::zeros(_FBimage.size(), _FBimage.type());
			samplingMat += _FBimage;
			for(int j = 0; j < lastTimeStepLarvaeCount; j++)
			{
				if(i != j)
				{
					samplingMat -= larvaEnergies[j]; 
				}
			}
			//Condensation-progression
			double cumulTotal = 0.0;
			vector<double> cumulProbArray;
			vector<double> sampleWeights;
			vector<LarvaModel> newSamples;		
			LarvaModel* currentModel = this->mLastTimeStepLarvae[i];
			if (this->mCurrentDescriptors.find(currentModel) == this->mCurrentDescriptors.end())
			{
				//Case 1/2: First condensation run
				// --> Only one base
				// predict new bases
				//Inverted spine
				GeneralTrackerFuncs::LarvaModelManager::InvertSpine(currentModel);
				for (int n = this->mSampleCount/2; n < this->mSampleCount; n++)
				{
					this->mCondensation.predict_sample_position(
						currentModel, 
						samplingMat, 
						cumulTotal, 
						newSamples, 
						sampleWeights, 
						cumulProbArray);		
				}
				//Normal spine
				GeneralTrackerFuncs::LarvaModelManager::InvertSpine(currentModel);
				for (int n = this->mSampleCount/2; n < this->mSampleCount; n++)
				{
					this->mCondensation.predict_sample_position(
						currentModel, 
						samplingMat, 
						cumulTotal, 
						newSamples, 
						sampleWeights, 
						cumulProbArray);		
				}
			}
			else
			{						
				//Case 2/2: Consecutive condensation run
				//this->predict_new_bases();
				for (int n = this->mSampleCount/2; n < this->mSampleCount; n++)
				{
					LarvaModel mod = this->mCondensation.pick_base_sample(
						this->mLargest_cumulative_prob[currentModel],
						this->mCurrentDescriptors[currentModel],
						this->mCumul_prob_array[currentModel],
						this->mSampleCount);
					//Inverted spine
					GeneralTrackerFuncs::LarvaModelManager::InvertSpine(currentModel);
					this->mCondensation.predict_sample_position(
						&mod, 
						samplingMat, 
						cumulTotal, 
						newSamples, 
						sampleWeights, 
						cumulProbArray);
					//Normal spine
					GeneralTrackerFuncs::LarvaModelManager::InvertSpine(currentModel);
					this->mCondensation.predict_sample_position(
						&mod, 
						samplingMat, 
						cumulTotal, 
						newSamples, 
						sampleWeights, 
						cumulProbArray);		
				}
			}					
					
			//Compute new model

			//////MEAN MODEL
			//LarvaModel* newModel = this->mCondensation.computeMeanModel(newSamples, sampleWeights);		
			//this->mModelManager->setNext(currentModel, newModel);
			//this->mLargest_cumulative_prob[newModel] = cumulTotal;
			//this->mCumul_prob_array[newModel] = cumulProbArray;
			//this->mCurrentDescriptors[newModel] = newSamples;
			//this->mCurrentTimeStepLarvae.push_back(newModel);
			//////MEAN MODEL - eof

			//BEST FITNESS
			double bestFitness;
			LarvaModel* newModel = this->mCondensation.chooseBestModel(newSamples, sampleWeights, bestFitness);
			if(bestFitness > this->mMinimalFitness)
			{
				this->mModelManager->setNext(currentModel, newModel);
				//Push new data to map
				this->mLargest_cumulative_prob[newModel] = cumulTotal;
				this->mCumul_prob_array[newModel] = cumulProbArray;
				this->mCurrentDescriptors[newModel] = newSamples;
				this->mCurrentTimeStepLarvae.push_back(newModel);
			}
			//BEST FITNESS - EOF

			//Remove old data
			this->mLargest_cumulative_prob.erase(currentModel);
			this->mCumul_prob_array.erase(currentModel);
			this->mCurrentDescriptors.erase(currentModel);

			samplingMat.release();
		}	
	}

	//---------Condensation-Part - EOF

	//---------Determine contours and search for new larvae
	this->mTracker->sendCurrentProgressToObservers("Bestimme neue Konturen...", 40);
	cv::Mat samplingMat = cv::Mat::zeros(_FBimage.size(), _FBimage.type());
	samplingMat += _FBimage;
	//double minContourArea = 0;
	for each(LarvaModel* larva in this->mCurrentTimeStepLarvae)
	{
		GeneralTrackerFuncs::Painter::PaintLarvaSilhouette(
			*larva, samplingMat, Scalar(0), 1.1f);	//slightly bigger to cover errors		
		//samplingMat -= larvaEnergies[j];			
	}
	//minContourArea *= 0.75;
	
	//Find Contours
	vector< vector<Point> > alphaContours = _preprocessing.getContours(samplingMat.clone());		
	int contourCount = alphaContours.size();
	//Progress Information
	this->mTracker->sendCurrentProgressToObservers(60);
	sstream.clear();sstream.str("");
	sstream << contourCount << " Konturen gefunden. Berechne Modelle...";
	this->mTracker->sendUpdateToObservers(sstream.str());
	//vector< GeneralTrackerFuncs::Contour* > contours;
	double progressPerContour = 30./contourCount;
	int progressCounter = 0;
	for (vector< vector<Point> >::iterator contIt = alphaContours.begin(); 
			contIt != alphaContours.end(); 
			contIt++, progressCounter++) 
	{
		if(cv::contourArea(*contIt) > this->mMinContourArea)
		{
			//Potential detection	
			LarvaModel* m = this->mModelFinder->getModelFromContour(
				&GeneralTrackerFuncs::Contour(*contIt), 
				_image);

			if(m)
			{
				m->setTimeIndex( _timeIndex );		
				this->mCurrentTimeStepLarvae.push_back(	m );
				out_models->push_back( m );
			}
		}
		this->mTracker->sendCurrentProgressToObservers((int)(60+progressPerContour*progressCounter));
	}
	sstream.clear();sstream.str("");
	sstream << "Im aktuellen Zeitschritt wurden " << this->mCurrentTimeStepLarvae.size() << " Modelle erfasst.";
	this->mTracker->sendUpdateToObservers(sstream.str());

	if(this->mMinContourArea == 0)
	{
		this->mTracker->sendCurrentProgressToObservers(91);
		this->mTracker->sendUpdateToObservers("Berechne Grenze für minimale Kontur-Fläche...");
		vector< double > contourAreas;
		for(vector<LarvaModel*>::const_iterator it = this->mCurrentTimeStepLarvae.begin();
			it != this->mCurrentTimeStepLarvae.end(); it++)
		{
			vector< Point > contour = (*it)->getContour();
			if(contour.size() > 0)
			{
				contourAreas.push_back(cv::contourArea(contour));
			}
		}
		//Median
		std::sort(contourAreas.begin(), contourAreas.end());	
		int size = contourAreas.size();
		this->mMinContourArea = contourAreas[size / 2];
		if (size % 2 == 0)
		{
			this->mMinContourArea = (contourAreas[size / 2 - 1] + this->mMinContourArea) / 2;
		}
		this->mMinContourArea *= 0.75;
		this->mTracker->sendCurrentProgressToObservers(95);
		sstream.clear();sstream.str("");
		sstream << "Grenze für minimale Kontur-Fläche: " << this->mMinContourArea;
		this->mTracker->sendUpdateToObservers(sstream.str());
	}

	this->mLastTimeStepLarvae = this->mCurrentTimeStepLarvae;

	//-----------------------------------------------------Progress Information
	this->mTracker->sendUpdateToObservers("Erkennung abgeschlossen.");
	
	this->mTracker->sendCurrentProgressToObservers(100);	
	//-----------------------------------------------------Progress Information - DONE
}