#include "GUIResultsLarva.h"

namespace Gui
{
	GUIResultsLarva::GUIResultsLarva(
		LarvaModel						* _model,
		Visualization::TrackerSceneLarva* _sceneLarva)
	{
		this->mBaseLarva	= _model;
		this->mSceneLarva	= _sceneLarva;
		this->mSceneLarva->addObserver(this);

		this->mUi.setupUi(this);
	
		//Hide "Larva was changed" message
		this->mUi.widgetChanged->setVisible(false);

		//Setup time frame selectors
		this->updateTimes();

		//Setup radius bounds
		this->mUi.doubleSpinBoxRadius->setMinimum(0);
		this->mUi.doubleSpinBoxRadius->setMaximum(
			GeneralTrackerFuncs::Toolbox::GetMaxDouble());

		//Init modification flag
		this->mModified = false;
	
		//Connect GmUi-Elements
		connect (this->mUi.spinBoxTime,				SIGNAL(valueChanged(int)),	this->mUi.sliderTime,	SLOT(setValue(int)));
		connect (this->mUi.sliderTime,				SIGNAL(valueChanged(int)),	this->mUi.spinBoxTime,	SLOT(setValue(int)));
		connect (this->mUi.sliderTime,				SIGNAL(valueChanged(int)),	this,					SLOT(update(int)));		
		connect (this->mUi.pushButtonColorDialog,	SIGNAL(clicked()),			this,					SLOT(updateColor()));	
		connect (this->mUi.checkBoxDisplayLarva,	SIGNAL(toggled (bool)),		this,					SLOT(displaySceneLarva(bool)));	
		connect (this->mUi.checkBoxPath,			SIGNAL(toggled (bool)),		this,					SLOT(displaySceneLarvaPath(bool)));	
		connect (this->mUi.checkBoxDist2Origin,		SIGNAL(toggled (bool)),		this,					SLOT(displaySceneLarvaDistance2Origin(bool)));	
		connect (this->mUi.pushButtonPrevCollision,	SIGNAL(clicked()),			this,					SLOT(setTimeToPrevCollsion()));	
		connect (this->mUi.pushButtonNextCollision,	SIGNAL(clicked()),			this,					SLOT(setTimeToNextCollsion()));	
		connect (this->mUi.pushButtonReset,			SIGNAL(clicked()),			this,					SLOT(reset()));
		connect (this->mUi.pushButtonApplyChanges,	SIGNAL(clicked()),			this,					SLOT(apply()));
		connect (this->mUi.pushDeleteLarva,			SIGNAL(clicked()),			this,					SLOT(deleteLarva()));
		connect (this->mUi.pushDeleteCurrent,		SIGNAL(clicked()),			this,					SLOT(deleteCurrentModel()));
		connect (this->mUi.pushUnite,				SIGNAL(clicked()),			this,					SLOT(uniteWithOtherModel()));
		connect (this->mUi.pushDivide,				SIGNAL(clicked()),			this,					SLOT(divideAfterCurrentTimestep()));
		connect (this->mUi.pushCopy2Prev,			SIGNAL(clicked()),			this,					SLOT(copyToPrevTimestep()));
		connect (this->mUi.pushCopy2Succ,			SIGNAL(clicked()),			this,					SLOT(copyToNextTimestep()));
		connect (this->mUi.pushInvert,				SIGNAL(clicked()),			this,					SLOT(invertLarva()));
		connect (this->mUi.pushFindLarva,			SIGNAL(clicked()),			this,					SLOT(centerSceneOnLarva()));
		connect (this->mUi.pushFindLarva,			SIGNAL(clicked()),			this,					SLOT(flash()));
		
		connect (this->mUi.doubleSpinBoxRadius,		SIGNAL(valueChanged(double)),	this,				SLOT(applyRadiusToSceneLarva(double)));
		connect (this->mUi.spinBoxRadiusPosition,	SIGNAL(valueChanged(int)),	this,					SLOT(updateDisplayedRadius(int)));

		//Insert values into mUi
		this->update(0);

		//Init with random color
		this->setColor(QColor(qrand() % 256, qrand() % 256, qrand() % 256));

		//Flashing of scene-larva
		this->mFlashTimer = new QTimer(this);
		this->mFlashTimer->setInterval(100); //100 ms
		QObject::connect(this->mFlashTimer,SIGNAL(timeout()),this,SLOT(flashEvent()));
	}


