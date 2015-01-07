#include "GUISettings.h"

namespace Gui
{
	#pragma region Singleton
	// Global static pointer used to ensure a single instance of the class.
	GUISettings* GUISettings::mInstance = NULL; 

	GUISettings* GUISettings::Instance()
	{
	   if (!mInstance)   // Only allow one instance of class to be generated.
		  mInstance = new GUISettings;
	   return mInstance;
	}
	#pragma endregion


	GUISettings::GUISettings(QWidget *parent)
		: QDialog(parent)
	{
		this->mUi.setupUi(this);
		this->loadValues();

		connect(this->mUi.pushMailReset,		SIGNAL(clicked()),			this, SLOT(resetMailValues()));
		connect(this->mUi.pushMailApply,		SIGNAL(clicked()),			this, SLOT(saveMailValues()));
		connect(this->mUi.pushMailSendTest,		SIGNAL(clicked()),			this, SLOT(sendTestMail()));

		connect(this->mUi.pushCSVReset,			SIGNAL(clicked()),			this, SLOT(resetCSVValues()));
		connect(this->mUi.pushCSVApply,			SIGNAL(clicked()),			this, SLOT(saveCSVValues()));
		connect(this->mUi.lineCSVCol,			SIGNAL(editingFinished ()), this, SLOT(updateCSVExample()));
		connect(this->mUi.lineCSVRow,			SIGNAL(editingFinished ()), this, SLOT(updateCSVExample()));
		
		connect(this->mUi.pushImportPlugin,		SIGNAL(clicked()),			this, SLOT(addPlugin()));
		connect(this->mUi.pushReloadPlugins,	SIGNAL(clicked()),			this, SLOT(reloadPlugins()));
		this->setPluginInformation();
	}


	GUISettings::~GUISettings(void)
	{
	}

	void GUISettings::loadValues()
	{
		InOut::SettingsIO::LoadSettings(
			this->mMailAddress,
			this->mMailDefaultRecipient, 
			this->mMailServer, 
			this->mMailUsername, 
			this->mMailPassword, 
			this->mMailPort,
			this->mCSVCol,
			this->mCSVRow);

		this->resetMailValues();
		this->resetCSVValues();
	}

#pragma region mail
	bool GUISettings::isMailInfoAvailable()
	{
		return (!this->mMailServer.isEmpty()
			&& !this->mMailUsername.isEmpty()
			&& !this->mMailPassword.isEmpty()
			&& !this->mMailAddress.isEmpty());
	}

	void GUISettings::resetMailValues()
	{
		this->mUi.lineSenderAddress->setText(this->mMailAddress);
		this->mUi.lineDefaultRecipient->setText(this->mMailDefaultRecipient);
		this->mUi.spinPort->setValue(this->mMailPort);
		this->mUi.linePassword->setText(this->mMailPassword);
		this->mUi.lineUsername->setText(this->mMailUsername);
		this->mUi.lineServer->setText(this->mMailServer);
	}

	void GUISettings::saveMailValues()
	{
		this->mMailAddress				= this->mUi.lineSenderAddress->text();
		this->mMailDefaultRecipient		= this->mUi.lineDefaultRecipient->text();
		this->mMailPort					= this->mUi.spinPort->value();
		this->mMailPassword				= this->mUi.linePassword->text();
		this->mMailUsername				= this->mUi.lineUsername->text();
		this->mMailServer				= this->mUi.lineServer->text();

		InOut::SettingsIO::SaveSettings(
			this->mMailAddress	,
			this->mMailDefaultRecipient, 
			this->mMailServer, 
			this->mMailUsername, 
			this->mMailPassword, 
			this->mMailPort,
			this->mCSVCol,
			this->mCSVRow);

		emit settingsChanged();
	}

