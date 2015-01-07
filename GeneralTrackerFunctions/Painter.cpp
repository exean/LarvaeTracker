#include "Painter.h"

namespace GeneralTrackerFuncs
{
	void Painter::PaintText(
		const char					*	_text, 
		cv::Mat						&	mod_image, 
		const cv::Point				&	_point, 
		const cv::Scalar			&	_color)
	{
		cv::putText(mod_image, _text, _point, 0, 0.5, _color, 1);
	}

	void Painter::PaintPolyline(
		const cv::vector<cv::Point>	*	_line, 
		cv::Mat						&	mod_image, 
		const cv::Scalar			&	_color, 
		const int						_lineWidth)
	{		
			for (
				cv::vector<cv::Point>::const_iterator it= (*_line).begin();
				it+1 != (*_line).end();
				it++)
			{
				cv::line(mod_image,*it,*(1+it),_color,_lineWidth);				
			}		
	}
	
	void Painter::PaintPolyline(
		const cv::vector<Vector2D>	*	_line, 
		cv::Mat						&	mod_image, 
		const cv::Scalar			&	_color, 
		const int						_lineWidth)
	{		
			for (cv::vector<Vector2D>::const_iterator it= (*_line).begin();
				it+1 != (*_line).end();
				it++)
			{
				cv::line(mod_image,(*it).toPoint(),(*(1+it)).toPoint(),_color,_lineWidth);				
			}		
	}

	void Painter::PaintLarva(
		LarvaModel					&	_larva, 
		cv::Mat						&	mod_image, 
		const cv::Scalar			&	_color, 
		const bool						_markHead,
		const bool						_fill, 
		const bool						_drawSpine)
	{
		vector<Vector2D>* spine = _larva.getSpine();
		vector<double> segWidth = *(_larva.getWidths());
	
		if(_drawSpine)
		{
			PaintPolyline(spine, mod_image, _color);
		}

		//Draw circle for each spine point
		for (unsigned int i = 0;
			i < segWidth.size();
			i++)
		{
			if(!_fill)
			{
				circle(mod_image, (*spine)[i].toPoint(), (int)(segWidth[i]/2), _color, 1, 1, 0);
				circle(mod_image, (*spine)[i].toPoint(), 1, _color, 2, 1, 0);
			}
			else
			{
				circle(mod_image, (*spine)[i].toPoint(), (int)(segWidth[i]/2), _color, -1, 1, 0);
			}
		}

		if(_markHead)
		{
			PaintText("head", mod_image, _larva.getHead().toPoint(), _color);
		}
	}

	void Painter::PaintLarvaSilhouette(
		LarvaModel					&	_larva, 
		cv::Mat						&	mod_image, 
		const cv::Scalar			&	_color,
		const float					_sizeFactor,
		const Vector2D					_translate)
	{
		vector<Vector2D>*	spine	= _larva.getSpine();
		vector<double>*segWidth		= _larva.getWidths();
	
		vector<Vector2D>::iterator		finalPoint  = spine->end();	
		vector<Vector2D>::iterator		spIt		= spine->begin() + 1;	
		vector<double>::iterator		widthIt		= segWidth->begin() + 1;	

		//Draw filled circles along spine
		for (;spIt != finalPoint;
			  spIt++, widthIt++)
		{
			double	lastWidth		= *(widthIt-1);
			Vector2D	lastPoint	= *(spIt-1);
			double	totalDistance	= Vector2D(lastPoint-*spIt).length();
			Vector2D direction		= Vector2D(*spIt-lastPoint);
			direction.normalize();

			for(int distance = 0; distance < totalDistance; distance++)
			{
				Vector2D	currentPoint= lastPoint + (_translate + direction*distance).toPoint();
				int		width			= (int)(_sizeFactor*(distance*(*widthIt)+(totalDistance-distance)*lastWidth)/(2*totalDistance));
				cv::circle(mod_image, currentPoint.toPoint(), width, _color, -1, 1, 0);		
			}
		}	
	}

	void Painter::PaintModelPath(
		const LarvaModel		&	_rootmodel, 
		cv::Mat					&	mod_image, 
		const cv::Scalar		&	_color)
	{
		for (
			LarvaModel currentModel = _rootmodel; 
			currentModel.hasNext(); 
			currentModel = currentModel.getNext())
		{
			cv::line(mod_image, currentModel.getCenter().toPoint(), currentModel.getNext()->getCenter().toPoint(), _color, 1);
		}
	}

	
	cv::Mat Painter::PaintDataset(
		vector<LarvaModel*>					&	_larvae)
	{
		//Estimate size
		int maxX=0, maxY=0;
		for each(LarvaModel* m in _larvae)
		{
			for(;m;m = m->getNext())
			{
				if(m->isResolved())
				{
					vector<Vector2D>* spine = m->getSpine();
					vector<double>	* widths = m->getWidths();
					for(int i = spine->size()-1; i >= 0; i--)
					{
						Vector2D spinepoint = spine->at(i);
						double width = widths->at(i);
						if(spinepoint.getX()+width > maxX)
						{
							maxX = (int)(spinepoint.getX()+width);
						}
						if(spinepoint.getY()+width > maxY)
						{
							maxY = (int)(spinepoint.getY()+width);
						}
					}
				}
			}
		}
		//create result image		
		cv::Mat img(maxY, maxX, CV_8UC3);
		img = cv::Scalar(255,255,255);//Background color
		for each(LarvaModel* m in _larvae)
		{
			GeneralTrackerFuncs::Painter::PaintModelPath(*m, img);
			GeneralTrackerFuncs::Painter::PaintLarva(*m->getLast(), img, cv::Scalar(0,255), true);
		}
		return img;
	}
}