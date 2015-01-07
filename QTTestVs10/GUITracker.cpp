#include "GUITracker.h"

namespace Gui
{
	GUITracker::GUITracker(void)
	{
		this->mProgressWidget = NULL;
		this->mAlgorithmSettingsLayout = NULL;
		this->setup();
	}

	GUITracker::GUITracker(const QString & presetFileName)
	{
		this->mProgressWidget = NULL;
		this->mAlgorithmSettingsLayout = NULL;
		this->setup();
		this->loadPreset(presetFileName);
	}

	GUITracker::~GUITracker(void)
	{
		delete this->mPreprocessor;
		if(this->mAlgorithmSettingsLayout) delete this->mAlgorithmSettingsLayout;
		if(this->mProgressWidget) delete this->mProgressWidget;
		for(map<QString, Dll::ABackgroundSubtractor* >::iterator bgIt = this->mAvailableBackgroundSubtractors.begin();
			bgIt != this->mAvailableBackgroundSubtractors.end();
			bgIt++)
			delete bgIt->second;
		for(map<QString, Dll::ATracker* >::iterator it = this->mAvailableAlgos.begin();
			it != this->mAvailableAlgos.end();
			it++)
			delete it->second;
	}

	void GUITracker::setup()
	{
		this->mPreprocessor		= new PreprocessingFuncs::Preprocessing();
		this->mThread			= 0;	
		this->mUi.setupUi(this);
		this->mCurrentImage		= new Visualization::TrackerGraphicsView();
		this->mUi.scrollArea->setWidget(this->mCurrentImage);
		this->mUi.splitter->setStretchFactor(1,1);//Makes sure graphics area has propper size
		this->setupFilesPage();
		this->setupPreprocessingPage();
		this->setupTrackerPage();		

		//Init random
		srand ( time(NULL) );
	}

	int GUITracker::getCompleteProgress()
	{
		return (this->mProgressWidget ? this->mProgressWidget->getCompleteProgress() : 0);
	}

	#pragma region Files
	void GUITracker::setupFilesPage()
	{
		//Add files
		this->mUi.buttonAddFiles->setAutoDefault(false);
		connect(this->mUi.buttonAddFiles, SIGNAL(clicked()), this, SLOT(addFiles()));

		//Remove all files
		this->mUi.buttonResetFiles->setAutoDefault(false);
		connect(this->mUi.buttonResetFiles, SIGNAL(clicked()), this, SLOT(resetFiles()));

		//Remove selected files
		this->mUi.buttonRemoveSelected->setAutoDefault(false);
		connect(this->mUi.buttonRemoveSelected, SIGNAL(clicked()), this, SLOT(removeFiles()));

		//Display image
		connect(this->mUi.listFiles,	SIGNAL(currentItemChanged (QListWidgetItem *,  QListWidgetItem *)), 
				this,				SLOT(selectImage(QListWidgetItem *, QListWidgetItem *)));

		//ROI	
		connect(this->mUi.spinROIX,				SIGNAL(valueChanged(int)),	this, SLOT(roiChanged()));
		connect(this->mUi.spinROIY,				SIGNAL(valueChanged(int)),	this, SLOT(roiChanged()));
		connect(this->mUi.spinROIWidth,			SIGNAL(valueChanged(int)),	this, SLOT(roiChanged()));
		connect(this->mUi.spinROIHeight,		SIGNAL(valueChanged(int)),	this, SLOT(roiChanged()));	
		connect(this->mUi.pushSetDefaultFPS,	SIGNAL(clicked()),			this, SLOT(setDefaultFPSPixPerMM()));	
		connect(this->mUi.pushLoadDefaultFPS,	SIGNAL(clicked()),			this, SLOT(loadDefaultFPSPixPerMM()));	
		connect(this->mUi.pushSetDefaultROI,	SIGNAL(clicked()),			this, SLOT(setDefaultROI()));	
		connect(this->mUi.pushLoadDefaultROI,	SIGNAL(clicked()),			this, SLOT(loadDefaultROI()));	

		//Scene
		connect(this->mUi.horizontalSliderScale,	SIGNAL(sliderMoved (int)),			this,								SLOT(changeScale(int)));	
		connect(this->mCurrentImage,				SIGNAL(scaleChanged(qreal,qreal)),	this,								SLOT(scaleChanged(qreal,qreal)));	
		connect(this->mUi.spinBoxScale,				SIGNAL(valueChanged(int)),			this->mUi.horizontalSliderScale,	SLOT(setValue(int)));
		connect(this->mUi.spinBoxScale,				SIGNAL(valueChanged(int)),			this,								SLOT(changeScale(int)));	
		connect(this->mUi.horizontalSliderScale,	SIGNAL(valueChanged(int)),			this->mUi.spinBoxScale,				SLOT(setValue(int)));	
		
		//Load default values
		this->loadDefaultFPSPixPerMM();
		this->loadDefaultROI();
	}

	void GUITracker::addFiles()
	{
		QFileDialog dialog(this);
		dialog.setNameFilter(tr("Images (*.png *.tif *.tiff *.jpg *.jpeg)"));	
		dialog.setFileMode(QFileDialog::ExistingFiles);
		QStringList fileNames;
		if (dialog.exec())
		{
			 fileNames = dialog.selectedFiles();
			 if(!fileNames.isEmpty())
			 {
				 this->mCurrentImage->getScene()->setImage(QImage(fileNames.at(0)));
				 this->roiChanged();
			 }
			 while(!fileNames.isEmpty())
			 {
				 QString s = fileNames.takeFirst();
				 new QListWidgetItem(/*QIcon(s),*/s,this->mUi.listFiles);		
			 }		 
		}
	}

