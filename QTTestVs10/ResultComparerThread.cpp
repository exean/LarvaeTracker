#include "ResultComparerThread.h"


ResultComparerThread::ResultComparerThread(
	const QString  & _larvaeGT, 
	const QString  & _larvaeCompare)
{
	this->mLarvaeGTFile = _larvaeGT;
	this->mLarvaeCompareFile = _larvaeCompare;
}


ResultComparerThread::~ResultComparerThread(void)
{
}

void ResultComparerThread::run()
{
	InOut::ImportFromCSV importer;

	QString rowSeperator, colSeperator;
	Gui::GUISettings::Instance()->getCSVValues(colSeperator, rowSeperator);
	vector< LarvaModel*> * larvaeGT = 
		importer.importLarvae(
			this->mLarvaeGTFile, rowSeperator, colSeperator);
	vector< LarvaModel*> * larvaeCompare = 
		importer.importLarvae(
			this->mLarvaeCompareFile, rowSeperator, colSeperator);
	
	if(!larvaeGT)
	{		
		emit badFile(this->mLarvaeGTFile);
	}
	else if(!larvaeCompare)
	{		
		emit badFile(this->mLarvaeCompareFile);
	}
	else
	{
		this->compareCenter(larvaeGT, larvaeCompare);
		this->compareEachSpinePoint(larvaeGT, larvaeCompare);
		this->compareSpineWidths(larvaeGT, larvaeCompare);

		//clean up
		if(larvaeGT)
		{
			for each(LarvaModel * m in *larvaeGT)
			{
				m->deleteChildren();
				delete m;
			}
			delete larvaeGT;
		}
		if(larvaeCompare)
		{
			for each(LarvaModel * m in *larvaeCompare)
			{
				m->deleteChildren();
				delete m;
			}
			delete larvaeCompare;
		}
	}
}

LarvaModel* ResultComparerThread::getBestMatch(		
	const std::vector<LarvaModel*> *	_models,
	const unsigned int				_requiredTimeIndex,
	const Vector2D						_position)
{
	double minDist	= GeneralTrackerFuncs::Toolbox::GetMaxDouble();
	LarvaModel* res = NULL;
	for each(LarvaModel* model in *_models)
	{
		while(model)
		{
			if(_requiredTimeIndex == model->getTimeIndex())
			{
				double currentDistance = 
					(_position - model->getCenter()).length();
				if(currentDistance < minDist)
				{
					minDist = currentDistance;
					res = model;

					if(minDist == 0)
					{
						//Best case has occured
						return res;
					}
				}
				break; //Current Larva finished (no duplicate timeindices exist)
			}
			model = model->getNext();
		}
	}
	return res;
}

void ResultComparerThread::compareCenter(
	const std::vector<LarvaModel*> * _truth,
	const std::vector<LarvaModel*> * _compare)
{
	double totalDistanceSum		= 0;
	double totalDistanceMed		= 0;
	vector< double > allDistances;//For median

	for each(LarvaModel* compareModel in *_compare)
	{
		while(compareModel)
		{
			LarvaModel * match = this->getBestMatch(
				_truth, 
				compareModel->getTimeIndex(), 
				compareModel->getCenter());

			if(match)
			{
				double distance = 
					(match->getCenter() - compareModel->getCenter()).length();

				allDistances.push_back(distance);
				totalDistanceSum += distance;
				compareModel = compareModel->getNext();
			}
		}
	}

	//Median:
	size_t size = allDistances.size();
	std::sort(allDistances.begin(), allDistances.end());	
	totalDistanceMed = allDistances[size / 2];
	if (size  % 2 == 0)
	{
		totalDistanceMed = (allDistances[size / 2 - 1] + totalDistanceMed) / 2;
	}

	//Insert results into gui
	emit centerComparisonFinished(
		allDistances[0], 
		*(allDistances.end()-1), 
		totalDistanceSum/allDistances.size(), 
		totalDistanceMed);
}

