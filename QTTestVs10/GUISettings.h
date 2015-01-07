/**
 *  @file GUISettings.h
 *
 *  @section DESCRIPTION
 *	GUI Component for editing global settings.
 */
#pragma once

#include <QtGui/QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "ui_QTSettings.h"
#include "SettingsIO.h"
#include "Mail.h"
#include "PluginLoader.h"

//Singleton
namespace Gui
{
	/**
	 *	GUI Component for editing global settings.
	 */
	class GUISettings
		 : public QDialog
	{
		Q_OBJECT

	public:
		static GUISettings* Instance();
		~GUISettings(void);

		/**
		*	Checks wether enough information is available to send an email.
		*	
		*	@result True, if suffiecient information is available
		*/
		bool isMailInfoAvailable();

		//Get Email-Information
		QString getMailDefaultRecipient	(){return this->mMailDefaultRecipient;	};
		QString getMailServer			(){return this->mMailServer;			};
		QString getMailUsername			(){return this->mMailUsername;			};
		QString getMailAddress			(){return this->mMailAddress;			};
		QString getMailPassword			(){return this->mMailPassword;			};
		int		getMailPort				(){return this->mMailPort;				};
		void	getCSVValues			(QString & out_ColSeperator,
										 QString & out_RowSeperator);

	private:	
		GUISettings(QWidget *parent = 0);
		static GUISettings* mInstance;
		
		/**
		*	Loads values from file.
		*/
		void loadValues();
		void setPluginInformation();
		void hideEvent(QHideEvent *);

		Ui::FormSettings	mUi;	
	
		//Settings values
		QString mMailDefaultRecipient;
		QString mMailServer;
		QString mMailUsername;
		QString mMailAddress;
		QString mMailPassword;
		int		mMailPort;
		QString mCSVCol;
		QString mCSVRow;

	private slots:
		//Mail
		void resetMailValues();
		void saveMailValues();
		void sendTestMail();
				
		//CSV
		void resetCSVValues();
		void saveCSVValues();
		void updateCSVExample();

		//Plugins
		void addPlugin();
		void reloadPlugins();

	signals:	
		void visibilityChanged	(bool);
		void settingsChanged	();
	};
}