#include "KonZertCore.h"

namespace CurvatureTrackerFuncs
{
	KonZertCore::KonZertCore(
		Dll::ATracker					*	_tracker,
		int									_segmentCount,
		double								_curvature_distance, 
		double								_curvature_spanThresh)
	{
		this->mTracker					= _tracker;
		this->mSegmentCount				= _segmentCount;
		this->mCurvature_distance		= _curvature_distance; 
		this->mCurvature_spanThresh		= _curvature_spanThresh;	
		this->mPreprocessing			=  new PreprocessingFuncs::Preprocessing();
	
		this->mSpineFinder				= new GeneralTrackerFuncs::SpineFinder(
											_curvature_distance, 
											_curvature_spanThresh);
		this->mModelFinder				= new GeneralTrackerFuncs::ModelFinder(
											this->mSpineFinder, 
											_segmentCount);
		this->mModelManager				= new GeneralTrackerFuncs::LarvaModelManager(
											this->mModelFinder);		
	}


	KonZertCore::~KonZertCore(void)
	{
		delete this->mPreprocessing;
		delete this->mModelManager;
		delete this->mModelFinder;	
		delete this->mSpineFinder;
	}

	void KonZertCore::processImage(
		cv::Mat										&	_image,
		cv::Mat										&	_FBimage,
		vector< LarvaModel* >						*	_models,
		const int										_timeIndex)
	{
		this->mCurrentTimeStepLarvae.clear();
		std::ostringstream sstream; // stream for number-appendation in progress notifications

		//Find Contours
		vector< vector<Point> > alphaContours = 
			this->mPreprocessing->getContours(_FBimage.clone());	
		vector< GeneralTrackerFuncs::Contour* > contours;
		for (vector< vector<Point> >::iterator contIt = alphaContours.begin(); 
				contIt != alphaContours.end(); 
				contIt++) 
		{
			GeneralTrackerFuncs::Contour * cont = new GeneralTrackerFuncs::Contour(*contIt);

			vector< vector< Point > > highCurvature =
				this->mSpineFinder->getHighCurvatureRegions(&(*contIt));
			for each(vector< Point > curvatureRegion in highCurvature)
			{
				cont->addHighCurvaturePoint(curvatureRegion[curvatureRegion.size()/2]);
			}

			contours.push_back(cont);
		}

		//Send progress-Information
		sstream.clear();sstream.str("");
		sstream << contours.size() << " Konturen gefunden. Berechne Modelle...";
		this->mTracker->sendCurrentProgressToObservers(sstream.str(), 5);

		//Assign new found contours to previously known models (via minimal distance)
		vector< GeneralTrackerFuncs::Contour* > currentTargetContours;
		vector< LarvaModel* >::const_iterator lastTimeStepLarvaeBegin	= this->mLastTimeStepLarvae.begin();
		vector< LarvaModel* >::const_iterator lastTimeStepLarvaeEnd		= this->mLastTimeStepLarvae.end();
		vector< LarvaModel* > currentAssignmentTargets;		
		this->mMatching.assignContoursToModels(
			lastTimeStepLarvaeBegin,
			lastTimeStepLarvaeEnd,
			contours,
			currentAssignmentTargets,
			currentTargetContours);
		this->mTracker->sendCurrentProgressToObservers(10);
		//...and other way round:
		//Assign previously known models to new found contours (via minimal distance)
		this->mMatching.assignModelsToContours(
			lastTimeStepLarvaeBegin,
			lastTimeStepLarvaeEnd,
			contours,
			currentAssignmentTargets,
			currentTargetContours);

		this->mTracker->sendCurrentProgressToObservers(
			"Neue Larven werden gesucht...", 15);

		//Check for new Larvae (->no link to previous timestep)
		this->checkForNewContours(
			currentAssignmentTargets, 
			currentTargetContours, 
			_image, 
			_timeIndex, 
			_models);

		//Send progress-Information
		this->mTracker->sendCurrentProgressToObservers(
			"Konflikte werden untersucht...", 20);

		//Deal with conflicts
		this->checkForConflictsAndAssignModels(
			currentTargetContours,
			currentAssignmentTargets,
			_image,
			_models);

		//Send progress-Information
		sstream.clear();sstream.str("");
		sstream << this->mCurrentTimeStepLarvae.size() << " Modelle erkannt.";
		this->mTracker->sendCurrentProgressToObservers(
			sstream.str(), 99);

		//Delete contours
		for each(GeneralTrackerFuncs::Contour* c in contours)
			delete c;

		this->mLastTimeStepLarvae = this->mCurrentTimeStepLarvae;

		this->mTracker->sendCurrentProgressToObservers(100);
	}

