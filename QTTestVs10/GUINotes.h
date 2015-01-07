/**
 *  @file GUINotes.h
 *
 * @section DESCRIPTION
 * GUI-Component for writing notes. Uses HTML.
 */

#pragma once

#include <QWidget>
#include <QLayout>
#include <QTextEdit>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QColorDialog>
#include "ui_QTNotes.h"

//Singleton
namespace Gui
{
	/**
	 * GUI-Component for writing notes. Uses HTML.
	 */
	class GUINotes : public QWidget
	{
		Q_OBJECT

	public:
		static GUINotes* Instance();

		~GUINotes(void);

		/**
		* Adds html to the note-file.
		*
		* @param	_html	HTML to be added.
		*/
		void addHTML(const QString &	_html);
		
		/**
		* Saves notes.
		*/
		void saveNotes();

	private:
		GUINotes(void);
		static GUINotes* mInstance;

		/**
		* Loads notes.
		*/
		void loadNotes();

		/**
		* Deletes all images from disk that
		* were specifically saved for the notes
		* but are no longer in use.
		*/
		void deleteUnusedImages();

		Ui::NotesForm	mUi;	

	private slots:
		void bold();
		void italic();
		void underline();
		void smaller();
		void larger();
		void colorForeground();
		void colorBackground();
		void table();
		void image();
		void toggleCode(bool showCode);
		//void subscript();
		//void superscript();
	};
}