	GUIResultsLarva::~GUIResultsLarva(void)
	{	
		delete this->mFlashTimer;
	}

	void GUIResultsLarva::centerSceneOnLarva()
	{
		emit requestCenterOn(this->mCurrentLarva->getCenter().getX(), this->mCurrentLarva->getCenter().getY());
	}

	bool GUIResultsLarva::canChangeTime()
	{
		if(this->mModified)
		{
			QMessageBox msgBox(
				"Ungespeicherte Modifikationen von "+this->mSceneLarva->mName,
				"Die Larve wurde manuell modifiziert. Vor dem ändern des Zeitindex muss die Modifikation gespeichert werden.",
				QMessageBox::Warning,
				QMessageBox::Save,
				QMessageBox::Discard,
				QMessageBox::Cancel);		
			int ret = msgBox.exec();
		
			switch (ret)
			{
			  case QMessageBox::Save:
				this->apply();
				break;
			  case QMessageBox::Discard:
				this->reset();
				break;
			  case QMessageBox::Cancel:
				//Stop
				return false;
				break;
			  default:
				// should never be reached
				break;
			}
		}	

		return true;
	}


	void GUIResultsLarva::deleteLarva()
	{
		QMessageBox msgBox;
		msgBox.setText("Wirklich löschen?");
		msgBox.setInformativeText("Hierdurch wird diese Larve vollständig aus den Ergebnissen entfernt. Dies bezieht sich auf alle Zeitschritte.");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setIcon(QMessageBox::Warning);
		if(msgBox.exec() == QMessageBox::Ok)
		{
			emit deleteResultsLarva(this);
		}
	}

	void GUIResultsLarva::deleteCurrentModel()
	{
		QMessageBox msgBox(
			"Wirklich löschen?",
			"Hierdurch werden die Ergebnisse dieser Larve für den aktuellen Zeitschritt unwiderruflich gelöscht.",
			QMessageBox::Warning,
			QMessageBox::Ok,
			QMessageBox::Cancel,
			0);
		if(msgBox.exec() == QMessageBox::Ok)
		{
			if(this->mBaseLarva == this->mCurrentLarva)//Remove first timestep
			{
				if(!this->mBaseLarva->hasNext())//Remove complete larva (only information for current timestep)
				{
					emit deleteResultsLarva(this);
				}
				else
				{
					//Instead of real deletion, data from second model is transefered
					//to first bc first model anchors all larva timesteps.
					LarvaModel* newFirst = this->mCurrentLarva->getNext();
					
					this->mBaseLarva->setCollision(newFirst->isCollision());
					this->mBaseLarva->setContour(newFirst->getContour());
					this->mBaseLarva->setHeadIntensity(newFirst->getHeadIntensity());
					this->mBaseLarva->setCenter(newFirst->getCenter());
					this->mBaseLarva->setNext(newFirst->getNext());
					this->mBaseLarva->setResolved(newFirst->isResolved());
					this->mBaseLarva->setSpine(*newFirst->getSpine());
					this->mBaseLarva->setTailIntensity(newFirst->getTailIntensity());
					this->mBaseLarva->setTimeIndex(newFirst->getTimeIndex());
					this->mBaseLarva->setWidths(*newFirst->getWidths());

					delete newFirst;
					
					int timemin = this->mUi.spinBoxTime->minimum()+1;
					this->mUi.spinBoxTime->setMinimum(timemin);
					this->mUi.sliderTime->setMinimum(timemin);
				}
			}
			else if(!this->mCurrentLarva->hasNext())//Remove last timestep
			{
				LarvaModel * newLast = this->mCurrentLarva->getPrev();
				newLast->setNext(NULL);
				delete this->mCurrentLarva;
				this->mCurrentLarva = newLast;
				int timemax = this->mUi.spinBoxTime->maximum()-1;
				this->mUi.spinBoxTime->setMaximum(timemax);
				this->mUi.sliderTime->setMaximum(timemax);
			}
			else
			{
				//Larva models between first ans last timepoint won't be deleted but set as unresolved
				this->mCurrentLarva->setResolved(false);
			}
			//Update
			this->setTimeIndex(this->mCurrentLarva->getTimeIndex());
		}
	}