	void KonZertCore::checkForNewContours(
		vector< LarvaModel* >					&	_currentAssignmentTargets,
		vector< GeneralTrackerFuncs::Contour* >	&	_currentTargetContours,
		const cv::Mat							&	_image,
		const int									_timeIndex,
		vector< LarvaModel* >					*	_models)
	{
		for(unsigned int i = 0; i < _currentAssignmentTargets.size();)
		{
			if(_currentAssignmentTargets[i])
			{
				i++;
			}
			else
			{
				//New detection
				//this contour will only appear once
				GeneralTrackerFuncs::Contour * currentContour = _currentTargetContours[i];
				LarvaModel* newLarva =  
					this->mModelManager->addNewLarva(	
						currentContour,
						_image,
						_timeIndex);
				this->mCurrentTimeStepLarvae.push_back(newLarva);
				_models->push_back(newLarva);
			
				_currentAssignmentTargets.erase(_currentAssignmentTargets.begin()+i);
				_currentTargetContours.erase(_currentTargetContours.begin()+i);
			}
		}
	}

	void KonZertCore::checkForConflictsAndAssignModels(
		vector< GeneralTrackerFuncs::Contour* >		& _currentTargetContours,
		vector< LarvaModel* >						& _currentAssignmentTargets,
		const Mat									& _image,
		vector< LarvaModel* >						* mod_allLarvaeBaseList)
	{
		for (unsigned int i = 0;
			i < _currentTargetContours.size();
			i++)
		{
			//Find collisions (same contour assigned to multiple models)
			vector< LarvaModel* > currentConflicts;
			this->mModelManager->findConflicts(
				i,
				_currentTargetContours,
				_currentAssignmentTargets,
				currentConflicts);
		
			if (currentConflicts.size() == 1)//No Conflicts
			{

				LarvaModel* split = currentConflicts[0]->getNext();
			
				LarvaModel * newModel = 
					this->mModelManager->addSuccessiveLarva(
						currentConflicts[0],
						_currentTargetContours[i],
						_image,
						mod_allLarvaeBaseList,
						false);

				if(newModel)
				{
					if((!split || !split->isResolved()) || (split->isCollision() || !newModel->isCollision())) 
						this->mCurrentTimeStepLarvae.push_back(newModel);
				
					if(split)
					{					
						if(this->prepareSplit(split, newModel, mod_allLarvaeBaseList))
						{
							vector<LarvaModel *> splitModels;
							splitModels.push_back(split);
							splitModels.push_back(newModel);
							this->resolveBackwards(splitModels, _image, mod_allLarvaeBaseList);
						}
					}
				}
			}
			else 
			{
				//Conflict
				this->resolveConflictsAndAssignModels(
					_currentTargetContours[i]->getContour(), 
					_image,
					currentConflicts,
					mod_allLarvaeBaseList);
			}
		}
	}

