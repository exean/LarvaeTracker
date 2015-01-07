#include "TrackerSceneLarvaCircle.h"

namespace Visualization
{
	TrackerSceneLarvaCircle::TrackerSceneLarvaCircle(const QString & name, bool resizable)
		: QGraphicsEllipseItem(0,0,0,0)
	{
		this->setToolTip(name);
		this->setCursor(Qt::CrossCursor);	
		this->setAcceptedMouseButtons(Qt::LeftButton);
		this->mResizable = resizable;
	}


	TrackerSceneLarvaCircle::~TrackerSceneLarvaCircle(void)
	{
	}

	void TrackerSceneLarvaCircle::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		if(this->mResizable && event->modifiers() == Qt::ControlModifier)//Resize on ctrl
		{
			this->setCursor(Qt::SizeBDiagCursor);
		}
		else//Else move
		{
			this->setCursor(Qt::ClosedHandCursor);
		}
	}

	void TrackerSceneLarvaCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		if(this->mResizable && event->modifiers() == Qt::ControlModifier)//Resize on ctrl
		{
			QPointF origin = this->rect().center();
			float radius = this->rect().width() / 2;
			QPointF last = event->lastPos();
			QPointF current = event->pos();
			float lastdistane = sqrt(pow(origin.x()-last.x(), 2)+pow(origin.y()-last.y(), 2));
			float currentdistance = sqrt(pow(origin.x()-current.x(), 2)+pow(origin.y()-current.y(), 2));
		
			radius += currentdistance - lastdistane;
		
			this->setRect(
				origin.x()-radius, 
				origin.y()-radius, 
				radius*2, 
				radius*2);		
		}
		else//Else move
		{
			QPointF trans = (event->pos()-event->lastPos());		
			this->setRect(this->rect().translated(trans));
		}
		this->notifyObservers();
	}

	void TrackerSceneLarvaCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
	{
		this->setCursor(Qt::CrossCursor);
	}
}