	void GUITracker::resetFiles()
	{	     
		this->mUi.listFiles->clear();
	}

	void GUITracker::removeFiles()
	{	   
		qDeleteAll(this->mUi.listFiles->selectedItems());
	}

	void GUITracker::selectImage(QListWidgetItem * item,  QListWidgetItem * )
	{
		if(item != 0)
		{
			this->mCurrentImage->getScene()->setImage(QImage(item->text()));
		}
	}

	vector<string> GUITracker::getFileList()
	{
		//Get files from list->vector<std::string>
		vector<string> files;
		int itemcount = this->mUi.listFiles->count();
		if(itemcount == 0)
		{
			QMessageBox msgBox;
			msgBox.setText("Keine Dateien ausgewählt!");
			msgBox.setInformativeText("Bitte wählen Sie unter 'Eingabe' zunächst die Dateien aus, auf denen das Tracking durchgeführt werden soll.");
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
			return files;
		}
		for (int i = 0; i < itemcount; i++)
		{
			files.push_back(this->mUi.listFiles->item(i)->text().toStdString());
		}
		return files;
	}

	void GUITracker::setDefaultFPSPixPerMM()
	{
		this->mSettingsIO.saveFPSPixPerMM(
			this->mUi.doubleFPS->value(),
			this->mUi.doublePixelMM->value());
	}

	void GUITracker::setDefaultROI()
	{
		this->mSettingsIO.saveROI(
			this->getROIRect());
	}	
	
	void GUITracker::loadDefaultFPSPixPerMM()
	{
		double fps, pixPerMm;
		
		if(this->mSettingsIO.loadFPSPixPerMM(
			fps,
			pixPerMm))
		{
			this->mUi.doubleFPS->setValue(fps);
			this->mUi.doublePixelMM->setValue(pixPerMm);
		}
	}

	void GUITracker::loadDefaultROI()
	{
		cv::Rect roi;

		if(this->mSettingsIO.loadROI(
			roi))
		{
			//ROI
			this->mUi.spinROIX->setValue(roi.x);
			this->mUi.spinROIY->setValue(roi.y);
			this->mUi.spinROIWidth->setValue(roi.width);
			this->mUi.spinROIHeight->setValue(roi.height);
		}
	}
	#pragma endregion

	#pragma region PreprocessingPage
	void GUITracker::setupPreprocessingPage()
	{
		this->setupBGSubtractionForm();
		this->setupPreprocessingForm();
	}
	#pragma endregion

	#pragma region BackgroundSubtraction
	void GUITracker::setupBGSubtractionForm()
	{
		//Setup form
		this->mUi.comboBGSubMethods->addItem("Keine Hintergrund-Trennung");
		this->mUi.comboBGSubMethods->insertSeparator(1);
		
		//Load available background subtractors
		Dll::PluginLoader * loader = Dll::PluginLoader::Instance();
		vector< Dll::ABackgroundSubtractor* > bgsubs = loader->getBackgroundSubtractors();

		for each(Dll::ABackgroundSubtractor* bgsub in bgsubs)
		{
			QString name = QString::fromStdString(bgsub->getName());
			this->mAvailableBackgroundSubtractors[name] = bgsub;
			this->mUi.comboBGSubMethods->addItem(name);
		}

		this->mUi.comboBGSubMethods->setCurrentIndex(0);

		//Load efault preprocessing setup
		this->loadDefaultBGSubtraction();

		connect(this->mUi.pushBGSubTest,			SIGNAL(clicked()),								this, SLOT(doBGSubtractionTest()));
		connect(this->mUi.comboBGSubMethods,		SIGNAL(currentIndexChanged (const QString&)),	this, SLOT(changeBGSubMethod(const QString &)));
		connect(this->mUi.pushLoadDefaultBGSub,		SIGNAL(clicked()),								this, SLOT(loadDefaultBGSubtraction()));
		connect(this->mUi.pushSetDefaultBGSub,		SIGNAL(clicked()),								this, SLOT(setDefaultBGSubtraction()));
		connect(this->mUi.doubleBGSubPercentage,	SIGNAL(valueChanged ( double )),				this, SLOT(setBGSubSampleRate( double )));
	}

	bool GUITracker::checkBackgroundSubtractorSettings()
	{
		if(this->mUi.comboBGSubMethods->currentIndex() > 0)
		{
			int fileCount = this->mUi.listFiles->count();
			const int sampleCount = (int)(fileCount * (this->mUi.doubleBGSubPercentage->value()/100));
			if(sampleCount < 1)
			{
				QMessageBox msgBox;
				msgBox.setText("Nicht genügend Samples!");
				msgBox.setInformativeText("Bitte erhöhen Sie den Prozentsatz oder wählen Sie unter 'Eingabe' zusätzliche Dateien aus.");
				msgBox.setStandardButtons(QMessageBox::Ok);
				msgBox.setIcon(QMessageBox::Critical);
				msgBox.exec();
				return false;
			}
		}
		return true;
	}