	void GUISettings::sendTestMail()
	{    
		bool mailSent = Mail::Instance()->sendMail(
			"PathFinder Test-Mail",
			"Wenn Sie diese E-Mail erhalten, sind die E-Mail-Einstellungen korrekt.",
			0,
			this->mUi.lineDefaultRecipient->text().toStdString().c_str(),
			this->mUi.lineUsername->text().toStdString().c_str(),
			this->mUi.linePassword->text().toStdString().c_str(),
			this->mUi.lineSenderAddress->text().toStdString().c_str(),
			this->mUi.lineServer->text().toStdString().c_str(),
			this->mUi.spinPort->value());
		
		if(mailSent)
		{
			QMessageBox(
				"Empfang prüfen", 
				"Bitte überprüfen Sie den Empfang der E-Mail zur Verifizierung der Einstellungen. Anschließend sollten Sie die Einstellungen per Klick auf 'Übernehmen' speichern.", 
				QMessageBox::Information, 
				QMessageBox::Ok,0,0).exec();
		}
		else
		{
			QMessageBox(
				"Test E-Mail nicht gesendet", 
				"Die E-Mail konnte nicht gesendet werden. Bitte überprüfen Sie die Einstellungen.", 
				QMessageBox::Critical, 
				QMessageBox::Ok,0,0).exec();
		}
	}
#pragma endregion

#pragma region plugins
	void GUISettings::addPlugin()
	{
		QFileDialog dialog(this);
		dialog.setNameFilter(tr("Plug-Ins (*.dll)"));	
		dialog.setFileMode(QFileDialog::ExistingFiles);
		QStringList fileNames;
		if (dialog.exec())
		{
			 fileNames = dialog.selectedFiles();
			 while(!fileNames.isEmpty())
			 {
				 QString s = fileNames.takeFirst();
				 QFileInfo fileInfo(s);
				 QString fileName = fileInfo.fileName();
				 QString destination = QApplication::applicationDirPath() + QDir::separator() + "Plugins" + QDir::separator();
				 for(unsigned int i = 0; QFile::exists(destination + fileName); i++)
				 {
					 fileName = fileInfo.baseName() + "(" + QString::number(i) + ")." + fileInfo.suffix();
				 }
				 QFile::copy(s, destination + fileName);
			 }		 
			 QMessageBox(
				"Auswahl hinzugefügt", 
				"Die Auswahl wurde in das Plug-In-Verzeichnis von PathFinder kopiert. Bitte veranlassen Sie ein neuladen der Plug-Ins, um die neuen Komponenten zu verwenden. Alternativ können Sie das Programm neu starten.",
				QMessageBox::Information,
				QMessageBox::Ok,0,0).exec();
		}
	}

	void GUISettings::reloadPlugins()
	{
		Dll::PluginLoader::Instance()->reloadPlugins();
		this->setPluginInformation();
	}

	void GUISettings::setPluginInformation()
	{
		this->mUi.plainPluginInfo->clear();

		Dll::PluginLoader * p = Dll::PluginLoader::Instance();
		vector< Dll::APlugin* > plugins = p->getAll();

		QString text = "";

		for each(Dll::APlugin* plugin in plugins)
		{
			text.append(plugin->getName().c_str());
			text.append(" v."+QString::number(plugin->getVersion())+"\n");
			text.append(plugin->getAuthor().c_str());
			text.append("\n");
			text.append(plugin->getDescription().c_str());
			text.append("\n\n");

			if(plugin->isMultiInstancingAllowed())
			{
				delete plugin;
			}
		}

		this->mUi.plainPluginInfo->setPlainText(text);
	}
#pragma endregion

#pragma region csv
	void GUISettings::resetCSVValues()
	{
		this->mUi.lineCSVRow->setText(this->mCSVRow);
		this->mUi.lineCSVCol->setText(this->mCSVCol);
		this->updateCSVExample();
	}

	void GUISettings::saveCSVValues()
	{
		this->mCSVRow	= this->mUi.lineCSVRow->text();
		this->mCSVCol	= this->mUi.lineCSVCol->text();

		InOut::SettingsIO::SaveSettings(
			this->mMailAddress	,
			this->mMailDefaultRecipient, 
			this->mMailServer, 
			this->mMailUsername, 
			this->mMailPassword, 
			this->mMailPort,
			this->mCSVCol,
			this->mCSVRow);

		emit settingsChanged();
	}

	void GUISettings::updateCSVExample()
	{    
		QString rowSeperator = this->mUi.lineCSVRow->text();
		QString colSeperator = this->mUi.lineCSVCol->text();

		if(!rowSeperator.compare("\\n")) rowSeperator = "\n";
		if(!colSeperator.compare("\\n")) colSeperator = "\n";

		this->mUi.textCSVExample->setText(
			"Reihe 1"+colSeperator+"Reihe 2"+colSeperator+"Reihe 3"+colSeperator+"Reihe 4"+rowSeperator+
			"Wert 1.1"+colSeperator+"Wert 1.2"+colSeperator+"Wert 1.3"+colSeperator+"Wert 1.4"+rowSeperator+
			"Wert 2.1"+colSeperator+"Wert 2.2"+colSeperator+"Wert 2.3"+colSeperator+"Wert 2.4"+rowSeperator+
			"Wert 3.1"+colSeperator+"Wert 3.2"+colSeperator+"Wert 3.3"+colSeperator+"Wert 3.4");
	}

	void GUISettings::getCSVValues(
		QString & out_ColSeperator,
		QString & out_RowSeperator)
	{    
		out_RowSeperator = this->mUi.lineCSVRow->text();
		out_ColSeperator = this->mUi.lineCSVCol->text();

		if(!out_RowSeperator.compare("\\n")) out_RowSeperator = "\n";
		if(!out_ColSeperator.compare("\\n")) out_ColSeperator = "\n";
	}
#pragma endregion


	void GUISettings::hideEvent(QHideEvent * )
	{    
		emit visibilityChanged(false);
	}
}