	void GUIResultsLarva::uniteWithOtherModel()
	{
		//Determine possible partners and show selection menu
		emit requestUnion(this);
	}

	void GUIResultsLarva::divideAfterCurrentTimestep()
	{
		if(this->mCurrentLarva->hasNext())
		{
			LarvaModel * cutOff = this->mCurrentLarva->getNext();
			this->mCurrentLarva->setNext(NULL);

			//Remove cutOff's pointer to mCurrentLarva so it may become a "first" itself
			cutOff->rmPointerToPrev();

			//Request new gmUi for cutOff
			emit addResultsLarva(cutOff);

			//Update max-time
			int timeFrameCount = this->mCurrentLarva->getTimeIndex();
			this->mUi.sliderTime->setMaximum	(timeFrameCount);
			this->mUi.spinBoxTime->setMaximum(timeFrameCount);
		}
	}
	
	void GUIResultsLarva::updateTimes()
	{
		int firstTimeFrame = this->mBaseLarva->getTimeIndex();
		this->mUi.sliderTime->setMinimum(firstTimeFrame);
		this->mUi.spinBoxTime->setMinimum(firstTimeFrame);
		this->mUi.sliderTime->setValue(firstTimeFrame);
		this->mUi.spinBoxTime->setValue(firstTimeFrame);
		int timeFrameCount = this->mBaseLarva->getLast()->getTimeIndex();
		this->mUi.sliderTime->setMaximum	(timeFrameCount);
		this->mUi.spinBoxTime->setMaximum(timeFrameCount);
	}

	void GUIResultsLarva::copyToNextTimestep()
	{
		int timeIndex = this->mCurrentLarva->getTimeIndex();
		if(this->mCurrentLarva->hasNext())
		{
			QMessageBox msgBox(
				"Daten überschreiben?",
				"Für den folgenden Zeitschritt sind bereits Daten vorhanden. Sollen diese überschrieben werden?",
				QMessageBox::Warning,
				QMessageBox::Ok,
				QMessageBox::Cancel,
				0);		
			if(msgBox.exec() == QMessageBox::Cancel)
			{
				return;
			}
			else
			{
				LarvaModel* next = this->mCurrentLarva->getNext();
				next->setResolved(this->mCurrentLarva->isResolved());
				next->setWidths(vector<double>(*this->mCurrentLarva->getWidths()));
				next->setSpine(vector<Vector2D>(*this->mCurrentLarva->getSpine()));
				next->setContour(vector<Point>(this->mCurrentLarva->getContour()));
				next->setCenter(this->mCurrentLarva->getCenter());
				next->setCollision(this->mCurrentLarva->isCollision());
				next->setHeadIntensity(this->mCurrentLarva->getHeadIntensity());
				next->setTailIntensity(this->mCurrentLarva->getTailIntensity());
//				next->setInterpolatedContour(this->mCurrentLarva->getInterpolatedContour());
			}
		}
		else
		{
			LarvaModel* next = new LarvaModel(this->mCurrentLarva, false);			
			this->mCurrentLarva->setNext(next);		
		}
		this->updateTimes();
		this->setTimeIndex(timeIndex+1);
	}

