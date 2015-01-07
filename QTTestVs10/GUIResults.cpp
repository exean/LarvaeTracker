#include "GUIResults.h"

namespace Gui
{
	GUIResults::GUIResults(
		vector< LarvaModel* >				*	_results, 
		Visualization::TrackerGraphicsView	*	_scene,
		int										_maxtime)
	{
		this->mLarvae = _results;
		this->mScene = _scene;
		this->mUi.setupUi(this);
		connect (this->mUi.spinBoxTime,	SIGNAL(valueChanged(int)), mUi.sliderTime,	SLOT(setValue(int)));
		connect (this->mUi.sliderTime,	SIGNAL(valueChanged(int)), mUi.spinBoxTime,	SLOT(setValue(int)));
		connect (this->mUi.spinBoxTime,	SIGNAL(valueChanged(int)), this,			SLOT(timeChanged(int)));

		this->mUi.sliderTime->setMaximum(_maxtime);
		this->mUi.spinBoxTime->setMaximum(_maxtime);

		//Seperate mUi's for each larva
		this->mTabCounter = 0;
		for(vector<LarvaModel*>::iterator it = _results->begin();
			it != _results->end();
			it++)
		{				
			this->addLarvaGui(*it);
		}
	}


	GUIResults::~GUIResults(void)
	{
	}

	
	void GUIResults::timeChanged(int _time)
	{
		emit this->timeChangedSignal(_time);
	}

	void GUIResults::addLarvaGui(LarvaModel * _larva)
	{		
		QString larvaname	= "Larva "+QString::number(this->mTabCounter++);
		GUIResultsLarva *w	= new GUIResultsLarva(_larva, this->mScene->getScene()->addLarva(_larva, larvaname));
		connect (this->mUi.sliderTime,	SIGNAL(valueChanged(int)),						w,		SLOT(setTimeIndex(int)));
		connect (w,						SIGNAL(changed(QWidget *, bool)),				this,	SLOT(setChanged(QWidget *, bool)));
		connect (w,						SIGNAL(deleteResultsLarva(GUIResultsLarva *)),	this,	SLOT(removeLarva(GUIResultsLarva *)));
		connect (w,						SIGNAL(addResultsLarva(LarvaModel *)),			this,	SLOT(addLarva(LarvaModel *)));
		connect (w,						SIGNAL(requestUnion(GUIResultsLarva *)),		this,	SLOT(uniteLarvae(GUIResultsLarva *)));
		connect (w,						SIGNAL(requestCenterOn(qreal, qreal)),			this,	SLOT(centerSceneOn(qreal, qreal)));
		
		this->mUi.tabWidgetLarvae->addTab(w, larvaname);
		w->setTimeIndex(this->mUi.sliderTime->value());
	}

