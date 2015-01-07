#include "GUIConsole.h"

namespace Gui
{
	GUIConsole::GUIConsole(QWidget *parent)
		: QDialog(parent)
	{
		 this->mUi.setupUi(this);

		 //Not editable
		 this->mUi.textConsole->setReadOnly(true);

		 //Vertical Scrollbar
		 this->mScroll = this->mUi.textConsole->verticalScrollBar();

		 //Title
		 this->mUi.textConsole->append("<center><h2><font color=#ff0000>P</font><font color=#e54b19>a</font><font color=#cc8f33>t</font><font color=#b2c74c>h</font><font color=#99ed66>F</font><font color=#7ffe7f>i</font><font color=#66f899>n</font><font color=#4cdcb2>d</font><font color=#33accc>e</font><font color=#196ce5>r</font></h2></center><p />");

		 this->mMaxLineCount = 100;
	}

	GUIConsole::~GUIConsole(void)
	{
	}

	void GUIConsole::addMessage(QString s, const QColor & color)
	{
		this->mUi.textConsole->append(
			"("
			+QDateTime::currentDateTime().toString("hh:mm:ss")
			+") <font color='"
			+color.name()
			+"'>"
			+s
			+"</font>");
		this->mMaxLineCount--;

		if(this->mMaxLineCount < 0)
		{
			QTextCursor cursor(this->mUi.textConsole->document()->begin().next().next());//Keep title
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.removeSelectedText();
			this->mMaxLineCount++;
		}

		//Scroll to bottom
		this->mScroll->setValue(this->mScroll->maximum());
	}

	void GUIConsole::hideEvent(QHideEvent * e)
	{    
		emit visibilityChanged(false);
	}
}