	void GUITracker::doBGSubtractionTest()
	{
		Dll::ABackgroundSubtractor* bgsub = this->getSelectedBackgroundSubtractor();

		if(bgsub)
		{
			QImage currentImage = this->mCurrentImage->getScene()->getImage();
		
			if(!currentImage.isNull() && this->checkBackgroundSubtractorSettings())
			{		
				cv::Mat mat;
				QList< QListWidgetItem* > selected = this->mUi.listFiles->selectedItems();
				if(selected.count() > 0)
				{
					mat = cv::imread
						(this->mUi.listFiles->selectedItems()[0]->text().toStdString())
						(this->getROIRect());
				}
				else
				{
					mat = cv::imread
						(this->mUi.listFiles->item(0)->text().toStdString())
						(this->getROIRect());
				}

				if(!bgsub->apply(mat)) 
				{					
					bgsub->create(
						this->getFileList(), 
						this->getROIRect());
					bgsub->apply(mat);
				}
				bgsub->apply(mat);

				cv::namedWindow("Background Subtraction Result");
				cv::imshow("Background Subtraction Result", mat);		
			}
		}
	}

	void GUITracker::changeBGSubMethod(const QString & methodName)
	{
		Dll::ABackgroundSubtractor*	 bgsub = this->getSelectedBackgroundSubtractor();
		if(bgsub)
		{
			this->mAvailableBackgroundSubtractors[methodName]->setSampleRate((float)(this->mUi.doubleBGSubPercentage->value()/100));
		}
	}

	Dll::ABackgroundSubtractor*	GUITracker::getSelectedBackgroundSubtractor()
	{
		QString currentSelection = this->mUi.comboBGSubMethods->currentText();
		std::map< QString, Dll::ABackgroundSubtractor* >::iterator bgit = 
			this->mAvailableBackgroundSubtractors.find(currentSelection);
		if(bgit == this->mAvailableBackgroundSubtractors.end())
		{
			return NULL;
		}
		return bgit->second;
	}

	void GUITracker::setBGSubSampleRate( double value)
	{
		Dll::ABackgroundSubtractor*	 bgsub = this->getSelectedBackgroundSubtractor();
		if(bgsub)
		{
			bgsub->setSampleRate((float)(value/100));
		}
	}

	void GUITracker::setDefaultBGSubtraction()
	{
		this->mSettingsIO.saveBGSubtraction(
			this->mUi.comboBGSubMethods->currentText(),
			this->mUi.doubleBGSubPercentage->value());
	}

	void GUITracker::loadDefaultBGSubtraction()
	{
		QString methodName;
		double percentage;
		if(this->mSettingsIO.loadBGSubtraction(
			methodName,
			percentage))
		{
			if(methodName.isEmpty())
			{
				this->mUi.comboBGSubMethods->setCurrentIndex(0);
			}
			else
			{
				for(int i = this->mUi.comboBGSubMethods->count(); i >= 0; i--)
				{
					if(!this->mUi.comboBGSubMethods->itemText(i).compare(methodName))
					{
						this->mUi.comboBGSubMethods->setCurrentIndex(i);
						break;
					}
				}
			}
			this->setBGSubSampleRate(percentage);
			this->mUi.doubleBGSubPercentage->setValue(percentage);
		}
	}
	#pragma endregion

	#pragma region TrackerPage
	void GUITracker::setupTrackerPage()
	{
		this->setupAvailableAlgos();

		connect(this->mUi.pushStartAlgo,		SIGNAL(clicked()),	this, SLOT(startAlgo()));	
		connect(this->mUi.pushSaveAsPreset,	SIGNAL(clicked()),	this, SLOT(savePreset()));	
	}

	void GUITracker::savePreset()
	{
		QString path = QFileDialog::getSaveFileName (this, tr("Save as preset"));
		if ( !path.isNull() )
		{
			//Collect methodnames and parameters:
			std::vector< std::string > methodNames;
			std::vector < std::vector< ParameterWrapper >* > methodParams;
			unsigned int itemCount = this->mUi.listPreprocessing->count();
			for(unsigned int i = 0;
				i < itemCount;
				i++)
			{
				QListWidgetItemWithData< vector< ParameterWrapper > > * item = 
					(QListWidgetItemWithData< vector< ParameterWrapper > > *)this->mUi.listPreprocessing->item(i);
				methodNames.push_back(item->text().toStdString());
				methodParams.push_back(item->getData());
			}

			std::vector< ParameterWrapper > algoParams;
			this->mAlgorithmSettingsLayout->setValues(algoParams);

			//write
			this->mSettingsIO.savePreset(
				this->mUi.doubleFPS->value(),
				this->mUi.doublePixelMM->value(),
				this->getROIRect(),
				methodNames,
				methodParams,
				this->mUi.comboBGSubMethods->currentText(),
				this->mUi.doubleBGSubPercentage->value(),
				this->mUi.comboAlgos->currentText(),
				algoParams,
				path);
		}
	}

