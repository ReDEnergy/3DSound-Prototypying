#include "DockWindow.h"
#include <iostream>

#include <include/math.h>
#include <include/utils.h>

#include <QtWidgets/QBoxLayout>
#include <QtCore/QFile>

#include <CSoundEditor.h>

using namespace std;

static unsigned int windowNumber = 0;

DockWindow::DockWindow()
{
	setWindowTitle("DockWindow");
	setMinimumWidth(120);
	setMinimumHeight(150);

	qtLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(0, 0, 0, 0);

	QWidget* layoutWidget = new QWidget();
	layoutWidget->setLayout(qtLayout);
	this->setWidget(layoutWidget);

	QObject::connect(this, &QDockWidget::topLevelChanged, this, &DockWindow::DockedEvent);

	LoadStyleSheet("stylesheet.qss");
}

void DockWindow::Init()
{
}

void DockWindow::Hide()
{
	hide();
}

void DockWindow::ReloadStyleSheet()
{
	if (styleSheet.size())
		LoadStyleSheet(styleSheet.c_str());
}

void DockWindow::LoadStyleSheet(const char * fileName)
{
	styleSheet = fileName;
	QFile File(CSoundEditor::GetStyleSheetFilePath(fileName).c_str());
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	setStyleSheet(StyleSheet);
}

void DockWindow::DockedEvent(bool state)
{
	cout << state << endl;
}
