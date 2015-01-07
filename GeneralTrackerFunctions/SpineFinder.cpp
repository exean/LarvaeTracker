#include "SpineFinder.h"

namespace GeneralTrackerFuncs
{
	SpineFinder::SpineFinder(			
		const double						_headSize,//small 10 (20) 
		const double						_spanThresh,//small 15 (25)
		const int							_allowedGap,//1
		const int							_minCurvePoints)
		: 
		mSpanThresh(_spanThresh), 
		mAllowedGap(_allowedGap), 
		mMinCurvePoints(_minCurvePoints)
	{
		this->mDistance			= std::sqrt(std::pow(this->mSpanThresh/2,2)+std::pow(_headSize, 2));
		this->mOpeningAngle		= 2* std::asin((this->mSpanThresh/2)/this->mDistance);
	}


	SpineFinder::~SpineFinder(void)
	{
	}

	void SpineFinder::findEndPoints(
		vector<Point>					*	_contour, 
		const Mat						&	_image, 
		vector<Point>					*	out_potentialEndpoints, 
		vector<Point>					*	out_correspondingBorderPoints0, 
		vector<Point>					*	out_correspondingBorderPoints1, 
		vector<double>					*	out_endPointIntensityValues,
		vector<Vector2D>				*	out_endPointCorrespondingSpinePoints,
		vector<float>					*	out_correspondingSpineWidths)
	{	
		unsigned int currentGap = 0;//_allowedGap+1;
		unsigned int currentCurvePoints = 0;
		vector<Point>::iterator currentCurveStartPoint	= _contour->end();
		vector<Point>::iterator firstCurvePoint			= _contour->begin();
		vector<Point>::iterator firstCurveEndPoint		= _contour->begin();
		vector<Point>::iterator lastDetectedCurvePoint	= _contour->begin();
		vector<Point> currentCurvatureRegion;
		vector<Point> firstCurvatureRegion;
	
		Mat smoothedIntensityImage = _image.clone();
		cv::medianBlur(smoothedIntensityImage, smoothedIntensityImage, 5);
	
		//Consider all points of the contour in turn
		for (vector<Point>::iterator it = _contour->begin(); it != _contour->end(); it++)
		{
			//Points of euklid. distance
			Point right, left;
			try
			{
				//Right side
				right = this->getPointOfDistance(it,_contour,this->mDistance,1);//getPointAlongContour(it,_contour,this->mDistance,1);
				//left side
				left = this->getPointOfDistance(it,_contour,this->mDistance,-1);//getPointAlongContour(it,_contour,this->mDistance,-1);
			}
			catch(...)
			{
				cout << "Contour too small." << endl;
				continue;
			}
			//Points of euklid. distance - eof

			//Check if result is valid
			double span = GeneralTrackerFuncs::Toolbox::GetDistance(left, right);
	
			if(span < this->mSpanThresh)			// Valid amount of curvature
			{
				if(currentGap > this->mAllowedGap)	// First Point of high-curvature-region
				{
					currentGap = 0;
					currentCurveStartPoint = it;
					currentCurvePoints = 0;
				
					if(firstCurvatureRegion.size() == 0)
						firstCurvatureRegion = currentCurvatureRegion;
					currentCurvatureRegion.clear();
				}
				if(firstCurvePoint == _contour->end())
					firstCurvePoint = it;
			
				lastDetectedCurvePoint = it;
				currentCurvatureRegion.push_back(*it);

				currentCurvePoints++;//Exact count is kept b/c gaps should not influence estimated curvature-region-size			
			}
			else
			{

				if(currentGap == this->mAllowedGap						// End of high-curvature-region, ...
				   && currentCurvePoints >= this->mMinCurvePoints)		// ... if region is large enough, ... 
				{
					// ... determine its center-point and add it.
					if(currentCurveStartPoint != _contour->end())
					{							
						vector<Point>::iterator endpoint;
						double intensity = -1;
						this->findHighCurvatureRegionCenter(
							currentCurveStartPoint,
							lastDetectedCurvePoint,
						//	boolImage,
							smoothedIntensityImage,
							_contour,
						//	currentCurvatureRegion,
							endpoint,
							intensity);
						if(intensity != -1)
						{
							out_potentialEndpoints->push_back(*endpoint);
							out_endPointIntensityValues->push_back(intensity);	

							Vector2D spinepoint;
							Point borderPoint0, borderPoint1;
							float radius;
							vector<Point> region; 
							region.push_back(*currentCurveStartPoint);
							region.push_back(*lastDetectedCurvePoint);
							region.push_back(*endpoint);

							this->layInCircle(_contour, region,*endpoint, radius, spinepoint, borderPoint0, borderPoint1);
							out_correspondingSpineWidths->push_back(2*radius);
							out_endPointCorrespondingSpinePoints->push_back(spinepoint);
							out_correspondingBorderPoints0->push_back(borderPoint0);
							out_correspondingBorderPoints1->push_back(borderPoint1);
						}
					}
					else 
					{
						firstCurveEndPoint = lastDetectedCurvePoint;
					}

				}
				currentGap++;
			}
		}
	
		//Handle Curvature regions at beginning and end of contour
		unsigned int initialGap			= std::distance(_contour->begin(), firstCurvePoint);
		unsigned int pointsAtBeginning		= std::distance(firstCurvePoint, firstCurveEndPoint);
		if(currentCurveStartPoint != _contour->end() &&
			initialGap+currentGap <= this->mAllowedGap)//Wrapped curve
		{
			if (currentCurvePoints+pointsAtBeginning >= this->mMinCurvePoints)		
			{	
				//Wrap curve region is large enough, ... 
				//...determine its center-point and add it.
				Point p = Point(*firstCurveEndPoint - *currentCurveStartPoint);
				p.x = currentCurveStartPoint->x+p.x/2;
				p.y = currentCurveStartPoint->y+p.y/2;
				if (cv::pointPolygonTest(*_contour, p, false) >= 0)//Check curvature region is valid (convex)
				{
					if(currentCurvePoints > pointsAtBeginning)//center point at end
						out_potentialEndpoints->push_back(*(currentCurveStartPoint+(currentCurvePoints+pointsAtBeginning)/2));
					else//center point at start
						out_potentialEndpoints->push_back(*(firstCurveEndPoint-(currentCurvePoints+pointsAtBeginning)/2));
			
					currentCurvatureRegion.insert(currentCurvatureRegion.end(), firstCurvatureRegion.begin(), firstCurvatureRegion.end());
					//determine intensity value for head/tail check					
					double intensity = Toolbox::GetMeanIntensity(smoothedIntensityImage, p, (*(out_potentialEndpoints->end()-1)));
				
					out_endPointIntensityValues->push_back(intensity);

					Vector2D spinepoint;
					Point borderPoint0, borderPoint1;
					float radius;
					vector<Point> region; 
					region.push_back(*currentCurveStartPoint);
					region.push_back(*firstCurveEndPoint);
					region.push_back((*(out_potentialEndpoints->end()-1)));

					this->layInCircle(_contour, region, (*(out_potentialEndpoints->end()-1)), radius, spinepoint, borderPoint0, borderPoint1);
					out_correspondingSpineWidths->push_back(2*radius);
					out_endPointCorrespondingSpinePoints->push_back(spinepoint);
					out_correspondingBorderPoints0->push_back(borderPoint0);
					out_correspondingBorderPoints1->push_back(borderPoint1);
				}
			}
		}
		//Seperate curvature regions at beginning and end
		else
		{
			//unhandled curvature region at beginning of contour
			if(initialGap <= this->mAllowedGap//else already handled
			   && pointsAtBeginning >= this->mMinCurvePoints)
			{
				vector<Point>::iterator endpoint;
				double intensity = -1;

				this->findHighCurvatureRegionCenter(
					firstCurvePoint,
					firstCurveEndPoint,
					//boolImage,
					smoothedIntensityImage,
					_contour,
				//	firstCurvatureRegion,
					endpoint,
					intensity);
				
						
				if(intensity != -1)
				{
					out_potentialEndpoints->push_back(*endpoint);
					out_endPointIntensityValues->push_back(intensity);

					Vector2D spinepoint;
					Point borderPoint0, borderPoint1;
					float radius;
					vector<Point> region; 
					region.push_back(*firstCurvePoint);
					region.push_back(*firstCurveEndPoint);
					region.push_back(*endpoint);

					this->layInCircle(_contour, region, *endpoint, radius, spinepoint, borderPoint0, borderPoint1);
					out_correspondingSpineWidths->push_back(2*radius);
					out_endPointCorrespondingSpinePoints->push_back(spinepoint);
					out_correspondingBorderPoints0->push_back(borderPoint0);
					out_correspondingBorderPoints1->push_back(borderPoint1);
				}
			}
		
			//Curve at end
			if(currentCurveStartPoint != _contour->end() 
				&& currentGap <= this->mAllowedGap
			    && currentCurvePoints >= this->mMinCurvePoints)
			{			
				vector<Point>::iterator endpoint;
				double intensity = -1;

				this->findHighCurvatureRegionCenter(
					currentCurveStartPoint,
					lastDetectedCurvePoint,
					//boolImage,
					smoothedIntensityImage,
					_contour,
			//		currentCurvatureRegion,
					endpoint,
					intensity);

				if(intensity != -1)
				{
					out_potentialEndpoints->push_back(*endpoint);
					out_endPointIntensityValues->push_back(intensity);

					Vector2D spinepoint;
					Point borderPoint0, borderPoint1;
					float radius;
					vector<Point> region; 
					region.push_back(*currentCurveStartPoint);
					region.push_back(*lastDetectedCurvePoint);
					region.push_back(*endpoint);

					this->layInCircle(_contour, region, *endpoint, radius, spinepoint, borderPoint0, borderPoint1);
					out_correspondingSpineWidths->push_back(2*radius);
					out_endPointCorrespondingSpinePoints->push_back(spinepoint);
					out_correspondingBorderPoints0->push_back(borderPoint0);
					out_correspondingBorderPoints1->push_back(borderPoint1);
				}
			}
		}
	}

