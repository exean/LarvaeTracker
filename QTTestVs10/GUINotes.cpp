#include "GUINotes.h"

namespace Gui
{
	#pragma region Singleton
	// Global static pointer used to ensure a single instance of the class.
	GUINotes* GUINotes::mInstance = NULL; 

	GUINotes* GUINotes::Instance()
	{
	   if (!mInstance)   // Only allow one instance of class to be generated.
		  mInstance = new GUINotes;
	   return mInstance;
	}
	#pragma endregion

	#pragma region con-/destructor
	GUINotes::GUINotes(void)
	{
		this->mUi.setupUi(this);

		connect(this->mUi.pushButtonBold,		SIGNAL(clicked()), this, SLOT(bold()));	
		connect(this->mUi.pushButtonItalic,		SIGNAL(clicked()), this, SLOT(italic()));	
		connect(this->mUi.pushButtonUnderline,	SIGNAL(clicked()), this, SLOT(underline()));	
		connect(this->mUi.pushButtonLarger,		SIGNAL(clicked()), this, SLOT(larger()));	
		connect(this->mUi.pushButtonSmaller,	SIGNAL(clicked()), this, SLOT(smaller()));	
		connect(this->mUi.pushButtonColFG,		SIGNAL(clicked()), this, SLOT(colorForeground()));	
		connect(this->mUi.pushButtonColBG,		SIGNAL(clicked()), this, SLOT(colorBackground()));	
		connect(this->mUi.pushButtonImage,		SIGNAL(clicked()), this, SLOT(image()));	
		connect(this->mUi.pushButtonTable,		SIGNAL(clicked()), this, SLOT(table()));	
		connect(this->mUi.pushButtonCode,		SIGNAL(clicked(bool)),this,SLOT(toggleCode(bool)));
		//connect(this->mUi.pushButtonSubscript,	SIGNAL(clicked()), this, SLOT(subscript()));
		//connect(this->mUi.pushButtonSuperscript,	SIGNAL(clicked()), this, SLOT(superscript()));

		this->loadNotes();
	}

	GUINotes::~GUINotes(void)
	{
	}
	#pragma endregion

	void GUINotes::addHTML(const QString &html)
	{
		if(this->mUi.widgetButtons->isEnabled())
		{
			this->mUi.textEditNotes->insertHtml(html);
		}
		else
		{
			this->mUi.textEditNotes->insertPlainText(html);
		}	
	}