	void GUIResultsLarva::copyToPrevTimestep()
	{
		int timeIndex = this->mCurrentLarva->getTimeIndex();
		if(timeIndex == 0)
		{
			QMessageBox msgBox(
				"Erster Zeitindex",
				"Es kann kein Model vor Zeitindex 0 eingefügt werden.",
				QMessageBox::Warning,
				QMessageBox::Ok,
				0,
				0);		
			msgBox.exec();
			return;
		}
		else if(this->mCurrentLarva->hasPrev())
		{
			QMessageBox msgBox(
				"Daten überschreiben?",
				"Für den vorhergehenden Zeitschritt sind bereits Daten vorhanden. Sollen diese überschrieben werden?",
				QMessageBox::Warning,
				QMessageBox::Ok,
				QMessageBox::Cancel,
				0);		
			if(msgBox.exec() == QMessageBox::Cancel)
			{
				return;
			}
			else
			{
				LarvaModel* prev = this->mCurrentLarva->getPrev();
				prev->setResolved(this->mCurrentLarva->isResolved());
				prev->setWidths(vector<double>(*this->mCurrentLarva->getWidths()));
				prev->setSpine(vector<Vector2D>(*this->mCurrentLarva->getSpine()));
				prev->setContour(vector<Point>(this->mCurrentLarva->getContour()));
				prev->setCenter(this->mCurrentLarva->getCenter());
				prev->setCollision(this->mCurrentLarva->isCollision());
				prev->setHeadIntensity(this->mCurrentLarva->getHeadIntensity());
				prev->setTailIntensity(this->mCurrentLarva->getTailIntensity());
//				prev->setInterpolatedContour(this->mCurrentLarva->getInterpolatedContour());
			}
		}
		else
		{
			LarvaModel* next = this->mCurrentLarva->getNext();
			this->mCurrentLarva->setTimeIndex(this->mCurrentLarva->getTimeIndex()-1);
			LarvaModel* prev = new LarvaModel(this->mCurrentLarva, false);	
			this->mCurrentLarva->setNext(prev);	
			prev->setNext(next);
		}
		this->updateTimes();
		this->setTimeIndex(timeIndex-1);
	}

#pragma region 	time-change
	void GUIResultsLarva::setTimeIndex(
		int _time)
	{			
		if(_time != this->mCurrentTime)
		{
			if(!this->canChangeTime())
			{
				this->mUi.spinBoxTime->setValue(this->mCurrentTime);
				this->mUi.sliderTime->setValue(this->mCurrentTime);

				//Stop
				return;
			}
		}

		if(!this->mModified)
		{
			if(_time < this->mUi.spinBoxTime->minimum()
				|| _time > this->mUi.spinBoxTime->maximum())
			{
				this->mSceneLarva->setVisible(false);
				this->mUi.spinBoxTime->setValue(this->mCurrentTime);
				this->mUi.sliderTime->setValue(this->mCurrentTime);

				return;
			}
			else
			{
				this->mSceneLarva->setVisible(this->mUi.checkBoxDisplayLarva->isChecked());
				this->mUi.spinBoxTime->setValue(_time);
			}
		}
	}