	void KonZertCore::resolveBackwards(
		vector< LarvaModel * >			_splitModels,
		const cv::Mat				&	_image,	
		vector< LarvaModel* >		*	mod_allLarvaeBaseList)
	{
		LarvaModel * prevModel = _splitModels[0]->getPrev();
		while(prevModel && !prevModel->isResolved())
		{		
			//Determine contour to be divided between splitmodels
			vector< Point > baseContour = prevModel->getContour();
			vector< vector< Point > > seperatedContours = 
				this->getSeperatedContours(&baseContour, _splitModels);
			
			//resolve each splitted model
			for(unsigned int i = 0; i < _splitModels.size(); i++)
			{
				if(!_splitModels[i] || !_splitModels[i]->hasPrev())
				{
					_splitModels[i] = 0;
					continue;
				}
				else if(seperatedContours[i].size() < 3)
				{					
					for(LarvaModel* pre = _splitModels[i];
						pre;
						pre = pre->getPrev())
					{
						pre->setResolved(false);
					}
					_splitModels[i] = 0;
					continue;
				}

				LarvaModel* splitModelPredecessor = _splitModels[i]->getPrev();

				//Close contour
				GeneralTrackerFuncs::Contour closedContour = 
					GeneralTrackerFuncs::Contour(
						this->getClosedContour(
							seperatedContours[i], 
							_splitModels[i]));

				//assign model
				_splitModels[i] = this->mModelFinder->getModelFromContour(
					&closedContour, 
					_image, 
					0,
					splitModelPredecessor);

				//Correct head alignment
				if(_splitModels[i]) 
				{
					_splitModels[i]->setCollision(
						baseContour.size()-seperatedContours[i].size() > 3);//3 points minimum for a plane
					if(_splitModels[i]->hasNext() && _splitModels[i]->isResolved() && _splitModels[i]->getNext()->isResolved())
					{
						const double distanceHeadHead = 
							(_splitModels[i]->getHead() - _splitModels[i]->getNext()->getHead()).length();
						const double distanceHeadTail = 
							(_splitModels[i]->getTail() - _splitModels[i]->getNext()->getHead()).length();
	
						if(distanceHeadHead > distanceHeadTail)
						{
							GeneralTrackerFuncs::LarvaModelManager::InvertSpine(_splitModels[i]);
						}
					}
				}
				else
				{
					LarvaModel *firstDetection = splitModelPredecessor->getNext();
					firstDetection->rmPointerToPrev();
					mod_allLarvaeBaseList->push_back(firstDetection);
					splitModelPredecessor->setNext(0);
				}
			}
			prevModel = prevModel->getPrev();
		}
	}