	void GUITracker::loadPreset()
	{
		QString path = QFileDialog::getOpenFileName (this, tr("Load preset"));
		if ( !path.isNull() )
		{
			this->loadPreset(path);
		}
	}
	void GUITracker::loadPreset	(const QString & path)
	{
		//load data
		QString bgSubMethodName;
		double bgSubPercentage;
		std::vector< std::string > methodNames;
		std::vector < std::vector< ParameterWrapper >* > methodParams;
		std::vector< ParameterWrapper > algoParams;
		QString algoName;
		double fps, pixPerMm;
		cv::Rect roi;
		if(this->mSettingsIO.loadPreset(
			fps,
			pixPerMm,
			roi,
			methodNames,
			methodParams,
			bgSubMethodName,
			bgSubPercentage,
			algoName,
			algoParams,
			&this->mAvailableAlgos,
			path))
		{
			this->mUi.doubleFPS->setValue(fps);
			this->mUi.doublePixelMM->setValue(pixPerMm);

			//ROI
			this->mUi.spinROIX->setValue(roi.x);
			this->mUi.spinROIY->setValue(roi.y);
			this->mUi.spinROIWidth->setValue(roi.width);
			this->mUi.spinROIHeight->setValue(roi.height);
		
			//delete existing Preprocessing entries
			this->mUi.listPreprocessing->clear();

			//Insert into form
			//BG-Subtraction
			for(int i = this->mUi.comboBGSubMethods->count(); i >= 0; i--)
			{
				if(this->mUi.comboBGSubMethods->itemText(i).compare(bgSubMethodName) == 0)
				{
					this->mUi.comboBGSubMethods->setCurrentIndex(i);
					break;
				}
			}
			this->mUi.doubleBGSubPercentage->setValue(bgSubPercentage);

			//Preprocessing
			std::vector< std::string >::const_iterator nameIt = methodNames.begin();
			std::vector< std::vector< ParameterWrapper >* >::iterator paramsIt = methodParams.begin();
			for(;
				nameIt != methodNames.end();
				nameIt++, paramsIt++)
			{
				QListWidgetItemWithData< vector< ParameterWrapper > > * item 
					= new QListWidgetItemWithData< vector< ParameterWrapper > >(
						QString(nameIt->c_str()), 
						this->mUi.listPreprocessing, 
						**paramsIt);
				this->mUi.listPreprocessing->addItem(item);
			}
			//Algorithm
			for(int i = this->mUi.comboAlgos->count(); i >= 0; i--)
			{
				if(this->mUi.comboAlgos->itemText(i).compare(algoName) == 0)
				{
					this->mUi.comboAlgos->setCurrentIndex(i);

					//Insert Algo params
					this->mAlgorithmSettingsLayout->setValues(algoParams);

					break;
				}
			}
		}
	}
	#pragma endregion

	#pragma region Scene
	cv::Rect GUITracker::getROIRect()
	{
		int x = this->mUi.spinROIX->value(),
			y = this->mUi.spinROIY->value(), 
			w = this->mUi.spinROIWidth->value(), 
			h = this->mUi.spinROIHeight->value();	
		
		if(0==x+y+w+h)
		{
			cv::Mat img = cv::imread(this->mUi.listFiles->item(0)->text().toStdString());		
			w = img.cols;
			h = img.rows;
			img.release();
		}
	
		return cv::Rect(x,y,w,h);
	}

	void GUITracker::setROIRect	(const cv::Rect & _roi)
	{
		this->mUi.spinROIX->setValue(_roi.x);
		this->mUi.spinROIY->setValue(_roi.y);
		this->mUi.spinROIWidth->setValue(_roi.width);
		this->mUi.spinROIHeight->setValue(_roi.height);
	}

	void GUITracker::roiChanged()
	{
		this->drawROI(
			this->mUi.spinROIX->value(), 
			this->mUi.spinROIY->value(), 
			this->mUi.spinROIWidth->value(), 
			this->mUi.spinROIHeight->value());
		
		this->mCurrentImage->ensureVisible(
			this->mUi.spinROIX->value(), 
			this->mUi.spinROIY->value(), 
			this->mUi.spinROIWidth->value(), 
			this->mUi.spinROIHeight->value(),
			10,10);
	}

	void GUITracker::drawROI(const int _x, const int _y, const int _width, const int _height)
	{	
		this->mCurrentImage->getScene()->setRect(_x,_y,_width,_height);
	}

	void GUITracker::scaleChanged (const qreal _x, const qreal)
	{
		this->mUi.horizontalSliderScale->setValue((int)(_x*100));
	}

	void GUITracker::changeScale (const int _scale)
	{
		this->mCurrentImage->setScale(qreal(_scale)/100.0);
	}

	#pragma endregion

	#pragma region Preprocessing
	void GUITracker::setupPreprocessingForm()
	{		
		//Load efault preprocessing setup
		this->loadDefaultPreprocessing();

		//Setup form
		QStringList preprocessingMethods;
		map<PreprocessingFuncs::PreprocessingMethods::EMethods,vector< ParameterWrapper > >* methods = 
			PreprocessingFuncs::PreprocessingMethods::GetAvailablePreprocessingMethods();
		for(map<PreprocessingFuncs::PreprocessingMethods::EMethods,vector< ParameterWrapper > >::iterator it = methods->begin(); 
			it != methods->end();
			it++)
		{
			preprocessingMethods.push_back(QString(PreprocessingFuncs::PreprocessingMethods::EMethodsToString(it->first).c_str()));
		}
		this->mUi.comboPrepMethods->addItems(preprocessingMethods);

		connect(this->mUi.listPreprocessing,	SIGNAL(itemDoubleClicked(QListWidgetItem *)),	this, SLOT(editPreprocessingMethod(QListWidgetItem *)));
		connect(this->mUi.pushEditPrep,		SIGNAL(clicked()),								this, SLOT(editPreprocessingMethod()));
		connect(this->mUi.pushAddPrep,		SIGNAL(clicked()),								this, SLOT(addPreprocessingMethod()));
		connect(this->mUi.pushRemovePrep,	SIGNAL(clicked()),								this, SLOT(removePreprocessingMethod()));	
		connect(this->mUi.pushMovePrepDown,	SIGNAL(clicked()),								this, SLOT(movePreprocessingMethodDown()));	
		connect(this->mUi.pushMovePrepUp,	SIGNAL(clicked()),								this, SLOT(movePreprocessingMethodUp()));		
		connect(this->mUi.pushSetDefaultPrep,SIGNAL(clicked()),								this, SLOT(setDefaultPreprocessing()));	
		connect(this->mUi.pushLoadDefaultPrep,SIGNAL(clicked()),								this, SLOT(loadDefaultPreprocessing()));	
		connect(this->mUi.pushPrepTest,		SIGNAL(clicked()),								this, SLOT(doPreprocessingTest()));		
	}

