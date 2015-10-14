#include "Utils.h"

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QRegExpValidator>

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

QWidget* Wrap(QLabel* label, QWidget *W)
{
	auto *layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(10);

	layout->addWidget(label);
	layout->addWidget(W);

	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}


QRegExpValidator* QtTextValidator::floatValidator = nullptr;

QRegExpValidator * QtTextValidator::GetFloatValidator()
{
	if (floatValidator == nullptr) {
		QRegExp rx("(-?[0-9]*\\.*[0-9]*)");
		floatValidator = new QRegExpValidator(rx);
	}
	return floatValidator;
}