	Point SpineFinder::getPointAlongContour(
		const vector<Point>::const_iterator		&	_start,
		const vector<Point>						*	_contour,
		const double								_distance,
		const char									_direction)
	{
		// go distance [_distance] from _start along contour
		//Right side
		vector<Point>::const_iterator it0, it1 = _start;
		double currentDistance = _distance;
		while(currentDistance > 0)
		{	
			it0 = it1;
			if(it1 == _contour->begin() && _direction < 0)
				it1 = _contour->end();
			it1 += _direction;
			if(it1 == _contour->end())
				it1 = _contour->begin();
			
			currentDistance -= GeneralTrackerFuncs::Toolbox::GetDistance(*it0, *it1);	

			if(it1 == _start && currentDistance > 0)
			{
				std::cout << "contour too small" << std::endl;
				break;
			}			
		}
		//get exact point between it0 and it1
		Vector2D v = Vector2D(*it0-*it1); 
		v.normalize(); 
		v *= -currentDistance;

		return (v + *it1).toPoint();
	}

	Point SpineFinder::getPointOfDistance(
		const vector<Point>::const_iterator		&	_start,
		const vector<Point>						*	_contour,
		const double								_distance,
		const char									_direction,
		const bool									_exact)
	{
		// find [_distance] distanced Point
		vector<Point>::const_iterator it0, it1 = _start;
		double currentDistance = _distance;
		do
		{	
			it0 = it1;
			if(it1 == _contour->begin() && _direction < 0)
				it1 = _contour->end();
			it1 += _direction;
			if(it1 == _contour->end())
				it1 = _contour->begin();
			
			if(GeneralTrackerFuncs::Toolbox::GetDistance(*_start, *it0) < _distance
				&& GeneralTrackerFuncs::Toolbox::GetDistance(*_start, *it1) >= _distance)
			{	
				if(_exact)
				{
					//get exact point between it0 and it1
					Vector2D direction(*it1-*it0); 
					direction.normalize();
					Vector2D result(*it0);
					while((result-*_start).length()-_distance > 1)
					{
						result += direction;
					}				
					return result.toPoint();
				}
				else
				{
					return *it1;
				}
			}		
		}
		while(it1 != _start);
		throw "Contour too small";
	}

