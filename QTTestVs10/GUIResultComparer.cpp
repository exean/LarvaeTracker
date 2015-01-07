#include "GUIResultComparer.h"

namespace Gui
{
	GUIResultComparer::GUIResultComparer(void)
		: mThread(NULL)
	{
		this->mUi.setupUi(this);

		//hide result-display
		this->mUi.groupBoxMassCenters->setVisible(false);
		this->mUi.groupBoxSpinepoints->setVisible(false);
		this->mUi.groupBoxWidths->setVisible(false);
		this->mUi.labelWorkingAnimation->setVisible(false);
		this->mMovie = new QMovie(":/QTTestVs10/loader.gif");	
		this->mUi.labelWorkingAnimation->setMovie(this->mMovie);

		QObject::connect(this->mUi.pushButtonLoadGroundTruth,	SIGNAL(clicked()), this, SLOT(selectTruthFile()));	
		QObject::connect(this->mUi.pushButtonLoadCompare,		SIGNAL(clicked()), this, SLOT(selectCompareFile()));	
		QObject::connect(this->mUi.pushButtonStart,				SIGNAL(clicked()), this, SLOT(startComparison()));	
	}

	GUIResultComparer::~GUIResultComparer(void)
	{
		delete this->mMovie;
		if(this->mThread) delete this->mThread;
	}

	void GUIResultComparer::selectTruthFile	()
	{
		QString fileName = QFileDialog::getOpenFileName(
			this,
			"Basis-Datensatz öffnen", 
			"", 
			"CSV-Datei (*.csv)");

		if(!fileName.isNull())
		{
			this->mUi.lineEditGroundTruth->setText(fileName);		
			this->mUi.pushButtonStart->setEnabled(
				!this->mUi.lineEditCompare->text().isEmpty());
		}
	}

	void GUIResultComparer::selectCompareFile()
	{
		QString fileName = QFileDialog::getOpenFileName(
			this,
			"Vergleichs-Datensatz öffnen", 
			"", 
			"CSV-Datei (*.csv)");

		if(!fileName.isNull())
		{
			this->mUi.lineEditCompare->setText(fileName);
			this->mUi.pushButtonStart->setEnabled(
				!this->mUi.lineEditGroundTruth->text().isEmpty());
		}
	}
	
	void GUIResultComparer::startComparison()
	{
		if(this->mThread)
		{
			//delete old instance
			delete this->mThread;
		}

		this->mUi.groupBoxMassCenters->setVisible(false);
		this->mUi.groupBoxSpinepoints->setVisible(false);
		this->mUi.groupBoxWidths->setVisible(false);
		this->mUi.labelWorkingAnimation->setVisible(true);
		this->mUi.pushButtonStart->setEnabled(false);
		this->mMovie->start();
			 
		this->mThread = new ResultComparerThread(
			this->mUi.lineEditGroundTruth->text(),
			this->mUi.lineEditCompare->text());

		QObject::connect(
			this->mThread,		
			SIGNAL(centerComparisonFinished(
				const double, 
				const double, 
				const double, 
				const double)), 
			this, 
			SLOT(receiveCenterComparisonResuls(
				const double, 
				const double, 
				const double, 
				const double)));	

		QObject::connect(
			this->mThread,		
			SIGNAL(spinepointComparisonFinished(
				const double, 
				const double, 
				const double, 
				const double)), 
			this, 
			SLOT(receiveSpinepointComparisonResuls(
				const double, 
				const double, 
				const double, 
				const double)));	

		QObject::connect(
			this->mThread,		
			SIGNAL(widthsComparisonFinished(
				const double, const double, const double, const double, 
				const double, const double, const double, const double, 
				const double, const double, const double, const double,  
				const double, const double, const double, const double)), 
			this, 
			SLOT(receiveWidthsComparisonResuls(
				const double, const double, const double, const double, 
				const double, const double, const double, const double, 
				const double, const double, const double, const double,  
				const double, const double, const double, const double)));	

		QObject::connect(
			this->mThread,		
			SIGNAL(badFile(
				const QString)),
			this, 
			SLOT(receiveBadFileNotification(
				const QString)));
			

		this->mThread->start();
	}