	void GUIResultsLarva::update(
		int _time)
	{		
		if(_time != this->mCurrentTime)
		{
			if(!this->canChangeTime())
			{
				this->mUi.spinBoxTime->setValue(this->mCurrentTime);
				this->mUi.sliderTime->setValue(this->mCurrentTime);

				//Stop
				return;
			}
		}

		if(!this->mModified)
		{
			//if(!this->mBaseLarva->getInterpolatedContour().empty())
			//	GeneralTrackerFuncs::Contour c(this->mBaseLarva->getInterpolatedContour());
			this->mCurrentLarva = this->mBaseLarva;

			double accdistance = 0;
			Vector2D pos(-1,-1);
			if(this->mCurrentLarva->isResolved() && this->mCurrentLarva->getSpine() && !this->mCurrentLarva->getSpine()->empty())
			{
				pos = this->mCurrentLarva->getCenter();
			}
			while(this->mCurrentLarva->getTimeIndex() < _time)
			{
				if(this->mCurrentLarva->hasNext())
				{		
					this->mCurrentLarva = this->mCurrentLarva->getNext();

					if(this->mCurrentLarva->isResolved() && this->mCurrentLarva->getSpine() && !this->mCurrentLarva->getSpine()->empty())
					{
						if(pos.getX() != -1)
						{
							accdistance += (this->mCurrentLarva->getCenter()-pos).length();
						}
						pos = this->mCurrentLarva->getCenter();
					}
				}
				else
				{
					this->mUi.spinBoxTime->setValue(this->mCurrentTime);
					this->mUi.sliderTime->setValue(this->mCurrentTime);
				
					//Stop
					return;
				}
			}

			this->mSceneLarva->setLarva(this->mCurrentLarva);
			this->mSceneLarva->setVisible(this->mUi.checkBoxDisplayLarva->isChecked());

			this->mUi.labelAccDistance->setText(
				!this->mCurrentLarva->isResolved() ? "-" :
				QString::number(accdistance));

			this->mUi.labelDist2Origin->setText(
				!this->mCurrentLarva->isResolved() ? "-" :
				QString::number(Vector2D(this->mBaseLarva->getCenter()-this->mCurrentLarva->getCenter()).length()));
			this->mUi.labelInterpolated->setText(
				!this->mCurrentLarva->isResolved() ? "-" :
				this->mCurrentLarva->isCollision() ? "Ja" : "Nein");	

			//}
			//else
			//{
			//	this->mUi.labelAccDistance->setText("");
			//	this->mUi.labelSpeed->setText("");
			//	this->mUi.labelAcceleration->setText("");
			//	this->mUi.labelDist2Origin->setText("");
			//	this->mUi.labelInterpolated->setText("");	
			//}

			this->mCurrentTime = _time;

			
			//Update radius-gui-elements
			this->mUi.spinBoxRadiusPosition->setEnabled(false);
			this->mUi.doubleSpinBoxRadius->setEnabled(false);
			if(this->mCurrentLarva->isResolved())
			{
				this->mUi.spinBoxRadiusPosition->setEnabled(true);
				this->mUi.doubleSpinBoxRadius->setEnabled(true);
				this->mUi.spinBoxRadiusPosition->setMinimum(0);
				this->mUi.spinBoxRadiusPosition->setMaximum(this->mCurrentLarva->getWidths()->size()-1);
				this->updateDisplayedRadius(this->mUi.spinBoxRadiusPosition->value());
			}
		}
	}
#pragma endregion

#pragma region Radius-GUI
	void GUIResultsLarva::updateDisplayedRadius (int _index)
	{
		qreal sceneRadius = 
			this->mSceneLarva->getRadius(_index);
		int mult = std::pow(10., this->mUi.doubleSpinBoxRadius->decimals());
		this->mUi.doubleSpinBoxRadius->setValue(
			std::floor(sceneRadius * mult + 0.5) / mult);
	}

	void GUIResultsLarva::applyRadiusToSceneLarva (double _radius)
	{
		int index = this->mUi.spinBoxRadiusPosition->value();
		qreal currentRadius = this->mSceneLarva->getRadius(index);
		int mult = std::pow(10., this->mUi.doubleSpinBoxRadius->decimals());
		if(std::floor(_radius * mult + 0.5)	!= std::floor(currentRadius * mult + 0.5))
		{
			this->mSceneLarva->setRadius(
				_radius,
				index);
		}
	}
#pragma endregion

	void GUIResultsLarva::setColor(QColor _color)
	{
		this->mColor = _color;
		//QPalette palette = this->mUi.labelColorDialog->palette();
		//palette.setColor(this->mUi.labelColorDialog->backgroundRole(), this->color);
		//this->mUi.labelColorDialog->setPalette(palette);
		//this->mUi.labelColorDialog->setAutoFillBackground(true);
		this->mUi.labelColorDialog->setStyleSheet(
			"QLabel { background-color : rgb("
			+QString::number(_color.red())+","
			+QString::number(_color.green())+","
			+QString::number(_color.blue())
			+"); color : black; }");

		this->mSceneLarva->setColor(_color);
	}

	void GUIResultsLarva::updateColor()
	{		
		QColor col = QColorDialog::getColor ( this->mColor, this, "Darstellungsfarbe wählen");
		if(col.isValid())
		{
			this->setColor(col);	
		}
	}

