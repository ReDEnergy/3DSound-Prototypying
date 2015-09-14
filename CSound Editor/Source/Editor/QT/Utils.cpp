#include "Utils.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

QWidget* Wrap(const char* labelText, unsigned int labelWidth,  QWidget *W)
{
	auto *label = new QLabel();
	label->setText(labelText);
	label->setFixedWidth(labelWidth);

	auto *layout = new QHBoxLayout();
	layout->setContentsMargins(10, 2, 2, 2);

	layout->addWidget(label);
	layout->addWidget(W);

	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}