	vector< vector<Point> > SpineFinder::getHighCurvatureRegions(
		const vector<Point>				*	_contour)
	{
		unsigned int currentGap							= this->mAllowedGap+1;//Init with >allowedGap so no attempt to close a curvature-region during first loop is made
		unsigned int initialGap							= 0;
		vector<Point>::const_iterator currentCurveStartPoint= _contour->end();
		vector<Point>::const_iterator lastDetectedCurvePoint= _contour->begin();
		vector<Point> currentCurvatureRegion;
		vector<Point> firstCurvatureRegion;		
	
		vector< vector<Point> > curvatureRegions;//Result

		//Consider all points of the contour in turn
		for (vector<Point>::const_iterator it = _contour->begin(); 
			it != _contour->end(); 
			it++)
		{
			Point right, left;
			try
			{
				//Right side
				right = this->getPointOfDistance(it,_contour,this->mDistance,1);//getPointAlongContour(it,_contour,this->mDistance,1);
				//left side
				left = this->getPointOfDistance(it,_contour,this->mDistance,-1);//getPointAlongContour(it,_contour,this->mDistance,-1);
			}
			catch(...)
			{
				cout << "Contour too small" << endl;
				currentGap++;
				continue;
			}

			//Check if result is valid
			double a = GeneralTrackerFuncs::Toolbox::GetDistance(left, right);
			double openingAngle = std::acos(-(a*a-2*(this->mDistance*this->mDistance))/(2*this->mDistance*this->mDistance));
	
			if(openingAngle <= this->mOpeningAngle)	// Valid amount of curvature
			{
				if(currentGap > this->mAllowedGap)	// First Point of new high-curvature-region
				{	
					currentCurveStartPoint = it;
					currentCurvatureRegion.clear();
				}
				currentGap = 0;		
			
				lastDetectedCurvePoint = it;
				currentCurvatureRegion.push_back(*it);		
			}
			else			// Not enough curvature, either gap or end of curve
			{
				if(currentGap == this->mAllowedGap)						// End of high-curvature-region
				{
					if(firstCurvatureRegion.empty())
					{
						//First curvature region will be examined together with last one.
						firstCurvatureRegion	= currentCurvatureRegion;
						initialGap				= std::distance(_contour->begin(), currentCurveStartPoint);
					}
					else if(currentCurvatureRegion.size() >= this->mMinCurvePoints)// Is region large enough?
					{
						//Check if curvature region is valid (convex)
						Point p = Point(*currentCurveStartPoint - *lastDetectedCurvePoint);
						p.x		= lastDetectedCurvePoint->x+p.x/2;
						p.y		= lastDetectedCurvePoint->y+p.y/2;

						if (cv::pointPolygonTest(*_contour, p, false) > 0)
						{
							curvatureRegions.push_back(currentCurvatureRegion);
						}
					}
				}
				currentGap++;
			}
		}
	
		//Handle Curvature regions at beginning and end of contour
		if(initialGap + currentGap <= this->mAllowedGap)//Wrapped curve
		{
			if (currentCurvatureRegion.size() + firstCurvatureRegion.size() >= this->mMinCurvePoints)		
			{	
				//Wrap curve region is large enough
				currentCurvatureRegion.insert(
					currentCurvatureRegion.end(), 
					firstCurvatureRegion.begin(), 
					firstCurvatureRegion.end());

				//Check if curvature region is valid (convex)
				Point p(*currentCurveStartPoint - *lastDetectedCurvePoint);
				p.x		= lastDetectedCurvePoint->x+p.x/2;
				p.y		= lastDetectedCurvePoint->y+p.y/2;

				if (cv::pointPolygonTest(*_contour, p, false) > 0)
				{			
					curvatureRegions.push_back(
						currentCurvatureRegion);
				}
			}
		}
		else
		{	//Seperate curvature regions at beginning and end
			//Curve at beginning:
			if(firstCurvatureRegion.size() >= this->mMinCurvePoints)
			{
				//Check if curvature region is valid (convex)
				Point p(*(firstCurvatureRegion.end()-1)  - *firstCurvatureRegion.begin());
				p.x		= firstCurvatureRegion.begin()->x+p.x/2;
				p.y		= firstCurvatureRegion.begin()->y+p.y/2;

				if (cv::pointPolygonTest(*_contour, p, false) > 0)
				{
					curvatureRegions.push_back(
						firstCurvatureRegion);
				}
			}

			//Curve at end:
			if(currentGap < this->mAllowedGap //Implies that last detected curve has not yet been dealt with
				&& currentCurvatureRegion.size() >= this->mMinCurvePoints)
			{
				//Check if curvature region is valid (convex)
				Point p(*currentCurveStartPoint - *lastDetectedCurvePoint);
				p.x		= lastDetectedCurvePoint->x+p.x/2;
				p.y		= lastDetectedCurvePoint->y+p.y/2;

				if (cv::pointPolygonTest(*_contour, p, false) > 0)
				{
					curvatureRegions.push_back(
						currentCurvatureRegion);
				}
			}
		}

		return curvatureRegions;
	}

