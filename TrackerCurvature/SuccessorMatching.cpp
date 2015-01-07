#include "SuccessorMatching.h"

namespace CurvatureTrackerFuncs
{
	SuccessorMatching::SuccessorMatching(void)
	{
		this->mMaxContourDistance = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
		this->mContourDistanceLimitFactor = 1.25;
		this->mSpeedSamplingCount = 4;
	}


	SuccessorMatching::~SuccessorMatching(void)
	{
	}

	void SuccessorMatching::assignContoursToModels(		
		const	vector< LarvaModel * >::const_iterator	&	_modelsBegin,
		const	vector< LarvaModel * >::const_iterator	&	_modelsEnd,
		const	vector< GeneralTrackerFuncs::Contour* >	&	_contours,
				vector< LarvaModel* >					&	mod_lastDetectedLarvaModels,
				vector< GeneralTrackerFuncs::Contour* >	&	mod_currentTargetContours)
	{
		vector<double> distances, spineLengths;
		for (vector< LarvaModel* >::const_iterator it = _modelsBegin; 
			it != _modelsEnd; 
			it++)
		{
			LarvaModel* current = (*it);
			Vector2D estimatedProgression = 
				this->estimateProgression(current);
			GeneralTrackerFuncs::Contour *currentContour = 
				GeneralTrackerFuncs::Toolbox::GetNearestContour(current->getCenter()+estimatedProgression, _contours);
			double currentContourDistance = 
				((current->getCenter()+estimatedProgression) - *currentContour->getCenterOfMass()).length();
			//Also compare curvature-region-distances
			if(current->isResolved())
			{
				for each(Point p in *currentContour->getHighCurvaturePoints())
				{
					currentContourDistance =
						std::min(
							currentContourDistance,
							std::min(
								((current->getHead()+estimatedProgression)-p).length(),
								((current->getTail()+estimatedProgression)-p).length()));
				}
			}
			if(this->mMaxContourDistance > currentContourDistance)
			{
				mod_currentTargetContours.push_back(currentContour);//this contour may appear multiple times in out_currentTargetContours 
				mod_lastDetectedLarvaModels.push_back(current);//this model may appear multiple times in out_lastDetectedLarvaModels (->union of models)
				distances.push_back(currentContourDistance);
				if(current->isResolved())
				{
					spineLengths.push_back(GeneralTrackerFuncs::Toolbox::GetLength(current->getSpine()));
				}
			};
			//else : No matching contour in range
		}

		//Update mMaxContourDistance
		this->updateMaxContourDistance(distances, spineLengths);
	}

	void SuccessorMatching::assignModelsToContours(
		const	vector< LarvaModel * >::const_iterator	& _modelsBegin,
		const	vector< LarvaModel * >::const_iterator	& _modelsEnd,
		const	vector< GeneralTrackerFuncs::Contour* >	& _contours,
				vector< LarvaModel* >					& mod_lastDetectedLarvaModels,
				vector< GeneralTrackerFuncs::Contour* >	& mod_currentTargetContours)
	{
		vector<double> distances, spineLengths;
		vector< GeneralTrackerFuncs::Contour* >::const_iterator contoursEnd = _contours.end();	
		vector< cv::Point > estimatedProgression;
		for (vector< LarvaModel* >::const_iterator modelIt = _modelsBegin; 
			modelIt != _modelsEnd; 
			modelIt++)
		{			
			estimatedProgression.push_back(cv::Point(0,0));//his->estimateProgression(*modelIt));
		}
		for (vector< GeneralTrackerFuncs::Contour* >::const_iterator contourIt = _contours.begin(); 
			contourIt != contoursEnd; 
			contourIt++)
		{
			LarvaModel *nearest = NULL;
			double leastDistance = std::numeric_limits<double>::max();
			Vector2D *contourMoment = (*contourIt)->getCenterOfMass();

			vector< cv::Point >::const_iterator estimatedProgressionIt
				= estimatedProgression.begin();
			for (vector< LarvaModel* >::const_iterator modelIt = _modelsBegin; 
				modelIt != _modelsEnd; 
				modelIt++, estimatedProgressionIt++)
			{				
				Vector2D larvaMoment = (*modelIt)->getCenter();
				double distance = (larvaMoment-(*contourMoment+*estimatedProgressionIt)).length();		
				
				//Also compare curvature-region-distances
				if((*modelIt)->isResolved())
				{
					for each(Point p in *(*contourIt)->getHighCurvaturePoints())
					{
						distance =
							std::min(
								distance,
								std::min(
									(((*modelIt)->getHead()+*estimatedProgressionIt) - p).length(),
									(((*modelIt)->getTail()+*estimatedProgressionIt) - p).length()));
					}
				}
				
				if(distance <= leastDistance)
				{
					nearest = *modelIt;
					leastDistance = distance;
				}

				//Take all in range
				if(this->mMaxContourDistance > distance)
				{
					//check that pair isnt already specified
					bool pairAlreadyExists = false;
					for(unsigned int i = 0; i < mod_currentTargetContours.size(); i++)
						if(mod_currentTargetContours[i] == *contourIt 
							&& mod_lastDetectedLarvaModels[i] == *modelIt)
						{
							pairAlreadyExists = true;
							break;
						}

					if(!pairAlreadyExists)
					{
						mod_currentTargetContours.push_back(*contourIt);
						mod_lastDetectedLarvaModels.push_back(*modelIt);
					}
				}
			}

			if(this->mMaxContourDistance > leastDistance)
			{
				//check that pair isnt already specified
				bool pairAlreadyExists = false;
				for(unsigned int i = 0; i < mod_currentTargetContours.size(); i++)
					if(mod_currentTargetContours[i] == *contourIt 
						&& mod_lastDetectedLarvaModels[i] == nearest)
					{
						pairAlreadyExists = true;
						break;
					}

				if(!pairAlreadyExists)
				{
					mod_currentTargetContours.push_back(*contourIt);//this contour may appear multiple times in out_currentTargetContours (->split of contour)
					mod_lastDetectedLarvaModels.push_back(nearest);//this model may appear multiple times in out_lastDetectedLarvaModels
					distances.push_back(leastDistance);
					spineLengths.push_back(GeneralTrackerFuncs::Toolbox::GetLength(nearest->getSpine()));
				}
			}
			else
			{//New detection
				mod_currentTargetContours.push_back(*contourIt);//this contour will only appear once in out_currentTargetContours
				mod_lastDetectedLarvaModels.push_back(NULL);
			}
		}

		//Update mMaxContourDistance
		this->updateMaxContourDistance(distances, spineLengths);
	}