	void GUIResults::uniteLarvae(GUIResultsLarva * _widget)
	{
		LarvaModel * widgetLarva	= _widget->getBaseLarva();

		unsigned int firstTime		= widgetLarva->getTimeIndex();
		unsigned int lastTime		= widgetLarva->getLast()->getTimeIndex();

		//Collect possible partners		
		QStringList possiblePartners;		
		for(int i = this->mUi.tabWidgetLarvae->count()-1; i >= 0; i--)
		{
			LarvaModel* currentLarva = this->mLarvae->at(i);
			unsigned int currentFirstTime = currentLarva->getTimeIndex();
			unsigned int currentLastTime = currentLarva->getLast()->getTimeIndex();
			if(currentFirstTime > lastTime
				|| currentLastTime < firstTime)
			{
				possiblePartners << this->mUi.tabWidgetLarvae->tabText(i);				
			}
		}

		if(possiblePartners.count() < 1)
		{
			//No partners found
			QMessageBox(
				"Vereinigung nicht möglich",
				"Es können nur Larven-Modelle vereinigt werden, deren Zeit-Frames sich nicht überschneiden. Für die aktuelle Larve existieren keine passenden Partner. Sie können Zeit-Frames entfernen, um eine Vereinigung zu ermöglichen.",
				QMessageBox::Warning,
				QMessageBox::Ok,0,0).exec();
		}
		else
		{
			//Show selection dialog
			bool ok;
			QString item = 
				QInputDialog::getItem(
					this, 
					"Larven-Modelle vereinigen", 
					"Folgende Modelle können mit der aktuellen Larva vereinigt werden: ", 
					possiblePartners,
					0, //current item
					false,//editable
					&ok);
			if (ok && !item.isEmpty())
			{
				//Get selected larva
				GUIResultsLarva * selected = NULL;
				for(int i = this->mUi.tabWidgetLarvae->count()-1; i >= 0; i--)
				{
					if(!QString::compare(this->mUi.tabWidgetLarvae->tabText(i), item))
					{
						selected = (GUIResultsLarva *)this->mUi.tabWidgetLarvae->widget(i);
						break;
					}
				}
				LarvaModel * selectedLarva = selected->getBaseLarva();

				//Unite models
				LarvaModel * earlierLarva, * latterLarva;
				GUIResultsLarva * widgetToDelete;//Widget corresponding to latter larvae becomes unneccessary
				GUIResultsLarva * widgetToKeep;
				if(lastTime < selectedLarva->getTimeIndex())//widget before selected
				{
					earlierLarva = widgetLarva;
					latterLarva = selectedLarva;
					widgetToDelete = selected;
					widgetToKeep = _widget;
				}
				else
				{
					earlierLarva = selectedLarva;
					latterLarva = widgetLarva;
					widgetToDelete = _widget;
					widgetToKeep = selected;
				}

				//Unite models
				unsigned int latterFirstTime = latterLarva->getTimeIndex();
				LarvaModel * earlierLarvaLast = earlierLarva->getLast();
				while(earlierLarvaLast->getTimeIndex()+1 < latterFirstTime)
				{	//Fill potential gap with unresolved models
					LarvaModel * filler = new LarvaModel(false);
					earlierLarvaLast->setNext(filler);
					earlierLarvaLast = filler;
				}
				earlierLarvaLast->setNext(latterLarva);

				//remove latter from result list
				this->mLarvae->erase(
					std::remove(
						this->mLarvae->begin(), 
						this->mLarvae->end(),
						latterLarva), 
					this->mLarvae->end());

				//delete gui/scene components
				Visualization::TrackerSceneLarva * sceneLarva = 
					widgetToDelete->getSceneLarva();
				this->mUi.tabWidgetLarvae->removeTab(
					this->mUi.tabWidgetLarvae->indexOf(widgetToDelete));
				this->mScene->getScene()->deleteLarva(sceneLarva);
				widgetToDelete->deleteLater();

				//update times
				widgetToKeep->updateTimes();
			}
		}
	}

	void GUIResults::addLarva(LarvaModel * _larva)
	{
		this->mLarvae->push_back(_larva);
		this->addLarvaGui(_larva);
	}

	void GUIResults::markChanged(int _tabIndex)
	{
		QString text = this->mUi.tabWidgetLarvae->tabText(_tabIndex);

		if(!text.endsWith("*"))
		{
			this->mUi.tabWidgetLarvae->setTabText(
				_tabIndex,
				text+"*");
		}
	}

	void GUIResults::markUnChanged(int _tabIndex)
	{
		QString text = this->mUi.tabWidgetLarvae->tabText(_tabIndex);

		if(text.endsWith("*"))
		{
			this->mUi.tabWidgetLarvae->setTabText(
				_tabIndex,
				text.left(text.length()-1));
		}
	}

	void GUIResults::setChanged (QWidget * _widget, bool _changed)
	{
		int tabIndex = this->mUi.tabWidgetLarvae->indexOf(_widget);
		if(_changed)
		{		
			this->markChanged(tabIndex);
		}
		else
		{
			this->markUnChanged(tabIndex);
		}
	}

	
	void GUIResults::removeLarva (GUIResultsLarva * _widget)
	{
		Visualization::TrackerSceneLarva	* sceneLarva	= _widget->getSceneLarva();
		LarvaModel							* larva			= _widget->getBaseLarva();

		//Remove tab
		this->mUi.tabWidgetLarvae->removeTab(
			this->mUi.tabWidgetLarvae->indexOf(_widget));

		//Remove from scene
		this->mScene->getScene()->deleteLarva(sceneLarva);

		//Remove from dataset
		this->mLarvae->erase(
			std::remove(
				this->mLarvae->begin(), 
				this->mLarvae->end(),
				larva), 
			this->mLarvae->end());

		//First kill the children before the mothers eyes
		larva->deleteChildren();
		//then kill her.
		delete larva;

		_widget->deleteLater();
	}

	
	void GUIResults::centerSceneOn	(qreal _x, qreal _y)
	{
		this->mScene->centerOn(_x, _y);
	}
}