	void SpineFinder::findSpine(
		vector<Point>					*	_contour, 
		const Vector2D					&	_head,
		const Point						&	_head_border0,
		const Point						&	_head_border1,
		const double					&	_head_width,
		const Vector2D					&	_tail,
		const Point						&	_tail_border0,
		const Point						&	_tail_border1,
		const double					&	_tail_width,
		const double					&	_maxWidth,
		vector<Vector2D>				*	out_spine, 
		vector<double>					*	out_segWidths,
		const int							_requestedSegmentCount)
	{
		//1.: find corresponding contour iterators for head/tail-border-points
		vector<Point>::const_iterator head_border0_it =
			Toolbox::GetNearestPointOnContour(_contour, _head_border0);
		vector<Point>::const_iterator head_border1_it =
			Toolbox::GetNearestPointOnContour(_contour, _head_border1);
		vector<Point>::const_iterator tail_border0_it =
			Toolbox::GetNearestPointOnContour(_contour, _tail_border0);
		vector<Point>::const_iterator tail_border1_it =
			Toolbox::GetNearestPointOnContour(_contour, _tail_border1);

		//2: determine corresponding head-/tail-borderpoints by distance along contour
		int contSize		= _contour->size();
//		int distHeadHead	= Toolbox::GetIteratorDistance(head_border0_it, head_border1_it, contSize);	
		int distHeadTail00	= Toolbox::GetIteratorDistance(head_border0_it, tail_border0_it, contSize);
		int distHeadTail01	= Toolbox::GetIteratorDistance(head_border0_it, tail_border1_it, contSize);
		int distHeadTail10	= Toolbox::GetIteratorDistance(head_border1_it, tail_border0_it, contSize);
		int distHeadTail11	= Toolbox::GetIteratorDistance(head_border1_it, tail_border1_it, contSize);

		vector<Point>::const_iterator headLonger	= head_border0_it;
		vector<Point>::const_iterator headShorter	= head_border1_it;
		vector<Point>::const_iterator tailLonger	= tail_border0_it;
		vector<Point>::const_iterator tailShorter	= tail_border1_it;

		//does shortest side begin at head0? (alternate head1)
		bool shorterSideIs0 = std::min(distHeadTail00, distHeadTail01) < std::min(distHeadTail10, distHeadTail11);
		if(shorterSideIs0)
		{
			headShorter	= head_border0_it;
			headLonger	= head_border1_it;
			if(distHeadTail00 < distHeadTail01)
			{
				tailLonger	= tail_border1_it;
				tailShorter = tail_border0_it;
			}
		}
		else if(distHeadTail10 < distHeadTail11)
		{
			if(distHeadTail10 < distHeadTail11)
			{
				tailLonger	= tail_border1_it;
				tailShorter = tail_border0_it;
			}
		}

		short step = 1;
		if(Toolbox::GetIteratorDistance(
				tailShorter,
				(_contour->begin()+((contSize + ((std::distance((vector<Point>::const_iterator)_contour->begin(), headShorter) - 1) % contSize)) % contSize)),
				contSize)
			> Toolbox::GetIteratorDistance(tailShorter, headShorter, contSize))				
			step = -1;

		//3.: compute full detail spine
		vector<Vector2D>	fullSpine;
		vector<double>		fullWidths;
		//Insert head
		fullSpine.push_back(_head);
		fullWidths.push_back(_head_width);
		// For each point on longer side find corresponding point(s) on shorter side
		for (vector<Point>::const_iterator i = headLonger;
			 i != tailLonger; 
			 i = _contour->begin()+((contSize + ((std::distance((vector<Point>::const_iterator)_contour->begin(), i) + step) % contSize)) % contSize)) 
		{
			double minDistance = Toolbox::GetMaxDouble();
			vector<Point>::const_iterator bestMatch = tailShorter;
			for (vector<Point>::const_iterator j = headShorter; 
				 j != tailShorter; 
				 j = _contour->begin()+((contSize+((std::distance((vector<Point>::const_iterator)_contour->begin(), j) - step) % contSize)) % contSize))
			{
				double currentDistance = Toolbox::GetDistance(*i, *j);
				if (currentDistance < minDistance) 
				{
					minDistance = currentDistance;
					bestMatch = j;
				}
			}
			if(bestMatch != tailShorter)
			{
				// Insertion of centerpoint between i and bestMatch:
				Point p((*i)-(*bestMatch));
				if(minDistance > _maxWidth)
				{// Outside bounds -> likely wrong measurement
					continue;
				}
				fullWidths.push_back(minDistance);//width);
				p.x = (*bestMatch).x+p.x/2;
				p.y = (*bestMatch).y+p.y/2;
				fullSpine.push_back(p);
			}
		}
		//Insert Tail
		fullSpine.push_back(_tail);
		fullWidths.push_back(_tail_width);

		//4.: reduce spine to requested segment count
		this->reduceSpine(&fullSpine, &fullWidths, _requestedSegmentCount, out_spine, out_segWidths);
	}

