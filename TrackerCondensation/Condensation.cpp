#include "Condensation.h"


Condensation::Condensation(void)
{
}


Condensation::~Condensation(void)
{
}

double Condensation::evaluate_observation_density(
			LarvaModel	&	_larva, 
	const	Mat			&	_fgImage, 
	const	float			_underFillWeightFactor)
{	
	if(_larva.isResolved())
	{
		//Get ROI
		double estimatedRoiRadius = 2*GeneralTrackerFuncs::Toolbox::GetLength(_larva.getSpine());	
		Vector2D translation(
			std::max(0.,_larva.getCenter().getX()-estimatedRoiRadius),
			std::max(0.,_larva.getCenter().getY()-estimatedRoiRadius));
		int width	= (int)std::min(2*estimatedRoiRadius, _fgImage.cols-translation.getX()-1);
		int height	= (int)std::min(2*estimatedRoiRadius, _fgImage.rows-translation.getY()-1);
		Mat roi		= _fgImage(cv::Rect(
				(int)translation.getX(), 
				(int)translation.getY(), 
				width, 
				height));

		//Create mask of model
		translation *= -1;
		cv::Mat mask = cv::Mat::zeros(roi.size(), roi.type());	
		GeneralTrackerFuncs::Painter::PaintLarvaSilhouette(_larva, mask, Scalar(255), 1.0f, translation);
		double fillFactor = mean(roi, mask)[0];

		//Create underfill-mask of model
		cv::Mat bordermask =  cv::Mat::zeros(roi.size(), roi.type());	
		GeneralTrackerFuncs::Painter::PaintLarvaSilhouette(_larva, bordermask, Scalar(255), 1.25f, translation);
		bordermask -= mask;
		double underfillFactor = _underFillWeightFactor*mean(roi, bordermask)[0];//punishes small models

		mask.release();
		bordermask.release();

		return (fillFactor-underfillFactor)/255.0;
	}
	return 0;
}


LarvaModel Condensation::predict_sample_position(
	LarvaModel *_oldObject)
{		
	LarvaModel result(_oldObject);

	double spineLength = GeneralTrackerFuncs::Toolbox::GetLength(_oldObject->getSpine());

	//Progression
	double per = 
		(_oldObject->hasPrev() ?
			(_oldObject->getCenter() - _oldObject->getPrev()->getCenter()).length()
			: 1)
		+ this->mRandom.gaussian(0.1
			* spineLength);//Noise

	this->mLarvaController.progress(&result, per);

	// Radial displacement	
	double rad = 
		this->mRandom.gaussian(
			.3*(spineLength-2*per));

	vector<Vector2D> *spine = result.getSpine();
	vector<Vector2D> newspine;
	int k = 0;
	for(vector<Vector2D>::iterator it = spine->begin();
		(it+1) != spine->end();
		it++, k++)
	{				
		double displace = rad*exp(-0.09*(k*k));
		Vector2D origin = Vector2D(*it);
		Vector2D displaceDirection = Vector2D(origin-*(it+1))
			.getOrthogonalVector();				
		displaceDirection.normalize();	
		newspine.push_back((origin + (displaceDirection*displace)));
	}
	k = 0;
	for(vector<Vector2D>::iterator it = spine->begin();
		(it+1) != spine->end();
		it++, k++)
	{
		*it = newspine[k];
	}

	//widths
	per = abs(1. + this->mRandom.gaussian(.15));
	vector<double>* widths = result.getWidths();
	for(vector<double>::iterator wIt = widths->begin();
		wIt != widths->end();
		wIt++)
		*wIt *= per;

	//translation
	Vector2D translation = Vector2D(
		this->mRandom.gaussian(per*0.25),
		this->mRandom.gaussian(per*0.25));
	this->mLarvaController.translate(&result, translation);

	return result;	
}

void Condensation::predict_sample_position(
	LarvaModel					* _oldObject, 
	const Mat					& _obsDensityData,
	double						& mod_cumulTotal,
	vector<LarvaModel>			& mod_sampleContainer, 
	vector<double>				& mod_weightContainer,
	vector<double>				& mod_cumulProbContainer)
{	
	LarvaModel predicted	=		this->predict_sample_position(_oldObject);	
	double sampleWeight		=		this->evaluate_observation_density(predicted, _obsDensityData);
	mod_sampleContainer.push_back	(predicted);
	mod_weightContainer.push_back	(sampleWeight);	
	mod_cumulProbContainer.push_back(mod_cumulTotal);							
	mod_cumulTotal			+=		sampleWeight;
}

/* This is binary search using cumulative probabilities to pick a base
 sample. The use of this routine makes Condensation O(NlogN) where N
 is the number of samples. It is probably better to pick base
 samples deterministically, since then the algorithm is O(N) and
 probably marginally more efficient, but this routine is kept here
 for conceptual simplicity and because it maps better to the
 published literature. */
LarvaModel Condensation::pick_base_sample(
	double				_largest_cumulative_prob,
	vector<LarvaModel>&	_currentDescriptors, 
	vector<double>&		_cumul_prob_array,
	int					_sampleCount)
{
	double choice = (this->mRandom.next()/RAND_MAX) * _largest_cumulative_prob;
	int low, middle, high;
	
	low = 0;
	high = _sampleCount;
	
	while (high > (low+1)) 
	{
		middle = (high+low)/2;
		if (choice > _cumul_prob_array[middle])
			low = middle;
		else 
			high = middle;
	}
	return _currentDescriptors[low];
}