	void GUITracker::addPreprocessingMethod()
	{
		QString methodName = this->mUi.comboPrepMethods->currentText();
	
		vector< ParameterWrapper >	parameters = 
			PreprocessingFuncs::PreprocessingMethods::GetAvailablePreprocessingMethods()->at(
				PreprocessingFuncs::PreprocessingMethods::StringToEMethods(methodName.toStdString()));

		new QListWidgetItemWithData< vector< ParameterWrapper > >(methodName, this->mUi.listPreprocessing, parameters);		
	}

	void GUITracker::removePreprocessingMethod()
	{
		qDeleteAll(this->mUi.listPreprocessing->selectedItems());
	}

	void GUITracker::movePreprocessingMethodDown()
	{
		int currentRow = this->mUi.listPreprocessing->currentRow();
		if(currentRow >= this->mUi.listPreprocessing->count()-1)
			return;
		QListWidgetItem* item = this->mUi.listPreprocessing->takeItem(currentRow);
		this->mUi.listPreprocessing->insertItem(currentRow+1, item);
		this->mUi.listPreprocessing->setCurrentRow(currentRow+1);
	}

	void GUITracker::movePreprocessingMethodUp()
	{
		int currentRow = this->mUi.listPreprocessing->currentRow();
		if(currentRow < 1)
			return;
		QListWidgetItem* item = this->mUi.listPreprocessing->takeItem(currentRow);
		this->mUi.listPreprocessing->insertItem(currentRow-1, item);
		this->mUi.listPreprocessing->setCurrentRow(currentRow-1);
	}

	void GUITracker::editPreprocessingMethod(QListWidgetItem * _item)
	{
		if(_item == NULL)
		{	
			//Get selected
			QList< QListWidgetItem* > items = this->mUi.listPreprocessing->selectedItems();
			if(items.size() ==  0)
			{
				return;
			}
			_item = items.at(0);
		}

		QDialog * dialog = new QDialog(this);		
		dialog->setWindowTitle(_item->text());
		QGridLayout * dp = new GUIParameterLayout(
			dialog, 
			((QListWidgetItemWithData< vector< ParameterWrapper > >*)_item)->getData(),
			true,
			true);		
		connect(dp,	SIGNAL(accepted()), dialog, SLOT(accept()));	
		connect(dp,	SIGNAL(rejected()), dialog, SLOT(reject()));	
		dialog->setLayout(dp);
		dialog->adjustSize();		
		dialog->exec();
	}

	void GUITracker::setDefaultPreprocessing()
	{
		//Collect methodnames and parameters:
		std::vector< std::string > methodNames;
		std::vector < std::vector< ParameterWrapper >* > methodParams;
		unsigned int itemCount = this->mUi.listPreprocessing->count();
		for(unsigned int i = 0;
			i < itemCount;
			i++)
		{
			QListWidgetItemWithData< vector< ParameterWrapper > > * item = 
				(QListWidgetItemWithData< vector< ParameterWrapper > > *)this->mUi.listPreprocessing->item(i);
			methodNames.push_back(item->text().toStdString());
			methodParams.push_back(item->getData());
		}

		//write
		this->mSettingsIO.savePreprocessing(
			methodNames,
			methodParams);
	}
	
	void GUITracker::loadDefaultPreprocessing()
	{
		//delete existing entries
		this->mUi.listPreprocessing->clear();

		//load std
		std::vector< std::string > methodNames;
		std::vector < std::vector< ParameterWrapper >* > methodParams;
		if(this->mSettingsIO.loadPreprocessing(methodNames, methodParams))
		{
			std::vector< std::string >::const_iterator nameIt = methodNames.begin();
			std::vector< std::vector< ParameterWrapper >* >::iterator paramsIt = methodParams.begin();
		
			for(;
				nameIt != methodNames.end();
				nameIt++, paramsIt++)
			{
				QListWidgetItemWithData< vector< ParameterWrapper > > * item 
					= new QListWidgetItemWithData< vector< ParameterWrapper > >(
						QString(nameIt->c_str()), 
						this->mUi.listPreprocessing, 
						**paramsIt);
				this->mUi.listPreprocessing->addItem(item);
			}
		}
	}