	 void SpineFinder::findHighCurvatureRegionCenter(
		vector<Point>::iterator		_currentCurveStartPoint,
		vector<Point>::iterator		_lastDetectedCurvePoint,	
		const Mat				&	_smoothedIntensityImage,
		const vector<Point>		*	_contour,
//		const vector<Point>		&	_currentCurvatureRegion,
		vector<Point>::iterator &	out_endpoint,
		double					&	out_intensity)
	{
		Point p = Point(*_currentCurveStartPoint - *_lastDetectedCurvePoint);
		p.x = _lastDetectedCurvePoint->x+p.x/2;
		p.y = _lastDetectedCurvePoint->y+p.y/2;
	
		if (cv::pointPolygonTest(*_contour, p, false) >= 0)//Check curvature region is valid (convex)
		{	
			out_endpoint = (_currentCurveStartPoint+std::distance(_currentCurveStartPoint, _lastDetectedCurvePoint)/2);		
			out_intensity = Toolbox::GetMeanIntensity(_smoothedIntensityImage, p, *out_endpoint);
		}
	}
			//	//determine intensity value for head/tail check
			//double intensity = Toolbox::GetMeanIntensity(smoothedIntensityImage, p, *foundPoint);
			//endPointIntensityValue = intensity; 

	//Reduce amount of points of spine-polyline
	void SpineFinder::reduceSpine(
		vector<Vector2D>				*	_spine,
		vector<double>					*	_segWidths,
		int									_segmentCount, 
		vector<Vector2D>				*	out_reducedSpine, 
		vector<double>					*	out_reducedSegWidths)
	{	
		//Calculate length of spine
		double spineLength = Toolbox::GetLength(_spine);

		double segmentSize = spineLength / _segmentCount;

		if(segmentSize == 0)
		{
			return;
		}
	
		//Head
		out_reducedSpine->push_back(*_spine->begin());
		out_reducedSegWidths->push_back(*_segWidths->begin());
	
		//Center
		double currentSegmentLength = 0;
		vector<double>::iterator widthIt = _segWidths->begin()+1; 
		for (vector<Vector2D>::iterator spineIt = _spine->begin()+1; 
			 spineIt != _spine->end();
			 spineIt++, widthIt++)
		{
			double currentDistance = (*(spineIt-1) - *spineIt).length();

			if(currentDistance == 0)
			{
				//Special case, if not checked this might lead to problems
				//due to double precision in exact endpoint computation in 
				//successive while-loop
				continue;
			}
			
			while(currentSegmentLength+currentDistance >= segmentSize)
			{
					//Compute endpoint of current segment
				double overlap = (currentSegmentLength+currentDistance)-segmentSize;
				Vector2D p0(*(spineIt-1));
				Vector2D p1(*spineIt);
				Vector2D v(p0-p1);
				v.normalize();
				v *= overlap;
				p1 += v;//=endpoint of current segment
				
				//Spine point
				out_reducedSpine->push_back(p1);
				
				//Segwidth
				out_reducedSegWidths->push_back((*(widthIt-1) * (currentDistance-overlap) + *widthIt * overlap) / currentDistance);

				//Update distances
				currentDistance = (p1 - *spineIt).length();
				currentSegmentLength = 0;

				//If all segments assigned skip remaining spine points
				if (--_segmentCount <= 1) 
				{
					spineIt = _spine->end()-1;//-->end loop
					break;
				}
			}
		
			currentSegmentLength += currentDistance;
		}
	
		//Tail
		out_reducedSpine->push_back(*(_spine->end()-1));
		out_reducedSegWidths->push_back(*(_segWidths->end()-1));	
	}