	void KonZertCore::resolveConflictsAndAssignModels(
		vector< Point >								* _contour,
		const Mat									& _image,
		vector< LarvaModel* >						& mod_currentConflicts,
		vector< LarvaModel* >						* mod_allLarvaeBaseList)
	{				
		vector< vector< Point > > seperatedContours = 
			this->getSeperatedContours(_contour, mod_currentConflicts);
		vector< LarvaModel* > computedModels;
		vector<vector< LarvaModel* >> modelsNeedingBackwardsResolving;
		LarvaModel* newModel;
		while(mod_currentConflicts.size() > 0)
		{
			//Deal with smallest first
			unsigned int smallestIndex = 0;
			unsigned int smallestSize = seperatedContours[smallestIndex].size();
			for(unsigned int i = 0; i < seperatedContours.size(); i++)
				if(smallestSize > seperatedContours[i].size())
				{
					smallestIndex = i;
					smallestSize = seperatedContours[i].size();
				}
			vector< Point > smallestContour = seperatedContours[smallestIndex];
			newModel = NULL;
			if(smallestSize > 3)//enough contour points to define a plane
			{			
				vector<Point> closedContour = 
						smallestSize < _contour->size()-2
						? this->getClosedContour(smallestContour, mod_currentConflicts[smallestIndex])
						: *_contour;

				LarvaModel* split = mod_currentConflicts[smallestIndex]->getNext();
				GeneralTrackerFuncs::Contour contourObject = GeneralTrackerFuncs::Contour(closedContour);
				newModel = this->mModelManager->addSuccessiveLarva(
					mod_currentConflicts[smallestIndex],
					&contourObject,
					_image,
					mod_allLarvaeBaseList,
					false);

				if(newModel && newModel->isResolved())
				{						
					newModel->setCollision(
						_contour->size() - smallestSize > 3);//3 points minimum for a plane

					if((!split || !split->isResolved()) || (split->isCollision() || !newModel->isCollision())) 
						this->mCurrentTimeStepLarvae.push_back(newModel);

					if(split && this->prepareSplit(split, newModel, mod_allLarvaeBaseList))
					{						
						vector< LarvaModel* > pair;
						pair.push_back(split); 
						pair.push_back(newModel);
						modelsNeedingBackwardsResolving.push_back(pair);
					}

					if((*(this->mCurrentTimeStepLarvae.end()-1)) == newModel)
					{
						computedModels.push_back(newModel);
					}
				}
			}
			if(computedModels.empty() 
				|| *(computedModels.end()-1) != newModel)
			{
				//Erase computed model
				mod_currentConflicts.erase(
					mod_currentConflicts.begin()+smallestIndex);				

				//Remove new models
				for(vector<vector< LarvaModel* >>::iterator splitIt = modelsNeedingBackwardsResolving.begin();
					modelsNeedingBackwardsResolving.end() != splitIt; splitIt++)
				{
					LarvaModel * first = (*splitIt)[1]->getFirst();
					computedModels.erase(
						std::remove(
							this->mCurrentTimeStepLarvae.begin(), 
							this->mCurrentTimeStepLarvae.end(),
							(*splitIt)[1]), 
						this->mCurrentTimeStepLarvae.end());
					mod_allLarvaeBaseList->erase(
						std::remove(
							mod_allLarvaeBaseList->begin(), 
							mod_allLarvaeBaseList->end(),
							first), 
						mod_allLarvaeBaseList->end());
					first->deleteChildren();
					delete first;
				}
				modelsNeedingBackwardsResolving.clear();

				for(vector< LarvaModel* >::iterator computedIt = computedModels.begin();
					computedIt != computedModels.end(); computedIt++)
				{
					this->mCurrentTimeStepLarvae.erase(
						std::remove(
							this->mCurrentTimeStepLarvae.begin(), 
							this->mCurrentTimeStepLarvae.end(),
							(*computedIt)), 
						this->mCurrentTimeStepLarvae.end());
					LarvaModel * prev = (*computedIt)->getPrev();
					prev->deleteChildren();					
					mod_currentConflicts.push_back(prev);
				}
				computedModels.clear();

				seperatedContours = 
					this->getSeperatedContours(_contour, mod_currentConflicts);

				continue;
			}

			mod_currentConflicts.erase(
				mod_currentConflicts.begin()+smallestIndex);
			seperatedContours.erase(
				seperatedContours.begin()+smallestIndex);
		}

		// Resolve splitting models backwards
		for(vector<vector< LarvaModel* >>::iterator bgresIt = modelsNeedingBackwardsResolving.begin();
			bgresIt != modelsNeedingBackwardsResolving.end(); bgresIt++)
		{
			this->resolveBackwards(*bgresIt, _image, mod_allLarvaeBaseList);
		}
	}