	void GUIResultComparer::receiveBadFileNotification(
		const QString _file)
	{
		QMessageBox::information(0, 
			"Fehler", 
			"Datei ["+_file+"] konnte nicht geladen werden. Stimmen die CSV-Parameter (Datei/Einstellungen)?");
		this->mUi.labelWorkingAnimation->setVisible(false);
		this->mUi.pushButtonStart->setEnabled(true);
		this->mMovie->stop();
	}
	
	void GUIResultComparer::computationFinished()
	{
		if(
			this->mUi.groupBoxMassCenters->isVisible()
			&& this->mUi.groupBoxSpinepoints->isVisible()
			&& this->mUi.groupBoxWidths->isVisible())
		{
			//All results received!			
			this->mUi.labelWorkingAnimation->setVisible(false);
			this->mUi.pushButtonStart->setEnabled(true);
			this->mMovie->stop();
		}
	}

	void GUIResultComparer::receiveCenterComparisonResuls(
			const double _min, 
			const double _max, 
			const double _mid, 
			const double _med)
	{
		this->mUi.lineEditMassMax->setText(QString::number(_max));
		this->mUi.lineEditMassMin->setText(QString::number(_min));
		this->mUi.lineEditMassMid->setText(QString::number(_mid));
		this->mUi.lineEditMassMed->setText(QString::number(_med));
		this->mUi.groupBoxMassCenters->setVisible(true);
		this->computationFinished();
	}

	void GUIResultComparer::receiveSpinepointComparisonResuls(
			const double _min, 
			const double _max, 
			const double _mid, 
			const double _med)
	{
		this->mUi.lineEditSpineMax->setText(QString::number(_max));
		this->mUi.lineEditSpineMin->setText(QString::number(_min));
		this->mUi.lineEditSpineMid->setText(QString::number(_mid));
		this->mUi.lineEditSpineMed->setText(QString::number(_med));
		this->mUi.groupBoxSpinepoints->setVisible(true);
		this->computationFinished();
	}

	void GUIResultComparer::receiveWidthsComparisonResuls(
			const double _minAbs, const double _minGen, const double _minSmaller, const double _minBigger, 
			const double _maxAbs, const double _maxGen, const double _maxSmaller, const double _maxBigger, 
			const double _midAbs, const double _midGen, const double _midSmaller, const double _midBigger,  
			const double _medAbs, const double _medGen, const double _medSmaller, const double _medBigger)
	{
		this->mUi.lineEditWidthMaxAbs->setText(QString::number(_maxAbs));
		this->mUi.lineEditWidthMinAbs->setText(QString::number(_minAbs));
		this->mUi.lineEditWidthMidAbs->setText(QString::number(_midAbs));
		this->mUi.lineEditWidthMedAbs->setText(QString::number(_medAbs));

		this->mUi.lineEditWidthMaxGen->setText(QString::number(_maxGen));
		this->mUi.lineEditWidthMinGen->setText(QString::number(_minGen));
		this->mUi.lineEditWidthMidGen->setText(QString::number(_midGen));
		this->mUi.lineEditWidthMedGen->setText(QString::number(_medGen));

		this->mUi.lineEditWidthMaxSmaller->setText(QString::number(_maxSmaller));
		this->mUi.lineEditWidthMinSmaller->setText(QString::number(_minSmaller));
		this->mUi.lineEditWidthMidSmaller->setText(QString::number(_midSmaller));
		this->mUi.lineEditWidthMedSmaller->setText(QString::number(_medSmaller));

		this->mUi.lineEditWidthMaxBigger->setText(QString::number(_maxBigger));
		this->mUi.lineEditWidthMinBigger->setText(QString::number(_minBigger));
		this->mUi.lineEditWidthMidBigger->setText(QString::number(_midBigger));
		this->mUi.lineEditWidthMedBigger->setText(QString::number(_medBigger));

		this->mUi.groupBoxWidths->setVisible(true);
		this->computationFinished();
	}
}