	void SpineFinder::layInCircle(
		const	vector<Point>	* _contour,
				vector<Point>	  _curvaturePoints,
		const	Point			& _endpoint,
				float			& out_radius,
				Vector2D		& out_spinePoint,
				Point			& out_border0,
				Point			& out_border1)
	{
		Point2f		centerPoint; 
		float		radius;
		Mat			curvaturePointsMat(_curvaturePoints);
		minEnclosingCircle(curvaturePointsMat, centerPoint, radius);
		curvaturePointsMat.release();
		//make circle smaller, keep attached to highest curvature point

		Vector2D center(centerPoint);
		Vector2D side;
		bool change = true; 
		do
		{
			change = false;
			Vector2D front(_endpoint);
			front -= center;

			if(!(front.getX() || front.getY()))
			{
				break;
			}

			front.normalize();
			side = front.getOrthogonalVector();

			double frontOverlap = cv::pointPolygonTest(*_contour, (center+(front*radius)).toPoint(), true);
			if(frontOverlap < 0)
			{
				center += front*frontOverlap;
				change = true;
			}
			
			while(radius > 1)			
			{	
				double distanceToContour = std::max(
					cv::pointPolygonTest(*_contour, (center+side*radius).toPoint(), true),
					cv::pointPolygonTest(*_contour, (center+side*-radius).toPoint(), true));
				if(distanceToContour < 0)//smaller zero->outside of contour
				{
					//radius too large for section
					radius += (float)distanceToContour;
					center += front * -distanceToContour;
					change = true;
				}
				else
				{
					break;
				}
			}

			//90° (general fit)
			if(radius > 1 
				&& cv::pointPolygonTest(*_contour, (center+(side*radius)).toPoint(), false) < 0)
			{
				radius -= 0.5f;
				center -= side*0.5f;
				center += front*0.5f;
				change = true;

				//Update front+side
				front = (center*-1 + _endpoint);
				if(!(front.getX() || front.getY()))
				{
					break;
				}
				front.normalize();
				side = front.getOrthogonalVector();
			}
				
			if(radius > 1 
				&& cv::pointPolygonTest(*_contour, (center+(side*-radius)).toPoint(), false) < 0)
			{
				radius -= 0.5f;
				center += side*0.5f;
				center += front*0.5f;
				change = true;

				//Update front+side
				front = (center*-1 + _endpoint);
				if(!(front.getX() || front.getY()))
				{
					break;
				}
				front.normalize();
				side = front.getOrthogonalVector();
			}

			//45° (more exact fit)
			Vector2D mid(front+side);
			mid.normalize();
			if(radius > 1 
				&& cv::pointPolygonTest(*_contour, (center+mid*radius).toPoint(), false) < 0)
			{
				radius -= 0.5f;
				center -= side*0.5f;
				center += front*0.5f;
				change = true;

				//Update front+side
				front = (center*-1 + _endpoint);
				if(!(front.getX() || front.getY()))
				{
					break;
				}
				front.normalize();
				side = front.getOrthogonalVector();
			}

			mid = (front-side);
			mid.normalize();
			if(radius > 1
				&& cv::pointPolygonTest(*_contour, (center+mid*radius).toPoint(), false) < 0)
			{
				radius -= 0.5f;
				center += side*0.5f;
				center += front*0.5f;
				change = true;
			}
		}
		while(radius > 1 && change);

		//Set results
		out_radius = std::abs(radius);
		out_spinePoint = center;
		out_border0 = (center+side*-radius).toPoint();
		out_border1 = (center+side*radius).toPoint();
	}

#pragma region AlternateMethods
	//void SpineFinder::layInCircle(//GROWING circle
	//	const	vector<Point>	* _contour,
	//			vector<Point>	  _curvaturePoints,
	//	const	Point			& _endpoint,
	//			float			& out_radius,
	//			Point			& out_spinePoint,
	//			Point			& out_border0,
	//			Point			& out_border1)
	//{
	//	out_border0 = *_curvaturePoints.begin();
	//	out_border1 = *(_curvaturePoints.end()-1);		
	//	Vector2D centerPoint(out_border0 - out_border1);		
	//	centerPoint *= 0.5;
	//	centerPoint += out_border1;
	//	out_radius = cv::pointPolygonTest(*_contour, centerPoint.toPoint(), true); 

