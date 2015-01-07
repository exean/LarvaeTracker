
#include "GUI.h"
#include <QtGui/QApplication>
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Gui::GUI *w = Gui::GUI::Instance();
	w->show();
	return a.exec();
}