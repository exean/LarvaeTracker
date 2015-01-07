#include "LarvaModelManager.h"
namespace GeneralTrackerFuncs
{

	LarvaModelManager::LarvaModelManager(ModelFinder * _ModelFinder)
	{
		this->mModelFinder = _ModelFinder;
	}


	LarvaModelManager::~LarvaModelManager(void)
	{
	}

	
	LarvaModel* LarvaModelManager::addNewLarva(	
		Contour						*	_currentTargetContour,
		const Mat					&	_image,
		const int						_timeIndex)
	{
		//Try to detect larva in contour
		LarvaModel* m = this->mModelFinder->getModelFromContour(
				_currentTargetContour, 
				_image);

		if(!m)
		{
			//Detection failed, add unresolved model
			m = new LarvaModel(false);
			m->setContour(*_currentTargetContour->getContour());
		}

		m->setTimeIndex(_timeIndex);		

		return m;
	}
	
	void LarvaModelManager::findConflicts(
		const int									_index,
		vector< GeneralTrackerFuncs::Contour* > &	mod_currentTargetContours,
		vector< LarvaModel* >					&	mod_lastDetectedLarvaModels,
		vector< LarvaModel* >					&	out_collidingLarvaModels)
	{
		//List of all Larva Models assigned to current target contour _index
		out_collidingLarvaModels.push_back(mod_lastDetectedLarvaModels[_index]);

		for (unsigned int j = _index+1; 
			j < mod_currentTargetContours.size(); )
		{
			if(mod_currentTargetContours[_index] == mod_currentTargetContours[j])
			{
				if(mod_lastDetectedLarvaModels[_index] != mod_lastDetectedLarvaModels[j])
				{
					out_collidingLarvaModels.push_back(mod_lastDetectedLarvaModels[j]);
				}

				// Remove model and target contour from list as model
				// has now been assigned to a conflicting contour
				mod_currentTargetContours.erase(mod_currentTargetContours.begin()+j);
				mod_lastDetectedLarvaModels.erase(mod_lastDetectedLarvaModels.begin()+j);
			}
			else 
			{
				j++;
			}	
		}
	}

	LarvaModel* LarvaModelManager::addSuccessiveLarva(
		LarvaModel								*	_larvaPredecessor,
		Contour									*	_targetContour,
		const cv::Mat								_image,
		vector< LarvaModel* >					*	mod_AllLarvaeBaseList,
		const bool									_useUnresolved)
	{
		//Try to detect larva in contour
		LarvaModel* m = this->mModelFinder->getModelFromContour(
			_targetContour, 
			_image, 
			_larvaPredecessor);

		if(!m)
		{
			//Detection failed
			if(_useUnresolved)//add unresolved model
			{
				m = new LarvaModel(false);
				m->setContour(*_targetContour->getContour());
			}
			else
			{
				return NULL;
			}
		}

		if(_larvaPredecessor->hasNext())//already has a successor?
		{
			//Split (same model assigned to multiple contours)
			//->set predecessor to ghost-type model
			LarvaModel* cloned		= new LarvaModel(_larvaPredecessor->getFirst(), true);
	
			LarvaModel* clonedLast	= cloned->getLast()->getPrev();
			clonedLast->deleteChildren();
			//direction check still applicable even though model 
			//wasnt truly resolved
			this->setNext(clonedLast, m);	

			//Add cloned to results
			if(mod_AllLarvaeBaseList)
			{
				mod_AllLarvaeBaseList->push_back(cloned);
			}
		}
		else
		{
			this->setNext(_larvaPredecessor, m);
		}
		return m;
	}

	void LarvaModelManager::InvertSpine(LarvaModel* mod_larva)
	{
		if(mod_larva->isResolved())
		{
			vector<Vector2D>* spine = mod_larva->getSpine();
			vector<double>* spineWidths = mod_larva->getWidths();
			vector<Vector2D> newspine;
			vector<double> newspineWidths;
			vector<double>::iterator spWIt = spineWidths->end();
			for(vector<Vector2D>::iterator spIt = spine->end();
				spIt != spine->begin();)
			{
				spIt--;
				spWIt--;
				newspine.push_back(*spIt);
				newspineWidths.push_back(*spWIt);
			}
			mod_larva->setSpine(newspine);
			mod_larva->setWidths(newspineWidths);

			double headIn = mod_larva->getHeadIntensity();
			mod_larva->setHeadIntensity(mod_larva->getTailIntensity());
			mod_larva->setTailIntensity(headIn);
		}
	};

