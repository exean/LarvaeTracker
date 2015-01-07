#include "TrackerSceneLandmark.h"

namespace Visualization
{
	TrackerSceneLandmark::TrackerSceneLandmark(
		QGraphicsScene * parent,
		const QString & name, 
		unsigned int x, 
		unsigned int y)
	{
		this->mParent = parent;
		this->setToolTip(name);
		this->initialize(x, y);
	}


	TrackerSceneLandmark::~TrackerSceneLandmark(void)
	{
		delete this->mPath;
		delete this->mCircle;
	}

	void TrackerSceneLandmark::initialize(unsigned int x, unsigned int y)
	{		
		QBrush	brush(QColor(0,255,255,100));
		QPen	pen	 (QColor(0,255,255));

		this->mCircle = new QGraphicsEllipseItem();
		this->mCircle->setRect(
			-5,
			-5,
			10, 
			10);	
		this->mCircle->setPen(pen);
		this->mCircle->setBrush(brush);
		this->mParent->addItem(this->mCircle);
		this->addToGroup(this->mCircle);
	
		this->mPath = this->mParent->addPath(QPainterPath());
		QPainterPath markpath;
		markpath.moveTo(0, 0);
		markpath.lineTo(-5, -15);
		markpath.lineTo(+5, -13);
		markpath.lineTo(-3, -10);
		markpath.lineTo(0, 0);
		this->mPath->setPath(markpath);
		this->mPath->setPen(pen);
		this->mPath->setBrush(brush);
		this->addToGroup(this->mPath);

		this->setPos(x, y);
	}

	bool TrackerSceneLandmark::contains(const QPointF& point)
	{
		return this->QGraphicsItemGroup::contains(point-this->pos());
	}


	void TrackerSceneLandmark::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		this->setCursor(Qt::ClosedHandCursor);
		this->lastMousePos = event->pos();
	}

	void TrackerSceneLandmark::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
	{
		//move
		QPointF trans = (event->pos()-this->lastMousePos);		
	
		this->moveBy(trans.x(), trans.y());
	}

	void TrackerSceneLandmark::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
	{
		this->setCursor(Qt::CrossCursor);
	}
}