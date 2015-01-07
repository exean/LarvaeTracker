//#include "GUILarvaFrame.h"
//
//
//GUILarvaFrame::GUILarvaFrame(LarvaModel* larvaModel, QWidget *parent) 
//	:  QWidget(parent)
//{
//	this->larva = larvaModel;
//	this->ui.setupUi(this);	
//	this->setupGUI();
//
//	connect(this->ui.checkDisplayPath, SIGNAL(toggled(bool)), this, SLOT(toggleDrawPath(bool)));
//}
//
//GUILarvaFrame::~GUILarvaFrame(void)
//{
//	delete this->larva;
//}
//
//
//void GUILarvaFrame::setupGUI()
//{
//	int counter = 1;
//	int pathLength = 0;
//	LarvaModel* currentModel = this->larva;
//
//	while(currentModel->hasNext())
//	{
//		this->larvaPath.push_back(currentModel->getMoment());
//		pathLength += Vector2D(currentModel->getMoment()-currentModel->getNext()->getMoment()).length();
//		this->ui.listDetections->addItem(QString::number(counter));
//		currentModel = currentModel->getNext();		
//		counter++;
//	}
//
//	this->pen = QPen(Qt::red);
//
//	this->ui.labTrackPointCount->setText(QString::number(counter));
//	this->ui.labPathLength->setText(QString::number(pathLength));
//}
//
//void GUILarvaFrame::toggleDrawPath (bool draw)
//{
//	if(draw)
//	{		
//		emit this->signalPaintPath(&this->larvaPath, this->pen);		
//	}
//}