	//Estimates progression from progression of predecessors
	Vector2D SuccessorMatching::estimateProgression(
		const LarvaModel				* _model)
	{
		Vector2D progression(0,0);
		LarvaModel * pre = _model->getPrev();
		if(pre)
		{
			vector< double > progressionDistance;
			for(unsigned int t = this->mSpeedSamplingCount;
				t > 0 && pre;
				t--)
			{
				progressionDistance.push_back(
					(pre->getNext()->getCenter() - pre->getCenter()).length());			
			}

			if(!progressionDistance.empty())
			{
				double firstProgression = progressionDistance[0];

				//Median progression speed
				size_t size = progressionDistance.size();
				std::sort(progressionDistance.begin(), progressionDistance.end());	
				double median = progressionDistance[size / 2];
				if (size  % 2 == 0)
				{
					median = (progressionDistance[size / 2 - 1] + median) / 2;
				}

				//estimate progression as progress in
				//same direction as direct predecessor
				//of measured distances
				Vector2D progression = _model->getCenter()-_model->getPrev()->getCenter();
				if(firstProgression != 0)
				{
					progression.setX(GeneralTrackerFuncs::Toolbox::Round((progression.getX() / firstProgression) * median));
					progression.setY(GeneralTrackerFuncs::Toolbox::Round((progression.getY() / firstProgression) * median));
				}
			}
		}

		return progression;
	}

	//Set double median times mContourDistanceLimitFactor as new mMaxContourDistance
	void SuccessorMatching::updateMaxContourDistance(
		vector<double>					& mod_distances,
		vector<double>					& mod_spineLengths)
	{		
		if(!mod_spineLengths.empty())
		{
			size_t size = mod_spineLengths.size();
			std::sort(mod_spineLengths.begin(), mod_spineLengths.end());	
			// Median
			double median = mod_spineLengths[size / 2];
			if (size  % 2 == 0)
			{
				median = (mod_spineLengths[size / 2 - 1] + median) / 2;
			}
			this->mMaxContourDistance = median / 2;
		}
		if(!mod_distances.empty())
		{
			 size_t size = mod_distances.size();
			 std::sort(mod_distances.begin(), mod_distances.end());		
			 ////Max
			 //double max = mod_distances[size - 1];
			 //if ( max > 0 )
			 //{
				//this->mMaxContourDistance = this->mContourDistanceLimitFactor * max;
			 //}

			 //alt: Median
			 double median = mod_distances[size / 2];
			 if (size  % 2 == 0)
			 {
				median = (mod_distances[size / 2 - 1] + median) / 2;
			 }
			 if ( median > 0 )
			 {
				this->mMaxContourDistance += this->mContourDistanceLimitFactor * median;
			 }
		}
	}
}