void ResultComparerThread::compareEachSpinePoint(
	const std::vector<LarvaModel*> * _truth,
	const std::vector<LarvaModel*> * _compare)
{
	double totalDistanceSum		= 0;
	double totalDistanceMed		= 0;
	vector< double > allDistances;//For median

	for each(LarvaModel* compareModel in *_compare)
	{
		while(compareModel)
		{
			if(compareModel->isResolved())
			{
				LarvaModel * match = this->getBestMatch(
					_truth, 
					compareModel->getTimeIndex(), 
					compareModel->getCenter());

				if(match && match->isResolved())
				{
					vector<Vector2D> *compareSpine = compareModel->getSpine();
					vector<Vector2D> *truthSpine = match->getSpine();
					for(int i = std::min(truthSpine->size(), compareSpine->size())-1;//Only equal length models should be compared, but better to be safe
						i >= 0; 
						i--)
					{
						double distance = 
							(compareSpine->at(i) - truthSpine->at(i)).length();

						allDistances.push_back(distance);
						totalDistanceSum += distance;
					}
				}
			}
			compareModel = compareModel->getNext();
		}
	}

	//Median:
	size_t size = allDistances.size();
	std::sort(allDistances.begin(), allDistances.end());	
	totalDistanceMed = allDistances[size / 2];
	if (size % 2 == 0)
	{
		totalDistanceMed = (allDistances[size / 2 - 1] + totalDistanceMed) / 2;
	}

	//Insert results into gui
	emit spinepointComparisonFinished(
		allDistances[0], 
		*(allDistances.end()-1), 
		totalDistanceSum/allDistances.size(), 
		totalDistanceMed);
}

void ResultComparerThread::compareSpineWidths(
	const std::vector<LarvaModel*> * _truth,
	const std::vector<LarvaModel*> * _compare)
{
	double genDifferenceSum		= 0;
	double absDifferenceSum		= 0;
	double smallerSum			= 0;
	double biggerSum			= 0;
	vector< double > absDifferences;
	vector< double > genDifferences;
	vector< double > compareSmaller;
	vector< double > compareBigger;

	for each(LarvaModel* compareModel in *_compare)
	{
		while(compareModel)
		{
			if(compareModel->isResolved())
			{
				LarvaModel * match = this->getBestMatch(
					_truth, 
					compareModel->getTimeIndex(), 
					compareModel->getCenter());

				if(match && match->isResolved())
				{
					vector<double> *compareWidths = compareModel->getWidths();
					vector<double> *truthWidths = match->getWidths();
					for(int i = std::min(truthWidths->size(), compareWidths->size())-1;//Only equal length models should be compared, but better to be safe
						i >= 0; 
						i--)
					{
						double difference = 
							compareWidths->at(i) - truthWidths->at(i);

						genDifferences.push_back(difference);
						genDifferenceSum += difference;

						if(difference >= 0)
						{
							compareBigger.push_back(difference);							
							biggerSum += difference;
						}
						if(difference <= 0)
						{
							compareSmaller.push_back(std::abs(difference));
							smallerSum += std::abs(difference);
						}

						absDifferences.push_back(std::abs(difference));
						absDifferenceSum += std::abs(difference);
					}
				}
			}
			compareModel = compareModel->getNext();
		}
	}

	//Median
	double median[4];
	size_t absSize = absDifferences.size();
	std::sort(absDifferences.begin(), absDifferences.end());	
	median[0] = absDifferences[absSize / 2];
	if (absSize % 2 == 0)
	{
		median[0] = (absDifferences[absSize / 2 - 1] + median[0]) / 2;
	}
	size_t genSize = genDifferences.size();
	std::sort(genDifferences.begin(), genDifferences.end());	
	median[1] = genDifferences[genSize / 2];
	if (genSize % 2 == 0)
	{
		median[1] = (genDifferences[genSize / 2 - 1] + median[1]) / 2;
	}
	size_t smallerSize = compareSmaller.size();
	std::sort(compareSmaller.begin(), compareSmaller.end());	
	median[2] = compareSmaller[smallerSize / 2];
	if (smallerSize % 2 == 0)
	{
		median[2] = (compareSmaller[smallerSize / 2 - 1] + median[2]) / 2;
	}
	size_t biggerSize = compareBigger.size();
	std::sort(compareBigger.begin(), compareBigger.end());	
	median[3] = compareBigger[biggerSize / 2];
	if (biggerSize % 2 == 0)
	{
		median[3] = (compareBigger[biggerSize / 2 - 1] + median[3]) / 2;
	}

	//Middle	
	double middle[4] = 
	{		
		absDifferenceSum	/ absSize,
		genDifferenceSum	/ genSize,
		smallerSum			/ smallerSize,
		biggerSum			/ biggerSize
	};

	//Min	
	double min[4] =
	{		
		absDifferences	[0],
		genDifferences	[0],
		compareSmaller	[0],
		compareBigger	[0]
	};

	//Max
	double max[4] =
	{		
		absDifferences	[absSize	-1],
		genDifferences	[genSize	-1],
		compareSmaller	[smallerSize-1],
		compareBigger	[biggerSize	-1]
	};

	//Insert results into gui
	emit widthsComparisonFinished(
		min[0],		min[1],		min[2],		min[3], 
		max[0],		max[1],		max[2],		max[3], 
		middle[0],	middle[1],	middle[2],	middle[3], 
		median[0],	median[1],	median[2],	median[3]);
}