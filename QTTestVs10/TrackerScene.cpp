#include "TrackerScene.h"

namespace Visualization
{
	TrackerScene::TrackerScene(void)
	{
		this->setBackgroundBrush(QColor(255, 255, 255)); // Schwarzer Hintergrund		
		this->mImage = this->addPixmap(QPixmap());
	
		//Init Region of Interests rects (region outside roi will be filled semi-transparent)
		this->mRoi.push_back(this->addRect(0, 0, 0, 0, QPen(Qt::red)));
		QBrush brush(QColor(255,0,0,125));	
		QPen pen(Qt::NoPen);
		for(int i = 0; i < 4; i++)
		{
			this->mRoi.push_back(this->addRect(0, 0, 0, 0, pen, brush));
		}
	
		this->mRectRoi = QRect(0,0,0,0);
	}


	TrackerScene::~TrackerScene(void)
	{
		for each(TrackerSceneLarva* l in this->mLarvae)
		{
			this->deleteLarva(l);
		}
	}


	void TrackerScene::setRect(int x, int y, int w, int h)
	{
		this->mRectRoi.setX(x);
		this->mRectRoi.setY(y);
		this->mRectRoi.setWidth(w);
		this->mRectRoi.setHeight(h);

		this->updateRoi();
	}

	void TrackerScene::setImage(const QImage img)
	{
		this->mImage->setPixmap(QPixmap::fromImage(img));

		if(img.width() == this->mRectRoi.width() && 
			img.height() == this->mRectRoi.height())
			this->mImage->setPos(this->mRectRoi.x(),this->mRectRoi.y());
		else
			this->mImage->setPos(0,0);
		
		this->updateRoi();
	}

	QImage TrackerScene::getImage()
	{
		return this->mImage->pixmap().toImage();
	}

	TrackerSceneLarva* TrackerScene::addLarva(LarvaModel* larva, const QString & name)
	{
		TrackerSceneLarva* res = new TrackerSceneLarva(this, larva, name);	
		this->mLarvae.push_back(res);
		return res;
	}

	void TrackerScene::deleteLarva(TrackerSceneLarva* larva)
	{
		this->mLarvae.erase(
			std::remove(
				this->mLarvae.begin(), 
				this->mLarvae.end(),
				larva), 
			this->mLarvae.end());
		delete larva;
	}

	void TrackerScene::updateRoi()
	{
		int width = (int)this->width();
		int height = (int)this->height();

		if(width == 0 && height == 0)
		{		
			//Scene empty
			return this->setROIvisible(false);
		}

		int x = this->mRectRoi.x();
		int y = this->mRectRoi.y();
		int w = this->mRectRoi.width();
		int h = this->mRectRoi.height();
	
		if(w == 0 && h == 0)
		{		
			//ROI empty
			return this->setROIvisible(false);
		}

		//ROI-Border:
		this->mRoi[0]->setRect(x,y,w,h);	
		//Outside ROI:
		this->mRoi[1]->setRect(x+w, 0, width-(x+w), height);
		this->mRoi[2]->setRect(x, 0, w, y);
		this->mRoi[3]->setRect(x, y+h, w, height-(y+h));
		this->mRoi[4]->setRect(0, 0, x, height);

		this->setROIvisible(true);
	}

	void TrackerScene::setROIvisible(bool visible)
	{
		for(int i = 0; i < 5; i++)
		{
			this->mRoi[i]->setVisible(visible);
		}
	}
}