LarvaModel* Condensation::computeMeanModel(
	const vector<LarvaModel>	&	_bases, 
	const vector<double>		&	_weights)
{
	double					weight			= pow(_weights[0],2);
	double					weightSum		= weight;	
	unsigned int			sampleCount		= _bases.size();
	vector<double>			segmentWidths;	//Segment Widths
	vector< Vector2D >		spinePoints;	//Spine Points
	
	//Initialize with first model	
	LarvaModel				model			= _bases[0];
	vector<double>		*	baseWidths		= model.getWidths();
	vector< Vector2D >	*	baseSpine		= model.getSpine();
	unsigned int			spineSize		= baseSpine->size();
	for (unsigned int j = 0; j < spineSize; j++)
	{
		spinePoints.push_back(	Vector2D(baseSpine->at(j))	* weight);
		segmentWidths.push_back(baseWidths->at(j)			* weight);
	}

	//Add remaining models
	for (unsigned int i = 1; i < sampleCount; i++)
	{
		weight								= pow(_weights[i],2);
		weightSum						   += weight;
		model								= _bases[i];
		vector<double>		*	baseWidths	= model.getWidths();
		vector< Vector2D >	*	baseSpine	= model.getSpine();
		unsigned int inversion				= 0;
		if((model.getHead()-_bases[0].getHead()).length() >
			(model.getTail()-_bases[0].getTail()).length())
		{
			inversion = spineSize-1;
		}		
		for (unsigned int j = 0; j < spineSize; j++) 
		{
			spinePoints[j]	+= Vector2D(baseSpine->at(std::abs((int)(inversion-j))))	* weight;
			segmentWidths[j]+= baseWidths->at(j)			* weight;
		}
	}
	
	//Compute mean model
	for (unsigned int i = 0; i < spineSize; i++) 
	{
		spinePoints[i].setX(spinePoints[i].getX() / weightSum);
		spinePoints[i].setY(spinePoints[i].getY() / weightSum);				
		segmentWidths[i] /= weightSum;
	}
	LarvaModel *result = new LarvaModel(_bases[0]);
	result->setSpine(spinePoints);
	result->setCenter(spinePoints[spineSize/2]);
	result->setWidths(segmentWidths);

	return result;
}

LarvaModel* Condensation::chooseBestModel(
	const vector<LarvaModel>	&	_bases, 
	const vector<double>		&	_weights,
	double						&	out_bestFitness)
{
	vector<double>::const_iterator bestWeight = _weights.begin();
	for(vector<double>::const_iterator i = _weights.begin();
		i != _weights.end();
		i++)
	{
		if(*i > *bestWeight)
		{
			bestWeight = i;
		}
	}
	out_bestFitness = *bestWeight;
	return new LarvaModel(_bases[bestWeight - _weights.begin()]);
}

Mat Condensation::getEnergy (
	LarvaModel			*	_prevLarva, 
	const Mat			&	_currentImg,
	const int				_sampleCount)
{
	Mat res = Mat::zeros(_currentImg.size(), CV_64FC1);
	if(_prevLarva->isResolved())
	{
		double estimatedRoiRadius = 2*GeneralTrackerFuncs::Toolbox::GetLength(_prevLarva->getSpine());	
		Vector2D translation(
			std::max(0.,_prevLarva->getCenter().getX()-estimatedRoiRadius),
			std::max(0.,_prevLarva->getCenter().getY()-estimatedRoiRadius));
		int width	= (int)std::min(2*estimatedRoiRadius, res.cols-translation.getX()-1);
		int height	= (int)std::min(2*estimatedRoiRadius, res.rows-translation.getY()-1);
		Mat roi		= res(cv::Rect(
				(int)translation.getX(), 
				(int)translation.getY(), 
				width, 
				height));
		translation*=-1;
		Scalar intensity = Scalar(1.f/(float)_sampleCount);

		//Inverted spine (expect wrong head/tail assignments)
		GeneralTrackerFuncs::LarvaModelManager::InvertSpine(_prevLarva);
		for (int i = _sampleCount/2; i < _sampleCount; i++)        
		{	
			Mat temp = Mat::zeros(roi.size(), CV_64FC1);	
			LarvaModel predictedModel = this->predict_sample_position(_prevLarva);

			GeneralTrackerFuncs::Painter::PaintLarvaSilhouette(
				predictedModel, 
				temp, 
				intensity, 
				1.0f, 
				translation);
			add(roi,temp,roi);
			temp.release();
		}

		//Normal spine
		GeneralTrackerFuncs::LarvaModelManager::InvertSpine(_prevLarva);
		for (int i = _sampleCount/2; i < _sampleCount; i++)        
		{	
			Mat temp = Mat::zeros(roi.size(), CV_64FC1);	
			LarvaModel predictedModel = this->predict_sample_position(_prevLarva);

			GeneralTrackerFuncs::Painter::PaintLarvaSilhouette(
				predictedModel, 
				temp,
				intensity,
				1.0f, 
				translation);
			add(roi,temp,roi);
			temp.release();
		}

		roi *= 255;
	}
	res.convertTo(res, CV_8UC1);

	return res;
}