	void GUITracker::doPreprocessingTest()
	{	
		//Collect methodnames and parameters:
		std::vector< PreprocessingFuncs::PreprocessingMethods::EMethods > methods;
		std::vector < std::vector< ParameterWrapper >* > methodParams;
		unsigned int itemCount = this->mUi.listPreprocessing->count();
		for(unsigned int i = 0;
			i < itemCount;
			i++)
		{
			QListWidgetItemWithData< vector< ParameterWrapper > > * item = 
				(QListWidgetItemWithData< vector< ParameterWrapper > > *)this->mUi.listPreprocessing->item(i);
			methods.push_back(
				PreprocessingFuncs::PreprocessingMethods::StringToEMethods(item->text().toStdString()));
			methodParams.push_back(item->getData());
		}

		//Get current image
		//QImage currentImage = this->mCurrentImage->getScene()->getImage();
		//if(!currentImage.isNull())
		//{
		//	cv::Mat mat = cv::Mat(currentImage.height(), currentImage.width(), CV_8UC4, (uchar*)currentImage.bits(), currentImage.bytesPerLine()); 
		//	int from_to[] = { 0,0,  1,1,  2,2 }; 
		//	cv::mixChannels( &mat, 1, &mat, 1, from_to, 3 ); 
		if(this->mUi.listFiles->count() > 0)
		{
			//Get file to which methods shall be applied
			QList< QListWidgetItem* > selectedFiles = this->mUi.listFiles->selectedItems();
			std::string file = (selectedFiles.count() > 0 
					? selectedFiles[0] : this->mUi.listFiles->item(0)
				)->text().toStdString();
			cv::Mat mat = cv::imread(file)(this->getROIRect());

			//Background subtraction
			Dll::ABackgroundSubtractor * bgSub = this->getSelectedBackgroundSubtractor();
			if(bgSub)
			{
				if(!bgSub->isReady())
				{
					bgSub->setSampleRate((float)(this->mUi.doubleBGSubPercentage->value()/100));
					bgSub->create(this->getFileList(), this->getROIRect());
				}
				bgSub->apply(mat);
			}

			//Apply
			cv::Mat img = this->mPreprocessor->getProcessedImage(
							mat,
							methods,
							methodParams);
			//Show result
			cv::namedWindow("Angewendete Vorverarbeitung");
			cv::imshow("Angewendete Vorverarbeitung", img);
			//cvtColor(img, img, CV_BGR2RGB); 
			//this->mCurrentImage->getScene()->setImage(
			//	QImage((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888));
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("Keine Datei ausgewählt!");
			msgBox.setInformativeText("Bitte wählen Sie unter 'Eingabe' zunächst die Dateien aus, auf denen das Tracking durchgeführt werden soll.");
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.exec();
		}
	}
	#pragma endregion

	//#pragma region Algorithm																																																																																																																													#pragma region Algortihms
	void GUITracker::setupAvailableAlgos()
	{			
		Dll::PluginLoader * loader = Dll::PluginLoader::Instance();
		vector< Dll::ATracker* > trackers = loader->getTrackers();

		for each(Dll::ATracker* tracker in trackers)
		{
			this->mAvailableAlgos[QString::fromStdString(tracker->getName())] = tracker;
		}

		for(map<QString, Dll::ATracker*>::iterator it = this->mAvailableAlgos.begin();
			it != this->mAvailableAlgos.end(); 
			it++)
		{
			this->mUi.comboAlgos->addItem((*it).first);
		}
		this->displaySettings(this->mUi.comboAlgos->currentText());

		connect(this->mUi.comboAlgos,				SIGNAL(currentIndexChanged (const QString&)),	this, SLOT(displaySettings(const QString &)));
		connect(this->mUi.pushLoadDefaultTracker,	SIGNAL(clicked()),								this, SLOT(loadDefaultAlgo()));	
		connect(this->mUi.pushSetDefaultTracker,		SIGNAL(clicked()),								this, SLOT(setDefaultAlgo()));	

		//Load default algo setup
		this->loadDefaultAlgo();
	}

	void GUITracker::displaySettings (const QString &_algoName)
	{
		emit text2GUI(this, "Algorithmus \""+_algoName+"\" ausgewählt.");

		//Clear old content
		qDeleteAll(this->mUi.groupAlgoSettings->children());	

		//Check new designation
		map<QString, Dll::ATracker*>::const_iterator algIt = this->mAvailableAlgos.find(_algoName); 
		if(algIt != this->mAvailableAlgos.end())
		{
			Dll::ATracker* selectedAlgo = (*algIt).second;

			//Fill new content
			this->mAlgorithmSettings = 
				selectedAlgo->getSettings();
			this->mAlgorithmSettingsLayout =
				new GUIParameterLayout(this->mUi.groupAlgoSettings, &this->mAlgorithmSettings);
			this->mUi.groupAlgoSettings->setLayout(
				this->mAlgorithmSettingsLayout);
		}
	}