	//	int index0 = 0, index1 = 0;

	//	while((centerPoint-_endpoint).length()-out_radius > 1)
	//	{
	//		//Update spinepoint
	//		//move in direction to endpoint
	//		Vector2D direction(centerPoint-_endpoint);
	//		direction *= -1;
	//		direction.normalize();
	//		centerPoint += direction;

	//		//realign
	//		double minDist = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
	//		int index = index0;
	//		for(vector<Point>::const_iterator curveIt = _curvaturePoints.begin()+index0;
	//			*curveIt != _endpoint;
	//			curveIt++)
	//		{
	//			index++;
	//			double currentDist = (centerPoint - *curveIt).length();
	//			if(currentDist < minDist)
	//			{
	//				minDist = currentDist;
	//				out_border0 = *curveIt;
	//				index0 = index;
	//			}				
	//		}
	//		minDist = GeneralTrackerFuncs::Toolbox::GetMaxDouble();
	//		index = index1;
	//		for(vector<Point>::const_iterator curveIt = _curvaturePoints.end()-1-index1;
	//			*curveIt != _endpoint;
	//			curveIt--)
	//		{
	//			index++;
	//			double currentDist = (centerPoint - *curveIt).length();
	//			if(currentDist < minDist)
	//			{
	//				minDist = currentDist;
	//				out_border1 = *curveIt;
	//				index1 = index;
	//			}
	//		}
	//		centerPoint = Vector2D(out_border0 - out_border1);		
	//		centerPoint *= 0.5;
	//		centerPoint += out_border1;