	#pragma region In/Out
	void GUINotes::loadNotes()
	{
		QFile file("notes_file"); 
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			this->mUi.textEditNotes->setHtml(file.readAll()); 
			file.close();
		}
	}

	void GUINotes::deleteUnusedImages()
	{
		//Get random filename
		if(QDir("notes-files").exists())
		{
			QString dirPath = "."+QString(QDir::separator())+"notes-files"+QString(QDir::separator());
			QDir myDir(dirPath);
			QStringList imageList = myDir.entryList(QStringList("*.png"));
			QString text = this->mUi.textEditNotes->toHtml();
			for(QList<QString>::iterator fileIt = imageList.begin();
				fileIt != imageList.end();
				fileIt++)
			{
				if(!text.contains(*fileIt, Qt::CaseInsensitive))
				{
					QFile::remove(dirPath+*fileIt);
				}
			}
		}	
	}

	void GUINotes::saveNotes()
	{	
		QFile file("notes_file");
		if(file.open(QIODevice::WriteOnly | QIODevice::Text))
		{		
			QTextStream out(&file);
			out << ((this->mUi.widgetButtons->isEnabled()) 
				? this->mUi.textEditNotes->toHtml()
				: this->mUi.textEditNotes->toPlainText());
			file.close();
		}
		this->deleteUnusedImages();
	}
	#pragma endregion

	#pragma region text format
	void GUINotes::bold()
	{	
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();

		QTextCharFormat format;
		if(cursor.charFormat ().fontWeight() == QFont::Bold)
		{
			format.setFontWeight(QFont::Normal);
		}
		else
		{
			format.setFontWeight(QFont::Bold);
		}

		cursor.mergeCharFormat(format);
	}

	void GUINotes::italic()
	{	
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	
		QTextCharFormat format;
		format.setFontItalic(!cursor.charFormat().fontItalic());

		cursor.mergeCharFormat(format);
	}

	void GUINotes::underline()
	{	
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	
		QTextCharFormat format;
		format.setFontUnderline(!cursor.charFormat().fontUnderline());

		cursor.mergeCharFormat(format);
	}

	void GUINotes::larger()
	{	
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	
		QTextCharFormat format;
		format.setFontPointSize(cursor.charFormat().fontPointSize()+1);

		cursor.mergeCharFormat(format);
	}

	void GUINotes::smaller()
	{	
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	
		QTextCharFormat format;
		format.setFontPointSize(cursor.charFormat().fontPointSize()-1);
	
		cursor.mergeCharFormat(format);
	}

	void GUINotes::colorForeground()
	{		
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();

		QColor col = cursor.charFormat().foreground().color();
		col = QColorDialog::getColor( col, this, "Textfarbe wählen");
		if(col.isValid())
		{		
			QTextCharFormat format;
			format.setForeground(QBrush(col));
			cursor.mergeCharFormat(format);
		}	
	}

	void GUINotes::colorBackground()
	{		
		QTextCursor cursor = this->mUi.textEditNotes->textCursor();

		QColor col = cursor.charFormat().background().color();
		col = QColorDialog::getColor( col, this, "Hintergrundfarbe wählen");
		if(col.isValid())
		{		
			QTextCharFormat format;
			format.setBackground(QBrush(col));
			cursor.mergeCharFormat(format);
		}	
	}

	void GUINotes::image()
	{	
		QString fileName = QFileDialog::getOpenFileName(
			this,
			"Bild öffnen", 
			"", 
			"Bilder (*.png *.jpg *.bmp)");

		if(!fileName.isNull())
		{
			this->mUi.textEditNotes->insertHtml("<img src='"+fileName+"' />");		
		}
	}

	void GUINotes::table()
	{		
		unsigned int rows = this->mUi.spinBoxRows->value();
		unsigned int cols = this->mUi.spinBoxCols->value();;

		if(rows > 0 && cols > 0)
		{
			QString tablepart = "", tablehead = "";
			for(unsigned int i = 0; i < cols; i++)
			{
				tablehead += "<th>&nbsp;</th>";
				tablepart += "<td></td>";
			}
			tablepart = "<tr>"+tablepart+"</tr>";
			QString table = "<tr>"+tablehead+"</tr>";
			for(unsigned int i = 1; i < rows; i++)
			{
				table += tablepart;
			}
			this->mUi.textEditNotes->insertHtml("<table border='1'>"+table+"</table>");		
		}
	}

	void GUINotes::toggleCode(bool showCode)
	{	
		if(showCode)
		{
			this->mUi.textEditNotes->setPlainText(this->mUi.textEditNotes->toHtml());

			//Disable buttons
			this->mUi.widgetButtons->setEnabled(false);				
		}
		else
		{
			this->mUi.textEditNotes->setHtml(this->mUi.textEditNotes->toPlainText());

			//Enable buttons
			this->mUi.widgetButtons->setEnabled(true);		
		}			
	}

	//void GUINotes::subscript()
	//{	
	//	QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	//	
	//	QTextCharFormat format;
	//	format.setFontPointSize(cursor.charFormat().verticalAlignment() == QTextCharFormat::VerticalAlignment::AlignSubScript
	//		? QTextCharFormat::VerticalAlignment::AlignNormal
	//		: QTextCharFormat::VerticalAlignment::AlignSubScript);
	//
	//	cursor.mergeCharFormat(format);
	//}
	//
	//void GUINotes::superscript()
	//{	
	//	QTextCursor cursor = this->mUi.textEditNotes->textCursor();
	//	
	//	QTextCharFormat format;
	//	format.setFontPointSize(cursor.charFormat().verticalAlignment() == QTextCharFormat::VerticalAlignment::AlignSuperScript
	//		? QTextCharFormat::VerticalAlignment::AlignNormal
	//		: QTextCharFormat::VerticalAlignment::AlignSuperScript);
	//
	//	cursor.mergeCharFormat(format);
	//}

	#pragma endregion
}