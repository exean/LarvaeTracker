#include "GUITabWidget.h"

namespace Gui
{
	GUITabWidget::GUITabWidget( QWidget* parent )
			: QTabWidget( parent ), labelNoTabs( NULL )
	{
		//Setup gui elements displayed whenever no tabs are existent
		this->labelNoTabs = new QLabel(
			"<h3>Willkommen in PathFinder!</h3><p /><img src=':/QTTestVs10/pathfinder.png' /><p />Bitte auf <span style='color: #29B000;font-weight:bold'>+</span> klicken um einen neuen Task zu erzeugen.", 
			this);    
		this->labelNoTabs->setTextFormat(Qt::RichText);
		this->labelNoTabs->setWordWrap(true);
		this->labelNoTabs->setAlignment(Qt::AlignCenter);
		this->updateLabelPos();
	}
 
	GUITabWidget::~GUITabWidget()
	{
		delete this->labelNoTabs;
	}
 
	void GUITabWidget::tabInserted( int aIndex )
	{
		if (count())
		{
			this->labelNoTabs->setVisible(false);
		}
	}
 
	void GUITabWidget::tabRemoved( int aIndex )
	{
		if (!count())
		{
			this->labelNoTabs->setVisible(true);
		}
	}
 
	void GUITabWidget::resizeEvent(QResizeEvent* e)
	{
		QTabWidget::resizeEvent(e);
		this->updateLabelPos();
	}
 
	void GUITabWidget::updateLabelPos()
	{
		//Center label
		int xpos = this->width() / 2 - this->labelNoTabs->width() / 2;
		int ypos = this->height() / 2 - this->labelNoTabs->height() / 2;
		this->labelNoTabs->move( xpos, ypos );
	}
}