	vector< vector< Point > > KonZertCore::getSeperatedContours(
		const vector< Point >						*	_contour,
		const vector< LarvaModel* >					&	_currentConflicts)
	{
		vector< vector< Point > > seperatedContours(_currentConflicts.size());
		Vector2D moment = GeneralTrackerFuncs::Toolbox::GetMoment(*_contour);

		for(vector< Point >::const_iterator contIt = _contour->begin();
			contIt != _contour->end(); contIt++)
		{		
			vector< int > indexOfBestLarvae;
			indexOfBestLarvae.push_back(0);
			double nearest = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
			int indexOfCurrentLarva = 0;
			for(vector< LarvaModel* >::const_iterator larvaIt = _currentConflicts.begin();
				larvaIt != _currentConflicts.end();
				larvaIt++, indexOfCurrentLarva++)
			{			
				if((*larvaIt) && (*larvaIt)->isResolved())
				{
					Point contPoint = *contIt;
					double currentDistance = 
						GeneralTrackerFuncs::Toolbox::GetMinimalDistance(contPoint/*-progress*/, *(*larvaIt)->getSpine());//larvaContour);//
					if(currentDistance < nearest)
					{
						nearest = currentDistance;
						indexOfBestLarvae.clear();
						indexOfBestLarvae.push_back(indexOfCurrentLarva);
					}
					//////Optional: On equal distance track both larvae
					//else if(currentDistance == nearest)
					//{
						//indexOfBestLarvae.push_back(indexOfCurrentLarva);
					//}
				}
			}
			for(vector< int >::iterator indIt = indexOfBestLarvae.begin(); 
				indIt != indexOfBestLarvae.end();
				indIt++)
			{
				seperatedContours[*indIt].push_back(*contIt);
			}
		}

		return seperatedContours;
	}

	vector< Point > KonZertCore::getClosedContour(
		const	vector< Point >	&	_contour,
				LarvaModel		*	_prev,
		const	int					_density)//Maximal distance of two pixels, if bigger it's considered a gap
	{
		
		vector< Point > result;

		//Check for gaps along contour
		for(unsigned int i = 0; i < _contour.size()-1; i++)
		{					
			Point pi = _contour[i];
			Point pi1 = _contour[i+1];
			result.push_back(pi);
			if(GeneralTrackerFuncs::Toolbox::GetDistance(pi, pi1) > _density)
			{
				this->getClosedSingleContourGap(i+1, _prev, _density, _contour, result);
			}
		}
	
		//Check for gap between end and start of vector (contour)
		Point pi = _contour[_contour.size()-1];
		Point pi1 = _contour[0];
		result.push_back(pi);
		if(GeneralTrackerFuncs::Toolbox::GetDistance(pi, pi1) > _density)
		{
			this->getClosedSingleContourGap(0, _prev, _density, _contour, result);
		}

		return result;
	}