	void GUIResultsLarva::displaySceneLarva(bool _visible)
	{		
		this->mSceneLarva->setVisible(_visible);
	}

	void GUIResultsLarva::displaySceneLarvaPath(bool _visible)
	{		
		this->mSceneLarva->setPathVisible(_visible);
	}

	void GUIResultsLarva::displaySceneLarvaDistance2Origin(bool _visible)
	{		
		this->mSceneLarva->setDistance2OriginVisible(_visible);
	}

	void GUIResultsLarva::setTimeToPrevCollsion()
	{
		LarvaModel *larva = this->mCurrentLarva;
		int time = this->mUi.spinBoxTime->value();
		while(larva->hasPrev())
		{
			larva = larva->getPrev();
			time--;

			if(larva->isCollision())
			{
				break;
			}
		}
		this->setTimeIndex(time);
	}

	void GUIResultsLarva::setTimeToNextCollsion()
	{
		LarvaModel *larva = this->mCurrentLarva;
		int time = this->mUi.spinBoxTime->value();
		while(larva->hasNext())
		{
			larva = larva->getNext();
			time++;

			if(larva->isCollision())
			{
				break;
			}
		}
		this->setTimeIndex(time);
	}

	void GUIResultsLarva::observedObjectChanged(Observable * source)
	{
		this->mUi.widgetChanged->setVisible(true);
		this->mModified = true;

		//Update radius displayed in gui
		if(this->mCurrentLarva->isResolved())
		{
			this->updateDisplayedRadius(this->mUi.spinBoxRadiusPosition->value());
		}

		//Inform observers
		emit changed(this, true);
	}

	void GUIResultsLarva::reset()
	{
		this->mSceneLarva->setLarva(this->mCurrentLarva);
		this->mSceneLarva->setVisible(this->mUi.checkBoxDisplayLarva->isChecked());
		this->mUi.widgetChanged->setVisible(false);
		//Update radius displayed in gui
		if(this->mCurrentLarva->isResolved())
		{
			this->updateDisplayedRadius(this->mUi.spinBoxRadiusPosition->value());
		}
		this->mModified = false;
		emit changed(this, false);
	}

	void GUIResultsLarva::apply()
	{
		this->mSceneLarva->applyCurrentValues(this->mCurrentLarva);
		this->mUi.widgetChanged->setVisible(false);
		this->mModified = false;
		//Recalc displayed data
		this->update(this->mUi.spinBoxTime->value());

		emit changed(this, false);
	}
	 
	void GUIResultsLarva::invertLarva()
	{		
		if(this->mUi.widgetChanged->isVisible())
		{
			switch( QMessageBox::information( this, "Geänderte Larve",
                        "Die Larve wurde manuell verändert.",
                        "Anwenden und invertieren", "Verwerfen und invertieren", "Abbrechen",
                        0, 1 ) ) 
			{
				case 0:
					this->apply();
					break;
				case 1:			
					break;
				case 2:
				default: 
					return;			
			}
		}
		
		this->mUi.widgetChanged->setVisible(false);

		GeneralTrackerFuncs::LarvaModelManager::InvertSpine(this->mCurrentLarva);		
		this->mSceneLarva->setLarva(this->mCurrentLarva);
		this->mSceneLarva->setVisible(this->mUi.checkBoxDisplayLarva->isChecked());

		//Recalc displayed data
		this->update(this->mUi.spinBoxTime->value());

		emit changed(this, false);
	}

	
#pragma region Flashing
	void GUIResultsLarva::flash()
	{
	    this->mFlashCount = 10;
		this->mFlashTimer->start();		
	}

	void GUIResultsLarva::flashEvent()
	{
		this->mSceneLarva->setVisible(this->mFlashCount % 2);
		this->mFlashCount--;
		if (this->mFlashCount <= 0) 
		{ 
			this->mSceneLarva->setVisible(this->mUi.checkBoxDisplayLarva->isChecked());
			this->mFlashTimer->stop(); 
		}
	}
#pragma endregion
}