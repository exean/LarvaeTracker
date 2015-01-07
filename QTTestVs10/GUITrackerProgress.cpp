#include "GUITrackerProgress.h"

namespace Gui
{
	GUITrackerProgress::GUITrackerProgress(void)
	{
		this->mUi.setupUi(this);
		this->mUi.labelDone->setVisible(false);
		this->mUi.lineMailRecipient->setVisible(false);
		this->mMovie = new QMovie(":/QTTestVs10/loader.gif");	
		this->mUi.labelWorkingAnimation->setMovie(this->mMovie);
		QObject::connect(this->mUi.pushCancel,			SIGNAL(clicked()),			this, SLOT(stopTracker())); 
		QObject::connect(this->mUi.pushSendMail,		SIGNAL(toggled ( bool )),	this, SLOT(setSendMail( bool ))); 

		//Mail
		this->updateMailSendableStatus();
		QObject::connect(Gui::GUISettings::Instance(),	SIGNAL(settingsChanged ()),	this, SLOT(updateMailSendableStatus())); 
	
		this->mMovie->start();
	}

	GUITrackerProgress::~GUITrackerProgress(void)
	{
		delete this->mMovie;
	}

	void GUITrackerProgress::updateMailSendableStatus ()
	{
		//Mail
		Gui::GUISettings * settings = Gui::GUISettings::Instance();
		if(settings->isMailInfoAvailable())
		{
			this->mUi.lineMailRecipient->setText(settings->getMailDefaultRecipient());
			this->mUi.pushSendMail->setEnabled(true);
			this->mUi.lineMailRecipient->setEnabled(true);
		}
		else
		{
			this->mUi.pushSendMail->setEnabled(false);
			this->mUi.lineMailRecipient->setEnabled(false);
		}
	}

	void GUITrackerProgress::stopTracker()
	{
		emit stop();
		this->mUi.labelAdditionalInfo->setText("Tracking wird abgebrochen...");
	}

	int GUITrackerProgress::getCompleteProgress()
	{
		return this->mCompleteProgress;
	}

	void GUITrackerProgress::setCurrentProgress(const int& p)
	{
		this->mUi.progressCurrent->setValue(p);
	}

	
	void GUITrackerProgress::setCompleteProgress(const int& p)
	{
		this->mCompleteProgress = p;
		this->mUi.progressComplete->setValue(p);
	}

	void GUITrackerProgress::setFinished(vector<LarvaModel*>* _results)
	{
		//Disable gui components
		this->mUi.pushCancel->setEnabled(false);
		this->mUi.labelWorkingAnimation->movie()->stop();
		this->mUi.labelWorkingAnimation->setVisible(false);
		this->mUi.labelDone->setVisible(true);		
		this->mUi.labelAdditionalInfo->setText("");

		//Send mail if possible & requested
		QString recipient = this->mUi.lineMailRecipient->text();
		if(!recipient.isEmpty() &&
			this->mUi.pushSendMail->isChecked())
		{
			//Create attachment file
			cv::Mat img = GeneralTrackerFuncs::Painter::PaintDataset(*_results);
			string filename = "";
			if(img.rows > 0 && img.cols > 0)
			{
				std::ostringstream sstream;
				sstream << "PathFinder-Result: " << _results->size() << " larvae";
				GeneralTrackerFuncs::Painter::PaintText(sstream.str().c_str(), img, cv::Point(10,20), cv::Scalar(0,0,255));
				sstream.clear();sstream.str("");				
				sstream << "./result" << std::rand() << ".jpg";
				filename = sstream.str();
				cv::imwrite(filename, img);
			}

			//Send mail
			Mail::Instance()->sendMail(
				"Tracking abgeschlossen",
				"Der Tracking-Vorgang der PathFinder-Software wurde soeben abgeschlossen.",
				filename.empty() ? 0 : filename.c_str(),
				recipient.toStdString().c_str());
			
			//Delete image file
			if(!filename.empty())
			{
				std::remove(filename.c_str());
			}

			this->mUi.pushSendMail->toggle();
		}
	}

	void GUITrackerProgress::receiveTextData (const QString& text)
	{
		this->mUi.labelProgress->setText(text);
	}

	void GUITrackerProgress::setSendMail (bool allowSend)
	{
		this->mUi.lineMailRecipient->setVisible(allowSend);
	}
}