	void GUITracker::startAlgo()
	{
		//Get background subtractor
		if(!this->checkBackgroundSubtractorSettings())
		{
			return;
		}
		Dll::ABackgroundSubtractor* bgSub = this->getSelectedBackgroundSubtractor();

		//Get files from list->vector<std::string>
		vector<string> files = this->getFileList();

		//Get Selected algo
		map<QString, Dll::ATracker*>::const_iterator algIt = this->mAvailableAlgos.find(this->mUi.comboAlgos->currentText()); 

		if(algIt == this->mAvailableAlgos.end())
			return;//no algo selected

		Dll::ATracker* currentAlgo = (*algIt).second; 

		//Collect & set Settings
		//unsigned int itemcount = this->mUi.layoutSettings->count()-1;
		//for (unsigned int i = 0; i < itemcount; i++)
		//{
		//	QLabel *lab = (QLabel*)(this->mUi.layoutSettings->itemAt(i)->widget());
		//	i++;
		//	QDoubleSpinBox *doub = (QDoubleSpinBox*)(this->mUi.layoutSettings->itemAt(i)->widget());		
		//	settings[lab->text().toStdString()] = doub->value();
		//}	
		this->mAlgorithmSettingsLayout->updateParams();
		currentAlgo->setSettings(this->mAlgorithmSettings);
		
		this->cleanThread(this->mThread);

		//Collect methodnames and parameters for preprocessing
		std::vector< PreprocessingFuncs::PreprocessingMethods::EMethods >		methods;
		std::vector < std::vector< ParameterWrapper >* >	methodParams;
		unsigned int itemCount = this->mUi.listPreprocessing->count();
		for(unsigned int i = 0;
			i < itemCount;
			i++)
		{
			QListWidgetItemWithData< vector< ParameterWrapper > > * item = 
				(QListWidgetItemWithData< vector< ParameterWrapper > > *)this->mUi.listPreprocessing->item(i);
			methods.push_back(PreprocessingFuncs::PreprocessingMethods::StringToEMethods(item->text().toStdString()));
			methodParams.push_back(item->getData());
		}
		this->mPreprocessor->setPreprocessingMethods(methods, methodParams);
	
		this->mThread = new TrackerConnection::TrackerThread(currentAlgo, files, this->getROIRect(), this->mPreprocessor, bgSub);
		QObject::connect(this->mThread,SIGNAL(text2GUI(const QString&)),			this,SLOT(receiveTextData(const QString&)));    
		QObject::connect(this->mThread,SIGNAL(image2GUI(const QImage&)),			this,SLOT(receiveImage(const QImage&)));   
		QObject::connect(this->mThread,SIGNAL(result2GUI(vector<LarvaModel*>*)),	this,SLOT(receiveTrackerResults(vector<LarvaModel*>*)));   
	
		this->mThread->start();

		//Progress display
		if(this->mProgressWidget) delete this->mProgressWidget;
		this->mProgressWidget = new GUITrackerProgress();
		QObject::connect(this->mThread,			SIGNAL(completeProgress2GUI(const int&)),	this->mProgressWidget,	SLOT(setCompleteProgress(const int&)));  
		QObject::connect(this->mThread,			SIGNAL(currentProgress2GUI(const int&)),	this->mProgressWidget,	SLOT(setCurrentProgress(const int&)));   
		QObject::connect(this->mThread,			SIGNAL(result2GUI(vector<LarvaModel*>*)),	this->mProgressWidget,	SLOT(setFinished(vector<LarvaModel*>*)));   
		QObject::connect(this->mProgressWidget,	SIGNAL(stop()),								this,					SLOT(stopTracker()));   
		QIcon icon;
        icon.addFile(QString::fromUtf8(":/QTTestVs10/working.png"), QSize(), QIcon::Normal, QIcon::Off);
		int index = this->mUi.trackerPages->addItem(this->mProgressWidget, icon, "Fortschritt");
		this->mUi.trackerPages->setCurrentIndex(index);	
		for(int i = 0; i < index; i++)
		{
			this->mUi.trackerPages->setItemEnabled(i, false);
		}
	}

	void GUITracker::finishAlgo()
	{
		//Display-Results-GUI
		QWidget *widgetRes = new GUIResults(
			this->mReceivedTrackerData, 
			this->mCurrentImage, 
			this->mMaximalTimeIndex-1);	
		QObject::connect(widgetRes,	SIGNAL(timeChangedSignal(int)),			this,						SLOT(showImage(int)));  
		QIcon iconRes;
        iconRes.addFile(QString::fromUtf8(":/QTTestVs10/result.png"), QSize(), QIcon::Normal, QIcon::Off);
		int index = this->mUi.trackerPages->addItem(widgetRes, iconRes, "Ergebnis");
		this->mUi.trackerPages->setCurrentIndex(index);	

		//Export-GUI
		QWidget *widgetEx = new QWidget;	
		this->mResultForm.setupUi(widgetEx);	
		QIcon iconEx;
        iconEx.addFile(QString::fromUtf8(":/QTTestVs10/save.png"), QSize(), QIcon::Normal, QIcon::Off);
		this->mUi.trackerPages->addItem(widgetEx, iconEx, "Export");	

		//Reenable tabs:
		for(int i = 0; i < index; i++)
		{
			this->mUi.trackerPages->setItemEnabled(i, true);
		}

		connect(this->mResultForm.exportPushButton, SIGNAL(clicked()),		this,						SLOT(startExport()));
		connect(this->mResultForm.checkThreePoints, SIGNAL(clicked(bool)),	this->mResultForm.spinP1,	SLOT(setEnabled(bool)));
		connect(this->mResultForm.checkThreePoints, SIGNAL(clicked(bool)),	this->mResultForm.spinP2,	SLOT(setEnabled(bool)));
		connect(this->mResultForm.checkThreePoints, SIGNAL(clicked(bool)),	this->mResultForm.spinP3,	SLOT(setEnabled(bool)));
	}

	void GUITracker::showImage(int _index)
	{		
		if(this->mUi.listFiles->count() > _index)
		{
			this->mCurrentImage->getScene()->setImage(
				QImage(this->mUi.listFiles->item(_index)->text()));
		}
	}

	void GUITracker::setDefaultAlgo	()
	{
		this->mAlgorithmSettingsLayout->updateParams();

		//write
		this->mSettingsIO.saveAlgo(
			this->mUi.comboAlgos->currentText(),
			this->mAlgorithmSettings);
	}

	void GUITracker::loadDefaultAlgo()
	{
		std::vector< ParameterWrapper > algoParams;
		QString algoName;
		if(this->mSettingsIO.loadAlgo(
			algoName,
			algoParams,
			&this->mAvailableAlgos))
		{
			//Algorithm
			for(int i = this->mUi.comboAlgos->count(); i >= 0; i--)
			{
				if(!this->mUi.comboAlgos->itemText(i).compare(algoName))
				{
					this->mUi.comboAlgos->setCurrentIndex(i);
					this->mAlgorithmSettingsLayout->setValues(algoParams);

					break;
				}
			}
		}
	}
//#pragma endregion

#pragma region Algorithm-Updates
	void GUITracker::stopTracker()
	{
		this->mThread->stop();
	}

