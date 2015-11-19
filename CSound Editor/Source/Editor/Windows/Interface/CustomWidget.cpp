#include "CustomWidget.h"
#include <iostream>

#include <include/math.h>
#include <include/utils.h>

#include <QtWidgets/QBoxLayout>
#include <QtCore/QFile>

#include <CSoundEditor.h>
#include <Editor/QtEditor.h>
#include <QIcon>

using namespace std;

static unsigned int windowNumber = 0;

CustomWidget::CustomWidget(QBoxLayout::Direction direction)
{
	SetIcon("colorwheel.png");

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

void CustomWidget::Toggle()
{
	if (isVisible())
		Hide();
	else
		show();
}

void CustomWidget::AddWidget(QWidget * widget)
{
	if (widget)
		qtLayout->addWidget(widget);
}

void CustomWidget::DetachFromParent()
{
	QWidget *widgetParent = (QWidget*)parent();
	if (widgetParent) {
		widgetParent->layout()->removeWidget(this);
		this->setParent(nullptr);
	}
}

void CustomWidget::SetAsToolWindow()
{
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
}

void CustomWidget::SetIcon(const char * fileName)
{
	setWindowIcon(*QtEditor::GetIcon(fileName));
}

void CustomWidget::SetStyleID(const char * ID)
{
	setObjectName(ID);
}

void CustomWidget::ReloadStyleSheet()
{
	if (styleSheet.size())
		LoadStyleSheet(styleSheet.c_str());
}

void CustomWidget::LoadStyleSheet(const char * fileName)
{
	styleSheet = fileName;
	QFile File(CSoundEditor::GetStyleSheetFilePath(fileName).c_str());
	File.open(QFile::ReadOnly);
	QString StyleSheet = QLatin1String(File.readAll());
	setStyleSheet(StyleSheet);
}