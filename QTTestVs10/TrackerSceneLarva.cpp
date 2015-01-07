#include "TrackerSceneLarva.h"

namespace Visualization
{
	TrackerSceneLarva::TrackerSceneLarva(
		QGraphicsScene	* _parent, 
		LarvaModel		* _larva,
		const QString	& _name)
	{
		this->mName				= _name;
		this->mParent			= _parent;
		this->mSpine			= this->mParent->addPath(QPainterPath());
		this->mPath				= this->mParent->addPath(QPainterPath());
		this->mDistance2Origin	= this->mParent->addPath(QPainterPath());
		this->mSilhouette		= this->mParent->addPolygon(QPolygonF());
		this->setLarva			(_larva);
	}

	TrackerSceneLarva::~TrackerSceneLarva(void)
	{
		this->mParent->removeItem(this->mSpine);
		delete this->mSpine;
		this->mParent->removeItem(this->mPath);
		delete this->mPath;
		this->mParent->removeItem(this->mDistance2Origin);
		delete this->mDistance2Origin;
		this->mParent->removeItem(this->mSilhouette);
		delete this->mSilhouette;
		for each(TrackerSceneLarvaCircle* c in this->mCircles)
		{
			this->mParent->removeItem(c);
			delete c;
		}
	}

	void TrackerSceneLarva::setColor(const QColor &_color)
	{
		QBrush	brush(QColor(_color.red(), _color.green(), _color.blue(), _color.alpha() / 4));
		QPen	pen(_color);

		this->mSpine->setPen(pen);	

		for(unsigned int i = 0; i < this->mCircles.size(); i++)
		{
			this->mCircles[i]->setPen(pen);
			this->mCircles[i]->setBrush(brush);	
		}
	
		this->mSilhouette->setPen(pen);	
		this->mSilhouette->setBrush(brush);	

		pen.setStyle(Qt::DashLine);
		this->mPath->setPen(pen);

		pen.setStyle(Qt::DotLine);
		this->mDistance2Origin->setPen(pen);

		//Mark head
		if(this->mCircles.size() > 0)
		{
			QBrush	brush(QColor(_color.red(), _color.green(), _color.blue(), 200));
			this->mCircles[0]->setBrush(brush);
		}
	}

	void TrackerSceneLarva::setLarva(LarvaModel* _larva)
	{			
		if(!_larva->isResolved())
		{
			this->setVisible(false);
			return;
		}
		else
		{
			this->setVisible(true);
		}

		vector<double>*		widths		= _larva->getWidths();
		vector<Vector2D>*	positions	= _larva->getSpine();

		unsigned int widthsCount = widths->size();
	
		//Empty model?
		if(widthsCount < 1)
		{
			this->setVisible(false);
			return;
		}

		//Reduce circles if neccessary, remaining circles will be reused
		while(this->mCircles.size() > widthsCount)
		{
			QGraphicsItem * c = *(this->mCircles.end()-1);
			this->mParent->removeItem(c);
			delete c;
			this->mCircles.pop_back();
		}

		//Add circles if neccessary, circles will be reused
		while(this->mCircles.size() < widthsCount)
		{
			TrackerSceneLarvaCircle* circle 
				= new TrackerSceneLarvaCircle(this->mName + ((this->mCircles.size()==0)?" (head)":""));
			circle->addObserver(this);
			this->mParent->addItem(circle); 
			this->mCircles.push_back(circle);		
		}
	
		//Iterators
		vector<double>::iterator	widthIt = widths->begin();
		vector<Vector2D>::iterator	posIt	= positions->begin();
		vector<Vector2D>::iterator	end		= positions->end();	

		//Update circles and spineline
		QPainterPath spinepath;
		spinepath.moveTo(posIt->getX(), posIt->getY());
		for(int i = 0;
			posIt != end; 
			i++, posIt++, widthIt++)
		{
			//Center-Line
			spinepath.lineTo(posIt->getX(), posIt->getY());
		
			//Width-Circles					
			this->mCircles[i]->setRect(
				posIt->getX()-(*widthIt)/2,
				posIt->getY()-(*widthIt)/2, 
				*widthIt, *widthIt);
		}
		this->mSpine->setPath(spinepath);	
	
		//Update path and distance2Origin
		Vector2D moment = _larva->getCenter();
		QPainterPath path;
		path.moveTo(moment.getX(), moment.getY());
		QPainterPath distance2Origin;
		distance2Origin.moveTo(moment.getX(), moment.getY());
		while(_larva->hasPrev())
		{
			_larva = _larva->getPrev();
			if(_larva->isResolved())
			{
				moment = _larva->getCenter();
				path.lineTo(moment.getX(), moment.getY());
			}
		}
		moment = _larva->getCenter();
		path.lineTo(moment.getX(), moment.getY());
		distance2Origin.lineTo(moment.getX(), moment.getY());
		this->mPath->setPath(path);
		this->mDistance2Origin->setPath(distance2Origin);

		updateSilhouette();
	}