	void KonZertCore::getClosedSingleContourGap(
		const	unsigned int						&	_gapEndIndex,
				LarvaModel							*	_prev,//Only required for some solutions
		const	int										_density,
		const	vector< Point >						&	_contourWGaps,
				vector< Point >						&	mod_contour)
	{
		Point gapStart = mod_contour[mod_contour.size()-1];
		Vector2D gapEnd(_contourWGaps[_gapEndIndex]);
		Vector2D direction(gapEnd - gapStart);
		direction.normalize();
		direction *= _density;
		Point insertionPoint = gapStart;
		Vector2D exactGapStart = *(mod_contour.end()-1);
		
		//Required for other gap-closing algorithms (comments)		
		Vector2D progress(0,0);
		if(_prev->hasPrev())
		{
			progress = (_prev->getCenter()-_prev->getPrev()->getCenter());
		}

		for(int i = 1;
			(gapEnd-insertionPoint).length() > _density 
				&& i < 200;//Failsafe
			i++)
		{
/*
			//-Curvature information at gap start/end
			Vector2D directionFromStart = mod_contour[mod_contour.size()-1];
			if(mod_contour.size() > 1)
			{
				directionFromStart -= mod_contour[mod_contour.size()-2];
			}
			else if(GeneralTrackerFuncs::Toolbox::GetDistance(
				*(_contourWGaps.end()-1), mod_contour[mod_contour.size()-1])
				<= _density)
			{
				directionFromStart -= *(_contourWGaps.end()-1);
			}
			else
			{	
				//Fallback to linear closing
				directionFromStart = gapEnd - directionFromStart;
			}
			directionFromStart.normalize();
			double distanceFromStart	= GeneralTrackerFuncs::Toolbox::GetDistance(gapStart, insertionPoint);
			double distanceToEnd		= (gapEnd - insertionPoint).length();
			double distanceSum			= distanceFromStart + distanceToEnd;
			//Factorize
			distanceFromStart			/= distanceSum;
			distanceToEnd				/= distanceSum;
			Vector2D directionToEnd(gapEnd - insertionPoint);
			directionToEnd.normalize();
			insertionPoint = insertionPoint + 
				((directionFromStart * distanceToEnd
					+ directionToEnd * distanceFromStart) * _density).toPoint();		
			mod_contour.push_back(insertionPoint);
			//-Curvature information at gap start/end EOF
*/

			//-Linear
			insertionPoint = gapStart + (direction*i).toPoint();
			mod_contour.push_back(insertionPoint);
			//-Linear EOF


/*	
			//-Width in accordance to previous spine
			Vector2D directionToEnd((exactGapStart*-1) + gapEnd);
			directionToEnd.normalize();
			Vector2D insertionVector = (directionToEnd*_density) + exactGapStart;//to end
			if(_prev->isResolved())
			{
				vector< Vector2D > * spine					= _prev->getSpine();
				vector< Vector2D >::const_iterator spineIt	= spine->begin();
				vector< Vector2D >::const_iterator spineEnd	= spine->end();

				vector< double > * spineWidth				= _prev->getWidths();
				vector< double >::const_iterator widthIt	= spineWidth->begin();

				Vector2D p1, p2;
				double width1 = 0, width2 = 0;
				double distance1 = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
				double distance2 = distance1;
				
				//Get nearest two points
				for(;
					spineIt != spineEnd;
					spineIt++, widthIt++)
				{
					double currentDistance = (insertionVector-(*spineIt+progress)).length();
					if(currentDistance < distance1)
					{
						distance2	= distance1;
						p2			= p1;
						width2		= width1;

						distance1	= currentDistance;
						p1			= *spineIt+progress;
						width1		= (*widthIt)/2;
					}
					else if(currentDistance < distance2)
					{
						distance2	= currentDistance;
						p2			= *spineIt+progress;
						width2		= (*widthIt)/2;
					}
				}
							
				//Weighted midPoint
				Vector2D midPoint((p1*distance2+p2*distance1)*(1/(distance2+distance1)));

				Vector2D directionFromMidPoint(insertionVector-midPoint);
				directionFromMidPoint.normalize();
				directionFromMidPoint *= (midPoint - *(mod_contour.end()-1)).length();//(width1*distance2+width2*distance1) / (distance2+distance1);

				insertionVector = (directionFromMidPoint + midPoint);				
			}
			exactGapStart = insertionVector;
			mod_contour.push_back(exactGapStart.toPoint());
			//-Width in accordance to previous spine - EOF
*/
/*	
			//-Use information From predecessor
			Point bestMatch = insertionPoint;
			if(_prev->isResolved())
			{
				double minDistance = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
				vector<Point> contour = _prev->getContour();
				for each(Point prevContourPoint in contour)
				{
					//Find nearest contour point
					double currentDistance = 
						((progress+prevContourPoint)-insertionPoint).length();
					if(currentDistance < minDistance)
					{
						bestMatch = prevContourPoint+progress.toPoint();
						minDistance = currentDistance;
					}
				}
			}
			//Make sure progress is made
			Point lastPoint = *(mod_contour.end()-1);
			if(bestMatch.x == lastPoint.x 
				&& bestMatch.y == lastPoint.y)
			{
				//No progress, fallback to linear
				bestMatch = insertionPoint;
			}
			mod_contour.push_back(bestMatch);
			//Linear prediction of next point
			direction = gapEnd - bestMatch;
			direction.normalize();
			insertionPoint = bestMatch + (direction*i).toPoint();			
			//-From previous EOF
*/
		}
	}