	void GUITracker::receiveTextData (const QString& _text)
	{
		this->mProgressWidget->receiveTextData(_text);
		emit text2GUI(this, _text);
	}

	void GUITracker::receiveImage(const QImage& _image)
	{
		this->mCurrentImage->getScene()->setImage(_image);
	}

	void GUITracker::receiveTrackerResults(vector<LarvaModel*>* _data)
	{
		this->mReceivedTrackerData = _data;	

		//Keep algo page so params can be reviewed but don't allow second run
		this->mUi.pushStartAlgo->setEnabled(false);
	
		//Determine timeindex of last detected larva
		this->mMaximalTimeIndex = 0;

		//Remove completely (in every timestep) unresolved models (=miss-detections)
		for(unsigned int i = 0; i < this->mReceivedTrackerData->size();)
		{
			LarvaModel *baseModel	= this->mReceivedTrackerData->at(i);
			LarvaModel *m			= baseModel;
			for(;;)
			{
				if(m->isResolved())
				{
					if(m != baseModel)//Delete unresolved models at beginning
					{
						m->getPrev()->setNext(0);
						baseModel->deleteChildren();
						delete baseModel;
						m->rmPointerToPrev();
						this->mReceivedTrackerData->at(i) = m;
					}
					LarvaModel* last = m->getLast();
					for(;//Delete unresolved models at end
						!last->isResolved();
						last = m->getLast())
					{
						last->getPrev()->deleteChildren();
					}
					this->mMaximalTimeIndex = std::max(this->mMaximalTimeIndex, last->getTimeIndex()+1);
					i++;
					break;
				}
				else if(!m->hasNext())
				{
					//completely unresolved
					baseModel->deleteChildren();
					delete baseModel;
					this->mReceivedTrackerData->erase(
						this->mReceivedTrackerData->begin() + i);
					break;
				}	
				else
				{
					m = m->getNext();
				}
			};
		}

		//Translate received models according to region of interest
		Point roiPos(this->mUi.spinROIX->value(), this->mUi.spinROIY->value());

		if(roiPos.x != 0 || roiPos.y != 0)
		{
			for(vector<LarvaModel*>::iterator larvaIt = this->mReceivedTrackerData->begin();
				larvaIt != this->mReceivedTrackerData->end();
				larvaIt++)
			{
				for(LarvaModel* larva = *larvaIt;			
					;//At end of loop
					larva = larva->getNext())
				{
					larva->setCenter(larva->getCenter()+roiPos);
				
					vector<Vector2D> *spine = larva->getSpine();

					for(vector<Vector2D>::iterator spineIt = spine->begin();
						spineIt != spine->end();
						spineIt++)		
					{
						*spineIt += roiPos; 
					}

					if(!larva->hasNext())
					{
						break;
					}
				}
			}
		}

		this->finishAlgo();
	}
#pragma endregion

	void GUITracker::startExport()
	{
		//Filename
		QFileDialog dialog(this);
		dialog.setNameFilter(tr("CSV-File (*.csv)"));	
		dialog.setAcceptMode(QFileDialog::AcceptSave);
		QString fileName;
		if (dialog.exec() == QFileDialog::Accepted)
		{
			fileName = dialog.selectedFiles().takeFirst();
			if(!fileName.endsWith(".csv", Qt::CaseInsensitive))
			{
				fileName += ".csv";
			}

			vector<Point> landmarks;
			if(this->mResultForm.checkDistLandmarks->isChecked())
			{
				landmarks = this->mCurrentImage->getLandmarkPoints();
			}

			//Indices for angle calculation
			int p1 = -1, p2 = -1, p3 = -1;
			if(this->mResultForm.checkThreePoints->isChecked())
			{
				p1 = this->mResultForm.spinP1->value();
				p2 = this->mResultForm.spinP2->value();
				p3 = this->mResultForm.spinP3->value();
			}

			QString rowSeperator, colSeperator;
			GUISettings::Instance()->getCSVValues(colSeperator, rowSeperator);

			InOut::ExportToCSV exporter;
			exporter.exportLarvae(
				rowSeperator.toStdString(),
				colSeperator.toStdString(),
				this->mResultForm.radioInMM->isChecked(),
				fileName.toStdString(),
				*this->mReceivedTrackerData,
				//Selected Parameters:
				this->mResultForm.checkAllXY->isChecked(),
				this->mResultForm.checkMoment->isChecked(),
				this->mResultForm.checkRadia->isChecked(),
				this->mResultForm.checkContourArea->isChecked(),
				this->mResultForm.checkCollision->isChecked(),
				this->mResultForm.checkNNDsitance->isChecked(),
				this->mResultForm.checkDistMoment->isChecked(),
				landmarks,
				this->mResultForm.checkSpeed->isChecked(),
				this->mResultForm.checkAcc->isChecked(),
				this->mResultForm.checkDist2Origin->isChecked(),
				this->mResultForm.checkAccDist->isChecked(),
				this->mResultForm.checkDirection->isChecked(),
				this->mMaximalTimeIndex,
				this->mUi.doublePixelMM->value(),
				this->mUi.doubleFPS->value(),
				p1, p2, p3);
		}
	}

	void GUITracker::cleanThread(TrackerConnection::TrackerThread* _thread)
	{
		if(_thread != 0)	
		{
			_thread->quit();
			while(_thread->isRunning())
			{
				QThread::currentThread()->wait(100);
			}
			delete _thread;
		}
	}
}