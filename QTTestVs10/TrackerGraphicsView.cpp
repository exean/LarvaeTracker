#include "TrackerGraphicsView.h"

namespace Visualization
{
	TrackerGraphicsView::TrackerGraphicsView(void)
	{
		//image output
		this->mScene = new TrackerScene();
		this->setScene(this->mScene);

		this->setupContextMenu();
	}


	TrackerGraphicsView::~TrackerGraphicsView(void)
	{
		delete this->mActionSaveImageForNotes;
		delete this->mActionSaveImage;
		delete this->mScene;
	}

	TrackerScene* TrackerGraphicsView::getScene()
	{
		return this->mScene;
	}

	void TrackerGraphicsView::setupContextMenu()
	{	
		this->mActionSetLandmark = new QAction(QIcon(":/QTTestVs10/landmark.png"), "Landmark setzen", this);	
		this->mContextMenu.addAction(this->mActionSetLandmark);
		connect(this->mActionSetLandmark, SIGNAL(triggered()), this, SLOT(setLandmark()));

		this->mActionRemoveLandmark = new QAction(QIcon(":/QTTestVs10/removeLandmark.png"), "Landmark entfernen", this);	
		this->mContextMenu.addAction(this->mActionRemoveLandmark);
		connect(this->mActionRemoveLandmark, SIGNAL(triggered()), this, SLOT(removeLandmark()));

		this->mContextMenu.addSeparator();

		this->mActionSaveImageForNotes = new QAction(QIcon(":/QTTestVs10/notes.png"), "Bild zu Notizen hinzufügen", this);
		this->mContextMenu.addAction(this->mActionSaveImageForNotes);
		connect(this->mActionSaveImageForNotes, SIGNAL(triggered()), this, SLOT(saveImageForNotes()));

		this->mActionSaveImage = new QAction(QIcon(":/QTTestVs10/save.png"), "Bild speichern", this);
		this->mContextMenu.addAction(this->mActionSaveImage);
		connect(this->mActionSaveImage, SIGNAL(triggered()), this, SLOT(saveImage()));
	}

	#pragma region Events
	void TrackerGraphicsView::contextMenuEvent ( QContextMenuEvent * event )
	{	
		this->mLastContextMenuPosition = this->mapToScene(event->pos());	

		this->mActionSetLandmark->setEnabled(true);
		this->mActionRemoveLandmark->setEnabled(false);

		//Landmark sensitive options
		for(vector<TrackerSceneLandmark*>::iterator it = this->mLandmarks.begin();
			it != this->mLandmarks.end();
			it++)
		{
			if((*it)->contains(this->mLastContextMenuPosition))
			{
				//option to delte landmark
				this->mActionSetLandmark->setEnabled(false);
				this->mActionRemoveLandmark->setEnabled(true);
				break;
			}
		}

		this->mContextMenu.popup(event->globalPos());
	}

	void TrackerGraphicsView::setScale(const qreal _scale)
	{
		QTransform transform = this->transform();
		this->scale(_scale/transform.m11(), _scale/transform.m22());
	}

	void TrackerGraphicsView::wheelEvent ( QWheelEvent * event )
	{	
		if(event->modifiers().testFlag(Qt::ControlModifier))
		{//scaling
			int numDegrees = event->delta() / 8;
			int numSteps = numDegrees / 15;
			
			qreal factor = 1.0 + qreal(numSteps) / 300.0;

			if(event->modifiers().testFlag(Qt::ShiftModifier))
			{//Faster scaling
				factor += qreal(numSteps) / 100.0;
			}
			this->scale(factor, factor);

			QTransform transform = this->transform();
			emit scaleChanged(transform.m11(), transform.m22());
		}
		else
		{
			QGraphicsView::wheelEvent(event);
		}
	}
	#pragma endregion

	#pragma region Save
	void TrackerGraphicsView::saveImageForNotes()  
	{
		QImage image(this->mScene->width(), this->mScene->height(), QImage::Format_RGB888);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		this->mScene->render(&painter);

		//Get random filename
		if(!QDir("notes-files").exists())
		{
			QDir().mkdir("notes-files");
		}
		QString dir = "."+QString(QDir::separator())+"notes-files"+QString(QDir::separator());
		QString filename(dir+QString::number(rand())+".png");
		while(QFile::exists(filename))
		{
			filename = dir+QString::number(rand())+".png";
		}
		//Get random filename - DONE

		image.save(filename);

		//Enter into notes
		QString style = (this->mScene->height() > 400) 
			? "height='400'"
			: "";
		Gui::GUINotes::Instance()->addHTML("<img src='"+filename+"' "+style+" />");
	}

	void TrackerGraphicsView::saveImage()  
	{
		QString filename = QFileDialog::getSaveFileName(
			this,
			"Bild speichern", 
			"", 
			"Image Files (*.png *.jpg *.bmp)");

		if( !filename.isNull() )
		{
			QImage image(this->mScene->width(), this->mScene->height(), QImage::Format_RGB888);
			QPainter painter(&image);
			painter.setRenderHint(QPainter::Antialiasing);
			this->mScene->render(&painter);
			image.save(filename);
		}
	}
	#pragma endregion

	#pragma region Landmarks
	void TrackerGraphicsView::setLandmark()
	{
		TrackerSceneLandmark *lndMk = new TrackerSceneLandmark(
				this->mScene,
				"Landmark " + QString::number(this->mLandmarks.size()), 
				this->mLastContextMenuPosition.x(),
				this->mLastContextMenuPosition.y());
		this->mLandmarks.push_back(lndMk);
		this->mScene->addItem(lndMk);
	}

	void TrackerGraphicsView::removeLandmark()
	{
		for(vector<TrackerSceneLandmark*>::iterator it = this->mLandmarks.begin();
			it != this->mLandmarks.end();
			it++)
		{
			if((*it)->contains(this->mLastContextMenuPosition))
			{
				this->mScene->removeItem((*it));
				this->mLandmarks.erase(it);
				break;
			}
		}
	}

	vector< Point > TrackerGraphicsView::getLandmarkPoints()
	{
		vector< Point > res;
		for(vector< TrackerSceneLandmark* >::iterator it = this->mLandmarks.begin();
			it != this->mLandmarks.end();
			it++)
		{
			QPoint point = (*it)->pos().toPoint();
			res.push_back(Point(point.x(), point.y()));
		}
		return res;
	}
	#pragma endregion
}