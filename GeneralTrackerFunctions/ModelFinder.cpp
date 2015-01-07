#include "ModelFinder.h"

namespace GeneralTrackerFuncs
{
	ModelFinder::ModelFinder(
		GeneralTrackerFuncs::SpineFinder *	_spineFinder, 
		const int							_segmentCount)
		: mSegmentCount(_segmentCount)
	{
		this->mSpineFinder = _spineFinder;
	}


	ModelFinder::~ModelFinder(void)
	{
	}

	LarvaModel* ModelFinder::getModelFromContour(
		Contour							*	_contour, 
		const Mat						&	_image, 
		LarvaModel						*	_predecessor,
		LarvaModel						*	mod_larva)
	{
		//Find head-/tail-points
		vector<Point>					potentialEndpoints;
		vector<Point>					correspondingEndPoints0;
		vector<Point>					correspondingEndPoints1;
		vector<double>					high_intensities;
		vector<Vector2D>				endPointCorrespondingSpinePoints;
		vector<float>					correspondingSpineWidths;
		this->mSpineFinder->findEndPoints(
			_contour->getContour(),
			_image, 
			&potentialEndpoints, 
			&correspondingEndPoints0,
			&correspondingEndPoints1,
			&high_intensities, 
			&endPointCorrespondingSpinePoints,
			&correspondingSpineWidths);

		int indexTail = 0, indexHead = 1; 
		int potentialEndpointsCount = potentialEndpoints.size();
		if(potentialEndpointsCount < 2)
		{
			//Could not find head and tail points
			return 0;
		}
		else if(_predecessor && _predecessor->isResolved())//predecessor defined
		{
			//choose points with min distance to points in pred
			double minHead = Toolbox::GetMaxDouble();
			double minTail = Toolbox::GetMaxDouble();
			Vector2D predHead = _predecessor->getHead();
			Vector2D predTail = _predecessor->getTail();
		
			double secondBestDistanceToHead = 
				GeneralTrackerFuncs::Toolbox::GetMaxDouble();
			double secondBestDistanceToTail = 
				GeneralTrackerFuncs::Toolbox::GetMaxDouble();
			int secondIndexHead = 0;
			int secondIndexTail = 0;

			//Determine most likely candidates for head and tail
			for(int i = 0; i < potentialEndpointsCount; i++)
			{
				Point currentPoint = potentialEndpoints[i];
				double distHead = 
					(predHead - currentPoint).length();
				double distTail = 
					(predTail - currentPoint).length();

				if(distHead < minHead)
				{ 
					minHead		= distHead;
					indexHead	= i;
				}
				else if(distHead < secondBestDistanceToHead)
				{
					secondBestDistanceToHead = distHead;
					secondIndexHead			 = i;
				}
				
				if(distTail < minTail)
				{
					minTail		= distTail;
					indexTail	= i;
				}
				else if(distTail < secondBestDistanceToTail)
				{
					secondBestDistanceToTail	= distTail;
					secondIndexTail				= i;
				}
			}

			if(indexHead == indexTail)
			{
				if(minHead+secondBestDistanceToTail 
					< secondBestDistanceToHead + minTail)
				{
					indexTail = secondIndexTail;
				}
				else
				{
					indexHead = secondIndexHead;
				}
			}
		}
		else//Use distances
		{
			int best0 = 0, best1 = 1;
			if(potentialEndpointsCount > 2)
			{
				//Setup distance check
				vector< vector<Point>::const_iterator > iterators0;
				vector< vector<Point>::const_iterator > iterators1;
				for(int i = 0; i < potentialEndpointsCount; i++)
				{
					iterators0.push_back(Toolbox::GetNearestPointOnContour(_contour->getContour(), correspondingEndPoints0[i]));
					iterators1.push_back(Toolbox::GetNearestPointOnContour(_contour->getContour(), correspondingEndPoints1[i]));
				}

				//Perform distance check
				int contourSize = _contour->getContour()->size();				
				int largestDistance = 0;
				for(int a = 0; a < potentialEndpointsCount; a++)
				{
					for(int b = a+1; b < potentialEndpointsCount; b++)
					{
						int dist =	
							std::min(	Toolbox::GetIteratorDistance(iterators0[a], iterators0[b], contourSize),
							std::min(	Toolbox::GetIteratorDistance(iterators1[a], iterators1[b], contourSize),
							std::min(	Toolbox::GetIteratorDistance(iterators1[a], iterators0[b], contourSize),
										Toolbox::GetIteratorDistance(iterators0[a], iterators1[b], contourSize))));
						if(dist > largestDistance)
						{
							largestDistance = dist;
							best0 = a;
							best1 = b;
						}
					}					
				}
			}

			//Distinguish head and tail by intensity and size.
			//Head is small with high intensity, tail large with low intensity.
			double headValue0 = high_intensities[best0] * correspondingSpineWidths[best1];
			double headValue1 = high_intensities[best1] * correspondingSpineWidths[best0];
			if(headValue0 > headValue1)
			{
				indexTail	= best1;				
				indexHead	= best0;				
			}
			else
			{
				indexTail	= best0;				
				indexHead	= best1;				
			}
		}

		LarvaModel *mod = mod_larva ? mod_larva : new LarvaModel;
		mod->setCenter(*_contour->getCenterOfMass());
		mod->setContour(*_contour->getContour());

		mod->setTailIntensity(high_intensities[indexTail]);
		mod->setHeadIntensity(high_intensities[indexHead]);

		//Spine
		vector<Vector2D> spine;
		vector<double> spineSegWidths;
		double maxSpineWidth = 0;
		if(_predecessor && _predecessor->isResolved())
		{
			vector< double > spineWidths = vector< double >(*_predecessor->getWidths());
			std::sort(spineWidths.begin(), spineWidths.end());	
			int size = spineWidths.size();
			double median = spineWidths[size / 2];
			if (size % 2 == 0)
			{
				median = (spineWidths[size / 2 - 1] + median) / 2;
			}
			maxSpineWidth = 1.5*median;
		}
		else
		{
			maxSpineWidth = Toolbox::GetMaxDouble();
		}

		this->mSpineFinder->findSpine(
			_contour->getContour(), 
			endPointCorrespondingSpinePoints[indexHead],
			correspondingEndPoints0[indexHead],
			correspondingEndPoints1[indexHead],
			correspondingSpineWidths[indexHead],
			endPointCorrespondingSpinePoints[indexTail],
			correspondingEndPoints0[indexTail],
			correspondingEndPoints1[indexTail],
			correspondingSpineWidths[indexTail],
			maxSpineWidth,
			&spine,
			&spineSegWidths,
			this->mSegmentCount);	

		mod->setSpine(spine);
		mod->setWidths(spineSegWidths);		
		mod->setResolved(spine.size() == this->mSegmentCount+1);

		return mod;
	}

	int ModelFinder::getHighestIntensityIndex(
		const vector<double> & high_intensities)
	{
		double highestInt = -1; 
		int highestIntensityIndex = 0;
		int i = 0;
		for(vector<double>::const_iterator itInt = high_intensities.begin(); 
			itInt != high_intensities.end();
			itInt++, i++) 
		{
			if(*itInt >= highestInt)
			{
				highestInt = *itInt;
				highestIntensityIndex = i;
			}
		}
	
		return highestIntensityIndex;
	}

	int ModelFinder::getLowestIntensityIndex(
			const vector<double> & high_intensities)
	{
		double lowestInt = Toolbox::GetMaxDouble();
		int i = 0;
		int lowestIntensityIndex = 0;
		for(vector<double>::const_iterator itInt = high_intensities.begin();
			itInt != high_intensities.end();
			i++, itInt++) 
		{
			if(*itInt <= lowestInt)
			{
				lowestInt = *itInt;
				lowestIntensityIndex = i;
			}
		}

		return lowestIntensityIndex;
	}

}