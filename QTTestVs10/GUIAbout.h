/**
 *  @file GUIAbout.h
 *
 * @section DESCRIPTION
 * An "about"-dialog that displays information about
 * the application.
 */

#pragma once

#include <QtGui/QDialog>
#include "ui_QTAbout.h"
#include "PluginLoader.h"

namespace Gui
{
	/**
	 * An "about"-dialog that displays information about
	 * the application.
	 */
	class GUIAbout : public QDialog
	{
		Q_OBJECT

	public:
		GUIAbout(QWidget *parent = 0);
		~GUIAbout(void);

	private:
		/**
		 *  Gets information from the currently loaded plugins.
		 */
		void insertPluginInfortmation(); 

		/**
		 *  Userinterface as created by QTCreator.
		 */
		Ui::FormAbout mUi;

	private slots:
		/**
		 *  Hide the dialog.
		 */
		void hide();
	};
}