	void KonZertCore::correctSpineWidths(
		LarvaModel								*	mod_model,
		const cv::Mat							&	_FBimage,
		const double								_maxDifference)
	{
		if(!mod_model)
		{
			return;
		}

		//Get reference spine widths
		LarvaModel* ref_model = mod_model->getPrev();
		vector< vector<double> > medianWidthsTemp;
		unsigned int medRange = 5;		
		unsigned int i = 0;
		while(i < medRange && ref_model)
		{
			if(ref_model->isResolved())
			{
				vector< double > * refWidths = ref_model->getWidths();
				unsigned int resIndex = 0;
				for(vector< double>::const_iterator it = refWidths->begin();
					refWidths->end() != it;
					it++, resIndex++)
				{
					if(resIndex >= medianWidthsTemp.size())
					{
						medianWidthsTemp.push_back(vector<double>());
					}
					medianWidthsTemp[resIndex].push_back(*it);
				}
				i++;
			}
			ref_model = ref_model->getPrev();
		}
		ref_model = mod_model->getNext();
		while(i < medRange && ref_model)
		{
			if(ref_model->isResolved())
			{
				vector< double > * refWidths = ref_model->getWidths();
				unsigned int resIndex = 0;
				for(vector< double>::const_iterator it = refWidths->begin();
					refWidths->end() != it;
					it++, resIndex++)
				{
					if(resIndex >= medianWidthsTemp.size())
					{
						medianWidthsTemp.push_back(vector<double>());
					}
					medianWidthsTemp[resIndex].push_back(*it);
				}
				i++;
			}
			ref_model = ref_model->getNext();
		}
		if(i == medRange)
		{
			vector<double> medianWidths;
			for(vector< vector< double > >::iterator it = medianWidthsTemp.begin();
				medianWidthsTemp.end() != it;
				it++)
			{
				std::sort(it->begin(), it->end());
				double median = (medRange  % 2 == 0)
					? it->at(medRange / 2)
					: ((it->at(medRange / 2) - 1) + it->at(medRange / 2)) / 2;			
				medianWidths.push_back(median);			
			}

			//Check spine widths
			vector<double> *current_widths		= mod_model->getWidths();
			vector<double>::iterator cWidIt		= current_widths->begin();
			vector<double>::iterator cWidEnd	= current_widths->end();
			vector<double>::iterator mWidIt		= medianWidths.begin();
			for(;
				cWidIt != cWidEnd;
				cWidIt++, mWidIt++)
			{
				*cWidIt = std::min(*mWidIt * (1+_maxDifference), std::max((1-_maxDifference), *cWidIt));
			}
		}

		//Correct out-of-contour larva-points
		vector<Vector2D>		*	current_spine		= mod_model->getSpine();
		vector<Vector2D>::iterator	current_spineBegin	= current_spine->begin();
		vector<Vector2D>::iterator	current_spineEnd	= current_spine->end();
		vector<double>			*	current_widths		= mod_model->getWidths();
		vector<double>::iterator	cWidIt				= current_widths->begin();
		vector<double>::iterator	cWidEnd				= current_widths->end();		
		Vector2D sidesVector;		
		for(vector<Vector2D>::iterator current_spineIt = current_spineBegin;
			current_spineIt != current_spineEnd;
			current_spineIt++)
		{
			if(current_spineIt == current_spineBegin)
			{
				//Head			
				sidesVector = (*current_spineIt-*(current_spineIt+1));
				sidesVector = sidesVector.getOrthogonalVector();
				sidesVector.normalize();
				sidesVector *= *cWidIt/2;
			}
			else if(current_spineIt == current_spineEnd-1)
			{
				//Tail
				sidesVector = (*current_spineIt-*(current_spineIt-1));
				sidesVector = sidesVector.getOrthogonalVector();
				sidesVector.normalize();
				sidesVector *= *cWidIt/2;
			}
			else
			{
				Vector2D a(*current_spineIt-*(current_spineIt-1));
				Vector2D b(*(current_spineIt+1)-*current_spineIt);
				double angle = std::acos(a.dot(b) / (a.length()*b.length()))/2;			
				b.rotate(angle);
				b.normalize();
				sidesVector = b;
			}

			bool posFG = GeneralTrackerFuncs::Toolbox::GetValueAt<uchar>(
							_FBimage, 
							(sidesVector + *current_spineIt).toPoint()) 
						!= 0;
			bool negFG = GeneralTrackerFuncs::Toolbox::GetValueAt<uchar>(
							_FBimage, 
							((sidesVector*-1) + *current_spineIt).toPoint()) 
						!= 0;
			bool spineFG = GeneralTrackerFuncs::Toolbox::GetValueAt<uchar>(
							_FBimage, 
							current_spineIt->toPoint()) 
						!= 0;
			if(posFG && !negFG)
			{
				if(!spineFG 
					&& GeneralTrackerFuncs::Toolbox::GetValueAt<uchar>(
							_FBimage, 
							(sidesVector*3 + *current_spineIt).toPoint()))//Wrong side of contour
				{
					*current_spineIt = (sidesVector*2 + *current_spineIt).toPoint();
				}
			}
			else if(!posFG && negFG)
			{
				if(!spineFG 
					&& GeneralTrackerFuncs::Toolbox::GetValueAt<uchar>(
							_FBimage, 
							(sidesVector*-3 + *current_spineIt).toPoint()))//Wrong side of contour
				{
					*current_spineIt = (sidesVector*-2 + *current_spineIt);
			
				}
			}
		}	
	}