	//		//Update radius
	//		out_radius = cv::pointPolygonTest(*_contour, centerPoint.toPoint(), true); 
	//	}	

	//	out_spinePoint = centerPoint;
	//}

		//void SpineFinder::findEndPoints_highestCurvature(
	//	vector<Point>					*	contour,
	//	vector<vector<Point>::iterator> *	potentialEndpoints, 
	//	vector<double>					*	endPointIntensityValues, 
	//	const Mat						&	image, 
	//	double								distance, 
	//	double								spanThresh)
	//{
	//	vector<float> curvature;

	//	//Erode for sharper curves
	//	cv::Mat erodedTmp;
	//	cv::Mat element(5,5,CV_8U,cv::Scalar(1));	//Choose size according to noise	
	//	cv::erode(image, erodedTmp, element);

	//	
	//	//Find points with highest curvature
	//	vector<Point>::iterator it = contour->begin();
	//	while(it != (contour->end()))
	//	{	
	//		//find points on contour of min distance [distance] to (*it) 

	//		//Right side
	//		vector<Point>::iterator it1 = it;
	//		while(sqrt(pow(double((*it).x-(*it1).x),2)+pow(double((*it).y-(*it1).y),2)) < distance)
	//		{			
	//			if(++it1 == (*contour).end())
	//				it1 = (*contour).begin();
	//			if(it1==it)
	//			{
	//				std::cout << "contour too small" << std::endl;
	//				break;// contour too small				
	//			}
	//		}
	//		Point right = (*it1);//Contour resolution is high enough to avoid calculating exact point of distance [distance]

	//		//left side
	//		it1 = it;
	//		while(Toolbox::GetDistance((*it),(*it1)) < distance)
	//		{			
	//			if(it1 == (*contour).begin())
	//				it1 = (*contour).end();
	//			--it1;
	//			if(it1==it)
	//			{
	//				break;// contour too small
	//			}
	//		}
	//		Point left = (*it1);//Contour resolution is high enough to avoid calculating exact point of distance [distance]

	//		//Check if result is valid
	//		double span = Toolbox::GetDistance(left, right);
	//		if(span < spanThresh)
	//		{
	//			potentialEndpoints->push_back(it);
	//			curvature.push_back((float)span);
	//		}

	//		++it;
	//	}
	//	
	//	//Display highest curvature points	
	//	float min1 = (float)spanThresh, min2 = (float)spanThresh;
	//	int ind1 = 0, ind2 = 0;
	//	for(vector<float>::iterator spanIt = curvature.begin();
	//		spanIt != curvature.end();
	//		spanIt++)
	//	{
	//		if(*spanIt <= min1)
	//		{
	//			min2 = min1;
	//			ind2 = ind1;

	//			min1 = *spanIt;			
	//			ind1 = spanIt - curvature.begin();
	//		}
	//		else if(*spanIt < min2)
	//		{
	//			min2 = *spanIt;
	//			ind2 = spanIt - curvature.begin();
	//		}
	//	}
	//}
#pragma endregion

}