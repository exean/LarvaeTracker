#include "Toolbox.h"

namespace GeneralTrackerFuncs
{
	Toolbox::Toolbox(void)
	{
	}


	Toolbox::~Toolbox(void)
	{
	}


	Vector2D Toolbox::GetMoment(
		const	vector<Point>						& _contour)
	{
		Moments mom = moments(cv::Mat(_contour));
		return Vector2D(mom.m10/mom.m00,mom.m01/mom.m00);
	}

	int	Toolbox::GetIteratorDistance(
		const std::vector<Point>::const_iterator	& _i0, 
		const std::vector<Point>::const_iterator	& _i1,
		const int									& _contourSize)
	{
		int res = std::distance(_i0, _i1);
		if(res < 0)
		{
			if(res > -_contourSize/2)
			{
				res *= -1;
			}
			else
			{
				res += _contourSize;	
			}
		}
		else if(res > _contourSize/2)
		{
			res = _contourSize - res;
		}
		return res;
	}

	double Toolbox::GetMeanIntensity(
		const	cv::Mat								&	_image, 
		const	cv::Point							&	_p1, 
		const	cv::Point							&	_p2)
	{
		//determine intensity value for head/tail check
		//Create mask
		cv::Mat mask = cv::Mat::zeros(_image.rows, _image.cols, 0);
		line(mask, _p1, _p2, Scalar(255,255,255), 1, 1, 0);
		double result = cv::mean(_image, mask)[0];
		mask.release();
		return result;
	}

	double Toolbox::GetDistance(const cv::Point & p1, const cv::Point & p2)
	{
		return sqrt(pow(double(p1.x-p2.x),2)+pow(double(p1.y-p2.y),2));
	}

	Contour *Toolbox::GetNearestContour(
		const Vector2D								&	_p, 
		const vector<Contour*>						&	_contours)
	{	
		Contour *nearest		= NULL;
		double leastDistance	= std::numeric_limits<double>::max();
	
		for (vector< Contour* >::const_iterator it = _contours.begin(); 
			it != _contours.end(); 
			it++)
		{				
			Vector2D *moment = (*it)->getCenterOfMass();
			double distance = sqrt(pow(double(_p.getX()-moment->getX()),2)+pow(double(_p.getY()-moment->getY()),2));		
			if(distance <= leastDistance)
			{
				nearest = (*it);
				leastDistance = distance;
			}
		}

		return nearest;//No fitting correspondence found
	}

	double Toolbox::GetLength(
		const vector< Vector2D >						*	_polyline)
	{
		if(_polyline->size() < 2)
		{
			return 0;
		}

		double length = 0;
		for (vector<Vector2D>::const_iterator i = _polyline->begin()+1;
			 i != _polyline->end();
			 i++) 
		{
			length += (*(i-1) - *i).length();
		}
		return length;
	}

	double Toolbox::GetMinimalDistance(
		const Vector2D								&	_p, 
		const vector< Point >						&	_polyline)
	{			
		if(_polyline.empty())
			return GetMaxDouble();

		Vector2D currentFromP((_p*-1)+_polyline[0]);
		double dist = currentFromP.length();
		for(vector< Point >::const_iterator it = _polyline.begin()+1;
			it != _polyline.end();
			it++)
		{
			Vector2D lastPolyPoint(*(it-1));
			Vector2D lastFromP = currentFromP;
			currentFromP = ((_p*-1) + *it);
			Vector2D toLast = lastPolyPoint-*it;
			double t = (toLast.getX()*lastFromP.getX() + toLast.getY()*lastFromP.getY())/toLast.length();

			if (t<0.)
				continue;

			double currentDist =
				(t<=1.) ? currentDist = pow(toLast.getX()*currentFromP.getY() - toLast.getY()*currentFromP.getX(), 2)/toLast.length()
						: currentDist = currentFromP.length();

			dist = min(dist, currentDist);
		}						
							
		return dist;
	}

	double Toolbox::GetMinimalDistance(
		const Vector2D								&	_p, 
		const vector< Vector2D >					&	_polyline)
	{			
		if(_polyline.empty())
			return GetMaxDouble();

		Vector2D currentFromP((_p*-1)+_polyline[0]);
		double dist = currentFromP.length();
		for(vector< Vector2D >::const_iterator it = _polyline.begin()+1;
			it != _polyline.end();
			it++)
		{
			dist = min(dist, GetDistance(_p, *(it-1), *it));
		}						
							
		return dist;
	}

	double Toolbox::GetDistance(
		const Vector2D								&	_p, 
		const Vector2D								&	_lineStart,
		const Vector2D								&	_lineEnd)
	{
		Vector2D n(_lineStart.getX()-_lineEnd.getX(), _lineStart.getY()-_lineEnd.getY());				

		double dist  = n.length();

		if(dist == 0)
			return (_lineStart-_p).length();

		double dist0 = (_p-_lineStart).length();
		double dist1 = (_p-_lineEnd).length();
				
		if(dist < dist0 || dist < dist1)
		{
			//distance to endpoint of line
			return std::min(dist1, dist0);			
		}
		
		if(n.getX() == 0)
		{
			//lazy solution for upright lines
			return GetDistance(
				Vector2D(_p.getY(),_p.getX()),
				Vector2D(_lineStart.getY(),_lineStart.getX()),
				Vector2D(_lineEnd.getY(),_lineEnd.getX()));
		}

		//Distance from line
		double mm = (double)n.getY() / (double)n.getX();
		double bb = _lineStart.getY() - _lineStart.getX()*mm;
		return std::abs(-mm*_p.getX()+_p.getY()-bb)/std::sqrt(mm*mm + 1);
	}

	vector<Point>::const_iterator Toolbox::GetNearestPointOnContour(
		const vector<Point>		* _contour,
		const Vector2D			& _point)
	{
		vector<Point>::const_iterator contourBegin = _contour->begin();
		vector<Point>::const_iterator contourEnd = _contour->end();
		double minDist = Toolbox::GetMaxDouble();
		vector<Point>::const_iterator nearest;
		for(vector<Point>::const_iterator contIt = contourBegin;
			contIt != contourEnd;
			contIt++)
		{
			double dist =(_point - *contIt).length();
			if(dist == 0)
			{
				nearest = contIt;
				break;
			}
			else if(dist < minDist)
			{
				nearest = contIt;
				minDist = dist;
			}
		}
		return nearest;
	}
}