	bool KonZertCore::prepareSplit(
		LarvaModel								*	mod_currentSuccessor,
		LarvaModel								*	mod_additionalSuccessor,
		vector<LarvaModel*>						*	mod_allLarvaeBaseList)
	{
		if(mod_currentSuccessor->isCollision() == mod_additionalSuccessor->isCollision()
			&& mod_currentSuccessor->isResolved() == mod_additionalSuccessor->isResolved())
		{
			// Acceptable Split:
			// Mark participants of split unresolved				
			LarvaModel *a = mod_currentSuccessor;		
			LarvaModel *b = mod_additionalSuccessor;		
			while(a->hasPrev())
			{					
				a = a->getPrev();
				b = b->getPrev();
				if(a->isResolved())
				{
					a->setResolved(false);
					b->setResolved(false);
				}
				else
				{
					break;
				}
			}				

			return true;
		}
		else if(!mod_additionalSuccessor->isCollision() || !mod_currentSuccessor->isResolved())
		{
			// Unacceptable Split:
			// Second model superior
			LarvaModel * splitsFirst = mod_currentSuccessor->getFirst();
			this->mCurrentTimeStepLarvae.erase(
				std::remove(
					this->mCurrentTimeStepLarvae.begin(), 
					this->mCurrentTimeStepLarvae.end(),
					mod_currentSuccessor), 
				this->mCurrentTimeStepLarvae.end());
			mod_allLarvaeBaseList->erase(
				std::remove(
					mod_allLarvaeBaseList->begin(), 
					mod_allLarvaeBaseList->end(),
					splitsFirst), 
				mod_allLarvaeBaseList->end());
			splitsFirst->deleteChildren();
			delete splitsFirst;
			return false;
		}
		else
		{
			// Unacceptable Split:
			// First model superior
			LarvaModel * newModelsFirst = mod_additionalSuccessor->getFirst();
			this->mCurrentTimeStepLarvae.erase(
				std::remove(
					this->mCurrentTimeStepLarvae.begin(), 
					this->mCurrentTimeStepLarvae.end(),
					mod_additionalSuccessor), 
				this->mCurrentTimeStepLarvae.end());
			mod_allLarvaeBaseList->erase(
				std::remove(
					mod_allLarvaeBaseList->begin(), 
					mod_allLarvaeBaseList->end(),
					newModelsFirst), 
				mod_allLarvaeBaseList->end());
			newModelsFirst->deleteChildren();
			delete newModelsFirst;
			return false;
		}
	}
}