	void TrackerSceneLarva::setVisible(bool _visible)
	{	
		vector<TrackerSceneLarvaCircle*>::iterator end = this->mCircles.end();	
		for(vector<TrackerSceneLarvaCircle*>::iterator it = this->mCircles.begin();
			it != end; 
			it++)
		{
			(*it)->setVisible(_visible);
		}	

		this->mSilhouette->setVisible(_visible);
		this->mSpine->setVisible(_visible);
	}

	void TrackerSceneLarva::setPathVisible (bool _visible)
	{
		this->mPath->setVisible(_visible);	
	}

	void TrackerSceneLarva::setDistance2OriginVisible (bool _visible)
	{
		this->mDistance2Origin->setVisible(_visible);
	}

	void TrackerSceneLarva::updateSilhouette()
	{
		QPolygonF polySilhouette;
		QVector<QPointF> silhouetteOtherSide;		

		QPainterPath path = this->mSpine->path();
		int pathSize = path.elementCount();

		for(int i = 1; i < pathSize; i++)
		{
			Vector2D v0(path.elementAt(i-1).x, path.elementAt(i-1).y);
			Vector2D v1(path.elementAt(i).x, path.elementAt(i).y);
			double width0 = this->mCircles[i-1]->rect().width() / 2;
			double width1 = this->mCircles[i]->rect().width() / 2;
		
			//determine normalized orthogonal vektor		
			Vector2D vOrth = (v0-v1).getOrthogonalVector();
			vOrth.normalize();

			Vector2D tmp = v0+vOrth*width0;
			polySilhouette << QPointF(tmp.getX(), tmp.getY());
			tmp = v0-vOrth*width0;
			silhouetteOtherSide << QPointF(tmp.getX(), tmp.getY());
			tmp = v1+vOrth*width1;
			polySilhouette << QPointF(tmp.getX(), tmp.getY());
			tmp = v1-vOrth*width1;
			silhouetteOtherSide << QPointF(tmp.getX(), tmp.getY());
		}

		for(QVector<QPointF>::iterator it = silhouetteOtherSide.end()-1;
			;
			it--)
		{
			polySilhouette << *it;

			if(it == silhouetteOtherSide.begin())
			{
				//Close polygon
				polySilhouette << polySilhouette[0];

				break;
			}
		}

		this->mSilhouette->setPolygon(polySilhouette);
	}

	void TrackerSceneLarva::observedObjectChanged(Observable * source)
	{
		TrackerSceneLarvaCircle*	circle	= (TrackerSceneLarvaCircle*)source;
		int							index	= std::find(this->mCircles.begin(), this->mCircles.end(), circle)-this->mCircles.begin();	

		//Update Spine
		QPainterPath	path	= this->mSpine->path();
		QPointF			pos		= circle->pos()+circle->rect().center();
		path.setElementPositionAt(index, pos.x(), pos.y());
		this->mSpine->setPath(path);

		//Update Silhouette
		this->updateSilhouette();

		this->notifyObservers();
	}
	
	qreal TrackerSceneLarva::getRadius	(int _index)
	{
		return this->mCircles.at(_index)->rect().height()/2;
	}

	void TrackerSceneLarva::setRadius	(qreal _radius, int _index)
	{
		TrackerSceneLarvaCircle* circle = this->mCircles[_index];
		QPointF	pos	= circle->rect().center();
		circle->setRect(
			pos.x()-_radius,
			pos.y()-_radius, 
			_radius*2, _radius*2);	
		this->observedObjectChanged(circle);
	}

	void TrackerSceneLarva::applyCurrentValues (LarvaModel* target)
	{
		vector<double>::iterator					widthIt		= target->getWidths()->begin();
		vector<Vector2D>::iterator					spineIt		= target->getSpine()->begin();
		vector<TrackerSceneLarvaCircle*>::iterator	circlesIt	= this->mCircles.begin();
		vector<TrackerSceneLarvaCircle*>::iterator	endIt		= this->mCircles.end();
		double x = 0, y = 0, complWidth = 0;

		for(;
			circlesIt != endIt;
			circlesIt++, widthIt++, spineIt++)
		{
			(*widthIt) = (*circlesIt)->rect().height();
			QPointF	pos	= (*circlesIt)->rect().center();
			spineIt->setX(pos.x());
			spineIt->setY(pos.y());

			x += pos.x() * (*widthIt);
			y += pos.y() * (*widthIt);
			complWidth += (*widthIt);
		}

		target->setCenter(Vector2D(x/complWidth, y/complWidth));
	}
}