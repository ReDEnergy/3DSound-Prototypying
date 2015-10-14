#include "CustomWidget.h"
#include <iostream>

#include <include/math.h>
#include <include/utils.h>

#include <QtWidgets/QBoxLayout>
#include <QtCore/QFile>

using namespace std;

static unsigned int windowNumber = 0;

CustomWidget::CustomWidget(QBoxLayout::Direction direction)
{
	qtLayout = new QBoxLayout(direction);
	qtLayout->setMargin(0);
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);

	this->setLayout(qtLayout);

	styleSheet = "";
}

void CustomWidget::Init()
{
	cout << "Init" << endl;
}

void CustomWidget::Hide()
{
	hide();
}

void CustomWidget::ReloadStyleSheet()
{
	if (styleSheet.size())
		LoadStyleSheet(styleSheet.c_str());
}

void CustomWidget::LoadStyleSheet(const char * fileLocation)
{
	styleSheet = fileLocation;
	QFile File(fileLocation);
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	setStyleSheet(StyleSheet);
}