	void LarvaModelManager::setNext(LarvaModel* mod_larva, LarvaModel* mod_next)
	{
		mod_larva->setNext(mod_next);

		//Check head/tail assignment (keep assignment as it is in mod_larva
		//and change mod_next accordingly).
		if(mod_larva->isResolved() && mod_next->isResolved())
		{
			const double distanceHeadHead = 
				std::min((mod_larva->getHead() - mod_next->getHead()).length(),
					(mod_larva->getTail() - mod_next->getTail()).length());
			const double distanceHeadTail = 
				std::min((mod_larva->getTail() - mod_next->getHead()).length(),
				(mod_larva->getHead() - mod_next->getTail()).length());
	
			if(distanceHeadHead > distanceHeadTail)
			{
				//Invert next
				LarvaModelManager::InvertSpine(mod_next);
			}
		}
	}

	
	bool LarvaModelManager::UniteOverTime(
		LarvaModel				* _larva, 
		vector< LarvaModel* >	* _allLarvae,
		unsigned int			  _timeGap)
	{
		LarvaModel * lastOfLarva	= _larva->getLast();
		unsigned int lastIndex		= lastOfLarva->getTimeIndex();
		int currentTimeDifference	= _timeGap+1;
		unsigned int segmentCount	= _larva->getSpine()->size();
	
		//Estimate progression
		Vector2D progression(0,0);
		if(lastOfLarva->hasPrev()
			&& lastOfLarva->getPrev()->isResolved())
		{
			progression = 
				lastOfLarva->getSpine()->at(segmentCount/2) 
				- lastOfLarva->getPrev()->getSpine()->at(segmentCount/2);
		}

		//Estimate regional bounds for successor
		Vector2D estimatedCenterPoint = 
			lastOfLarva->getSpine()->at(segmentCount/2) + (progression*0.5);
		LarvaModel* match = NULL;
		const double minDistance = 
			std::max(std::sqrt((double)(progression.getX()*progression.getX()+progression.getY()*progression.getY()))
					+ lastOfLarva->getWidths()->at(segmentCount/2),
				GeneralTrackerFuncs::Toolbox::GetLength(lastOfLarva->getSpine())/2);
		double currentDistance = minDistance;
		vector< LarvaModel* > potentialSuccessors;

		//Search successor for _larva
		for each(LarvaModel * latterLarva in *_allLarvae)
		{
			LarvaModel * larva = latterLarva->getFirst();
			int timeDifference = larva->getTimeIndex() - lastIndex;
			if(timeDifference > 0 && timeDifference <= currentTimeDifference)
			{
				double distance = 
					(larva->getSpine()->at(segmentCount/2) - estimatedCenterPoint).length();
				if(distance < currentDistance || (timeDifference < currentTimeDifference && distance < minDistance))
				{
					//Model is either nearer in time or equal in time and nearer in space
					currentDistance = distance;
					match = larva;
					currentTimeDifference = timeDifference;
				}
			}
		}

		//match found?
		if(match)
		{
			//Make sure head/tail assignment matches
			if((lastOfLarva->getHead()-match->getHead()).length() > 
				(lastOfLarva->getTail()-match->getHead()).length())
			{
				//Invert match
				for(LarvaModel* current = match; current; current = current->getNext())
				{
					GeneralTrackerFuncs::LarvaModelManager::InvertSpine(current);
				}
			}

			vector<Vector2D>::const_iterator larvaSpineItEnd =	lastOfLarva->getSpine()->end();
			LarvaModel* prev = lastOfLarva;
			const double normfactor = (1./currentTimeDifference);

			//Interpolate gap-models
			for(int distFromPrevious = 1; 
				distFromPrevious < currentTimeDifference; 
				distFromPrevious++)
			{
				LarvaModel* gapLarva = new LarvaModel();
				vector<Vector2D>::const_iterator 
					larvaSpineIt	=	lastOfLarva->getSpine()->begin(),
					matchSpineIt	=	match->getSpine()->begin();
				vector<double>::const_iterator 
					larvaWidthIt	=	lastOfLarva->getWidths()->begin(),							
					matchWidthIt	=	match->getWidths()->begin();
				vector<Vector2D>	interpolatedSpine;
				vector<double>		interpolatedWidths;
				for(;
					larvaSpineIt != larvaSpineItEnd;
					larvaSpineIt++, matchSpineIt++, larvaWidthIt++, matchWidthIt++)
				{
					interpolatedSpine.push_back(
						(*larvaSpineIt*(int)(currentTimeDifference-distFromPrevious)
						+ *matchSpineIt*(int)distFromPrevious)*normfactor);
					interpolatedWidths.push_back(
						(*larvaWidthIt*(int)(currentTimeDifference-distFromPrevious)
						+*matchWidthIt*(int)distFromPrevious)*normfactor);
				}
				gapLarva->setSpine(interpolatedSpine);
				gapLarva->setWidths(interpolatedWidths);
				gapLarva->setCenter(
					(lastOfLarva->getCenter()+match->getCenter())*0.5);
				gapLarva->setCollision(true);

				prev->setNext(gapLarva);			
				prev = gapLarva;
			}
			prev->setNext(match);

			//remove match from all larvae
			_allLarvae->erase(
				std::remove(
					_allLarvae->begin(), 
					_allLarvae->end(),
					match), 
				_allLarvae->end());

			return true;
		}
		return false;
	}

	void LarvaModelManager::CorrectHeadAssignmentByDirection(
		LarvaModel			*	mod_larva, 
		const unsigned int	_stepWidth)
	{		
		LarvaModel * current = mod_larva;

		//Count in how many cases an inversion seems beneficial. 0 Neutral, >0 invert
		int invert = 0;
		
		for(LarvaModel * next = current->getNext(); 
			next; 
			current = next, next = current->getNext())
		{
			if(current->isResolved())
			{
				LarvaModel* compareModel = next;
				for(unsigned int i = 1; i < _stepWidth && compareModel; i++)
				{
					compareModel = compareModel->getNext();
				}
				if(compareModel && compareModel->isResolved())
				{
					//Use direction of movement
					Vector2D movement = (compareModel->getCenter() - current->getCenter());
					if((current->getHead()-(current->getTail()+movement)).length() 
						> (current->getHead()-current->getTail()).length())
					{
						invert++;
					}
					else
					{
						invert--;
					}
				}
			}
		}

		if(invert > 0)
		{
			//Invert all
			for(LarvaModel* currentLarva = mod_larva;
				currentLarva;
				currentLarva = currentLarva->getNext())
			{
				GeneralTrackerFuncs::LarvaModelManager::InvertSpine(currentLarva);
			}
		}
	}
}
